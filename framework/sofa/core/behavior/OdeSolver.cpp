/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2015 INRIA, USTL, UJF, CNRS, MGH                    *
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
* Authors: The SOFA Team (see Authors.txt)                                    *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/behavior/OdeSolver.h>
#include <cstdlib>
#include <cmath>
#include <sofa/core/objectmodel/BaseNode.h>

namespace sofa
{

namespace core
{

namespace behavior
{

OdeSolver::OdeSolver()
{}

OdeSolver::~OdeSolver()
{}


//const OdeSolver::MechanicalMatrix OdeSolver::M(1,0,0);
//const OdeSolver::MechanicalMatrix OdeSolver::B(0,1,0);
//const OdeSolver::MechanicalMatrix OdeSolver::K(0,0,1);



bool OdeSolver::insertInNode( objectmodel::BaseNode* node )
{
    node->addOdeSolver(this);
    Inherit1::insertInNode(node);
    return true;
}

bool OdeSolver::removeInNode( objectmodel::BaseNode* node )
{
    node->removeOdeSolver(this);
    Inherit1::removeInNode(node);
    return true;
}


} // namespace behavior

} // namespace core

} // namespace sofa

