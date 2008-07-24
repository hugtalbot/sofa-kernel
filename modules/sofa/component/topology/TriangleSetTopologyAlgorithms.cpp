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
#include <sofa/component/topology/TriangleSetTopologyAlgorithms.h>
#include <sofa/component/topology/TriangleSetTopologyAlgorithms.inl>
#include <sofa/defaulttype/Vec3Types.h>

namespace sofa
{

namespace component
{

namespace topology
{
using namespace sofa::defaulttype;

#ifndef SOFA_FLOAT
template class TriangleSetTopologyAlgorithms<Vec3dTypes>;
template class TriangleSetTopologyAlgorithms<Vec2dTypes>;
template class TriangleSetTopologyAlgorithms<Vec1dTypes>;
#endif

#ifndef SOFA_DOUBLE
template class TriangleSetTopologyAlgorithms<Vec3fTypes>;
template class TriangleSetTopologyAlgorithms<Vec2fTypes>;
template class TriangleSetTopologyAlgorithms<Vec1fTypes>;
#endif

} // namespace topology

} // namespace component

} // namespace sofa
