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



// File automatically generated by "generateTypedef"


#ifndef SOFA_TYPEDEF_Engine_double_H
#define SOFA_TYPEDEF_Engine_double_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>




#include <SofaEngine/AverageCoord.h>
#include <SofaEngine/BoxROI.h>
#include <SofaEngine/DilateEngine.h>
#include <SofaMiscEngine/Distances.h>
#include <SofaEngine/ExtrudeQuadsAndGenerateHexas.h>
#include <SofaEngine/ExtrudeSurface.h>
#include <SofaEngine/GenerateRigidMass.h>
#include <SofaEngine/GroupFilterYoungModulus.h>
#include <SofaEngine/IndexValueMapper.h>
#include <SofaEngine/JoinPoints.h>
#include <SofaEngine/MergeMeshes.h>
#include <SofaEngine/MergePoints.h>
#include <SofaEngine/MeshBarycentricMapperEngine.h>
#include <SofaEngine/NormalsFromPoints.h>
#include <SofaEngine/PlaneROI.h>
#include <SofaEngine/PointsFromIndices.h>
#include <SofaEngine/QuatToRigidEngine.h>
#include <SofaEngine/RandomPointDistributionInSurface.h>
#include <SofaEngine/RigidToQuatEngine.h>
#include <SofaEngine/SphereROI.h>
#include <SofaEngine/Spiral.h>
#include <SofaEngine/SubsetTopology.h>
#include <SofaEngine/TextureInterpolation.h>
#include <SofaEngine/TransformEngine.h>
#include <SofaEngine/TransformPosition.h>
#include <SofaEngine/ValuesFromPositions.h>
#include <SofaEngine/Vertex2Frame.h>



//---------------------------------------------------------------------------------------------
//Typedef for AverageCoord
typedef sofa::component::engine::AverageCoord<sofa::defaulttype::StdRigidTypes<3, double> > AverageCoordRigid3d;
typedef sofa::component::engine::AverageCoord<sofa::defaulttype::StdRigidTypes<2, double> > AverageCoordRigid2d;
typedef sofa::component::engine::AverageCoord<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > AverageCoord2d;
typedef sofa::component::engine::AverageCoord<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > AverageCoord3d;



//---------------------------------------------------------------------------------------------
//Typedef for BoxROI
typedef sofa::component::engine::BoxROI<sofa::defaulttype::StdRigidTypes<3, double> > BoxROIRigid3d;
typedef sofa::component::engine::BoxROI<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > BoxROI3d;
typedef sofa::component::engine::BoxROI<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > BoxROI6d;



//---------------------------------------------------------------------------------------------
//Typedef for DilateEngine
typedef sofa::component::engine::DilateEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > DilateEngine3d;



//---------------------------------------------------------------------------------------------
//Typedef for Distances
typedef sofa::component::engine::Distances<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > Distances3d;



//---------------------------------------------------------------------------------------------
//Typedef for ExtrudeQuadsAndGenerateHexas
typedef sofa::component::engine::ExtrudeQuadsAndGenerateHexas<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ExtrudeQuadsAndGenerateHexas3d;



//---------------------------------------------------------------------------------------------
//Typedef for ExtrudeSurface
typedef sofa::component::engine::ExtrudeSurface<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ExtrudeSurface3d;



//---------------------------------------------------------------------------------------------
//Typedef for GenerateRigidMass
typedef sofa::component::engine::GenerateRigidMass<sofa::defaulttype::StdRigidTypes<3, double>, sofa::defaulttype::RigidMass<3, double> > GenerateRigidMassRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for GroupFilterYoungModulus
typedef sofa::component::engine::GroupFilterYoungModulus<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > GroupFilterYoungModulus3d;



//---------------------------------------------------------------------------------------------
//Typedef for IndexValueMapper
typedef sofa::component::engine::IndexValueMapper<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > IndexValueMapper3d;



//---------------------------------------------------------------------------------------------
//Typedef for JoinPoints
typedef sofa::component::engine::JoinPoints<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > JoinPoints3d;



//---------------------------------------------------------------------------------------------
//Typedef for MergeMeshes
typedef sofa::component::engine::MergeMeshes<sofa::defaulttype::StdRigidTypes<3, double> > MergeMeshesRigid3d;
typedef sofa::component::engine::MergeMeshes<sofa::defaulttype::StdRigidTypes<2, double> > MergeMeshesRigid2d;
typedef sofa::component::engine::MergeMeshes<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > MergeMeshes1d;
typedef sofa::component::engine::MergeMeshes<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > MergeMeshes2d;
typedef sofa::component::engine::MergeMeshes<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > MergeMeshes3d;



//---------------------------------------------------------------------------------------------
//Typedef for MergePoints
typedef sofa::component::engine::MergePoints<sofa::defaulttype::StdRigidTypes<3, double> > MergePointsRigid3d;
typedef sofa::component::engine::MergePoints<sofa::defaulttype::StdRigidTypes<2, double> > MergePointsRigid2d;
typedef sofa::component::engine::MergePoints<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > MergePoints1d;
typedef sofa::component::engine::MergePoints<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > MergePoints2d;
typedef sofa::component::engine::MergePoints<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > MergePoints3d;



//---------------------------------------------------------------------------------------------
//Typedef for MeshBarycentricMapperEngine
typedef sofa::component::engine::MeshBarycentricMapperEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > MeshBarycentricMapperEngine3d;



//---------------------------------------------------------------------------------------------
//Typedef for NormalsFromPoints
typedef sofa::component::engine::NormalsFromPoints<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > NormalsFromPoints3d;



//---------------------------------------------------------------------------------------------
//Typedef for PlaneROI
typedef sofa::component::engine::PlaneROI<sofa::defaulttype::StdRigidTypes<3, double> > PlaneROIRigid3d;
typedef sofa::component::engine::PlaneROI<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > PlaneROI3d;



//---------------------------------------------------------------------------------------------
//Typedef for PointsFromIndices
typedef sofa::component::engine::PointsFromIndices<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > PointsFromIndices3d;



//---------------------------------------------------------------------------------------------
//Typedef for QuatToRigidEngine
typedef sofa::component::engine::QuatToRigidEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > QuatToRigidEngine3d;



//---------------------------------------------------------------------------------------------
//Typedef for RandomPointDistributionInSurface
typedef sofa::component::engine::RandomPointDistributionInSurface<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RandomPointDistributionInSurface3d;



//---------------------------------------------------------------------------------------------
//Typedef for RigidToQuatEngine
typedef sofa::component::engine::RigidToQuatEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RigidToQuatEngine3d;



//---------------------------------------------------------------------------------------------
//Typedef for SphereROI
typedef sofa::component::engine::SphereROI<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SphereROI3d;



//---------------------------------------------------------------------------------------------
//Typedef for Spiral
typedef sofa::component::engine::Spiral<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > Spiral3d;



//---------------------------------------------------------------------------------------------
//Typedef for SubsetTopology
typedef sofa::component::engine::SubsetTopology<sofa::defaulttype::StdRigidTypes<3, double> > SubsetTopologyRigid3d;
typedef sofa::component::engine::SubsetTopology<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SubsetTopology3d;



//---------------------------------------------------------------------------------------------
//Typedef for TextureInterpolation
typedef sofa::component::engine::TextureInterpolation<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > TextureInterpolation1d;
typedef sofa::component::engine::TextureInterpolation<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > TextureInterpolation2d;
typedef sofa::component::engine::TextureInterpolation<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TextureInterpolation3d;



//---------------------------------------------------------------------------------------------
//Typedef for TransformEngine
typedef sofa::component::engine::TransformEngine<sofa::defaulttype::StdRigidTypes<3, double> > TransformEngineRigid3d;
typedef sofa::component::engine::TransformEngine<sofa::defaulttype::StdRigidTypes<2, double> > TransformEngineRigid2d;
typedef sofa::component::engine::TransformEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > TransformEngine1d;
typedef sofa::component::engine::TransformEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > TransformEngine2d;
typedef sofa::component::engine::TransformEngine<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TransformEngine3d;



//---------------------------------------------------------------------------------------------
//Typedef for TransformPosition
typedef sofa::component::engine::TransformPosition<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TransformPosition3d;



//---------------------------------------------------------------------------------------------
//Typedef for ValuesFromPositions
typedef sofa::component::engine::ValuesFromPositions<sofa::defaulttype::StdRigidTypes<3, double> > ValuesFromPositionsRigid3d;
typedef sofa::component::engine::ValuesFromPositions<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ValuesFromPositions3d;



//---------------------------------------------------------------------------------------------
//Typedef for Vertex2Frame
typedef sofa::component::engine::Vertex2Frame<sofa::defaulttype::StdRigidTypes<3, double> > Vertex2FrameRigid3d;





#ifndef SOFA_FLOAT
typedef AverageCoordRigid3d AverageCoordRigid3;
typedef AverageCoordRigid2d AverageCoordRigid2;
typedef AverageCoord2d AverageCoord2;
typedef AverageCoord3d AverageCoord3;
typedef BoxROIRigid3d BoxROIRigid3;
typedef BoxROI3d BoxROI3;
typedef BoxROI6d BoxROI6;
typedef DilateEngine3d DilateEngine3;
typedef Distances3d Distances3;
typedef ExtrudeQuadsAndGenerateHexas3d ExtrudeQuadsAndGenerateHexas3;
typedef ExtrudeSurface3d ExtrudeSurface3;
typedef GenerateRigidMassRigid3d GenerateRigidMassRigid3;
typedef GroupFilterYoungModulus3d GroupFilterYoungModulus3;
typedef IndexValueMapper3d IndexValueMapper3;
typedef JoinPoints3d JoinPoints3;
typedef MergeMeshesRigid3d MergeMeshesRigid3;
typedef MergeMeshesRigid2d MergeMeshesRigid2;
typedef MergeMeshes1d MergeMeshes1;
typedef MergeMeshes2d MergeMeshes2;
typedef MergeMeshes3d MergeMeshes3;
typedef MergePointsRigid3d MergePointsRigid3;
typedef MergePointsRigid2d MergePointsRigid2;
typedef MergePoints1d MergePoints1;
typedef MergePoints2d MergePoints2;
typedef MergePoints3d MergePoints3;
typedef MeshBarycentricMapperEngine3d MeshBarycentricMapperEngine3;
typedef NormalsFromPoints3d NormalsFromPoints3;
typedef PlaneROIRigid3d PlaneROIRigid3;
typedef PlaneROI3d PlaneROI3;
typedef PointsFromIndices3d PointsFromIndices3;
typedef QuatToRigidEngine3d QuatToRigidEngine3;
typedef RandomPointDistributionInSurface3d RandomPointDistributionInSurface3;
typedef RigidToQuatEngine3d RigidToQuatEngine3;
typedef SphereROI3d SphereROI3;
typedef Spiral3d Spiral3;
typedef SubsetTopologyRigid3d SubsetTopologyRigid3;
typedef SubsetTopology3d SubsetTopology3;
typedef TextureInterpolation1d TextureInterpolation1;
typedef TextureInterpolation2d TextureInterpolation2;
typedef TextureInterpolation3d TextureInterpolation3;
typedef TransformEngineRigid3d TransformEngineRigid3;
typedef TransformEngineRigid2d TransformEngineRigid2;
typedef TransformEngine1d TransformEngine1;
typedef TransformEngine2d TransformEngine2;
typedef TransformEngine3d TransformEngine3;
typedef TransformPosition3d TransformPosition3;
typedef ValuesFromPositionsRigid3d ValuesFromPositionsRigid3;
typedef ValuesFromPositions3d ValuesFromPositions3;
typedef Vertex2FrameRigid3d Vertex2FrameRigid3;
#endif

#endif
