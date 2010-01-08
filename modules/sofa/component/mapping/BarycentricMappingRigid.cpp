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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPINGRIGID_CPP
#include <sofa/component/mapping/BarycentricMapping.inl>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/Mapping.inl>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>

#include <sofa/helper/PolarDecompose.h>

#ifdef SOFA_IP_TRACES
#include <sofa/component/mapping/_IP_MapTraceMacros.h>
#endif

namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;
using namespace core;
using namespace core::componentmodel::behavior;

SOFA_DECL_CLASS(BarycentricMappingRigid)

// Register in the Factory
int BarycentricMappingRigidClass = core::RegisterObject("")
#ifndef SOFA_FLOAT
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Rigid3dTypes> > > >()
        //.add< BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Rigid3dTypes> > > >()
#endif
#ifndef SOFA_DOUBLE
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Rigid3fTypes> > > >()
        //.add< BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Rigid3fTypes> > > >()
#endif
#ifndef SOFA_FLOAT
#ifndef SOFA_DOUBLE
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Rigid3dTypes> > > >()
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Rigid3fTypes> > > >()
        //.add< BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Rigid3dTypes> > > >()
        //.add< BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Rigid3fTypes> > > >()
#endif
#endif
        ;


template <>
int BarycentricMapperTetrahedronSetTopology<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>::addPointOrientationInTetra ( const int tetraIndex, const Matrix3 baryCoorsOrient )
{
    //storing the frame in 3 maps: one direction vector in one map  (3 coor. elements inside a map)
    // IPTR_BARCPP_ADDOR("addPointOrientation BEGIN" << endl);
    for (unsigned int dir = 0; dir < 3; dir++)
    {
        helper::vector<MappingData>& vectorData = *(mapOrient[dir].beginEdit());
        vectorData.resize ( mapOrient[dir].getValue().size() +1 );
        MappingData& data = *vectorData.rbegin();
        mapOrient[dir].endEdit();
        data.in_index = tetraIndex;
        // IPTR_BARCPP_ADDOR("baryCoords of vector["<<dir<<"]: ");
        for (unsigned int coor = 0; coor < 3; coor++)
        {
            data.baryCoords[coor] = ( Real ) baryCoorsOrient[coor][dir];
            //IPNTR_BARCPP_ADDOR(data.baryCoords[coor] << " ");
        }
        //IPNTR_BARCPP_ADDOR(endl);

    }
    // IPTR_BARCPP_ADDOR("addPointOrientation END" << endl);
    return map.getValue().size()-1;
}



template <>  //typename defaulttype::Vec3dTypes,  typename defaulttype::Rigid3dTypes>
void BarycentricMapperTetrahedronSetTopology<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>::init ( const defaulttype::Rigid3dTypes::VecCoord& out, const defaulttype::Vec3dTypes::VecCoord& in )
{
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_INIT("BarycentricMapperTetrahedronSetTopology::init BEGIN " << endl);
    // IPTR_BARCPP_INIT("In: " << in << endl);
    // IPTR_BARCPP_INIT("Out: " << out << endl);
    IPTR_BARCPP_INIT("out size = " << out.size() << endl);
#endif

    _container->getContext()->get ( _geomAlgo );
    initialTetraPos = in;
    prevTetraRotation.resize(out.size());

    int outside = 0;
    const sofa::helper::vector<topology::Tetrahedron>& tetrahedra = this->topology->getTetrahedra();

    sofa::helper::vector<Matrix3> bases;
    sofa::helper::vector<Vector3> centers;

    clear ( out.size() );
    bases.resize ( tetrahedra.size() );
    centers.resize ( tetrahedra.size() );
    for ( unsigned int t = 0; t < tetrahedra.size(); t++ )
    {
        Mat3x3d m,mt;
        m[0] = in[tetrahedra[t][1]]-in[tetrahedra[t][0]];
        m[1] = in[tetrahedra[t][2]]-in[tetrahedra[t][0]];
        m[2] = in[tetrahedra[t][3]]-in[tetrahedra[t][0]];
        mt.transpose ( m );
        bases[t].invert ( mt );
        centers[t] = ( in[tetrahedra[t][0]]+in[tetrahedra[t][1]]+in[tetrahedra[t][2]]+in[tetrahedra[t][3]] ) *0.25;
    }

    //find the closest tetra for given points of beam
    for ( unsigned int i=0; i<out.size(); i++ )
    {
        //IPB
        Vector3 pos = out[i].getCenter(); // Rigid3dTypes::GetCPos(out[i]); // pos = defaulttype::Rigid3dType::getCPos(out[i]);

        //associate the point with the tetrahedron, point in Barycentric coors wrt. the closest tetra, store to an associated structure
        Vector3 coefs;
        int index = -1;
        double distance = 1e10;
        for ( unsigned int t = 0; t < tetrahedra.size(); t++ )
        {
            Vec3d v = bases[t] * ( pos - in[tetrahedra[t][0]] );
            double d = std::max ( std::max ( -v[0],-v[1] ),std::max ( -v[2],v[0]+v[1]+v[2]-1 ) );
            if ( d>0 ) d = ( pos-centers[t] ).norm2();
            if ( d<distance )
            {
                coefs = v; distance = d; index = t;
            }
        }
        if ( distance>0 ) ++outside;

        //convert the orientation to basis given by closest tetrahedron
        Quat quatA = out[i].getOrientation();
        Matrix3 orientationMatrix, orientationMatrixBary;
        quatA.toMatrix(orientationMatrix);    //direction vectors stored as columns
        orientationMatrix.transpose(); //to simplify the multiplication below
        for (unsigned c=0; c < 3; c++)
        {
            orientationMatrixBary[c]=bases[index]*orientationMatrix[c];
            orientationMatrixBary[c].normalize();
        }
        orientationMatrixBary.transpose();  //to get the directions as columns

        //store the point and orientation in local coordinates of tetrahedra frame
        addPointInTetra ( index, coefs.ptr() );
        addPointOrientationInTetra(index, orientationMatrixBary);

        //get tetraorientation (in the initial position)
        Quat quatInitRot;
        quatInitRot.fromMatrix(forceField->getActualTetraRotation(index));
        prevTetraRotation[i]=quatInitRot;
#ifdef SOFA_IP_TRACES
        IPTR_BARCPP_INIT("Initial tetra["<<index<<"] rotation matrix = " << forceField->getActualTetraRotation(index) << endl);
        IPTR_BARCPP_INIT("Initial tetra["<<index<<"] rotation quaternion = " << prevTetraRotation[i] << endl);
#endif

    }
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_INIT("BarycentricMapperTetrahedronSetTopology::init END" << endl);
#endif
}

template <>
void BarycentricMapperTetrahedronSetTopology<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>::apply ( defaulttype::Rigid3dTypes::VecCoord& out, const defaulttype::Vec3dTypes::VecCoord& in )
{
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_APPLY( "BarycentricMapperTetrahedronSetTopology<SPEC>::apply BEGIN" << endl);
#endif

    //get number of point being mapped
    unsigned int nbPoints = map.getValue().size();
    out.resize (nbPoints);
    const sofa::helper::vector<topology::Tetrahedron>& tetrahedra = this->topology->getTetrahedra();
    glPointPositions.resize( nbPoints );
    for (int vt=0; vt < 4; vt++)
        glVertexPositions[vt].resize( nbPoints );

    defaulttype::Vec3dTypes::VecCoord inCopy = in;

    for ( unsigned int i=0; i<map.getValue().size(); i++ )
    {
        //get barycentric coors for given point (node of a beam in this case)
        const Real fx = map.getValue()[i].baryCoords[0];
        const Real fy = map.getValue()[i].baryCoords[1];
        const Real fz = map.getValue()[i].baryCoords[2];
        int index = map.getValue()[i].in_index;
        const topology::Tetrahedron& tetra = tetrahedra[index];

        //set the modified position
        for (unsigned int vt=0; vt < 4; vt++)
            glVertexPositions[vt][i] =  in[tetra[vt]];
        glPointPositions[i] = in[tetra[0]] * ( 1-fx-fy-fz ) + in[tetra[1]] * fx + in[tetra[2]] * fy + in[tetra[3]] * fz ;
        defaulttype::Rigid3dTypes::setCPos(out[i] , glPointPositions[i] );
    }

    //point running over each DoF (assoc. with frame) in the out vector; get it from the mapOrient[0]
    for (unsigned int point = 0; point < mapOrient[0].getValue().size(); point++)
    {
        int index = mapOrient[0].getValue()[point].in_index;
        const topology::Tetrahedron& tetra = tetrahedra[index];
        Matrix3 orientationMatrix, polarMatrixQ, polarMatrixS; // orthogMatrix
        Matrix3 m,basis;
        m[0] = in[tetra[1]]-in[tetra[0]];
        m[1] = in[tetra[2]]-in[tetra[0]];
        m[2] = in[tetra[3]]-in[tetra[0]];
        basis.transpose ( m );

        for (unsigned int dir = 0; dir < 3; dir++)   //go through the three maps
        {
            Vector3 inGlobal;
            inGlobal[0] = mapOrient[dir].getValue()[point].baryCoords[0];
            inGlobal[1] = mapOrient[dir].getValue()[point].baryCoords[1];
            inGlobal[2] = mapOrient[dir].getValue()[point].baryCoords[2];

            orientationMatrix[dir]= basis*inGlobal;
        }

        orientationMatrix.transpose();
        polar_decomp(orientationMatrix, polarMatrixQ, polarMatrixS);
        Quat quatA;
        quatA.fromMatrix(polarMatrixQ);
        defaulttype::Rigid3dTypes::setCRot(out[point], quatA);

        if (forceField != 0)
        {
            Matrix3 tetraOrientation = forceField->getActualTetraRotation(index);
            Quat tetraOrientQuat, tetraOrientDiffQuat;
            tetraOrientQuat.fromMatrix(tetraOrientation);
            tetraOrientDiffQuat = tetraOrientQuat.quatDiff(tetraOrientQuat, prevTetraRotation[point]);
            Vector3 tetraOrientDiffEuler = tetraOrientDiffQuat.toEulerVector();
            prevTetraRotation[point] = tetraOrientQuat;

#ifdef SOFA_IP_TRACES
            IPTR_BARCPP_APPLY("Matrix ["<<index<<"] orientation quaternion: " << tetraOrientation << endl);
            IPTR_BARCPP_APPLY("Tetrahedron ["<<index<<"] orientation quaternion: " << tetraOrientQuat << endl);
            IPTR_BARCPP_APPLY("previous position: " << prevTetraRotation[point] << endl);
            IPTR_BARCPP_APPLY("Euler difference: " << tetraOrientDiffEuler << endl);
#endif

        }

#ifdef SOFA_IP_TRACES
        IPTR_BARCPP_APPLY("orientation Matrix for point["<<point<<"] in updated frame = " << orientationMatrix << endl);
        IPTR_BARCPP_APPLY("polarQ Matrix for point["<<point<<"] in updated frame = " << polarMatrixQ << endl);
        IPTR_BARCPP_APPLY("Quaternion: " << quatA << endl);
#endif




        //BRUTE force for numerical approximation of the jacobian for rotation part
        //pertube each component of each vertex and compute the rotation
        /*Real delta = 0.001;
        for (unsigned int v = 0; v < 4; v++) {
            for (unsigned int dim = 0; dim < 3; dim++) {
                Vector3 pertVector;
                pertVector[dim] = delta;
                inCopy[tetra[v]] += pertVector;
                m[0] = inCopy[tetra[1]]-inCopy[tetra[0]];
                m[1] = inCopy[tetra[2]]-inCopy[tetra[0]];
                m[2] = inCopy[tetra[3]]-inCopy[tetra[0]];
                basis.transpose ( m );

                inCopy[tetra[v]] -= pertVector;
                for (unsigned int dir = 0; dir < 3; dir++) { //go through the three maps
                    Vector3 inGlobal;
                    inGlobal[0] = mapOrient[dir].getValue()[point].baryCoords[0];
                    inGlobal[1] = mapOrient[dir].getValue()[point].baryCoords[1];
                    inGlobal[2] = mapOrient[dir].getValue()[point].baryCoords[2];

                    orientationMatrix[dir]= basis*inGlobal;
                }
                orientationMatrix.transpose();
                polar_decomp(orientationMatrix, polarMatrixQ, polarMatrixS);
                quatA.fromMatrix(polarMatrixQ);
            }
        }*/
    }


} //apply

template <>
void BarycentricMapperTetrahedronSetTopology<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>::applyJT ( defaulttype::Vec3dTypes::VecDeriv& out, const defaulttype::Rigid3dTypes::VecDeriv& in )
{
    const sofa::helper::vector<topology::Tetrahedron>& tetrahedra = this->topology->getTetrahedra();
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_APPLYJT( "BarycentricMapperTetrahedronSetTopology::applyJT BEGIN " << endl);
    //unsigned int nbTetra = this->topology->getTetrahedra().size();
    //IPTR_BARCPP_APPLYJT( "  number of tetrahedra = " << nbTetra );
#endif
    if ((!maskTo)||(maskTo&& !(maskTo->isInUse())) )
    {
        maskFrom->setInUse(false);
        for ( unsigned int i=0; i<map.getValue().size(); i++ )
        {
            //get velocity of the DoF
            const defaulttype::Rigid3dTypes::DPos v = defaulttype::Rigid3dTypes::getDPos(in[i]);
            const defaulttype::Rigid3dTypes::DRot torque = defaulttype::Rigid3dTypes::getDRot(in[i]);
            defaulttype::Rigid3dTypes::DRot torqueNormalized = torque;
            torqueNormalized.normalize();
            Real torqueMagnitude = torque.norm();

            //get its coordinated wrt to the associated tetra with given index
            const OutReal fx = ( OutReal ) map.getValue()[i].baryCoords[0];
            const OutReal fy = ( OutReal ) map.getValue()[i].baryCoords[1];
            const OutReal fz = ( OutReal ) map.getValue()[i].baryCoords[2];
            int index = map.getValue()[i].in_index;
            const topology::Tetrahedron& tetra = tetrahedra[index];

            out[tetra[0]] += v * ( 1-fx-fy-fz );
            out[tetra[1]] += v * fx;
            out[tetra[2]] += v * fy;
            out[tetra[3]] += v * fz;

            //compute the linear forces for each vertex from the torque


#ifdef SOFA_IP_TRACES
            IPNTR_BARCPP_APPLYJT(v*(1-fx - fy - fz) << endl);
            IPNTR_BARCPP_APPLYJT(v*fx << endl);
            IPNTR_BARCPP_APPLYJT(v*fy << endl);
            IPNTR_BARCPP_APPLYJT(v*fz << endl);
            IPTR_BARCPP_APPLYJT("tetra[" << index << "]: " << glVertexPositions[0][i] << "; "
                    << glVertexPositions[1][i] << "; "
                    << glVertexPositions[2][i] << "; "
                    << glVertexPositions[3][i] << endl);
            IPTR_BARCPP_APPLYJT("torque = " << torque  <<  " |"<<torqueMagnitude<<"| in point = " << glPointPositions[i] << endl);
#endif
            if (torqueMagnitude > 10e-4)
            {
                for (unsigned int vt = 0; vt < 4; vt++)
                {
                    //compute the normal from vertex to the axis of rotation
                    /*Real numer = 0, denom = torque.norm();
                    IPTR_BARCPP_APPLYJT("   vertex["<<vt<<"]: " << glVertexPositions[vt][i] << endl);
                    for (unsigned int dim = 0; dim < 3; dim++)
                        numer += torque[dim] * (glPointPositions[i][dim]-glVertexPositions[vt][i][dim]);
                    Real parT = (denom != 0) ? -numer/denom : 0;
                    IPTR_BARCPP_APPLYJT("       nomin = " << numer << "  denomin = " << denom << "   parT = " << parT << endl);
                    Vector3 normalToAxis = glVertexPositions[vt][i] - (glPointPositions[i] + torque*parT);*/

                    Vector3 vectorRigidVertex, vectorRigidVertexNormalized, forceDirection(0, 0, 0);
                    vectorRigidVertex =  glVertexPositions[vt][i] - glPointPositions[i];  //distance RIGID-vertex
                    vectorRigidVertexNormalized = vectorRigidVertex;
                    vectorRigidVertexNormalized.normalize();
                    Real forceSize = 0 , theta = 0;
                    Real distRigidVertex = vectorRigidVertex.norm();
                    bool toApply = false;
                    //IPTR_BARCPP_APPLYJT("       normal to axis = " << normalToAxis << " norm = " << normalToAxis.norm() << endl);

                    //compute the force direction
                    forceDirection = cross(torque, vectorRigidVertex);
                    //forceDirection.normalize();
                    theta = acos(dot(torqueNormalized, vectorRigidVertexNormalized));
                    Real denom = (distRigidVertex*sin(theta));
                    denom *= denom;
                    if ( denom >  10e-5)
                        toApply = true;

                    forceDirection *= (1/denom);
                    forceSize = forceDirection.norm(); // torqueMagnitude / denom;
                    //forceDirection = forceDirection * forceSize;

#ifdef SOFA_IP_TRACES
                    IPTR_BARCPP_APPLYJT("distance: " << vectorRigidVertex.norm()<< endl);
                    IPTR_BARCPP_APPLYJT("theta = " << theta << "  forceSize = " << forceSize << endl);
                    if (toApply)
                    {
                        IPTR_BARCPP_APPLYJT("adding linear force: " << forceDirection << endl);
                    }
                    else
                    {
                        IPTR_BARCPP_APPLYJT("not adding linear force: " << forceDirection << endl);
                    }
#endif
                    if (toApply)
                    {
                        out[tetra[vt]] += forceDirection;
                    }
                }
            }


        }
    }
}  //applyJT

template <>
void BarycentricMapperTetrahedronSetTopology<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>::applyJ (defaulttype::Rigid3dTypes::VecDeriv& out, const defaulttype::Vec3dTypes::VecDeriv& in )
{
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_APPLYJ( "BarycentricMapperTetrahedronSetTopology::applyJ BEGIN " << endl);
    IPTR_BARCPP_APPLYJ( "Velocities: " << in << endl);
#endif
    out.resize ( map.getValue().size() );
    const sofa::helper::vector<topology::Tetrahedron>& tetrahedra = this->topology->getTetrahedra();


    if ((!maskTo)||(maskTo&& !(maskTo->isInUse())) )
    {
        for ( unsigned int i=0; i<map.getValue().size(); i++ )
        {
            const Real fx = map.getValue()[i].baryCoords[0];
            const Real fy = map.getValue()[i].baryCoords[1];
            const Real fz = map.getValue()[i].baryCoords[2];
            int index = map.getValue()[i].in_index;
            const topology::Tetrahedron& tetra = tetrahedra[index];
            defaulttype::Rigid3dTypes::setDPos(out[i] , in[tetra[0]] * ( 1-fx-fy-fz )
                    + in[tetra[1]] * fx
                    + in[tetra[2]] * fy
                    + in[tetra[3]] * fz );
        }
    }
    else
    {
        typedef core::componentmodel::behavior::BaseMechanicalState::ParticleMask ParticleMask;
        const ParticleMask::InternalStorage &indices=maskTo->getEntries();


        ParticleMask::InternalStorage::const_iterator it;
        for (it=indices.begin(); it!=indices.end(); it++)
        {
            const int i=(int)(*it);
            const Real fx = map.getValue()[i].baryCoords[0];
            const Real fy = map.getValue()[i].baryCoords[1];
            const Real fz = map.getValue()[i].baryCoords[2];
            int index = map.getValue()[i].in_index;
            const topology::Tetrahedron& tetra = tetrahedra[index];
            defaulttype::Rigid3dTypes::setDPos(out[i] , in[tetra[0]] * ( 1-fx-fy-fz )
                    + in[tetra[1]] * fx
                    + in[tetra[2]] * fy
                    + in[tetra[3]] * fz );
        }
    }
#ifdef SOFA_IP_TRACES
    IPTR_BARCPP_APPLYJ( "BarycentricMapperTetrahedronSetTopology::applyJ END " << endl);
#endif
} //applyJ




#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Rigid3dTypes> > >;
//template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Rigid3dTypes> > >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, Rigid3dTypes >;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Rigid3fTypes> > >;
//template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Rigid3fTypes> > >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, Rigid3fTypes >;
#endif
#ifndef SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Rigid3fTypes> > >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Rigid3dTypes> > >;
//template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Rigid3fTypes> > >;
//template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Rigid3dTypes> > >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, Rigid3dTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, Rigid3fTypes >;
template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, Rigid3dTypes >;
#endif
#endif



} // namespace mapping

} // namespace component

} // namespace sofa

