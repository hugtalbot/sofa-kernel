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
#ifndef SOFA_COMPONENT_FORCEFIELD_HomogenizedHEXAHEDRONFEMFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_HomogenizedHEXAHEDRONFEMFORCEFIELD_INL

#include <sofa/component/forcefield/HomogenizedHexahedronFEMForceFieldAndMass.h>


#include <sofa/component/linearsolver/NewMatMatrix.h>

// #include <sofa/simulation/tree/GNode.h>

// #include <sofa/simulation/common/InitVisitor.h>
// #include <sofa/simulation/common/UpdateContextVisitor.h>

// #include <sofa/core/Mapping.h>
// #include <sofa/core/componentmodel/behavior/MappedModel.h>
// #include <sofa/core/componentmodel/behavior/State.h>
// #include <sofa/component/visualmodel/VisualModelImpl.h>

#include <iomanip>


using std::cerr;
using std::endl;
using std::set;


// using sofa::simulation::tree::GNode;


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;

using topology::SparseGridTopology;
using namespace linearsolver;

// 	  template <class DataTypes>
// 	  const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::IndicesOf_FineHexa_FineNode_forXYZAssemblingOrder[27][2]=
// 	  {
// 		  { 0  , 0 }, // tranche z=0
// 		  { 0 , 1 },
// 		  { 1 , 1 },
// 		  {  2,0  },
// 		  {  2, 1 },
// 		  {  3, 1 },
// 		  {  2, 3 },
// 		  {  2, 2 },
// 		  {  3, 2 },
//
// 		  { 0  , 4 }, // z=1
// 		  { 0 , 5 },
// 		  { 1 , 5 },
// 		  {  2,4  },
// 		  {  2, 5 },
// 		  {  3, 5 },
// 		  {  2, 7 },
// 		  {  2, 6 },
// 		  {  3, 6 },
//
// 		  { 4  , 4 }, // z=2
// 		  { 4 , 5 },
// 		  { 5 , 5 },
// 		  {  6,4  },
// 		  {  6, 5 },
// 		  {  7, 5 },
// 		  {  6, 7 },
// 		  {  6, 6 },
// 		  {  7, 6 }
// 	  };



template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::FineHexa_FineNode_IndiceForAssembling[8][8]=
{
    // for an (fine elem, fine vertex) given -> what position in the assembled matrix
    {  0,  1,  4, 3,  9, 10,  13, 12},
    {  1, 2, 5, 4, 10, 11, 14, 13},
    {  3, 4,  7, 6,  12, 13,  16, 15},
    { 4, 5, 8, 7, 13, 14, 17, 16},
    {  9, 10,  13, 12,  18, 19,  22, 21},
    { 10, 11, 14, 13, 19, 20, 23, 22},
    {  12, 13,  16, 15,  21, 22,  25, 24},
    { 13, 14, 17, 16, 22, 23, 26, 25}
};


/*
	template <class DataTypes>
	const bool HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::IS_CONSTRAINED[8][8]=
	{
      // for an (fine elem, fine vertex) given -> what position in the assembled matrix
		{  1, 0, 0, 0, 0, 0, 0, 0},
		{  0, 1, 0, 0, 0, 0, 0, 0},
		{  0, 0, 0, 1, 0, 0, 0, 0},
		{  0, 0, 1, 0, 0, 0, 0, 0},
		{  0, 0, 0, 0, 1, 0, 0, 0},
		{  0, 0, 0, 0, 0, 1, 0, 0},
		{  0, 0, 0, 0, 0, 0, 0, 1},
		{  0, 0, 0, 0, 0, 0, 1, 0},
	};





// 		{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	 12,13,	14,	15,	16,	17,	18,	 19,	20,	21,	22,	23,	24,	25,	26},
// 		{0,	0,	1,	1,	2,	3,	3,	4,	2,	5,	6,	7	,8,	9,	10,	11,	12,	13,	4	,14,	5,	15,	16,	17,	7,	18,	6}

	template <class DataTypes>
	const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::FineHexa_FineNode_IndiceForCutAssembling[8][8]=
	{
      // for an (fine elem, fine vertex) given -> what position in the assembled matrix
		{  0,  0,  2, 1,  5, 6,  9, 8},
		{  0, 1, 3, 2, 6, 7, 10, 9},
		{  1, 2,  4, 3,  8, 9,  12, 11},
		{  2, 3, 2, 4, 9, 10, 13, 12},
		{  5, 6,  9, 8 ,  4, 14,  16, 15},
		{  6, 7, 10, 9, 14, 5, 17, 16},
		{  8, 9,  12, 11,  15, 16,  18, 7},
		{  9, 10, 13, 12, 16, 17, 6, 18}
	};
	*/



template <class DataTypes>
const bool HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::IS_CONSTRAINED_27[27] =
{
    1,0,1, 0,0,0, 1,0,1, //tranche devant
    0,0,0, 0,0,0, 0,0,0,  //milieu
    1,0,1, 0,0,0, 1,0,1
};


template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::FineHexa_FineNode_IndiceForCutAssembling_27[27]=
{0,	0,	1,	1,	2,	3,	3,	4,	2,	5,	6,	7	,8,	9,	10,	11,	12,	13,	4	,14,	5,	15,	16,	17,	7,	18,	6};



template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::CoarseToFine[8]=
{ 0, 2, 8, 6, 18, 20, 26, 24 };



template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::WEIGHT_MASK[27*3][8*3]=
{
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0},
    {0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0},
    {0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0},
    {0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0},
    {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1},
    {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}
};

template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::WEIGHT_MASK_CROSSED[27*3][8*3]=
{
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-1,1,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-1,0,1,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,-1,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,-1,1,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-1,-1,1,-1,-1,1,-1,-1,1,-1,-1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,-1,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,-1,1,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,-1,1,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,-1,0,1,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,-1,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0},
    {0,1,-1,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {-1,1,-1,-1,1,-1,0,0,0,0,0,0,-1,1,-1,-1,1,-1,0,0,0,0,0,0},
    {0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
    {0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0},
    {0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {1,-1,-1,0,0,0,0,0,0,1,-1,-1,1,-1,-1,0,0,0,0,0,0,1,-1,-1},
    {0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0},
    {0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0},
    {0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0},
    {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1},
    {0,0,0,1,-1,-1,1,-1,-1,0,0,0,0,0,0,1,-1,-1,1,-1,-1,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,1,0,-1},
    {0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,1,-1},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,-1,1,-1,-1,1,-1,0,0,0,0,0,0,-1,1,-1,-1,1,-1},
    {0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
    {0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0},
    {0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,-1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,-1,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,1,-1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,-1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,1,-1,-1,1,-1,-1,1,-1,-1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,1,-1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,-1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,-1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,-1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}
};

template <class DataTypes>
const int HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::WEIGHT_MASK_CROSSED_DIFF[27*3][8*3]=
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,-2,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,-2,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-1,-1,0,-1,-1,0,-1,-1,0,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0},
    {0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-1,0,-1,-1,0,-1,0,0,0,0,0,0,-1,0,-1,-1,0,-1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0},
    {0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,-1,-1,0,0,0,0,0,0,0,-1,-1,0,-1,-1,0,0,0,0,0,0,0,-1,-1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,-1,-1,0,-1,-1,0,0,0,0,0,0,0,-1,-1,0,-1,-1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2},
    {0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,-1,0,-1,-1,0,-1,0,0,0,0,0,0,-1,0,-1,-1,0,-1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0},
    {0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,-2,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,0,0,0,0,0,0,-2,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,0,-1,-1,0,-1,-1,0,-1,-1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2,0,0,-2,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};






template <class DataTypes>
void HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::init()
{
// 		  cerr<<"HomogenizedHexahedronFEMForceFieldAndMass<DataTypes>::init()\n";
    // init topology, virtual levels, calls computeMechanicalMatricesByCondensation, handles masses
    NonUniformHexahedronFEMForceFieldAndMassT::init();



//         // create dynamically a sub-graph in order to have a better homogenized mapping
//         // the idea is to non-uniformly maps the finest dofs into the coarse level
//         // and then really maps others dofs (visual, collision) into the finest dofs (by a classic barycentricMapping for example)
//         if( this->_nbVirtualFinerLevels.getValue()!= 0)
//         {
//
//
//
//           // TODO: create new gnode with mechanicalstate containing the finest nodes and a mapping contening the non linear mapping
//           GNode* parentGNode = dynamic_cast<GNode*>( this->getContext()); // the actual node
//
//           GNode* newGNode = new GNode(); // the new sub-graph
//
//           newGNode->setName("automatically created for homogenized mapping");
//           newGNode->setShowBehaviorModels(true);
//           newGNode->setShowVisualModels(true);
//           newGNode->setShowMappings(true);
//           newGNode->setShowMechanicalMappings(true);
//           newGNode->setShowForceFields(true);
//           newGNode->setShowInteractionForceFields(true);
//
//
//           _finestDOF = new MechanicalObjectT();
//           _finestDOF->setName("finest dofs");
//           newGNode->addObject( _finestDOF );
//
//           _mapping = new MappingT( this->mstate,_finestDOF);
//           _mapping->setName("homogenized mapping for the finest dofs");
//           newGNode->addObject( _mapping );
//
//           newGNode->addObject( this->_sparseGrid->_virtualFinerLevels[this->_sparseGrid->getNbVirtualFinerLevels()-this->_nbVirtualFinerLevels.getValue()] );
//
//
//           // move all children gnodes downstair this new gnode
//           vector< GNode*> gNodes;
//           for(GNode::ChildIterator it = parentGNode->child.begin(); it != parentGNode->child.end(); ++it)
//           {
//             gNodes.push_back( *it );
//           }
//           for(unsigned i=0;i<gNodes.size();++i)
//           {
//             newGNode->moveChild( gNodes[i] );
//
// 			for(GNode::ObjectIterator it = gNodes[i]->object.begin(); it != gNodes[i]->object.end(); ++it)
// 			{
// // 				cerr<<(*it)->getTypeName()<<endl;
//
// 				//since previous mapping are moved (but already initialize bu create(...) fonction, we have to manually map the fromModel with the new created dofs
//
// 				if( core::Mapping< core::componentmodel::behavior::State<DataTypes>,core::componentmodel::behavior::MappedModel<ExtVec3fTypes> >* map = dynamic_cast<core::Mapping<core::componentmodel::behavior::State<DataTypes>,core::componentmodel::behavior::MappedModel<ExtVec3fTypes> > *>(*it) )
// 				{
// 					// VISUAL MAPPING
// 					map->setModels( _finestDOF, dynamic_cast<core::componentmodel::behavior::MappedModel<ExtVec3fTypes>*>(map->getTo()) );
// // 					cerr<<"core::Mapping<DataTypes,MappedModel* "<<map->getName()<<endl;
// 				}
// 				else if( MechanicalMappingT* map = dynamic_cast<MechanicalMappingT*>(*it) )
// 				{
// 					//MECHANICAL MAPPING
// 					map->setModels( _finestDOF, dynamic_cast<MechanicalStateT*>(map->getTo()) );
// // 					cerr<<"MechanicalMappingT* "<<map->getName()<<endl;
// 				}
// 			}
//           }
//
//
//
//           // really add the new gnode in the graph
//           parentGNode->addChild(newGNode);
//
//
//         }






}




/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////




template<class T>
void HomogenizedHexahedronFEMForceFieldAndMass<T>::computeMechanicalMatricesByCondensation( )
{
    _weights.resize( this->_nbVirtualFinerLevels.getValue() );
    int finestLevel = this->_sparseGrid->getNbVirtualFinerLevels()-this->_nbVirtualFinerLevels.getValue();

    for(int i=0; i<this->_nbVirtualFinerLevels.getValue(); ++i)
    {
        _weights[i].resize( this->_sparseGrid->_virtualFinerLevels[finestLevel+i]->getNbHexas() );
    }

    _finalWeights.resize( _weights[0].size() );


    for (unsigned int i=0; i<this->_indexedElements->size(); ++i)
    {
        if( _finestToCoarse.getValue() && this->_nbVirtualFinerLevels.getValue()!=0 )
            computeMechanicalMatricesDirectlyFromTheFinestToCoarse( (*this->_elementStiffnesses.beginEdit())[i], (*this->_elementMasses.beginEdit())[i], i );
        else
            computeMechanicalMatricesIterativly( (*this->_elementStiffnesses.beginEdit())[i], (*this->_elementMasses.beginEdit())[i], i, 0 );
    }


    if( !_finestToCoarse.getValue())
        for (unsigned int i=0; i<this->_indexedElements->size(); ++i)
        {
            Weight A; A.identity();

            helper::fixed_array<int,8> finerChildren = this->_sparseGrid->_hierarchicalCubeMap[i];

            for(int w=0; w<8; ++w)
                computeFinalWeights( A, i, finerChildren[w], 1 );
        }




// 			  	for( unsigned i=0;i<_weights.size();++i)
// 				{
// 					printMatlab(cerr,_weights[i][0]);
// 					cerr<<"\n-----------------\n";
// 				}
// 				printMatlab(cerr,_finalWeights[0].second);
// 				cerr<<endl;


// 				//VERIF
// 				for(int i=0;i<8*3;++i)
// 				{
// 					Real sum = 0.0;
// 					for(int j=0;j<8*3;++j)
// 					{
// 						sum += _finalWeights[0].second[i][j];
// 					}
// 					if( fabs(sum-1.0)>1.0e-3 )
// 						cerr<<"WARNING HomogenizedHexahedronFEMForceFieldAndMass _finalWeights sum != 1  "<<sum<<endl;
// 				}



    _weights.resize(0);




// 			  printMatlab( cerr,this->_elementStiffnesses.getValue()[0] );
}



template<class T>
void HomogenizedHexahedronFEMForceFieldAndMass<T>::computeMechanicalMatricesDirectlyFromTheFinestToCoarse( ElementStiffness &K, ElementMass &M, const int elementIndice)
{
    helper::vector<int> finestChildren;

    //find them
    findFinestChildren( finestChildren, elementIndice );


    SparseGridTopology*finestSparseGrid = this->_sparseGrid->_virtualFinerLevels[ this->_sparseGrid->getNbVirtualFinerLevels()-this->_nbVirtualFinerLevels.getValue() ];

    cerr<<"finestChildren.size() : "<<finestChildren.size()<<endl;
    cerr<<"finestSparseGrid->getNbHexas() : "<<finestSparseGrid->getNbHexas()<<endl;

    int sizeass=2;
    for(int i=0; i<this->_nbVirtualFinerLevels.getValue(); ++i)
        sizeass = (sizeass-1)*2+1;
    sizeass = sizeass*sizeass*sizeass;

    NewMatMatrix assembledMatrix(sizeass*3),assembledMass(sizeass*3);
    assembledMatrix.resize(sizeass*3,sizeass*3);
    assembledMass.resize(sizeass*3,sizeass*3);
    cerr<<assembledMatrix.rowSize()<<"x"<<assembledMatrix.colSize()<<endl;



    helper::vector<ElementStiffness> finestStiffnesses(finestChildren.size());
    helper::vector<ElementMass> finestMasses(finestChildren.size());


    std::map<int,int> map_idxq_idxass; // map a fine point idx to a assembly (local) idx
    int idxass = 0;



    // compute the classical mechanical matrices at the finest level
    for(unsigned i=0 ; i < finestChildren.size() ; ++i )
    {
        computeClassicalMechanicalMatrices(finestStiffnesses[i],finestMasses[i],finestChildren[i],this->_sparseGrid->getNbVirtualFinerLevels()-this->_nbVirtualFinerLevels.getValue());

        const SparseGridTopology::Hexa& hexa = finestSparseGrid->getHexa( finestChildren[i] );


        for(int w=0; w<8; ++w) // sommets
        {
            // idx for assembly
            if( !map_idxq_idxass[ hexa[w] ] )
            {
                map_idxq_idxass[ hexa[w] ] = /*FineHexa_FineNode_IndiceForAssembling[i][w];*/idxass;
                idxass++;
            }
        }

        // assembly
        for(int j=0; j<8; ++j) // vertices1
        {
            int v1 = map_idxq_idxass[hexa[j]];

            for(int k=0; k<8; ++k) // vertices2
            {
                int v2 = map_idxq_idxass[hexa[k]];



                for(int m=0; m<3; ++m)
                    for(int n=0; n<3; ++n)
                    {
                        assembledMatrix.add( v1*3+m, v2*3+n, finestStiffnesses[i][j*3+m][k*3+n] );
                        assembledMass.add( v1*3+m, v2*3+n, finestMasses[i][j*3+m][k*3+n] );
                    }
            }
        }
    }




    std::map<int,int> map_idxq_idxcutass; // map a fine point idx to a the cut assembly (local) idx
    int idxcutass = 0;
    std::map<int,bool> map_idxq_coarse;
    helper::fixed_array<int,8> map_idxcoarse_idxfine;
    const SparseGridTopology::Hexa& coarsehexa = this->_sparseGrid->getHexa( elementIndice );

// 		cerr<<"BUILT\n";

    for(int i=0; i<sizeass; ++i)
    {
        for( std::map<int,int>::iterator it = map_idxq_idxass.begin(); it!=map_idxq_idxass.end(); ++it)
            if( (*it).second==i)
            {
// 					cerr<<(*it).first<<" "<<(*it).second<<endl;
                bool ok=false;
                Coord finesommet = finestSparseGrid->getPointPos( (*it).first );
                for( unsigned sc=0; sc<8; ++sc)
                {
                    Coord coarsesommet = this->_sparseGrid->getPointPos( coarsehexa[sc] );
                    if( coarsesommet == finesommet )
                    {
                        map_idxq_idxcutass[(*it).second] = sc;
                        map_idxq_coarse[  (*it).second] = true;
                        map_idxcoarse_idxfine[ sc ] = (*it).second;
                        ok=true;
                        break;
                    }
                }
                if( !ok )
                {
                    map_idxq_idxcutass[ (*it).second] = idxcutass;
                    map_idxq_coarse[(*it).second] = false;
                    idxcutass++;
                }
            }
    }





// 		for( std::map<int,int>::iterator it = map_idxq_idxass.begin();it!=map_idxq_idxass.end();++it)
// 		{
// 			bool ok=false;
// 			Coord finesommet = finestSparseGrid->getPointPos( (*it).first );
// 			for( unsigned sc=0;sc<8;++sc)
// 			{
// 				Coord coarsesommet = this->_sparseGrid->getPointPos( coarsehexa[sc] );
// 				if( coarsesommet == finesommet )
// 				{
// 					map_idxq_idxcutass[(*it).second] = sc;
// 					map_idxq_coarse[  (*it).second] = true;
// 					map_idxcoarse_idxfine[ sc ] = (*it).first;
// 					ok=true;
// 					break;
// 				}
// 			}
// 			if( !ok )
// 			{
// 				map_idxq_idxcutass[ (*it).second] = idxcutass;
// 				map_idxq_coarse[(*it).second] = false;
// 				idxcutass++;
// 			}
// 		}





// 		for(unsigned i=0 ; i < finestChildren.size() ; ++i )
// 		{
// 			computeClassicalMechanicalMatrices(finestStiffnesses[i],finestMasses[i],finestChildren[i],this->_sparseGrid->getNbVirtualFinerLevels()-this->_nbVirtualFinerLevels.getValue());
//
// 			const SparseGridTopology::Hexa& hexa = finestSparseGrid->getHexa( finestChildren[i] );
//
//
// 			for(int w=0;w<8;++w) // sommets
// 			{
//
// 				cerr<<map_idxq_idxass[ hexa[w] ] <<",";
// 			}
// 			cerr<<endl;
// 		}


// 		for( std::map<int,int>::iterator it = map_idxq_idxass.begin();it!=map_idxq_idxass.end();++it)
// 		{
// 			cerr<<(*it).first<<" "<<(*it).second<<endl;
// 		}
//
// 		for( std::map<int,int>::iterator it = map_idxq_idxcutass.begin();it!=map_idxq_idxcutass.end();++it)
// 		{
// 			cerr<<(*it).second<<",";
// 		}
// 		cerr<<endl;
//
// 		for( std::map<int,bool>::iterator it = map_idxq_coarse.begin();it!=map_idxq_coarse.end();++it)
// 		{
// 			cerr<<(*it).second<<",";
// 		}
// 		cerr<<endl;


// 		cerr<<map_idxcoarse_idxfine<<endl;


    NewMatMatrix Kg; // stiffness of contrained nodes
    Kg.resize(sizeass*3,8*3);
    NewMatMatrix  A; // [Kf -G] ==  Kf (stiffness of free nodes) with the constaints
    A.resize(sizeass*3,sizeass*3);
    NewMatMatrix  Ainv;
// 		Ainv.resize(sizeass*3,sizeass*3);



    for ( int i=0; i<sizeass; ++i)
    {
        int col = map_idxq_idxcutass[i];

        if( map_idxq_coarse[i] )
        {
            for(int lig=0; lig<sizeass; ++lig)
            {
                for(int m=0; m<3; ++m)
                    for(int n=0; n<3; ++n)
                        Kg.add( lig*3+m,col*3+n,assembledMatrix.element(lig*3+m,i*3+n) );
            }
        }
        else
        {
            for(int lig=0; lig<sizeass; ++lig)
            {
                for(int m=0; m<3; ++m)
                    for(int n=0; n<3; ++n)
                        A.add( lig*3+m,col*3+n,assembledMatrix.element(lig*3+m,i*3+n) );
            }
        }
    }





// 		  put -G entries into A
    for(int i=0; i<8; ++i) // for all constrained nodes
    {
        A.add( map_idxcoarse_idxfine[i]*3   , (sizeass-8+i)*3   , -1.0);
        A.add( map_idxcoarse_idxfine[i]*3+1 , (sizeass-8+i)*3+1 , -1.0);
        A.add( map_idxcoarse_idxfine[i]*3+2 , (sizeass-8+i)*3+2 , -1.0);
    }







    Ainv = A.i();







    NewMatMatrix  Ainvf;
// 		Ainvf.resize((sizeass-8)*3,sizeass*3);
    Ainv.getSubMatrix( 0,0, (sizeass-8)*3,sizeass*3,Ainvf);






    NewMatMatrix  W;
// 		W.resize((sizeass-8)*3,8*3);
    W = - Ainvf * Kg;





    NewMatMatrix  WB;
    WB.resize(sizeass*3,8*3);
    for(int i=0; i<sizeass*3; ++i)
    {
        int idx = i/3;
        int mod = i%3;
        if( map_idxq_coarse[idx] )
            WB.add( i , map_idxq_idxcutass[idx]*3+mod , 1.0);
        else
            for(int j=0; j<8*3; ++j)
            {
                WB.add( i,j, W.element( map_idxq_idxcutass[idx]*3+mod, j));
            }
    }


// 		cerr<<"KB2 = ";
// 		assembledMatrix.printMatlab(cerr);
// 		cerr<<"A2 = ";
// 		A.printMatlab(cerr);
// 		cerr<<"Kg2 = ";
// 		Kg.printMatlab(cerr);
// 		cerr<<"Ainv2 = ";
// 		Ainv.printMatlab(cerr);
// 		cerr<<"Ainvf2 = ";
// 		Ainvf.printMatlab(cerr);
// 		cerr<<"W2 = ";
// 		W.printMatlab(cerr);
// 		cerr<<"WB2 = ";
// 		WB.printMatlab(cerr);


// 		for( map<int,int>::iterator it = map_idxq_idxass.begin(); it!=map_idxq_idxass.end();++it)
// 		{
// 			if( map_idxq_coarse[ (*it).second ] )
// 				cerr<< map_idxq_idxcutass[(*it).second] <<" "<<finestSparseGrid->getPointPos( (*it).first )<<endl;
// 		}


    NewMatMatrix  mask;
    mask.resize(sizeass*3,8*3);

    Coord a = this->_sparseGrid->getPointPos(coarsehexa[0]);
    Coord b = this->_sparseGrid->getPointPos(coarsehexa[6]);
    Coord dx( b[0]-a[0],0,0),dy( 0,b[1]-a[1],0), dz( 0,0,b[2]-a[2]);
    Coord inv_d2( 1.0/(dx*dx),1.0/(dy*dy),1.0/(dz*dz) );
    for( map<int,int>::iterator it = map_idxq_idxass.begin(); it!=map_idxq_idxass.end(); ++it)
    {
        int localidx = (*it).second; // indice du noeud fin dans l'assemblage


        if( map_idxq_coarse[ (*it).second ] )
        {
// 				cerr<<map_idxq_idxcutass[ (*it).second ]<<" "<<finestSparseGrid->getPointPos( (*it).first )<<endl;
            int localcoarseidx = map_idxq_idxcutass[ (*it).second ];
            mask.set( localidx*3  , localcoarseidx*3   , 1);
            mask.set( localidx*3+1, localcoarseidx*3+1 , 1);
            mask.set( localidx*3+2, localcoarseidx*3+2 , 1);
        }
        else
        {

            // find barycentric coord
            Coord p = finestSparseGrid->getPointPos( (*it).first ) - a;

            Real fx = p*dx*inv_d2[0];
            Real fy = p*dy*inv_d2[1];
            Real fz = p*dz*inv_d2[2];


            helper::fixed_array<Real,8> baryCoefs;
            baryCoefs[0] = (1-fx) * (1-fy) * (1-fz);
            baryCoefs[1] = fx * (1-fy) * (1-fz);
            baryCoefs[2] = fx * (fy) * (1-fz);
            baryCoefs[3] = (1-fx) * (fy) * (1-fz);
            baryCoefs[4] = (1-fx) * (1-fy) * (fz);
            baryCoefs[5] = fx * (1-fy) * (fz);
            baryCoefs[6] = fx * (fy) * (fz);
            baryCoefs[7] = (1-fx) * (fy) * fz;


// 				cerr<<localidx<<"        "<<baryCoefs<<endl<<finestSparseGrid->getPointPos( (*it).first )<<" = ";

            for(int i=0; i<8; ++i)
            {
                if( baryCoefs[i]>1.0e-5 )
                {
// 						cerr<<"("<<i<<") "<<this->_sparseGrid->getPointPos( i )<<" + "<<endl;
                    mask.set( localidx*3  , i*3   , 1);
                    mask.set( localidx*3+1, i*3+1 , 1);
                    mask.set( localidx*3+2, i*3+2 , 1);
                }
            }
// 				cerr<<endl<<endl<<endl;
        }
    }


// 		for(int i=0;i<sizeass*3;++i)
// 		{
// 			for(int j=0;j<8*3;++j)
// 			{
// 				if( mask.element(i,j) != WEIGHT_MASK[i][j])
// 				{
// 					cerr<<"MASK ERROR "<<i/3<<" "<<mask.Row(i).Sum()<<"\n";
// 					break;
// 				}
// 			}
// 		}



// 		Coord a = this->_sparseGrid->getPointPos(coarsehexa[0]);
// 		Coord b = this->_sparseGrid->getPointPos(coarsehexa[6]);
//
// 		helper::vector< defaulttype::Vector6 > inPlan(sizeass); // is a point in planes  0yz, 1yz, x0z, x1z, xy0, xy1
//
// 		for( map<int,int>::iterator it = map_idxq_idxass.begin(); it!=map_idxq_idxass.end();++it)
// 		{
// 			Coord p = finestSparseGrid->getPointPos( (*it).first );
// 			int localidx = (*it).second;
//
// // 			if( map_idxq_coarse[ (*it).second ] )
// // 			{
// // 				// GROSSIER
// // 			}
// // 			else
// 			{
// 				if( p[0] == a[0] ) // plan 0yz
// 				{
// 					inPlan[ localidx ][0] = 1;
// 				}
// 				if( p[0] == b[0] ) // plan 1yz
// 				{
// 					inPlan[ localidx ][1] = 1;
// 				}
// 				if( p[1] == a[1] ) // plan x0z
// 				{
// 					inPlan[ localidx ][2] = 1;
// 				}
// 				if( p[1] == b[1] ) // plan x1z
// 				{
// 					inPlan[ localidx ][3] = 1;
// 				}
// 				if( p[2] == a[2] ) // plan xy0
// 				{
// 					inPlan[ localidx ][4] = 1;
// 				}
// 				if( p[2] == b[2] ) // plan xy1
// 				{
// 					inPlan[ localidx ][5] = 1;
// 				}
//
// 				switch( static_cast<int>(inPlan[ localidx ].sum()) )
// 				{
// 					case 0: // in the middle
// 					{
// 						for(int i=0;i<8;++i)
// 						{
// 							mask( localidx*3  , i*3   ) = 1;
// 							mask( localidx*3+1, i*3+1 ) = 1;
// 							mask( localidx*3+2, i*3+2 ) = 1;
// 						}
// 						break;
// 					}
// 					case 1: // on a plane
// 					{
// 						helper::fixed_array<int,4> whichCoarseNodesInLocalIndices;
// 						for(int i=0;i<6;++i)
// 						{
// 							if( inPlan[ localidx ][i] )
// 							{
// 								switch(i)
// 								{
// 									case 0:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(0,3,4,7);
// 										break;
// 									case 1:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(1,2,5,6);
// 										break;
// 									case 2:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(0,1,4,5);
// 										break;
// 									case 3:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(2,3,6,7);
// 										break;
// 									case 4:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(0,1,2,3);
// 										break;
// 									case 5:
// 										whichCoarseNodesInLocalIndices = helper::fixed_array<int,4>(4,5,6,7);
// 										break;
// 								}
// 								break;
// 							}
// 						}
//
// 						for(int i=0;i<4;++i)
// 						{
//
// 						}
//
// 						break;
// 					}
// 							case 2: // on an edge
// 								break;
// 								case 3: // a coarse node
// 									break;
// 					default:
// 						cerr<<"HomogenizedHexahedronFEMForceFieldAndMass<T>::computeMechanicalMatricesDirectlyFromTheFinestToCoarse   ERROR  WEIGHT_MASK\n";
//  				}
//
// 			}
// 		}




    // apply the mask to take only concerned values (an edge stays an edge, a face stays a face, if corner=1 opposite borders=0....)
    NewMatMatrix WBmeca;
    WBmeca.resize(sizeass*3,8*3);
    for(int i=0; i<sizeass*3; ++i)
    {
        for(int j=0; j<8*3; ++j)
        {
            if( mask.element(i,j) /*WEIGHT_MASK[i][j]*/ )
                WBmeca.set(i,j,WB.element(i,j));
        }
    }


// 		cerr<<"WB : "<<WB<<endl;
    cerr<<"WBmeca : "<<WBmeca<<endl;



    // normalize the coefficient to obtain sum(coefs)==1
    for(int i=0; i<sizeass*3; ++i)
    {
        Real sum = 0.0;
        for(int j=0; j<8*3; ++j)
        {
            sum += WBmeca.element(i,j);
        }
        for(int j=0; j<8*3; ++j)
        {
            WBmeca.set(i,j, WBmeca.element(i,j) / sum );
// 				WB.set(i,j, WB.element(i,j) / sum );
        }
    }

    // 		cerr<<"mask : "<<mask<<endl;
// 		cerr<<"WB : "<<WB<<endl;
// 		cerr<<"WBmeca : "<<WBmeca<<endl;

// 		WBmeca=WB;

    NewMatMatrix Kc, Mc; // coarse stiffness
// 		Kc.resize(8*3,8*3);
// 		Mc.resize(8*3,8*3);
    Kc = WBmeca.t() * assembledMatrix * WBmeca;
    Mc = WBmeca.t() * assembledMass * WBmeca;





    for(int i=0; i<8*3; ++i)
        for(int j=0; j<8*3; ++j)
        {
            K[i][j]=Kc.element(i,j);
            M[i][j]=Mc.element(i,j);
        }




    if( !_completeInterpolation.getValue() ) // take WBmeca as the object interpolation
    {
        WB = WBmeca;
    }


    for(unsigned i=0 ; i < finestChildren.size() ; ++i )
    {
        const SparseGridTopology::Hexa& hexa = finestSparseGrid->getHexa( finestChildren[i] );
        for(int j=0; j<8; ++j)
        {
            for( int k=0; k<8*3; ++k)
            {
                _finalWeights[finestChildren[i]].second[j*3  ][k] = WB.element( map_idxq_idxass[ hexa[j] ]*3   ,k);
                _finalWeights[finestChildren[i]].second[j*3+1][k] = WB.element( map_idxq_idxass[ hexa[j] ]*3+1 ,k);
                _finalWeights[finestChildren[i]].second[j*3+2][k] = WB.element( map_idxq_idxass[ hexa[j] ]*3+2 ,k);
            }
        }
        _finalWeights[finestChildren[i]].first = elementIndice;
    }

    /*
    		cerr<<"Kf = ";
    		printMatlab( cerr,finestStiffnesses[0] );
    		cerr<<"Kc = ";
    		printMatlab( cerr,K );*/


}





template<class T>
void HomogenizedHexahedronFEMForceFieldAndMass<T>::findFinestChildren( helper::vector<int>& finestChildren, const int elementIndice, int level)
{
    if (level == this->_nbVirtualFinerLevels.getValue())
    {
        finestChildren.push_back( elementIndice );
    }
    else
    {
        helper::fixed_array<int,8> finerChildren;
        if (level == 0)
        {
            finerChildren = this->_sparseGrid->_hierarchicalCubeMap[elementIndice];
        }
        else
        {
            finerChildren = this->_sparseGrid->_virtualFinerLevels[this->_nbVirtualFinerLevels.getValue()-level]->_hierarchicalCubeMap[elementIndice];
        }

        for ( int i=0; i<8; ++i) //for 8 virtual finer element
        {
            findFinestChildren( finestChildren, finerChildren[i], level+1 );
        }
    }
}



template<class T>
void HomogenizedHexahedronFEMForceFieldAndMass<T>::computeMechanicalMatricesIterativly( ElementStiffness &K, ElementMass &M, const int elementIndice,  int level)
{
    // 		  NonUniformHexahedronFEMForceFieldAndMassT::computeMechanicalMatricesByCondensation(K,M,elementIndice,level);


    if (level == this->_nbVirtualFinerLevels.getValue())
    {
        computeClassicalMechanicalMatrices(K,M,elementIndice,this->_sparseGrid->getNbVirtualFinerLevels()-level);
// 		  printMatlab( cerr, K );
    }
    else
    {
        helper::fixed_array<int,8> finerChildren;

        topology::SparseGridTopology* sparseGrid,*finerSparseGrid;

        if (level == 0)
        {
            sparseGrid = this->_sparseGrid;
            finerSparseGrid = this->_sparseGrid->_virtualFinerLevels[this->_sparseGrid->getNbVirtualFinerLevels()-1];
        }
        else
        {
            sparseGrid = this->_sparseGrid->_virtualFinerLevels[this->_sparseGrid->getNbVirtualFinerLevels()-level];
            finerSparseGrid = this->_sparseGrid->_virtualFinerLevels[this->_sparseGrid->getNbVirtualFinerLevels()-level-1];
        }

        finerChildren = sparseGrid->_hierarchicalCubeMap[elementIndice];


        helper::fixed_array<ElementStiffness,8> finerK;
        helper::fixed_array<ElementMass,8> finerM;

        for ( int i=0; i<8; ++i) //for 8 virtual finer element
        {
            if (finerChildren[i] != -1)
            {
                computeMechanicalMatricesIterativly(finerK[i], finerM[i], finerChildren[i], level+1);
            }

// 			cerr<<"K "<<i<<" : "<<finerK[i]<<endl;

        }




//           cerr<<"\n***LEVEL "<<level<<"    element "<<elementIndice<<endl;



        // assemble the matrix of 8 child
        Mat<27*3, 27*3, Real> assembledMatrix;
        Mat<27*3, 27*3, Real> assembledMass;


        for ( int i=0; i<8; ++i) //for 8 virtual finer element
        {
            if( finerChildren[i]!=-1)
            {
                for(int j=0; j<8; ++j) // vertices1
                {
                    int v1 = FineHexa_FineNode_IndiceForAssembling[i][j];

                    for(int k=0; k<8; ++k) // vertices2
                    {
                        int v2 = FineHexa_FineNode_IndiceForAssembling[i][k];

                        for(int m=0; m<3; ++m)
                            for(int n=0; n<3; ++n)
                            {
                                assembledMatrix[ v1*3+m ][ v2*3+n ] += finerK[i][j*3+m][k*3+n];
                                assembledMass[ v1*3+m ][ v2*3+n ] += finerM[i][j*3+m][k*3+n];
                            }
                    }
                }
            }
            else
                cerr<<"WARNING: a child is void (during assembly)\n";
        }


        Mat<27*3, 8*3, Real> Kg; // stiffness of contrained nodes
        Mat<27*3, 27*3, Real> A; // [Kf -G]  Kf (stiffness of free nodes) with the constaints
        Mat<27*3, 27*3, Real> Ainv;


        for ( int i=0; i<27; ++i)
        {
            int col = FineHexa_FineNode_IndiceForCutAssembling_27[i];

            if( IS_CONSTRAINED_27[i] )
            {
                for(int lig=0; lig<27; ++lig)
                {
                    for(int m=0; m<3; ++m)
                        for(int n=0; n<3; ++n)
                            Kg[ lig*3+m ][ col*3+n ] = assembledMatrix[lig*3+m][i*3+n];
                }
            }
            else
            {
                for(int lig=0; lig<27; ++lig)
                {
                    for(int m=0; m<3; ++m)
                        for(int n=0; n<3; ++n)
                            A[ lig*3+m ][ col*3+n ] = assembledMatrix[lig*3+m][i*3+n];
                }
            }

        }


        // put -G entries into A
        for(int i=0; i<8; ++i) // for all constrained nodes
        {
            A[ CoarseToFine[i]*3   ][ (27-8+i)*3   ] = -1.0;
            A[ CoarseToFine[i]*3+1 ][ (27-8+i)*3+1 ] = -1.0;
            A[ CoarseToFine[i]*3+2 ][ (27-8+i)*3+2 ] = -1.0;
        }


// 		  cerr<<"KB = "; printMatlab( cerr, assembledMatrix );
// 		  cerr<<"A = ";
// 		  printMatlab( cerr, A );
// 		  cerr<<"Kg = ";
// 		  printMatlab( cerr, Kg );




        Ainv.invert(A);




// 		  Mat<8*3, 27*3, Real> Ainvg;
// 		  for(int i=0;i<8;++i)
// 		  {
// 				  for(int m=0;m<3;++m)
// 					  for(int n=0;n<3;++n)
// 						  Ainvg[i*3+m] = - Ainv.line( (27-8+i)*3+m );
// 		  }
//
// 		  K = Ainvg * Kg;




        Mat<(27-8)*3, 27*3, Real> Ainvf;
        for(int i=0; i<27-8; ++i)
        {
            for(int m=0; m<3; ++m)
// 				  for(int n=0;n<3;++n)
                Ainvf[i*3+m] = - Ainv.line( i*3+m );
        }




        Mat<(27-8)*3, 8*3, Real> W;
        W = Ainvf * Kg;


        Mat<27*3, 8*3, Real> WB;
        for(int i=0; i<27*3; ++i)
        {
            int idx = i/3;
            int mod = i%3;
            if( IS_CONSTRAINED_27[idx] )
                WB[ i ][ FineHexa_FineNode_IndiceForCutAssembling_27[idx]*3+mod ] = 1.0;
            else
                WB[ i ] = W[ FineHexa_FineNode_IndiceForCutAssembling_27[idx]*3+mod ];
        }



// 		  cerr<<"Ainv = ";
// 		  printMatlab( cerr, Ainv );
// 		  cerr<<"Ainvf = ";
// 		  printMatlab( cerr, Ainvf );
// 		  cerr<<"W = ";
// 		  printMatlab( cerr, W );
// 		  cerr<<"WB = ";
// 		  printMatlab( cerr, WB );



        // apply the mask to take only concerned values (an edge stays an edge, a face stays a face, if corner=1 opposite borders=0....)
        Mat<27*3, 8*3, Real> WBmeca;
        for(int i=0; i<27*3; ++i)
        {
            for(int j=0; j<8*3; ++j)
            {
                if( WEIGHT_MASK[i][j] )
                    WBmeca[i][j]=WB[i][j];
            }
        }


        helper::vector<Real> sum_wbmeca(27*3,0);
        // normalize the coefficient to obtain sum(coefs)==1
        for(int i=0; i<27*3; ++i)
        {
// 			  Real sum = 0.0;
            for(int j=0; j<8*3; ++j)
            {
                sum_wbmeca[i] += WBmeca[i][j];
            }
            for(int j=0; j<8*3; ++j)
            {
                WBmeca[i][j] /= sum_wbmeca[i];
            }
        }


// 		  cerr<<"\nWsofa = ";
// 		  printMatlab( cerr, WB );


        K = WBmeca.multTranspose( assembledMatrix * WBmeca );


// 		  cerr<<"\nWsofa = ";
// 		  printMatlab( cerr, W );
// 		  cerr<<"\nWBsofa = ";
// 		  printMatlab( cerr, WB );



// 		  cerr<<"K is sym : "<<K.isSymetric()<<endl;



// 		  cerr<<"\nAinv1sofa = ";
// 		  printMatlab( cerr, Ainv1 );
// 		  cerr<<"\nKsofa = ";
// 		  printMatlab( cerr, K );


        M = WBmeca.multTranspose( assembledMass * WBmeca );

// 		  cerr<<WB[16*3+1]<<endl;

// 		  helper::fixed_array< Mat<8*3, 8*3, Real>, 8 >  Welem; // weights matrices per elem : from the coarse elem to each fine element


        if( !_completeInterpolation.getValue() ) // take WBmeca as the object interpolation
        {
            WB = WBmeca;
        }
        else
        {
// 			  for(int i=0;i<27*3;++i)
// 			  {
// 				  for(int j=0;j<8*3;++j)
// 				  {
// 						  WB[i][j] *= WEIGHT_MASK_CROSSED[i][j];
// 				  }
// 			  }
//
//
// // 			  cerr<<"WEIGHT_MASK_CROSSED : \n";
// // 			  cerr<<WB[16*3+1]<<endl;
//
//
// 		  // normalize the coefficient to obtain sum(coefs)==1
// 			  for(int i=0;i<27*3;++i)
// 			  {
// // 				  Real sum = 0.0;
// // 				  for(int j=0;j<8*3;++j)
// // 				  {
// // 					  sum += (WBmeca[i][j]);
// // 				  }
//
// 				  for(int j=0;j<8*3;++j)
// 				  {
// 					  WB[i][j] /= sum_wbmeca[i];
// 				  }
// 			  }



            for(int i=0; i<27*3; ++i)
            {
                for(int j=0; j<8*3; ++j)
                {
                    if( !WEIGHT_MASK_CROSSED_DIFF[i][j] )
                        WB[i][j] = WBmeca[i][j];
                    else
                    {
                        WB[i][j] *= WEIGHT_MASK_CROSSED_DIFF[i][j]*2.5;
                    }
                }
            }



        }

// 		  cerr<<"normalize : \n";
// 		  cerr<<WB[16*3+1]<<endl;


        for(int elem=0; elem<8; ++elem)
        {
            for(int i=0; i<8; ++i)
            {
// 				  Welem[elem][i*3  ] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3 ];
// 				  Welem[elem][i*3+1] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3+1 ];
// 				  Welem[elem][i*3+2] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3+2 ];

                _weights[this->_nbVirtualFinerLevels.getValue()-level-1][finerChildren[elem]][i*3  ] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3  ];
                _weights[this->_nbVirtualFinerLevels.getValue()-level-1][finerChildren[elem]][i*3+1] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3+1];
                _weights[this->_nbVirtualFinerLevels.getValue()-level-1][finerChildren[elem]][i*3+2] = WB [ FineHexa_FineNode_IndiceForAssembling[ elem ][ i ]*3+2];
            }

// 			  if(finerChildren[elem]==2)
// 			  {
// 				  cerr<<"BUILD\n";
// 				  cerr<<this->_nbVirtualFinerLevels.getValue()-level-1<<endl;
// 				  cerr<<_weights[this->_nbVirtualFinerLevels.getValue()-level-1][finerChildren[elem]]<<endl;
// 			  }
        }





// 		  cerr<<FineHexa_FineNode_IndiceForAssembling[ 1 ][ 2 ]<<" : "<<WB [ FineHexa_FineNode_IndiceForAssembling[ 0 ][ 3 ]*3  ]<<endl;




// 		  cerr<<"\nWcsofa = ";
// 		  printMatlab( cerr, _weights[0][0] );
// 		  cerr<<"\nKcsofa = ";
// 		  printMatlab( cerr, finerK[0] );

// // 		  std::map<int,helper::fixed_array<int,8> > maptmp;
// //
// // 		  for(int i=0;i<27;++i)
// // 			  for(int j=0;j<8;++j)
// // 				  maptmp[i][
// //
// // 		  for(int i=0;i<8;++i)
// // 			  for(int j=0;j<8;++j)
// // 				  maptmp[ FineHexa_FineNode_IndiceForAssembling[i][j] ][i] = j;
// //
// // 		  cerr<<"MAP = {";
// // 		  for( std::map<int,helper::fixed_array<int,8> >::iterator it = maptmp.begin();it != maptmp.end() ;++it)
// // 		  {
// // 			  cerr<<"{";
// // 			  for(int i=0;i<8;++i)
// // 				  cerr<<(*it).second[i]<<",";
// // 			cerr<<"},\n";
// // 		  }
// // 		  cerr<<"}\n";


        // put weights into the mapping
// 		  for(int i=0;i<8;++i)
// 		  {
// 			  for(int j=0;j<8;++j)
// 			  {
// 				  if( !IS_CONSTRAINED[i][j] )
// 				  {
// 					  _mapping->_weights[
// 				  }
//
// 			  }
// 		  }



    }


}


template<class T>
void HomogenizedHexahedronFEMForceFieldAndMass<T>::computeFinalWeights( const Weight &W, const int coarseElementIndice, const int elementIndice,  int level)
{
// 		  for(int i=0;i<level*3;++i)cerr<<" ";
// 		  cerr<<"computeFinalWeights "<<elementIndice<<"  "<<level<<endl;

    if( elementIndice == -1 ) return;

    Weight A = _weights[ this->_nbVirtualFinerLevels.getValue()-level ][elementIndice]* W;

    if (level == this->_nbVirtualFinerLevels.getValue())
    {
// 			  if( elementIndice==2 )
// 			  {
// 				  cerr<<"COMPUTE_FINAL\n";
// 				  cerr<<this->_nbVirtualFinerLevels.getValue()-level<<endl;
// 				  printMatlab(cerr,_weights[0][2]);
// 				  printMatlab(cerr,W);
// 			  }

// 			  _weights[ this->_nbVirtualFinerLevels.getValue()-level ][elementIndice] = A;
        _finalWeights[ elementIndice ] = std::pair<int,Weight>(coarseElementIndice, A);
    }
    else
    {
        topology::SparseGridTopology* sparseGrid;

        sparseGrid = this->_sparseGrid->_virtualFinerLevels[this->_sparseGrid->getNbVirtualFinerLevels()-level];

        helper::fixed_array<int,8> finerChildren = sparseGrid->_hierarchicalCubeMap[elementIndice];

        for(int i=0; i<8; ++i)
            computeFinalWeights( A, coarseElementIndice, finerChildren[i], level+1);
    }
}



} // namespace forcefield

} // namespace component

} // namespace sofa

#endif

