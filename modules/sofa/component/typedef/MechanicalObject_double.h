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
#ifndef SOFA_TYPEDEF_MECHANICAL_DOUBLE_H
#define SOFA_TYPEDEF_MECHANICAL_DOUBLE_H


#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/LaparoscopicRigidTypes.h>
#include <sofa/component/base/container/MechanicalObject.h>


typedef sofa::defaulttype::Vec1dTypes   Particles1d;
typedef Particles1d::VecDeriv           VecDeriv1d;
typedef Particles1d::VecCoord           VecCoord1d;
typedef Particles1d::Deriv              Deriv1d;
typedef Particles1d::Coord              Coord1d;
typedef sofa::defaulttype::Vec2dTypes   Particles2d;
typedef Particles2d::VecDeriv           VecDeriv2d;
typedef Particles2d::VecCoord           VecCoord2d;
typedef Particles2d::Deriv              Deriv2d;
typedef Particles2d::Coord              Coord2d;
typedef sofa::defaulttype::Vec3dTypes   Particles3d;
typedef Particles3d::VecDeriv           VecDeriv3d;
typedef Particles3d::VecCoord           VecCoord3d;
typedef Particles3d::Deriv              Deriv3d;
typedef Particles3d::Coord              Coord3d;
typedef sofa::defaulttype::Vec6dTypes   Particles6d;
typedef Particles6d::VecDeriv           VecDeriv6d;
typedef Particles6d::VecCoord           VecCoord6d;
typedef Particles6d::Deriv              Deriv6d;
typedef Particles6d::Coord              Coord6d;

typedef sofa::defaulttype::Rigid2dTypes Rigid2d;
typedef Rigid2d::VecDeriv               VecDerivRigid2d;
typedef Rigid2d::VecCoord               VecCoordRigid2d;
typedef Rigid2d::Deriv                  DerivRigid2d;
typedef Rigid2d::Coord                  CoordRigid2d;
typedef sofa::defaulttype::Rigid3dTypes Rigid3d;
typedef Rigid3d::VecDeriv               VecDerivRigid3d;
typedef Rigid3d::VecCoord               VecCoordRigid3d;
typedef Rigid3d::Quat                   Quat3d;
typedef Rigid3d::Deriv                  DerivRigid3d;
typedef Rigid3d::Coord                  CoordRigid3d;

//Mechanical Object
//---------------------
//Deformable
typedef sofa::component::MechanicalObject<sofa::defaulttype::Vec1dTypes> MechanicalObject1d;
typedef sofa::component::MechanicalObject<sofa::defaulttype::Vec2dTypes> MechanicalObject2d;
typedef sofa::component::MechanicalObject<sofa::defaulttype::Vec3dTypes> MechanicalObject3d;
typedef sofa::component::MechanicalObject<sofa::defaulttype::Vec6dTypes> MechanicalObject6d;
//---------------------
//Rigid
typedef sofa::component::MechanicalObject<sofa::defaulttype::Rigid2dTypes> MechanicalObjectRigid2d;
typedef sofa::component::MechanicalObject<sofa::defaulttype::Rigid3dTypes> MechanicalObjectRigid3d;
//---------------------
//Laparoscopic
#ifndef SOFA_FLOAT
typedef sofa::component::MechanicalObject<sofa::defaulttype::LaparoscopicRigid3Types> MechanicalObjectLaparoscopicRigid3;
#endif

#ifndef SOFA_FLOAT
typedef MechanicalObject1d MechanicalObject1;
typedef MechanicalObject2d MechanicalObject2;
typedef MechanicalObject3d MechanicalObject3;
typedef MechanicalObject6d MechanicalObject6;

typedef MechanicalObjectRigid2d MechanicalObjectRigid2;
typedef MechanicalObjectRigid3d MechanicalObjectRigid3;


typedef Particles1d          Particles1;
typedef VecDeriv1d	     VecDeriv1;
typedef VecCoord1d	     VecCoord1;
typedef Deriv1d	     	     Deriv1;
typedef Coord1d	     	     Coord1;
typedef Particles2d	     Particles2;
typedef VecDeriv2d	     VecDeriv2;
typedef VecCoord2d	     VecCoord2;
typedef Deriv2d	     	     Deriv2;
typedef Coord2d	     	     Coord2;
typedef Particles3d	     Particles3;
typedef VecDeriv3d	     VecDeriv3;
typedef VecCoord3d	     VecCoord3;
typedef Deriv3d	     	     Deriv3;
typedef Coord3d	     	     Coord3;
typedef Particles6d	     Particles6;
typedef VecDeriv6d	     VecDeriv6;
typedef VecCoord6d	     VecCoord6;
typedef Deriv6d	     	     Deriv6;
typedef Coord6d	     	     Coord6;

typedef Rigid2d	     	     Rigid2;
typedef VecDerivRigid2d      VecDerivRigid2;
typedef VecCoordRigid2d      VecCoordRigid2;
typedef DerivRigid2d	     DerivRigid2;
typedef CoordRigid2d	     CoordRigid2;
typedef Rigid3d	     	     Rigid3;
typedef VecDerivRigid3d      VecDerivRigid3;
typedef VecCoordRigid3d      VecCoordRigid3;
typedef Quat3d		     Quat3;
typedef DerivRigid3d	     DerivRigid3;
typedef CoordRigid3d	     CoordRigid3;
#endif


#endif
