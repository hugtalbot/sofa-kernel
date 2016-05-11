/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_MASS_UNIFORMMASS_INL
#define SOFA_COMPONENT_MASS_UNIFORMMASS_INL

#include <SofaBaseMechanics/UniformMass.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/topology/Topology.h>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/helper/accessor.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/DataTypeInfo.h>
#include <SofaBaseMechanics/AddMToMatrixFunctor.h>
#include <sofa/simulation/common/Simulation.h>
#include <iostream>
#include <string.h>

#ifdef SOFA_SUPPORT_MOVING_FRAMES
#include <sofa/core/behavior/InertiaForce.h>
#endif



namespace sofa
{

namespace component
{

namespace mass
{

using helper::WriteAccessor;
using helper::ReadAccessor;
using helper::WriteOnlyAccessor;
using helper::vector;

using std::list;

using core::behavior::Mass;
using core::topology::BaseMeshTopology;
using core::topology::TopologyChange;
using core::MechanicalParams;
using core::behavior::MultiMatrixAccessor;
using core::visual::VisualParams;
using core::ConstVecCoordId;

using defaulttype::BaseVector;
using defaulttype::Vec;
using defaulttype::Vec3d;
using defaulttype::DataTypeInfo;
using defaulttype::BaseMatrix;



template <class DataTypes, class MassType>
UniformMass<DataTypes, MassType>::UniformMass()
    : mass ( initData ( &mass, MassType ( 1.0f ), "mass",
                          "Mass of each particle" ) )

    , totalMass ( initData ( &totalMass, (SReal)0.0, "totalmass",
                               "Sum of the particles' masses" ) )

    , filenameMass ( initData ( &filenameMass, "filename",
                                  "Rigid file to load the mass parameters" ) )

    , showCenterOfGravity ( initData ( &showCenterOfGravity, false, "showGravityCenter",
                                         "display the center of gravity of the system" ) )

    , showAxisSize ( initData ( &showAxisSize, 1.0f, "showAxisSizeFactor",
                                  "factor length of the axis displayed (only used for rigids)" ) )

    , computeMappingInertia ( initData ( &computeMappingInertia, false, "compute_mapping_inertia",
                                           "to be used if the mass is placed under a mapping" ) )

    , showInitialCenterOfGravity ( initData ( &showInitialCenterOfGravity, false, "showInitialCenterOfGravity",
                                                "display the initial center of gravity of the system" ) )

    , showX0 ( initData ( &showX0, false, "showX0",
                            "display the rest positions" ) )

    , localRange ( initData ( &localRange, Vec<2,int> ( -1,-1 ), "localRange",
                                "optional range of local DOF indices. \n"
                              "Any computation involving only indices outside of this range \n"
                              "are discarded (useful for parallelization using mesh partitionning)" ) )

    , d_indices ( initData ( &d_indices, "indices",
                             "optional local DOF indices. Any computation involving only indices outside of this list are discarded" ) )

    , handleTopoChange ( initData ( &handleTopoChange, false, "handleTopoChange",
                                      "The mass and totalMass are recomputed on particles add/remove." ) )

    , preserveTotalMass( initData ( &preserveTotalMass, false, "preserveTotalMass",
                                      "Prevent totalMass from decreasing when removing particles."))
{
    this->addAlias(&totalMass,"totalMass");
    constructor_message();
}


template <class DataTypes, class MassType>
UniformMass<DataTypes, MassType>::~UniformMass()
{}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::setMass ( const MassType& m )
{
    mass.setValue ( m );
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::setTotalMass ( SReal m )
{
    totalMass.setValue ( m );
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::reinit()
{
    WriteAccessor<Data<vector<int> > > indices = d_indices;
    m_doesTopoChangeAffect = false;
    if(mstate==NULL) return;

    //If localRange is set, update indices
    if (localRange.getValue()[0] >= 0
        && localRange.getValue()[1] > 0
        && localRange.getValue()[1] + 1 < (int)mstate->getSize())
    {
        indices.clear();
        for(int i=localRange.getValue()[0]; i<=localRange.getValue()[1]; i++)
            indices.push_back(i);
    }

    //If no given indices
    if(indices.size()==0)
    {
        indices.clear();
        for(int i=0; i<(int)mstate->getSize(); i++)
            indices.push_back(i);
        m_doesTopoChangeAffect = true;
    }

    //Update mass and totalMass
    if (totalMass.getValue() > 0)
    {
        MassType *m = mass.beginEdit();
        *m = ( ( typename DataTypes::Real ) totalMass.getValue() / indices.size() );
        mass.endEdit();
    }
    else
        totalMass.setValue ( indices.size() * (Real)mass.getValue() );

}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::init()
{
    loadRigidMass ( filenameMass.getFullPath() );
    if ( filenameMass.getValue().empty() )
        filenameMass.setDisplayed ( false );
    Mass<DataTypes>::init();
    reinit();
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::handleTopologyChange()
{
    BaseMeshTopology *meshTopology = getContext()->getMeshTopology();
    WriteAccessor<Data<vector<int> > > indices = d_indices;

    if(m_doesTopoChangeAffect)
    {
        indices.clear();
        for(size_t i=0; i<mstate->getSize(); i++)
            indices.push_back(i);
    }

    if ( meshTopology != 0 )
    {
        list< const TopologyChange * >::const_iterator it = meshTopology->beginChange();
        list< const TopologyChange * >::const_iterator itEnd = meshTopology->endChange();

        while ( it != itEnd )
        {
            switch ( ( *it )->getChangeType() )
            {
            case core::topology::POINTSADDED:
                if ( handleTopoChange.getValue() && m_doesTopoChangeAffect)
                {
                    MassType* m = mass.beginEdit();
                    *m = ( ( typename DataTypes::Real ) totalMass.getValue() / mstate->getSize() );
                    mass.endEdit();
                }
                break;

            case core::topology::POINTSREMOVED:
                if ( handleTopoChange.getValue() && m_doesTopoChangeAffect)
                {
                    if (!preserveTotalMass.getValue())
                        totalMass.setValue (mstate->getSize() * (Real)mass.getValue() );
                    else
                        mass.setValue( static_cast< MassType >( ( typename DataTypes::Real ) totalMass.getValue() / mstate->getSize()) );
                }
                break;

            default:
                break;
            }

            ++it;
        }
    }
}


// -- Mass interface
template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMDx ( const core::MechanicalParams*,
                                                DataVecDeriv& vres,
                                                const DataVecDeriv& vdx,
                                                SReal factor)
{
    helper::WriteAccessor<DataVecDeriv> res = vres;
    helper::ReadAccessor<DataVecDeriv> dx = vdx;

    WriteAccessor<Data<vector<int> > > indices = d_indices;

    MassType m = mass.getValue();
    if ( factor != 1.0 )
        m *= ( typename DataTypes::Real ) factor;

    for ( unsigned int i=0; i<indices.size(); i++ )
        res[indices[i]] += dx[indices[i]] * m;
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::accFromF ( const core::MechanicalParams*,
                                                  DataVecDeriv& va,
                                                  const DataVecDeriv& vf )
{
    WriteOnlyAccessor<DataVecDeriv> a = va;
    ReadAccessor<DataVecDeriv> f = vf;

    ReadAccessor<Data<vector<int> > > indices = d_indices;

    MassType m = mass.getValue();
    for ( unsigned int i=0; i<indices.size(); i++ )
        a[indices[i]] = f[indices[i]] / m;
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMDxToVector ( BaseVector * resVect,
                                                        const VecDeriv* dx,
                                                        SReal mFact,
                                                        unsigned int& offset )
{
    SOFA_UNUSED(resVect);
    SOFA_UNUSED(dx);
    SOFA_UNUSED(mFact);
    SOFA_UNUSED(offset);
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addGravityToV(const MechanicalParams* mparams,
                                                     DataVecDeriv& d_v)
{
    if (mparams)
    {
        VecDeriv& v = *d_v.beginEdit();

        const SReal* g = getContext()->getGravity().ptr();
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2] );
        Deriv hg = theGravity * ( Real ) (mparams->dt());
        if ( this->f_printLog.getValue() )
            serr << "UniformMass::addGravityToV hg = "<<theGravity<<"*"<<mparams->dt()<<"="<<hg<<sendl;
        for ( unsigned int i=0; i<v.size(); i++ )
        {
            v[i] += hg;
        }

        d_v.endEdit();
    }
}

template <class DataTypes, class MassType>
#ifdef SOFA_SUPPORT_MAPPED_MASS
void UniformMass<DataTypes, MassType>::addForce ( const core::MechanicalParams* mparams, DataVecDeriv& vf, const DataVecCoord& /*x*/, const DataVecDeriv& /*v*/)
#else
#ifdef SOFA_SUPPORT_MOVING_FRAMES
void UniformMass<DataTypes, MassType>::addForce ( const core::MechanicalParams*, DataVecDeriv& vf, const DataVecCoord& x, const DataVecDeriv& v )
#else
void UniformMass<DataTypes, MassType>::addForce ( const core::MechanicalParams*, DataVecDeriv& vf, const DataVecCoord& /*x*/, const DataVecDeriv& /*v*/ )
#endif
#endif
{
#ifndef SOFA_SUPPORT_MOVING_FRAMES
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if ( this->m_separateGravity.getValue() )
        return;
#endif

    helper::WriteAccessor<DataVecDeriv> f = vf;

    // weight
    const SReal* g = getContext()->getGravity().ptr();
    Deriv theGravity;
    DataTypes::set
    ( theGravity, g[0], g[1], g[2] );
    const MassType& m = mass.getValue();
    Deriv mg = theGravity * m;
    if ( this->f_printLog.getValue() )
        serr<<"UniformMass::addForce, mg = "<<mass<<" * "<<theGravity<<" = "<<mg<<sendl;


#ifdef SOFA_SUPPORT_MOVING_FRAMES
    // velocity-based stuff
    core::objectmodel::BaseContext::SpatialVector vframe = getContext()->getVelocityInWorld();
    core::objectmodel::BaseContext::Vec3 aframe = getContext()->getVelocityBasedLinearAccelerationInWorld() ;

    // project back to local frame
    vframe = getContext()->getPositionInWorld() / vframe;
    aframe = getContext()->getPositionInWorld().backProjectVector ( aframe );
#endif

    ReadAccessor<Data<vector<int> > > indices = d_indices;

    // add weight and inertia force
    if (this->m_separateGravity.getValue()) for ( unsigned int i=0; i<indices.size(); i++ )
    {
#ifdef SOFA_SUPPORT_MOVING_FRAMES
        f[indices[i]] += core::behavior::inertiaForce ( vframe,aframe,m,x[indices[i]],v[indices[i]] );
#endif
    }
    else for ( unsigned int i=0; i<indices.size(); i++ )
    {
#ifdef SOFA_SUPPORT_MOVING_FRAMES
        f[indices[i]] += mg + core::behavior::inertiaForce ( vframe,aframe,m,x[indices[i]],v[indices[i]] );
#else
        f[indices[i]] += mg;
#endif
    }


#ifdef SOFA_SUPPORT_MAPPED_MASS
    if ( compute_mapping_inertia.getValue() )
    {
        helper::ReadAccessor< Data<VecDeriv> > acc = *mparams->readDx(mstate);
        // add inertia force due to acceleration from the motion of the mapping (coriolis type force)
        if ( acc.size() != f.size() )
            return;

        for ( unsigned int i=0; i<f.size(); i++ )
        {
            Deriv coriolis = -acc[i]*m;
            f[i] += coriolis;
        }
    }
#endif
}

template <class DataTypes, class MassType>
SReal UniformMass<DataTypes, MassType>::getKineticEnergy ( const MechanicalParams* params,
                                                           const DataVecDeriv& d_v  ) const
{
    SOFA_UNUSED(params);

    ReadAccessor<DataVecDeriv> v = d_v;
    ReadAccessor<Data<vector<int> > > indices = d_indices;

    SReal e = 0;
    const MassType& m = mass.getValue();

    for ( unsigned int i=0; i<indices.size(); i++ )
        e+= v[indices[i]]*m*v[indices[i]];

    return e/2;
}

template <class DataTypes, class MassType>
SReal UniformMass<DataTypes, MassType>::getPotentialEnergy ( const MechanicalParams* params,
                                                             const DataVecCoord& d_x  ) const
{
    SOFA_UNUSED(params);
    ReadAccessor<DataVecCoord> x = d_x;
    ReadAccessor<Data<vector<int> > > indices = d_indices;

    SReal e = 0;
    const MassType& m = mass.getValue();

    Vec3d g( getContext()->getGravity());
    Deriv gravity;
    DataTypes::set(gravity, g[0], g[1], g[2]);

    Deriv mg = gravity * m;

    for ( unsigned int i=0; i<indices.size(); i++ )
        e -= mg*x[indices[i]];

    return e;
}


// does nothing by default, need to be specialized in .cpp
template <class DataTypes, class MassType>
defaulttype::Vector6
UniformMass<DataTypes, MassType>::getMomentum ( const core::MechanicalParams* params,
                                                const DataVecCoord& d_x,
                                                const DataVecDeriv& d_v  ) const
{
    SOFA_UNUSED(params);
    SOFA_UNUSED(d_x);
    SOFA_UNUSED(d_v);
    return defaulttype::Vector6();
}


/// Add Mass contribution to global Matrix assembling
template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMToMatrix (const MechanicalParams *mparams,
                                                     const MultiMatrixAccessor* matrix)
{
    const MassType& m = mass.getValue();

    const int N = DataTypeInfo<Deriv>::size();

    AddMToMatrixFunctor<Deriv,MassType> calc;
    MultiMatrixAccessor::MatrixRef r = matrix->getMatrix(mstate);

    Real mFactor = (Real)mparams->mFactorIncludingRayleighDamping(this->rayleighMass.getValue());

    ReadAccessor<Data<vector<int> > > indices = d_indices;
    for ( unsigned int i=0; i<indices.size(); i++ )
        calc ( r.matrix, m, r.offset + N*indices[i], mFactor);
}


template <class DataTypes, class MassType>
SReal UniformMass<DataTypes, MassType>::getElementMass ( unsigned int ) const
{
    return ( SReal ) ( mass.getValue() );
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::getElementMass ( unsigned int  index ,
                                                        BaseMatrix *m ) const
{
    SOFA_UNUSED(index);

    static const BaseMatrix::Index dimension = (BaseMatrix::Index) DataTypeInfo<Deriv>::size();
    if ( m->rowSize() != dimension || m->colSize() != dimension )
        m->resize ( dimension, dimension );

    m->clear();
    AddMToMatrixFunctor<Deriv,MassType>() ( m, mass.getValue(), 0, 1 );
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::draw(const VisualParams* vparams)
{
    if ( !vparams->displayFlags().getShowBehaviorModels() )
        return;

    ReadAccessor<VecCoord> x = mstate->read(ConstVecCoordId::position())->getValue();
    ReadAccessor<Data<vector<int> > > indices = d_indices;

    Coord gravityCenter;
    std::vector<  sofa::defaulttype::Vector3 > points;
    for ( unsigned int i=0; i<indices.size(); i++ )
    {
        sofa::defaulttype::Vector3 p;
        p = DataTypes::getCPos(x[indices[i]]);

        points.push_back ( p );
        gravityCenter += x[indices[i]];
    }


#ifdef SOFA_SMP
    static float colorTab[][4]=
    {
        {1.0f,0.0f,0.0f,1.0f},
        {1.0f,1.0f,0.0f,1.0f},
        {0.0f,1.0f,0.0f,1.0f},
        {0.0f,1.0f,1.0f,1.0f},
        {0.0f,0.0f,1.0f,1.0f},
        {0.5f,.5f,.5f,1.0f},
        {0.5f,0.0f,0.0f,1.0f},
        {.5f,.5f,0.0f,1.0f},
        {0.0f,1.0f,0.0f,1.0f},
        {0.0f,1.0f,1.0f,1.0f},
        {0.0f,0.0f,1.0f,1.0f},
        {0.5f,.5f,.5f,1.0f}
    };
    if(vparams->displayFlags().getShowProcessorColor())
    {
        unsigned int proc=Core::Processor::get_current()->get_pid();
        color = colorTab[proc%12];
    }
#endif

    if ( showCenterOfGravity.getValue() )
    {
        gravityCenter /= x.size();
        const sofa::defaulttype::Vec4f color(1.0,1.0,0.0,1.0);

        Real axisSize = showAxisSize.getValue();
        sofa::defaulttype::Vector3 temp;

        for ( unsigned int i=0 ; i<3 ; i++ )
            if(i < Coord::spatial_dimensions )
                temp[i] = gravityCenter[i];

        vparams->drawTool()->drawCross(temp, axisSize, color);
    }
}

template<class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::loadRigidMass ( std::string )
{
    //If the template is not rigid, we hide the Data filenameMass, to avoid confusion.
    filenameMass.setDisplayed ( false );
    mass.setDisplayed ( false );
}

} // namespace mass

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_MASS_UNIFORMMASS_INL
