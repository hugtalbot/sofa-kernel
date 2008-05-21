/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/linearsolver/LULinearSolver.h>
#include <sofa/component/linearsolver/NewMatMatrix.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;
using namespace sofa::simulation;

SOFA_DECL_CLASS(LULinearSolver)

int LULinearSolverClass = core::RegisterObject("Linear system solver using the conjugate gradient iterative algorithm")
        .add< LULinearSolver<NewMatMatrix,NewMatVector> >(true)
        .add< LULinearSolver<NewMatSymmetricMatrix,NewMatVector> >()
        .add< LULinearSolver<NewMatBandMatrix,NewMatVector> >()
        .add< LULinearSolver<NewMatSymmetricBandMatrix,NewMatVector> >()
        ;

} // namespace linearsolver

} // namespace component

} // namespace sofa

