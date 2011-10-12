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
#ifndef SOFA_CORE_BEHAVIOR_CONSTRAINTSOLVER_H
#define SOFA_CORE_BEHAVIOR_CONSTRAINTSOLVER_H

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/behavior/BaseConstraintSet.h>
#include <sofa/core/behavior/BaseMechanicalState.h>

namespace sofa
{

namespace core
{

namespace behavior
{

/**
 *  \brief Component responsible for the expression and solution of system of equations related to constraints
 *
 */
class SOFA_CORE_API ConstraintSolver : public virtual objectmodel::BaseObject
{
public:

    SOFA_ABSTRACT_CLASS(ConstraintSolver, objectmodel::BaseObject);
protected:
    ConstraintSolver();

    virtual ~ConstraintSolver();
public:
    /**
     * Launch the sequence of operations in order to solve the constraints
     */
    virtual void solveConstraint(const ConstraintParams *, MultiVecId res1, MultiVecId res2=MultiVecId::null());

    /**
     * Do the precomputation: compute free state, or propagate the states to the mapped mechanical states, where the constraint can be expressed
     */
    virtual bool prepareStates(const ConstraintParams *, MultiVecId res1, MultiVecId res2=MultiVecId::null())=0;

    /**
     * Create the system corresponding to the constraints
     */
    virtual bool buildSystem(const ConstraintParams *, MultiVecId res1, MultiVecId res2=MultiVecId::null())=0;

    /**
     * Use the system previously built and solve it with the appropriate algorithm
     */
    virtual bool solveSystem(const ConstraintParams *, MultiVecId res1, MultiVecId res2=MultiVecId::null())=0;

    /**
     * Correct the Mechanical State with the solution found
     */
    virtual bool applyCorrection(const ConstraintParams *, MultiVecId res1, MultiVecId res2=MultiVecId::null())=0;


    /// @name Resolution DOFs vectors API
    /// @{

    VecDerivId getForce() const
    {
        return m_fId;
    }

    void setForce(VecDerivId id)
    {
        m_fId = id;
    }

    VecDerivId getDx() const
    {
        return m_dxId;
    }

    void setDx(VecDerivId id)
    {
        m_dxId = id;
    }

    /// @}

protected:

    VecDerivId m_fId;
    VecDerivId m_dxId;
};

} // namespace behavior

} // namespace core

} // namespace sofa

#endif
