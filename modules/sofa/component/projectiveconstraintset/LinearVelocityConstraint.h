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
#ifndef SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_LINEARVELOCITYCONSTRAINT_H
#define SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_LINEARVELOCITYCONSTRAINT_H

#include <sofa/core/behavior/ProjectiveConstraintSet.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/BaseVector.h>
#include <sofa/helper/vector.h>
#include <sofa/component/topology/PointSubset.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace component
{

namespace projectiveconstraintset
{

/** impose a motion to given DOFs (translation and rotation)
	The motion between 2 key times is linearly interpolated
*/
template <class TDataTypes>
class LinearVelocityConstraint : public core::behavior::ProjectiveConstraintSet<TDataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(LinearVelocityConstraint,TDataTypes),SOFA_TEMPLATE(core::behavior::ProjectiveConstraintSet,TDataTypes));

    typedef TDataTypes DataTypes;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::MatrixDeriv MatrixDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef typename MatrixDeriv::RowType MatrixDerivRowType;
    typedef Data<VecCoord> DataVecCoord;
    typedef Data<VecDeriv> DataVecDeriv;
    typedef Data<MatrixDeriv> DataMatrixDeriv;
    typedef topology::PointSubset SetIndex;
    typedef helper::vector<unsigned int> SetIndexArray;

public :
    /// indices of the DOFs the constraint is applied to
    Data<SetIndex> m_indices;
    /// the key frames when the motion is defined by the user
    Data<helper::vector<Real> > m_keyTimes;
    /// the motions corresponding to the key frames
    Data<VecDeriv > m_keyVelocities;
    /// the coordinates on which to applay velocities
    Data<SetIndex> m_coordinates;

    /// the key times surrounding the current simulation time (for interpolation)
    Real prevT, nextT;
    ///the velocities corresponding to the surrouding key times
    Deriv prevV, nextV;
    ///position at the previous step for constrained DOFs position
    VecCoord x0;

    LinearVelocityConstraint();

    virtual ~LinearVelocityConstraint();

    ///methods to add/remove some indices, keyTimes, keyVelocity
    void clearIndices();
    void addIndex(unsigned int index);
    void removeIndex(unsigned int index);
    void clearKeyVelocities();
    /**add a new key movement
    @param time : the simulation time you want to set a movement (in sec)
    @param movement : the corresponding motion
    for instance, addKeyMovement(1.0, Deriv(5,0,0) ) will set a translation of 5 in x direction a time 1.0s
    **/
    void addKeyVelocity(Real time, Deriv movement);


    /// -- Constraint interface
    void init();
    void projectResponse(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& resData);
    void projectVelocity(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& vData);
    void projectPosition(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecCoord& xData);
    void projectJacobianMatrix(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataMatrixDeriv& cData);

    /// Handle topological changes
    virtual void handleTopologyChange();

    virtual void draw();

protected:

    sofa::core::topology::BaseMeshTopology* topology;

    /// Define TestNewPointFunction (for topology changes)
    static bool FCTestNewPointFunction(int, void*, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >& );

    /// Define RemovalFunction (for topology changes)
    static void FCRemovalFunction ( int , void*);
};


#if defined(WIN32) && !defined(SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_LINEARVELOCITYCONSTRAINT_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec2dTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec1dTypes>;
//extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec6dTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Rigid3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec2fTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec1fTypes>;
//extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Vec6fTypes>;
extern template class SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_API LinearVelocityConstraint<defaulttype::Rigid3fTypes>;
#endif
#endif


} // namespace projectiveconstraintset

} // namespace component

} // namespace sofa


#endif


