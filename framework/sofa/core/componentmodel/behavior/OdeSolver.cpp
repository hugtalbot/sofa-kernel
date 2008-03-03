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
#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <stdlib.h>
#include <math.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

OdeSolver::OdeSolver()
//: /*mat(NULL),*/ result(0)
{}

OdeSolver::~OdeSolver()
{}

#if 0
OdeSolver::VectorIndexAlloc::VectorIndexAlloc()
    : maxIndex(VecId::V_FIRST_DYNAMIC_INDEX-1)
{}

unsigned int OdeSolver::VectorIndexAlloc::alloc()
{
    int v;
    if (vfree.empty())
        v = ++maxIndex;
    else
    {
        v = *vfree.begin();
        vfree.erase(vfree.begin());
    }
    vused.insert(v);
    return v;
}

bool OdeSolver::VectorIndexAlloc::free(unsigned int v)
{
    if (v < VecId::V_FIRST_DYNAMIC_INDEX)
        return false;
// @TODO: Check for errors
    vused.erase(v);
    vfree.insert(v);
    return true;
}
#endif

const OdeSolver::MechanicalMatrix OdeSolver::M(1,0,0);
const OdeSolver::MechanicalMatrix OdeSolver::B(0,1,0);
const OdeSolver::MechanicalMatrix OdeSolver::K(0,0,1);

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

