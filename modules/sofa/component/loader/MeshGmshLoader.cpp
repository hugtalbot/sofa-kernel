/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/ObjectFactory.h>
#include <sofa/component/loader/MeshGmshLoader.h>
#include <iostream>

namespace sofa
{

namespace component
{

namespace loader
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(MeshGmshLoader)

int MeshGmshLoaderClass = core::RegisterObject("Specific mesh loader for Gmsh file format.")
        .add< MeshGmshLoader >()
        ;

bool MeshGmshLoader::load()
{
    std::cout << "Loading Gmsh file: " << m_filename << std::endl;

    FILE* file;
    char cmd[1024];
    bool fileRead = false;
    unsigned int gmshFormat = 0;

    // -- Loading file
    const char* filename = m_filename.getFullPath().c_str();
    if ((file = fopen(filename, "r")) == NULL)
    {
        std::cerr << "Error: MeshGmshLoader: Cannot read file '" << m_filename << "'." << std::endl;
        return false;
    }

    // -- Looking for Gmsh version of this file.
    if (!strncmp(cmd,"$MeshFormat",11)) // Reading gmsh 2.0 file
    {
        gmshFormat = 2;
        // std::cout << "Gmsh format 2.0" << std::endl;
        readLine(cmd, sizeof(cmd), file); // we don't care about this line
        readLine(cmd, sizeof(cmd), file);
        if (strncmp(cmd,"$EndMeshFormat",14)) // it should end with $EndMeshFormat
        {
            std::cerr << "Error: MeshGmshLoader: File '" << m_filename << "' is empty." << std::endl;
            fclose(file);
            return false;
        }
        else
            readLine(cmd, sizeof(cmd), file);
    }
    else
    {
        gmshFormat = 1;
        //std::cout << "Gmsh format 1.0" << std::endl;
    }

    // -- Reading file
    readLine(cmd, sizeof(cmd), file);
    if (!strncmp(cmd,"$NOD",4) || !strncmp(cmd,"$Nodes",6)) // Gmsh format
    {
        fileRead = readGmsh(file, gmshFormat);
    }
    else //if it enter this "else", it means there is a problem before in the factory or in canLoad()
    {
        std::cerr << "Error: MeshGmshLoader: File '" << m_filename << "' finally appears not to be a Gmsh file." << std::endl;
        fclose(file);
        return false;
    }

    return fileRead;
}



bool MeshGmshLoader::readGmsh(FILE *file, const unsigned int gmshFormat)
{
    std::cout << "Reading Gmsh file: " << gmshFormat << std::endl;

    char cmd[1024];

    unsigned int npoints = 0;
    unsigned int nelems = 0;

    unsigned int nlines = 0;
    unsigned int ntris = 0;
    unsigned int nquads = 0;
    unsigned int ntetras = 0;
    unsigned int ncubes = 0;

    // --- Loading Vertices ---
    fscanf(file, "%d\n", &npoints); //Loading number of Point
    helper::vector<sofa::defaulttype::Vector3>& my_positions = *(positions.beginEdit());

    std::vector<unsigned int> pmap; // map for reordering vertices possibly not well sorted
    for (unsigned int i=0; i<npoints; ++i)
    {
        unsigned int index = i;
        double x,y,z;
        fscanf(file, "%d %lf %lf %lf\n", &index, &x, &y, &z);
        my_positions.push_back(Vector3(x, y, z));

        if (pmap.size() <= index)
            pmap.resize(index+1);

        pmap[index] = i; // In case of hole or switch
        //std::cout << "pmap[" << index << "] = " << pmap[index] << std::endl;
    }
    positions.endEdit();

    readLine(cmd, sizeof(cmd), file);
    if (strncmp(cmd,"$ENDNOD",7) && strncmp(cmd,"$EndNodes",9))
    {
        std::cerr << "Error: MeshGmshLoader: '$ENDNOD' or '$EndNodes' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }


    // --- Loading Elements ---
    readLine(cmd, sizeof(cmd), file);
    if (strncmp(cmd,"$ELM",4) && strncmp(cmd,"$Elements",9))
    {
        std::cerr << "Error: MeshGmshLoader: '$ELM' or '$Elements' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }

    fscanf(file, "%d\n", &nelems); //Loading number of Element

    helper::vector<helper::fixed_array <unsigned int,2> >& my_edges = *(edges.beginEdit());
    helper::vector<helper::fixed_array <unsigned int,3> >& my_triangles = *(triangles.beginEdit());
    helper::vector<helper::fixed_array <unsigned int,4> >& my_quads = *(quads.beginEdit());
    helper::vector<helper::fixed_array <unsigned int,4> >& my_tetrahedra = *(tetrahedra.beginEdit());
    helper::vector<helper::fixed_array <unsigned int,8> >& my_hexahedra = *(hexahedra.beginEdit());


    for (unsigned int i=0; i<nelems; ++i) // for each elem
    {
        int index, etype, rphys, relem, nnodes, ntags, tag;

        if (gmshFormat==1)
        {
            // version 1.0 format is
            // elm-number elm-type reg-phys reg-elem number-of-nodes <node-number-list ...>
            fscanf(file, "%d %d %d %d %d", &index, &etype, &rphys, &relem, &nnodes);
        }
        else if (gmshFormat == 2)
        {
            // version 2.0 format is
            // elm-number elm-type number-of-tags < tag > ... node-number-list
            fscanf(file, "%d %d %d", &index, &etype, &ntags);
            for (int t=0; t<ntags; t++)
                fscanf(file, "%d", &tag); // read the tag but don't use it

            switch (etype)
            {
            case 1: // Line
                nnodes = 2;
                break;
            case 2: // Triangle
                nnodes = 3;
                break;
            case 3: // Quad
                nnodes = 4;
                break;
            case 4: // Tetra
                nnodes = 4;
                break;
            case 5: // Hexa
                nnodes = 8;
                break;
            default:
                std::cerr << "Error: MeshGmshLoader: Elements of type 1, 2, 3, 4, 5, or 6 expected. Element of type " << etype << " found." << std::endl;
                nnodes = 0;
            }
        }
        //store real index of node and not line index

        helper::vector <unsigned int> nodes;
        nodes.resize (nnodes);

        for (int n=0; n<nnodes; ++n)
        {
            int t = 0;
            fscanf(file, "%d",&t);
            nodes[n] = (((unsigned int)t)<pmap.size())?pmap[t]:0;
            //std::cout << "nodes[" << n << "] = " << nodes[n] << std::endl;
        }

        switch (etype)
        {
        case 1: // Line
            my_edges.push_back (helper::fixed_array <unsigned int,2>(nodes[0], nodes[1]));
            ++nlines;
            break;
        case 2: // Triangle
            my_triangles.push_back (helper::fixed_array <unsigned int,3>(nodes[0], nodes[1], nodes[2]));
            ++ntris;
            break;
        case 3: // Quad
            my_quads.push_back (helper::fixed_array <unsigned int,4>(nodes[0], nodes[1], nodes[2], nodes[3]));
            ++nquads;
            break;
        case 4: // Tetra
            my_tetrahedra.push_back (helper::fixed_array <unsigned int,4>(nodes[0], nodes[1], nodes[2], nodes[3]));
            ++ntetras;
            break;
        case 5: // Hexa
            helper::fixed_array <unsigned int,8> hexa;
            for (unsigned int n=0; n<8; n++)
                hexa[n] = nodes[n];
            my_hexahedra.push_back (hexa);
            ++ncubes;
            break;
        }
        skipToEOL(file);
    }

    edges.endEdit();
    triangles.endEdit();
    quads.endEdit();
    tetrahedra.endEdit();
    hexahedra.endEdit();

    readLine(cmd, sizeof(cmd), file);
    if (strncmp(cmd,"$ENDELM",7) && strncmp(cmd,"$EndElements",12))
    {
        std::cerr << "Error: MeshGmshLoader: '$ENDELM' or '$EndElements' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }

    // 	std::cout << "Loading topology complete:";
    // 	if (npoints>0) std::cout << ' ' << npoints << " points";
    // 	if (nlines>0)  std::cout << ' ' << nlines  << " lines";
    // 	if (ntris>0)   std::cout << ' ' << ntris   << " triangles";
    // 	if (nquads>0)  std::cout << ' ' << nquads  << " quads";
    // 	if (ntetras>0) std::cout << ' ' << ntetras << " tetrahedra";
    // 	if (ncubes>0)  std::cout << ' ' << ncubes  << " cubes";
    // 	std::cout << std::endl;

    fclose(file);
    return true;
}


} // namespace loader

} // namespace component

} // namespace sofa

