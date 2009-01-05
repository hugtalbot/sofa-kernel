/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/component/constraint/LagrangianMultiplierAttachConstraint.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/component/base/container/MechanicalObject.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace constraint
{

SOFA_DECL_CLASS(LagrangianMultiplierAttachConstraint)

using namespace sofa::defaulttype;
using namespace sofa::helper;

int LagrangianMultiplierAttachConstraintClass = core::RegisterObject("TODO-LagrangianMultiplierAttachConstraintClass")
#ifndef SOFA_FLOAT
        .add< LagrangianMultiplierAttachConstraint<Vec3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< LagrangianMultiplierAttachConstraint<Vec3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class LagrangianMultiplierAttachConstraint<Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class LagrangianMultiplierAttachConstraint<Vec3fTypes>;
#endif

} // namespace constraint

} // namespace component

} // namespace sofa

