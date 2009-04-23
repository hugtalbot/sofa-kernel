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
#ifndef SOFA_COMPONENT_TOPOLOGY_EDGESETGEOMETRYALGORITHMS_H
#define SOFA_COMPONENT_TOPOLOGY_EDGESETGEOMETRYALGORITHMS_H

#include <sofa/component/topology/PointSetGeometryAlgorithms.h>
#include <sofa/component/topology/CommonAlgorithms.h>

namespace sofa
{

namespace component
{

namespace topology
{
/** \brief A class used as an interface with an array : Useful to compute geometric information on each edge in an efficient way
*
*/
template < class T>
class BasicArrayInterface
{
public:
    // Access to i-th element.
    virtual T & operator[](int i)=0;
    virtual ~BasicArrayInterface() {}

};

using core::componentmodel::topology::BaseMeshTopology;
typedef BaseMeshTopology::EdgeID EdgeID;
typedef BaseMeshTopology::Edge Edge;
typedef BaseMeshTopology::SeqEdges SeqEdges;
typedef BaseMeshTopology::VertexEdges VertexEdges;

/**
* A class that provides geometry information on an EdgeSet.
*/
template < class DataTypes >
class EdgeSetGeometryAlgorithms : public PointSetGeometryAlgorithms<DataTypes>
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::CPos CPos;
    enum { NC = CPos::static_size };


    EdgeSetGeometryAlgorithms()
        : PointSetGeometryAlgorithms<DataTypes>()
    {}

    virtual ~EdgeSetGeometryAlgorithms() {}

    /// computes the length of edge no i and returns it
    Real computeEdgeLength(const EdgeID i) const;

    /// computes the edge length of all edges and stores it in the array interface
    void computeEdgeLength( BasicArrayInterface<Real> &ai) const;

    /// computes the initial length of edge no i and returns it
    Real computeRestEdgeLength(const EdgeID i) const;

    /// computes the initial square length of edge no i and returns it
    Real computeRestSquareEdgeLength(const EdgeID i) const;

    void computeEdgeAABB(const EdgeID i, CPos& minCoord, CPos& maxCoord) const;

    Coord computeEdgeCenter(const EdgeID i) const;

    Coord computeEdgeDirection(const EdgeID i) const;

    void getEdgeVertexCoordinates(const EdgeID i, Coord[2]) const;

    void getRestEdgeVertexCoordinates(const EdgeID i, Coord[2]) const;

    // test if a point is on the triangle indexed by ind_e
    bool isPointOnEdge(const sofa::defaulttype::Vec<3,double> &pt, const unsigned int ind_e) const;

    // compute barycentric coefficients
    sofa::helper::vector< double > compute2PointsBarycoefs(const Vec<3,double> &p, unsigned int ind_p1, unsigned int ind_p2) const;

    /// compute the projection coordinate of a point x on the edge i
    sofa::helper::vector< double > computePointProjectionOnEdge (const EdgeID i, sofa::defaulttype::Vec<3,double> coord_x);


    void writeMSHfile(const char *filename) const;
};

#if defined(WIN32) && !defined(SOFA_COMPONENT_TOPOLOGY_EDGESETGEOMETRYALGORITHMS_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec2dTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec1dTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Rigid3dTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Rigid2dTypes>;
#endif

#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec2fTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Vec1fTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Rigid3fTypes>;
extern template class SOFA_COMPONENT_TOPOLOGY_API EdgeSetGeometryAlgorithms<defaulttype::Rigid2fTypes>;
#endif
#endif

} // namespace topology

} // namespace component

} // namespace sofa

#endif
