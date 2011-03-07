/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2010 MGH, INRIA, USTL, UJF, CNRS                    *
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



// File automatically generated by "generateTypedef"


#ifndef SOFA_TYPEDEF_InteractionForceField_float_H
#define SOFA_TYPEDEF_InteractionForceField_float_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>


#ifdef SOFA_GPU_CUDA
#include <sofa/gpu/cuda/CudaTypesBase.h>
#include <sofa/gpu/cuda/CudaTypes.h>
#endif
#ifdef SOFA_GPU_OPENCL
#include <sofa/gpu/opencl/OpenCLTypes.h>
#endif


#include <sofa/component/interactionforcefield/BoxStiffSpringForceField.h>
#include <sofa/component/interactionforcefield/FrameSpringForceField.h>
#include <sofa/component/interactionforcefield/InteractionEllipsoidForceField.h>
#include <sofa/component/interactionforcefield/JointSpringForceField.h>
#include <sofa/component/interactionforcefield/MeshSpringForceField.h>
#include <sofa/component/interactionforcefield/PenalityContactForceField.h>
#include <sofa/component/interactionforcefield/QuadBendingSprings.h>
#include <sofa/component/interactionforcefield/RegularGridSpringForceField.h>
#include <sofa/component/interactionforcefield/RepulsiveSpringForceField.h>
#include <sofa/component/interactionforcefield/SpringForceField.h>
#include <sofa/component/interactionforcefield/StiffSpringForceField.h>
#include <sofa/component/interactionforcefield/TriangleBendingSprings.h>
#include <sofa/component/interactionforcefield/VectorSpringForceField.h>



//---------------------------------------------------------------------------------------------
//Typedef for BoxStiffSpringForceField
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > BoxStiffSpringForceField1f;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > BoxStiffSpringForceField2f;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > BoxStiffSpringForceField3f;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, float>, sofa::defaulttype::Vec<6, float>, float> > BoxStiffSpringForceField6f;



//---------------------------------------------------------------------------------------------
//Typedef for FrameSpringForceField
typedef sofa::component::interactionforcefield::FrameSpringForceField<sofa::defaulttype::StdRigidTypes<3, float> > FrameSpringForceFieldRigid3f;



//---------------------------------------------------------------------------------------------
//Typedef for InteractionEllipsoidForceField
typedef sofa::component::interactionforcefield::InteractionEllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float>, sofa::defaulttype::StdRigidTypes<3, float> > InteractionEllipsoidForceField3f_Rigid3f;



//---------------------------------------------------------------------------------------------
//Typedef for JointSpringForceField
typedef sofa::component::interactionforcefield::JointSpringForceField<sofa::defaulttype::StdRigidTypes<3, float> > JointSpringForceFieldRigid3f;



//---------------------------------------------------------------------------------------------
//Typedef for MeshSpringForceField
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > MeshSpringForceField1f;
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > MeshSpringForceField2f;
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > MeshSpringForceField3f;



//---------------------------------------------------------------------------------------------
//Typedef for PenalityContactForceField
typedef sofa::component::interactionforcefield::PenalityContactForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > PenalityContactForceField3f;



//---------------------------------------------------------------------------------------------
//Typedef for QuadBendingSprings
typedef sofa::component::interactionforcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > QuadBendingSprings2f;
typedef sofa::component::interactionforcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > QuadBendingSprings3f;



//---------------------------------------------------------------------------------------------
//Typedef for RegularGridSpringForceField
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > RegularGridSpringForceField1f;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > RegularGridSpringForceField2f;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > RegularGridSpringForceField3f;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, float>, sofa::defaulttype::Vec<6, float>, float> > RegularGridSpringForceField6f;



//---------------------------------------------------------------------------------------------
//Typedef for RepulsiveSpringForceField
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > RepulsiveSpringForceField1f;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > RepulsiveSpringForceField2f;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > RepulsiveSpringForceField3f;



//---------------------------------------------------------------------------------------------
//Typedef for SpringForceField
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > SpringForceField1f;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > SpringForceField2f;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > SpringForceField3f;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, float>, sofa::defaulttype::Vec<6, float>, float> > SpringForceField6f;



//---------------------------------------------------------------------------------------------
//Typedef for StiffSpringForceField
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > StiffSpringForceField1f;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > StiffSpringForceField2f;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > StiffSpringForceField3f;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, float>, sofa::defaulttype::Vec<6, float>, float> > StiffSpringForceField6f;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleBendingSprings
typedef sofa::component::interactionforcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > TriangleBendingSprings2f;
typedef sofa::component::interactionforcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > TriangleBendingSprings3f;



//---------------------------------------------------------------------------------------------
//Typedef for VectorSpringForceField
typedef sofa::component::interactionforcefield::VectorSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > VectorSpringForceField3f;





#ifdef SOFA_FLOAT
typedef BoxStiffSpringForceField1f BoxStiffSpringForceField1;
typedef BoxStiffSpringForceField2f BoxStiffSpringForceField2;
typedef BoxStiffSpringForceField3f BoxStiffSpringForceField3;
typedef BoxStiffSpringForceField6f BoxStiffSpringForceField6;
typedef FrameSpringForceFieldRigid3f FrameSpringForceFieldRigid3;
typedef InteractionEllipsoidForceField3f_Rigid3f InteractionEllipsoidForceField3_Rigid3;
typedef JointSpringForceFieldRigid3f JointSpringForceFieldRigid3;
typedef MeshSpringForceField1f MeshSpringForceField1;
typedef MeshSpringForceField2f MeshSpringForceField2;
typedef MeshSpringForceField3f MeshSpringForceField3;
typedef PenalityContactForceField3f PenalityContactForceField3;
typedef QuadBendingSprings2f QuadBendingSprings2;
typedef QuadBendingSprings3f QuadBendingSprings3;
typedef RegularGridSpringForceField1f RegularGridSpringForceField1;
typedef RegularGridSpringForceField2f RegularGridSpringForceField2;
typedef RegularGridSpringForceField3f RegularGridSpringForceField3;
typedef RegularGridSpringForceField6f RegularGridSpringForceField6;
typedef RepulsiveSpringForceField1f RepulsiveSpringForceField1;
typedef RepulsiveSpringForceField2f RepulsiveSpringForceField2;
typedef RepulsiveSpringForceField3f RepulsiveSpringForceField3;
typedef SpringForceField1f SpringForceField1;
typedef SpringForceField2f SpringForceField2;
typedef SpringForceField3f SpringForceField3;
typedef SpringForceField6f SpringForceField6;
typedef StiffSpringForceField1f StiffSpringForceField1;
typedef StiffSpringForceField2f StiffSpringForceField2;
typedef StiffSpringForceField3f StiffSpringForceField3;
typedef StiffSpringForceField6f StiffSpringForceField6;
typedef TriangleBendingSprings2f TriangleBendingSprings2;
typedef TriangleBendingSprings3f TriangleBendingSprings3;
typedef VectorSpringForceField3f VectorSpringForceField3;
#endif

#endif
