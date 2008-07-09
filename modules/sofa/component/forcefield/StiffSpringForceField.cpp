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
// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/forcefield/StiffSpringForceField.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;


SOFA_DECL_CLASS(StiffSpringForceField)

// Register in the Factory
int StiffSpringForceFieldClass = core::RegisterObject("Stiff springs for implicit integration")
#ifndef SOFA_FLOAT
        .add< StiffSpringForceField<Vec3dTypes> >()
        .add< StiffSpringForceField<Vec2dTypes> >()
        .add< StiffSpringForceField<Vec1dTypes> >()
        .add< StiffSpringForceField<Vec6dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< StiffSpringForceField<Vec3fTypes> >()
        .add< StiffSpringForceField<Vec2fTypes> >()
        .add< StiffSpringForceField<Vec1fTypes> >()
        .add< StiffSpringForceField<Vec6fTypes> >()
#endif
        ;
#ifndef SOFA_FLOAT
template class StiffSpringForceField<Vec3dTypes>;
template class StiffSpringForceField<Vec2dTypes>;
template class StiffSpringForceField<Vec1dTypes>;
template class StiffSpringForceField<Vec6dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class StiffSpringForceField<Vec3fTypes>;
template class StiffSpringForceField<Vec2fTypes>;
template class StiffSpringForceField<Vec1fTypes>;
template class StiffSpringForceField<Vec6fTypes>;
#endif


} // namespace forcefield

} // namespace component

} // namespace sofa

