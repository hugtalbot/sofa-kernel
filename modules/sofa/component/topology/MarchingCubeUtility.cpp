/**** Geometry Shader Marching Cubes
	* Copyright Cyril Crassin, tt 2007.
	* This code is partially based on the example of
	* Paul Bourke "Polygonising a scalar field" located at :
	* http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
****/

#include <sofa/component/topology/MarchingCubeUtility.h>
#include <sofa/component/mapping/ImplicitSurfaceMapping.h>

#define PRECISION 10000.0f

namespace sofa
{

namespace component
{

namespace topology
{

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
void MarchingCubeUtility::VertexInterp(const float isolevel, const Vec3f &p1, const Vec3f &p2, const float valp1, const float valp2, Vec3f &p) const
{
    float mu = (isolevel - valp1) / (valp2 - valp1);
    p = p1 + (p2 - p1) * mu;
    p[0] = (int)(p[0]*PRECISION)/PRECISION;
    p[1] = (int)(p[1]*PRECISION)/PRECISION;
    p[2] = (int)(p[2]*PRECISION)/PRECISION;
}


bool MarchingCubeUtility::testGrid(const float v, const float isolevel) const
{
    return (v<isolevel);
}


/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
int MarchingCubeUtility::Polygonise(const GridCell &grid, float isolevel, sofa::helper::vector< IdVertex > &triangles,
        std::map< Vec3f, IdVertex> &map_vertices,
        std::map< IdVertex, Vec3f> &map_indices,
        unsigned int &ID) const
{
    using namespace sofa::component::mapping; //to grant access to tri and edge tables
    int i,ntriang;
    int cubeindex;
    Vec3f vertindex[12];
    /*
    Determine the index into the edge table which
    tells us which vertices are inside of the surface
    */
    cubeindex = 0;
    if (testGrid(grid.val[0], isolevel)) cubeindex |= 1;
    if (testGrid(grid.val[1], isolevel)) cubeindex |= 2;
    if (testGrid(grid.val[2], isolevel)) cubeindex |= 4;
    if (testGrid(grid.val[3], isolevel)) cubeindex |= 8;
    if (testGrid(grid.val[4], isolevel)) cubeindex |= 16;
    if (testGrid(grid.val[5], isolevel)) cubeindex |= 32;
    if (testGrid(grid.val[6], isolevel)) cubeindex |= 64;
    if (testGrid(grid.val[7], isolevel)) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (MarchingCubeEdgeTable[cubeindex] == 0) return(0);

    /* Find the vertices where the surface intersects the cube */
    if (MarchingCubeEdgeTable[cubeindex] & 1)
        VertexInterp(isolevel,grid.pos[0],grid.pos[1],grid.val[0],grid.val[1],vertindex[0]);
    if (MarchingCubeEdgeTable[cubeindex] & 2)
        VertexInterp(isolevel,grid.pos[1],grid.pos[2],grid.val[1],grid.val[2],vertindex[1]);
    if (MarchingCubeEdgeTable[cubeindex] & 4)
        VertexInterp(isolevel,grid.pos[2],grid.pos[3],grid.val[2],grid.val[3],vertindex[2]);
    if (MarchingCubeEdgeTable[cubeindex] & 8)
        VertexInterp(isolevel,grid.pos[3],grid.pos[0],grid.val[3],grid.val[0],vertindex[3]);
    if (MarchingCubeEdgeTable[cubeindex] & 16)
        VertexInterp(isolevel,grid.pos[4],grid.pos[5],grid.val[4],grid.val[5],vertindex[4]);
    if (MarchingCubeEdgeTable[cubeindex] & 32)
        VertexInterp(isolevel,grid.pos[5],grid.pos[6],grid.val[5],grid.val[6],vertindex[5]);
    if (MarchingCubeEdgeTable[cubeindex] & 64)
        VertexInterp(isolevel,grid.pos[6],grid.pos[7],grid.val[6],grid.val[7],vertindex[6]);
    if (MarchingCubeEdgeTable[cubeindex] & 128)
        VertexInterp(isolevel,grid.pos[7],grid.pos[4],grid.val[7],grid.val[4],vertindex[7]);
    if (MarchingCubeEdgeTable[cubeindex] & 256)
        VertexInterp(isolevel,grid.pos[0],grid.pos[4],grid.val[0],grid.val[4],vertindex[8]);
    if (MarchingCubeEdgeTable[cubeindex] & 512)
        VertexInterp(isolevel,grid.pos[1],grid.pos[5],grid.val[1],grid.val[5],vertindex[9]);
    if (MarchingCubeEdgeTable[cubeindex] & 1024)
        VertexInterp(isolevel,grid.pos[2],grid.pos[6],grid.val[2],grid.val[6],vertindex[10]);
    if (MarchingCubeEdgeTable[cubeindex] & 2048)
        VertexInterp(isolevel,grid.pos[3],grid.pos[7],grid.val[3],grid.val[7],vertindex[11]);

    /* Create the triangle */
    ntriang = 0;
    std::map< Vec3f, IdVertex>::iterator iter;
    Vec3f current_P;
    IdVertex current_ID;
    for (i=0; MarchingCubeTriTable[cubeindex][i]!=-1; i+=3)
    {
        for (IdVertex j=0; j<3; ++j)
        {
            current_P = vertindex[MarchingCubeTriTable[cubeindex][i+j]];
            //Search if the current Vertex P is already stored with an ID
            iter = map_vertices.find(current_P);
            if (iter != map_vertices.end()) current_ID = iter->second;
            else
            {
                //Add new Vertex in map
                ID++;
                current_ID = ID;
                map_vertices.insert(std::make_pair(current_P, current_ID));
                map_indices.insert (std::make_pair(current_ID,current_P));
            }
            triangles.push_back(current_ID);
        }
        ntriang+=3;
    }
    return(ntriang);
}



void MarchingCubeUtility::RenderMarchCube(const float *data, const float isolevel,
        sofa::helper::vector< IdVertex > &mesh,
        std::map< IdVertex, Vec3f>       &map_indices) const
{
    unsigned int ID = 0;

    std::map< Vec3f, IdVertex> map_vertices;

    Vec3f gridStep=Vec3f(2.0f/((float)gridsize[0]),2.0f/((float)gridsize[1]),2.0f/((float)gridsize[2]));

    Vec<3,int> dataGridStep(size[0]/gridsize[0],size[1]/gridsize[1],size[2]/gridsize[2]);

    IdVertex counter_triangles=0;
    for(int k=0; k<gridsize[2]-1; k++)
        for(int j=0; j<gridsize[1]-1; j++)
            for(int i=0; i<gridsize[0]-1; i++)
            {
                GridCell cell;
                Vec3f vcurf(i, j, k);
                Vec<3,int> vcuri(i, j, k);

                cell.pos[0]=vcurf.linearProduct(gridStep)-Vec3f(1.0f,1.0f,1.0f);
                Vec<3,int> valPos0=vcuri.linearProduct(dataGridStep);
                cell.val[0]=data[valPos0[0] + valPos0[1]*size[0] + valPos0[2]*size[0]*size[1]];

                Vec<3,int> valPos;

                cell.pos[1]=cell.pos[0]+Vec3f(gridStep[0], 0, 0);
                if(i==gridsize[0]-1)
                    valPos=valPos0;
                else
                    valPos=valPos0+Vec<3,int>(dataGridStep[0], 0, 0);
                cell.val[1]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];

                cell.pos[2]=cell.pos[0]+Vec3f(gridStep[0], gridStep[1], 0);
                valPos=valPos0+Vec<3,int>(i==gridsize[0]-1 ? 0 : dataGridStep[0], j==gridsize[1]-1 ? 0 : dataGridStep[1], 0);
                cell.val[2]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];

                cell.pos[3]=cell.pos[0]+Vec3f(0, gridStep[1], 0);
                valPos=valPos0+Vec<3,int>(0, j==gridsize[1]-1 ? 0 : dataGridStep[1], 0);
                cell.val[3]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];



                cell.pos[4]=cell.pos[0]+Vec3f(0, 0, gridStep[2]);
                valPos=valPos0+Vec<3,int>(0, 0, k==gridsize[2]-1 ? 0 : dataGridStep[2]);
                cell.val[4]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];


                cell.pos[5]=cell.pos[0]+Vec3f(gridStep[0], 0, gridStep[2]);
                valPos=valPos0+Vec<3,int>(i==gridsize[0]-1 ? 0 : dataGridStep[0], 0, k==gridsize[2]-1 ? 0 : dataGridStep[2]);
                cell.val[5]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];

                cell.pos[6]=cell.pos[0]+Vec3f(gridStep[0], gridStep[1], gridStep[2]);
                valPos=valPos0+Vec<3,int>(i==gridsize[0]-1 ? 0 : dataGridStep[0], j==gridsize[1]-1 ? 0 : dataGridStep[1], k==gridsize[2]-1 ? 0 : dataGridStep[2]);
                cell.val[6]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];

                cell.pos[7]=cell.pos[0]+Vec3f(0, gridStep[1], gridStep[2]);
                valPos=valPos0+Vec<3,int>(0, j==gridsize[1]-1 ? 0 : dataGridStep[1], k==gridsize[2]-1 ? 0 : dataGridStep[2]);
                cell.val[7]=data[valPos[0] + valPos[1]*size[0] + valPos[2]*size[0]*size[1]];


                int numvert=Polygonise(cell, isolevel, mesh, map_vertices, map_indices, ID);
                counter_triangles += numvert/3;
            }


}

void MarchingCubeUtility::createMesh( const sofa::helper::vector< IdVertex > &mesh,
        std::map< IdVertex,  Vec3f>       &map_indices,
        sofa::helper::io::Mesh &m) const
{
    vector<Vector3> &vertices                 = m.getVertices();
    vector< vector < vector <int> > > &facets = m.getFacets();

    for (unsigned int i=0; i<map_indices.size(); ++i)
    {
        vertices.push_back(map_indices[i+1]);
    }

    vector<int> vIndices;
    const vector<int> nIndices = vector<int>(3,0);
    const vector<int> tIndices = vector<int>(3,0);
    vector< vector< int > > vertNormTexIndices;
    for (unsigned int i=0; i<mesh.size(); i+=3)
    {
        vIndices.clear();
        vertNormTexIndices.clear();
        vIndices.push_back(mesh[i]-1);	           vIndices.push_back(mesh[i+1]-1);	    vIndices.push_back(mesh[i+2]-1);
        vertNormTexIndices.push_back(vIndices);  vertNormTexIndices.push_back(nIndices);  vertNormTexIndices.push_back(tIndices);
        facets.push_back(vertNormTexIndices);
    }
}

void MarchingCubeUtility::createMesh( const float *data,  const float isolevel, sofa::helper::io::Mesh &m) const
{
    using sofa::helper::vector;
    using sofa::defaulttype::Vector3;

    sofa::helper::vector< IdVertex >  mesh;
    std::map< IdVertex, Vec3f>  map_indices;

    //Do the Marching Cube
    RenderMarchCube(data, isolevel, mesh, map_indices);
    createMesh(mesh, map_indices, m);
}
}
}
}
