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
#ifndef SOFA_COMPONENT_TOPOLOGY_SPARSEGRIDTOPOLOGY_H
#define SOFA_COMPONENT_TOPOLOGY_SPARSEGRIDTOPOLOGY_H

#include <string>


#include <sofa/component/topology/MeshTopology.h>
#include <sofa/helper/MarchingCubeUtility.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/component/topology/RegularGridTopology.h>

#include <sofa/helper/io/Mesh.h>
namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;
using sofa::helper::MarchingCubeUtility;

/** A sparse grid topology. Like a sparse FFD building from the bounding box of the object. Starting from a RegularGrid, only valid cells containing matter (ie intersecting the original surface mesh or totally inside the object) are considered.
Valid cells are tagged by a Type BOUNDARY or INSIDE
WARNING: the corresponding node in the XML file has to be placed BEFORE the MechanicalObject node, in order to excute its init() before the MechanicalObject one in order to be able to give dofs
   */
class SparseGridTopology : public MeshTopology
{
public:

    typedef fixed_array<Vector3,8> CubeCorners;
    typedef enum {OUTSIDE,INSIDE,BOUNDARY} Type; ///< each cube has a type depending on its filling ratio



    SparseGridTopology(bool _isVirtual=false);

//					virtual void reinit() {updateMesh();};

// 					static const float WEIGHT[8][8];
    static const float WEIGHT27[8][27];
    static const int cornerIndicesFromFineToCoarse[8][8];

    bool load(const char* filename);
    virtual void init();
    void buildAsFinest(); ///< building from a mesh file
    void buildFromFiner(); ///< building by condensating a finer sparse grid (used if setFinerSparseGrid has initializated _finerSparseGrid before calling init() )
    void buildVirtualFinerLevels(); ///< building eventual virtual finer levels (cf _nbVirtualFinerLevels)

    typedef std::map<Vector3,int> MapBetweenCornerPositionAndIndice;///< a vertex indice for a given vertex position in space

    /// connexion between several coarsened levels
    typedef std::vector<fixed_array<int,8> > HierarchicalCubeMap; ///< a cube indice -> corresponding 8 child indices on the potential _finerSparseGrid
    HierarchicalCubeMap _hierarchicalCubeMap;
    typedef helper::vector<int> InverseHierarchicalCubeMap; ///< a fine cube indice -> corresponding coarser cube indice
    InverseHierarchicalCubeMap _inverseHierarchicalCubeMap;

    typedef std::map<int,float> AHierarchicalPointMap;
// 					typedef helper::vector< std::pair<int,float> >  AHierarchicalPointMap;
    typedef helper::vector< AHierarchicalPointMap > HierarchicalPointMap; ///< a point indice -> corresponding 27 child indices on the potential _finerSparseGrid with corresponding weight
    HierarchicalPointMap _hierarchicalPointMap;
    typedef helper::vector< AHierarchicalPointMap > InverseHierarchicalPointMap; ///< a fine point indice -> corresponding some parent points for interpolation
    InverseHierarchicalPointMap _inverseHierarchicalPointMap;
    typedef helper::vector< int > PointMap;
    PointMap _pointMap; ///< a coarse point indice -> corresponding point in finer level
    PointMap _inversePointMap;  ///< a fine point indice -> corresponding point in coarser level


    enum {UP,DOWN,RIGHT,LEFT,BEFORE,BEHIND,NUM_CONNECTED_NODES};
    typedef helper::vector< helper::fixed_array<int,NUM_CONNECTED_NODES> > NodeAdjacency; ///< a node -> its 6 neighboors
    NodeAdjacency _nodeAdjacency;
    typedef helper::vector< helper::vector<int> >NodeCubesAdjacency; ///< a node -> its 8 neighboor cells
    NodeCubesAdjacency _nodeCubesAdjacency;
    typedef helper::vector< helper::vector<int> >NodeCornersAdjacency; ///< a node -> its 8 corners of neighboor cells
    NodeCornersAdjacency _nodeCornersAdjacency;


    helper::vector<SparseGridTopology*> _virtualFinerLevels; ///< saving the virtual levels (cf _nbVirtualFinerLevels)


    Vec<3, int> getN() const { return n.getValue();}
    int getNx() const { return n.getValue()[0]; }
    int getNy() const { return n.getValue()[1]; }
    int getNz() const { return n.getValue()[2]; }

    void setN(Vec<3,int> _n) {n.setValue(_n);}
    void setNx(int _n) { n.setValue(Vec<3,int>(_n             ,n.getValue()[1],n.getValue()[2])); }
    void setNy(int _n) { n.setValue(Vec<3,int>(n.getValue()[0],_n             ,n.getValue()[2])); }
    void setNz(int _n) { n.setValue(Vec<3,int>(n.getValue()[0],n.getValue()[1],_n)             ); }

    int getNbVirtualFinerLevels() const { return _nbVirtualFinerLevels.getValue();}
    void setNbVirtualFinerLevels(int n) {_nbVirtualFinerLevels.setValue(n);}

    void setMin(Vector3 _min) {min.setValue(_min);}
    void setXmin(SReal _min) { min.setValue(Vector3(_min             ,min.getValue()[1],min.getValue()[2])); }
    void setYmin(SReal _min) { min.setValue(Vector3(min.getValue()[0],_min             ,min.getValue()[2])); }
    void setZmin(SReal _min) { min.setValue(Vector3(min.getValue()[0],min.getValue()[1],_min)             ); }


    void setMax(Vector3 _max) {min.setValue(_max);}

    void setXmax(SReal _max) { max.setValue(Vector3(_max             ,max.getValue()[1],max.getValue()[2])); }
    void setYmax(SReal _max) { max.setValue(Vector3(max.getValue()[0],_max             ,max.getValue()[2])); }
    void setZmax(SReal _max) { max.setValue(Vector3(max.getValue()[0],max.getValue()[1],_max)             ); }

    Vector3 getMin() {return min.getValue();}
    SReal getXmin() { return min.getValue()[0]; }
    SReal getYmin() { return min.getValue()[1]; }
    SReal getZmin() { return min.getValue()[2]; }

    Vector3 getMax() {return max.getValue();}
    SReal getXmax() { return max.getValue()[0]; }
    SReal getYmax() { return max.getValue()[1]; }
    SReal getZmax() { return max.getValue()[2]; }

    bool hasPos()  const { return true; }

    /// return the cube containing the given point (or -1 if not found),
    /// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
    virtual int findCube(const Vector3& pos, SReal& fx, SReal &fy, SReal &fz);

    /// return the cube containing the given point (or -1 if not found),
    /// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
    virtual int findNearestCube(const Vector3& pos, SReal& fx, SReal &fy, SReal &fz);

    /// return indices of 6 neighboor cubes
    virtual helper::fixed_array<int,6> findneighboorCubes( int indice );


    /// return the type of the i-th cube
    virtual Type getType( int i );

    SparseGridTopology* getFinerSparseGrid() const {return _finerSparseGrid;}
    void setFinerSparseGrid( SparseGridTopology* fsp ) {_finerSparseGrid=fsp;}
    SparseGridTopology* getCoarserSparseGrid() const {return _coarserSparseGrid;}
    void setCoarserSparseGrid( SparseGridTopology* csp ) {_coarserSparseGrid=csp;}

    void updateMesh();

    RegularGridTopology _regularGrid; ///< based on a corresponding RegularGrid
    vector< int > _indicesOfRegularCubeInSparseGrid; ///< to redirect an indice of a cube in the regular grid to its indice in the sparse grid
    vector< int > _indicesOfCubeinRegularGrid; ///< to redirect an indice of a cube in the sparse grid to its indice in the regular grid

    Vector3 getPointPos( int i ) { return Vector3( seqPoints[i][0],seqPoints[i][1],seqPoints[i][2] ); }

    void getMesh( sofa::helper::io::Mesh &m);


    void setDimVoxels( int a, int b, int c) { dim_voxels.setValue(Vec<3, unsigned int>(a,b,c));}
    void setSizeVoxel( float a, float b, float c) { dim_voxels.setValue(Vec3f(a,b,c));}

    bool getVoxel(unsigned int x, unsigned int y, unsigned int z)
    {
        return getVoxel(dim_voxels.getValue()[0]*dim_voxels.getValue()[1]*z + dim_voxels.getValue()[0]*y + x);
    }

    bool getVoxel(unsigned int index) const
    {
        const int i = index%8;
        unsigned char c = dataVoxels.getValue()[index>>3];
        return ((c&((int)(pow(2.0f, i)))) >> i) == 1;
    };


protected:
    bool isVirtual;
    /// cutting number in all directions
    Data< Vec<3, int>    > n;
    Data< Vector3 > min;
    Data< Vector3 > max;

    Data< int > _nbVirtualFinerLevels; ///< create virtual (not in the animation tree) finer sparse grids in order to dispose of finest information (usefull to compute better mechanical properties for example)

    Data< Vec<3, unsigned int>  > dim_voxels;
    Data< Vector3 >                 size_voxel;
    Data< unsigned int >          resolution;
    Data< unsigned int >          smoothData;


    virtual void updateEdges();
    virtual void updateQuads();
    virtual void updateHexas();


    MarchingCubeUtility                 MC;
    Data< vector< unsigned char > >     dataVoxels;
    bool                                _usingMC;

    sofa::helper::vector<Type> _types; ///< BOUNDARY or FULL filled cells

    /// start from a seed cell (i,j,k) the OUTSIDE filling is propagated to neighboor cells until meet a BOUNDARY cell (this function is called from all border cells of the RegularGrid)
    void propagateFrom( const int i, const int j, const int k,
            RegularGridTopology& regularGrid,
            vector<Type>& regularGridTypes,
            vector<bool>& alreadyTested  ) const;

    void computeBoundingBox(const helper::vector<Vector3>& vertices,
            SReal& xmin, SReal& xmax,
            SReal& ymin, SReal& ymax,
            SReal& zmin, SReal& zmax) const;

    void voxelizeTriangleMesh(helper::io::Mesh* mesh,
            RegularGridTopology& regularGrid,
            vector<Type>& regularGridTypes) const;

    void buildFromTriangleMesh(const std::string& filename);

    void buildFromRegularGridTypes(RegularGridTopology& regularGrid, const vector<Type>& regularGridTypes);

    /** Create a sparse grid from a .voxel file
    	.voxel file format (ascii):
    	512  // num voxels x
    	512  // num voxels y
    	246  // num voxels z
    	0.7  // voxels size x [mm]
    	0.7  // voxels size y [mm]
    	2    // voxels size z [mm]
    	0 0 255 0 0 ... // data
    */
    void buildFromVoxelFile(const std::string& filename);
    void buildFromRawVoxelFile(const std::string& filename);

    template< class T>
    void constructCollisionModels(const sofa::helper::vector< sofa::core::componentmodel::topology::BaseMeshTopology * > &list_mesh,
            const sofa::helper::vector< sofa::helper::vector< Vec<3,T> >* >            &list_X,
            const sofa::helper::vector< unsigned int> mesh_MC,
            std::map< unsigned int, Vector3 >     map_indices) const;

    SparseGridTopology* _finerSparseGrid; ///< an eventual finer sparse grid that can be used to built this coarser sparse grid
    SparseGridTopology* _coarserSparseGrid; ///< an eventual coarser sparse grid

    void setVoxel(int index, unsigned char value)
    {
        if (value)
        {
            (*dataVoxels.beginEdit())[index>>3] |= (int) pow(2.0f, (int)(index%8));
        }
        else
        {
            const int i = index%8;
            const int mask = (int) pow(2.0f, i);
            if (((*dataVoxels.beginEdit())[index>>3]&mask)>>i) (*dataVoxels.beginEdit())[index>>3] -= mask;
        }
    };


    /*	/// to compute valid cubes (intersection between mesh segments and cubes)
    typedef struct segmentForIntersection{
    	Vector3 center;
    	Vector3 dir;
    	SReal norm;
    	segmentForIntersection(const Vector3& s0, const Vector3& s1)
    	{
    		center = (s0+s1)*.5;
    		dir = center-s0;
    		norm = dir.norm();
    		dir /= norm;
    	};
    } SegmentForIntersection;
    struct ltSegmentForIntersection // for set of SegmentForIntersection
    {
    	bool operator()(const SegmentForIntersection& s0, const SegmentForIntersection& s1) const
    	{
    		return s0.center < s1.center || s0.norm < s1.norm;
    	}
    };
    typedef struct cubeForIntersection{
    	Vector3 center;
    	fixed_array<Vector3,3> dir;
    	Vector3 norm;
    	cubeForIntersection( const CubeCorners&  corners )
    	{
    		center = (corners[7] + corners[0]) * .5;

    		norm[0] = (center[0] - corners[0][0]);
    		dir[0] = Vector3(1,0,0);

    		norm[1] = (center[1] - corners[0][1]);
    		dir[1] = Vector3(0,1,0);

    		norm[2] = (center[2] - corners[0][2]);
    		dir[2] = Vector3(0,0,1);
    	}
    } CubeForIntersection;
    /// return true if there is an intersection between a SegmentForIntersection and a CubeForIntersection
    bool intersectionSegmentBox( const SegmentForIntersection& seg, const CubeForIntersection& cube  ); */

    bool _alreadyInit;

public :

#ifdef SOFA_NEW_HEXA
    virtual const SeqHexas& getHexas()
    {
        if( !_alreadyInit ) init();
        return sofa::component::topology::MeshTopology::getHexas();
    }
#else
    virtual const SeqCubes& getHexas()
    {
        if( !_alreadyInit ) init();
        return sofa::component::topology::MeshTopology::getHexas();
    }
#endif
    virtual int getNbPoints() const
    {
        if( !_alreadyInit ) const_cast<SparseGridTopology*>(this)->init();
        return sofa::component::topology::MeshTopology::getNbPoints();
    }

    virtual int getNbHexas() { return this->getHexas().size();}

};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
