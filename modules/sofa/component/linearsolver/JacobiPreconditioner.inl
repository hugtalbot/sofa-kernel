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
// Author: Hadrien Courtecuisse
//
// Copyright: See COPYING file that comes with this distribution
#ifndef SOFA_COMPONENT_LINEARSOLVER_JACOBIPRECONDITIONER_INL
#define SOFA_COMPONENT_LINEARSOLVER_JACOBIPRECONDITIONER_INL

#include <sofa/component/linearsolver/JacobiPreconditioner.h>
#include <sofa/component/linearsolver/NewMatMatrix.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/component/linearsolver/DiagonalMatrix.h>
#include <sofa/component/linearsolver/SparseMatrix.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <sofa/helper/system/thread/CTime.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;
using namespace sofa::simulation;
using namespace sofa::core::objectmodel;
using sofa::helper::system::thread::CTime;
using sofa::helper::system::thread::ctime_t;
using std::cerr;
using std::endl;

template<class TMatrix, class TVector>
JacobiPreconditioner<TMatrix,TVector>::JacobiPreconditioner()
    : f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )
{
}

/// Solve P^-1 Mx= P^-1 b
// P[i][j] = M[i][j] ssi i=j
//P^-1[i][j] = 1/M[i][j]
template<class TMatrix, class TVector>
void JacobiPreconditioner<TMatrix,TVector>::solve (Matrix& /*M*/, Vector& z, Vector& r)
{
    for (unsigned i=0; i<z.size(); i++) z.set(i,invDiag[i] * r[i]); //si i==j;
}

template<class TMatrix, class TVector>
void JacobiPreconditioner<TMatrix,TVector>::setSystemMBKMatrix(double mFact, double bFact, double kFact)
{
    Inherit::setSystemMBKMatrix(mFact,bFact,kFact);
    invDiag.resize(this->currentGroup->systemMatrix->colSize());
    for (unsigned i=0; i<this->currentGroup->systemMatrix->colSize(); i++) invDiag.set(i,1.0 / this->currentGroup->systemMatrix->element(i,i)); //si i==j;
}

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
