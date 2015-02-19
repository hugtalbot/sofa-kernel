/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_SKELETALMOTIONCONSTRAINT_INL
#define SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_SKELETALMOTIONCONSTRAINT_INL

#include <sofa/component/projectiveconstraintset/SkeletalMotionConstraint.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/behavior/ProjectiveConstraintSet.inl>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/helper/gl/template.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/component/topology/TopologySubsetData.inl>

#include <iostream>

namespace sofa
{

namespace component
{

namespace projectiveconstraintset
{

using namespace core::topology;

using namespace sofa::defaulttype;
using namespace sofa::helper;
using namespace sofa::core::behavior;

template <class DataTypes>
SkeletalMotionConstraint<DataTypes>::SkeletalMotionConstraint() : ProjectiveConstraintSet<DataTypes>()
    , skeletonJoints(initData(&skeletonJoints, "joints", "skeleton joints"))
    , skeletonBones(initData(&skeletonBones, "bones", "skeleton bones"))
	, animationSpeed(initData(&animationSpeed, 1.0f, "animationSpeed", "animation speed"))
    , active(initData(&active, true, "active", "is the constraint active?"))
{
    
}

template <class DataTypes>
SkeletalMotionConstraint<DataTypes>::~SkeletalMotionConstraint()
{
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::init()
{
    nextPositions.resize(skeletonBones.getValue().size());
    ProjectiveConstraintSet<DataTypes>::init();
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::reset()
{
    ProjectiveConstraintSet<DataTypes>::reset();
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::findKeyTimes()
{
    //Note: works only if the times are sorted

    Real cT = (Real) this->getContext()->getTime() * animationSpeed.getValue();
    
    finished = false;
    for(unsigned int i = 0; i < skeletonJoints.getValue().size(); ++i)
    {
        SkeletonJoint<DataTypes>& skeletonJoint = (*skeletonJoints.beginEdit())[i];
        

        if(skeletonJoint.mChannels.empty() || skeletonJoint.mChannels.size() != skeletonJoint.mTimes.size())
            continue;

        for(unsigned int j = 0; j < skeletonJoint.mTimes.size(); ++j)
        {
            Real keyTime = (Real) skeletonJoint.mTimes[j];
            if(keyTime <= cT)
            {
                //if(skeletonJoint.mPreviousMotionTime < keyTime)
                {
                    skeletonJoint.mPreviousMotionTime = keyTime;
                    const RigidCoord<3, Real>& motion = skeletonJoint.mChannels[j];
                    skeletonJoint.mPreviousMotion = motion;
                }
                if(prevT < keyTime)
                    prevT = keyTime;
            }
            else
            {
                //if(skeletonJoint.mNextMotionTime > keyTime)
                {
                    skeletonJoint.mNextMotionTime = keyTime;
                    const RigidCoord<3, Real>& motion = skeletonJoint.mChannels[j];
                    skeletonJoint.mNextMotion = motion;
                }
                if(nextT > keyTime)
                    nextT = keyTime;

                finished = true;
                break; 
            }

        }

		skeletonJoints.endEdit();

        
    }
}

template <class DataTypes>
template <class DataDeriv>
void SkeletalMotionConstraint<DataTypes>::projectResponseT(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataDeriv& dx)
{
    if( !active.getValue() ) return;

    for(unsigned int i = 0; i < dx.size(); ++i)
        dx[i] = Deriv();
    /*Real cT = (Real) this->getContext()->getTime() * animationSpeed.getValue();

    if(0.0 != cT)
    {
        findKeyTimes();

        if(finished && nextT != prevT)
        {
            //set the motion to the Dofs
            for(unsigned int i = 0; i < dx.size(); ++i)
                dx[i] = Deriv();
        }
    }*/
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::projectResponse(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& resData)
{
    if( !active.getValue() ) return;

    helper::WriteAccessor<DataVecDeriv> res = resData;
    projectResponseT<VecDeriv>(mparams /* PARAMS FIRST */, res.wref());
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::projectVelocity(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& vData)
{
    if( !active.getValue() ) return;

    helper::WriteAccessor<DataVecDeriv> dx = vData;
    helper::ReadAccessor<DataVecCoord> x =((MechanicalObject*)this->getContext()->getMechanicalState())->readPositions();
    Real cT = (Real) this->getContext()->getTime() * animationSpeed.getValue();
    Real dt = (Real) this->getContext()->getDt();

    if(0.0 != cT)
    {
        findKeyTimes();
        if(finished)
        {
            // compute the position of the bones at cT + dt
            this->interpolatePosition<Coord>(cT+dt, nextPositions);
            // compute the velocity using finite difference
            for (unsigned i=0; i<nextPositions.size(); i++)
            {
                dx[i] = DataTypes::coordDifference(nextPositions[i], x[i])/dt;
//                 dx[i].getVCenter() = (nextPositions[i].getCenter() - x[i].getCenter())/dt;
//                 helper::Quater<SReal> tmp;
//                 Vec<3, Real> diff = tmp.angularDisplacement( nextPositions[i].getOrientation(), x[i].getOrientation());
//                 dx[i].getVOrientation() = diff/dt;
            }

        }
        else
        {
            for(unsigned int i = 0; i < dx.size(); ++i)
                dx[i] = Deriv();
        }
    }
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::projectPosition(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecCoord& xData)
{
    if( !active.getValue() ) return;

    helper::WriteAccessor<DataVecCoord> x = xData;
    Real cT = (Real) this->getContext()->getTime() * animationSpeed.getValue();

    if(0.0 != cT)
    {
        findKeyTimes();

        // if we found 2 keyTimes, we have to interpolate a velocity (linear interpolation)
        interpolatePosition<Coord>(cT, x.wref());
    }
}

template <class DataTypes>
template <class MyCoord>
void SkeletalMotionConstraint<DataTypes>::interpolatePosition(Real cT, typename boost::enable_if<boost::is_same<MyCoord, RigidCoord<3, Real> >, VecCoord>::type& x)
{
    // set the motion to the SkeletonJoint corresponding rigid
    
    if(finished)    
        for(unsigned int i = 0; i < skeletonJoints.getValue().size(); ++i)
        {
            
            SkeletonJoint<DataTypes>& skeletonJoint = (*skeletonJoints.beginEdit())[i];
            if(  skeletonJoint.mPreviousMotionTime !=  skeletonJoint.mNextMotionTime)
            {
                Real dt = (cT - skeletonJoint.mPreviousMotionTime) / (skeletonJoint.mNextMotionTime - skeletonJoint.mPreviousMotionTime);

                const helper::vector<RigidCoord<3, Real> >& channels = skeletonJoint.mChannels;

                if(channels.empty())
                    continue;

                skeletonJoint.mLocalRigid.getCenter() = skeletonJoint.mPreviousMotion.getCenter() + (skeletonJoint.mNextMotion.getCenter() - skeletonJoint.mPreviousMotion.getCenter()) * dt;
                skeletonJoint.mLocalRigid.getOrientation().slerp(skeletonJoint.mPreviousMotion.getOrientation(), skeletonJoint.mNextMotion.getOrientation(), (float) dt, true);

                skeletonJoints.endEdit();
            }
            else
            {
                const helper::vector<RigidCoord<3, Real> >& channels = skeletonJoint.mChannels;

                if(channels.empty())
                    continue;

                skeletonJoint.mLocalRigid.getCenter() = skeletonJoint.mNextMotion.getCenter();
                skeletonJoint.mLocalRigid.getOrientation() = skeletonJoint.mNextMotion.getOrientation();

                skeletonJoints.endEdit();
            }
        }
    else
    {
        for(unsigned int i = 0; i < skeletonJoints.getValue().size(); ++i)
        {
            SkeletonJoint<DataTypes>& skeletonJoint = (*skeletonJoints.beginEdit())[i];

            const helper::vector<RigidCoord<3, Real> >& channels = skeletonJoint.mChannels;

            if(channels.empty())
                continue;

            skeletonJoint.mLocalRigid.getCenter() = skeletonJoint.mNextMotion.getCenter();
            skeletonJoint.mLocalRigid.getOrientation() = skeletonJoint.mNextMotion.getOrientation();

			skeletonJoints.endEdit();
        }
    }

    // apply the final transformation from skeletonBones to dofs here
    localToGlobal<Coord>(x);
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::projectJacobianMatrix(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataMatrixDeriv& cData)
{
    if( !active.getValue() ) return;

    helper::WriteAccessor<DataMatrixDeriv> c = cData;

    MatrixDerivRowIterator rowIt = c->begin();
    MatrixDerivRowIterator rowItEnd = c->end();

    while(rowIt != rowItEnd)
    {
        projectResponseT<MatrixDerivRowType>(mparams /* PARAMS FIRST */, rowIt.row());
        ++rowIt;
    }
}

template <class DataTypes>
template <class MyCoord>
void SkeletalMotionConstraint<DataTypes>::localToGlobal(typename boost::enable_if<boost::is_same<MyCoord, RigidCoord<3, Real> >, VecCoord>::type& x)
{
    for(unsigned int i = 0; i < skeletonJoints.getValue().size(); ++i)
    {
        SkeletonJoint<DataTypes>& skeletonJoint = (*skeletonJoints.beginEdit())[i];

        RigidCoord< 3, Real> worldRigid = skeletonJoint.mLocalRigid;

        // break if the parent joint is the root
        for(int parentIndex = skeletonJoint.mParentIndex; -1 != parentIndex; parentIndex = skeletonJoints.getValue()[parentIndex].mParentIndex)
        {
            RigidCoord< 3, Real> parentLocalRigid = skeletonJoints.getValue()[parentIndex].mLocalRigid;
            worldRigid = parentLocalRigid.mult(worldRigid);
        }

        skeletonJoint.mWorldRigid = worldRigid;

		skeletonJoints.endEdit();
    }

    for(unsigned int i = 0; i < skeletonBones.getValue().size(); ++i)
        x[i] = skeletonJoints.getValue()[skeletonBones.getValue()[i]].mWorldRigid;
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::setSkeletalMotion(const helper::vector<SkeletonJoint<DataTypes> >& skeletonJoints, const helper::vector<SkeletonBone>& skeletonBones)
{
    this->skeletonJoints.setValue(skeletonJoints);
    this->skeletonBones.setValue(skeletonBones);
    this->init();
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::addChannel(unsigned int jointIndex , Coord channel, double time)
{
	(*skeletonJoints.beginEdit())[jointIndex].addChannel(channel, time);
	skeletonJoints.endEdit();
}

// Matrix Integration interface
template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::applyConstraint(defaulttype::BaseMatrix * /*mat*/, unsigned int /*offset*/)
{
    if( !active.getValue() ) return;

    //sout << "applyConstraint in Matrix with offset = " << offset << sendl;
    /*const unsigned int N = Deriv::size();
    const SetIndexArray & indices = m_indices.getValue();

    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
    {
    	// Reset Fixed Row and Col
    	for (unsigned int c=0;c<N;++c)
    		mat->clearRowCol(offset + N * (*it) + c);
    	// Set Fixed Vertex
    	for (unsigned int c=0;c<N;++c)
    		mat->set(offset + N * (*it) + c, offset + N * (*it) + c, 1.0);
    }*/
}

template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::applyConstraint(defaulttype::BaseVector * /*vect*/, unsigned int /*offset*/)
{
    if( !active.getValue() ) return;

    //sout << "applyConstraint in Vector with offset = " << offset << sendl;
    /*const unsigned int N = Deriv::size();

    const SetIndexArray & indices = m_indices.getValue();
    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
    {
    	for (unsigned int c=0;c<N;++c)
    		vect->clear(offset + N * (*it) + c);
    }*/
}

// display the paths the constrained dofs will go through
template <class DataTypes>
void SkeletalMotionConstraint<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    if( !active.getValue() ) return;

    if (!vparams->displayFlags().getShowBehaviorModels())
        return;

    sofa::helper::vector<Vector3> points;
    sofa::helper::vector<Vector3> linesX;
    sofa::helper::vector<Vector3> linesY;
    sofa::helper::vector<Vector3> linesZ;
    sofa::helper::vector<Vector3> colorFalloff;

    Vector3 point;
    Vector3 line;

    // draw joints (not bones we draw them differently later)
    {
        for(unsigned int i = 0; i < skeletonJoints.getValue().size(); ++i)
        {
            RigidCoord< 3, Real> jointWorldRigid = skeletonJoints.getValue()[i].mWorldRigid;

            unsigned int j;
            for(j = 0; j < skeletonBones.getValue().size(); ++j)
                if((int)i == skeletonBones.getValue()[j])
                    break;

            if(skeletonBones.getValue().size() != j)
                continue;

            point = DataTypes::getCPos(jointWorldRigid);
            points.push_back(point);

            linesX.push_back(point);
            line = point + DataTypes::getCRot(jointWorldRigid).rotate(Vec3f(0.1f, 0.0f, 0.0f));
            linesX.push_back(line);

            linesY.push_back(point);
            line = point + DataTypes::getCRot(jointWorldRigid).rotate(Vec3f(0.0f, 0.1f, 0.0f));
            linesY.push_back(line);

            linesZ.push_back(point);
            line = point + DataTypes::getCRot(jointWorldRigid).rotate(Vec3f(0.0f, 0.0f, 0.1f));
            linesZ.push_back(line);
        }
        vparams->drawTool()->drawPoints(points, 10, Vec<4, float> (1.0f , 0.5f , 0.5f , 1.0f));
        vparams->drawTool()->drawLines (linesX,  2, Vec<4, float> (0.75f, 0.0f , 0.0f , 1.0f));
        vparams->drawTool()->drawLines (linesY,  2, Vec<4, float> (0.0f , 0.75f, 0.0f , 1.0f));
        vparams->drawTool()->drawLines (linesZ,  2, Vec<4, float> (0.0f , 0.0f , 0.75f, 1.0f));
    }

    points.clear();
    linesX.clear();
    linesY.clear();
    linesZ.clear();

    // draw bones now
    {
        for(unsigned int i = 0; i < skeletonBones.getValue().size(); ++i)
        {
            RigidCoord< 3, Real> boneWorldRigid = skeletonJoints.getValue()[skeletonBones.getValue()[i]].mWorldRigid;

            point = DataTypes::getCPos(boneWorldRigid);
            points.push_back(point);

            linesX.push_back(point);
            line = point + DataTypes::getCRot(boneWorldRigid).rotate(Vec3f(0.1f, 0.0f, 0.0f));
            linesX.push_back(line);

            linesY.push_back(point);
            line = point + DataTypes::getCRot(boneWorldRigid).rotate(Vec3f(0.0f, 0.1f, 0.0f));
            linesY.push_back(line);

            linesZ.push_back(point);
            line = point + DataTypes::getCRot(boneWorldRigid).rotate(Vec3f(0.0f, 0.0f, 0.1f));
            linesZ.push_back(line);
        }
        vparams->drawTool()->drawPoints(points, 10, Vec<4, float> (1.0f, 0.5f, 0.5f, 1.0f));
        vparams->drawTool()->drawLines (linesX, 2 , Vec<4, float> (1.0f, 0.0f, 0.0f, 1.0f));
        vparams->drawTool()->drawLines (linesY, 2 , Vec<4, float> (0.0f, 1.0f, 0.0f, 1.0f));
        vparams->drawTool()->drawLines (linesZ, 2 , Vec<4, float> (0.0f, 0.0f, 1.0f, 1.0f));
    }
}

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
