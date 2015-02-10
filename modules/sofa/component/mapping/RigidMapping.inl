/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_MAPPING_RIGIDMAPPING_INL
#define SOFA_COMPONENT_MAPPING_RIGIDMAPPING_INL

#include <sofa/component/mapping/RigidMapping.h>
#include <sofa/core/visual/VisualParams.h>

#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/Mapping.inl>
#include <sofa/core/State.h>

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/io/SphereLoader.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/helper/gl/template.h>

#include <sofa/simulation/common/Simulation.h>

#include <string.h>
#include <iostream>
#include <cassert>
#include <numeric>
#include <istream>



using std::cout;
using std::endl;
using std::cerr;

namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;


template <class TIn, class TOut>
class RigidMapping<TIn, TOut>::Loader : public helper::io::MassSpringLoader,
        public helper::io::SphereLoader
{
public:

    RigidMapping<TIn, TOut>* dest;
    helper::WriteAccessor<Data<VecCoord> > points;

    Loader(RigidMapping<TIn, TOut>* dest) :
        dest(dest),
        points(dest->points)
    {
    }
    virtual void addMass(SReal px, SReal py, SReal pz, SReal, SReal, SReal,
                         SReal, SReal, bool, bool)
    {
        Coord c;
        Out::set(c, px, py, pz);
        points.push_back(c); //Coord((Real)px,(Real)py,(Real)pz));
    }
    virtual void addSphere(SReal px, SReal py, SReal pz, SReal)
    {
        Coord c;
        Out::set(c, px, py, pz);
        points.push_back(c); //Coord((Real)px,(Real)py,(Real)pz));
    }
};

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::load(const char *filename)
{
    points.beginEdit()->resize(0);
    points.endEdit();

    if (strlen(filename) > 4
            && !strcmp(filename + strlen(filename) - 4, ".xs3"))
    {
        Loader loader(this);
        loader.helper::io::MassSpringLoader::load(filename);
    }
    else if (strlen(filename) > 4
             && !strcmp(filename + strlen(filename) - 4, ".sph"))
    {
        Loader loader(this);
        loader.helper::io::SphereLoader::load(filename);
    }
    else if (strlen(filename) > 0)
    {
        // Default to mesh loader
        helper::io::Mesh* mesh = helper::io::Mesh::Create(filename);
        if (mesh != NULL)
        {
            helper::WriteAccessor<Data<VecCoord> > points = this->points;

            points.resize(mesh->getVertices().size());
            for (unsigned int i = 0; i < mesh->getVertices().size(); i++)
            {
                Out::set(points[i],
                         mesh->getVertices()[i][0],
                         mesh->getVertices()[i][1],
                         mesh->getVertices()[i][2]);
            }
            delete mesh;
        }
    }
}


template <class TIn, class TOut>
RigidMapping<TIn, TOut>::RigidMapping()
    : Inherit()
    , points(initData(&points, "initialPoints", "Local Coordinates of the points"))
    , index(initData(&index, (unsigned)0, "index", "input DOF index"))
    , fileRigidMapping(initData(&fileRigidMapping, "fileRigidMapping", "Filename"))
    , useX0(initData(&useX0, false, "useX0", "Use x0 instead of local copy of initial positions (to support topo changes)"))
    , indexFromEnd(initData(&indexFromEnd, false, "indexFromEnd", "input DOF index starts from the end of input DOFs vector"))
    , rigidIndexPerPoint(initData(&rigidIndexPerPoint, "rigidIndexPerPoint", "For each mapped point, the index of the Rigid it is mapped from"))
    , globalToLocalCoords(initData(&globalToLocalCoords, "globalToLocalCoords", "are the output DOFs initially expressed in global coordinates"))
    , maskFrom(NULL)
    , maskTo(NULL)
    , matrixJ()
    , updateJ(false)
{
    //std::cout << "RigidMapping Creation\n";
    this->addAlias(&fileRigidMapping, "filename");
}

template <class TIn, class TOut>
unsigned int RigidMapping<TIn, TOut>::getRigidIndex( unsigned int pointIndex ) const
{
    // do we really need this crap?
    if( points.getValue().size() == rigidIndexPerPoint.getValue().size() ) return rigidIndexPerPoint.getValue()[pointIndex];
    else
    {
        if( !indexFromEnd.getValue() ) return index.getValue();
        else return this->fromModel->getSize()-1-index.getValue();
    }
}

template <class TIn, class TOut>
int RigidMapping<TIn, TOut>::addPoint(const Coord& c)
{
    helper::WriteAccessor<Data<VecCoord> > points = this->points;
    int i = points.size();
    points.push_back(c);
    return i;
}

template <class TIn, class TOut>
int RigidMapping<TIn, TOut>::addPoint(const Coord& c, unsigned int indexFrom)
{
    VecCoord& points = *this->points.beginEdit();
    unsigned int i = points.size();
    points.push_back(c);
    this->points.endEdit();

    helper::vector<unsigned int>& rigidIndexPerPoint = *this->rigidIndexPerPoint.beginEdit();

    if( i && rigidIndexPerPoint.size()!=i )
    {
        rigidIndexPerPoint.resize( i+1 );
        std::fill( rigidIndexPerPoint.begin(), rigidIndexPerPoint.end()-1, getRigidIndex(0) );
    }
    else rigidIndexPerPoint.push_back(indexFrom);

    this->rigidIndexPerPoint.endEdit();
    return i;
}


template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::reinit()
{
    if (this->points.getValue().empty() && this->toModel != NULL && !useX0.getValue())
    {
        //        cerr<<"RigidMapping<TIn, TOut>::init(), from " << this->fromModel->getName() << " to " << this->toModel->getName() << endl;
        const VecCoord& xTo = *this->toModel->getX();
        helper::WriteAccessor<Data<VecCoord> > points = this->points;
        points.resize(xTo.size());
        unsigned int i = 0;
        if (globalToLocalCoords.getValue())
        {
            //            cerr<<"globalToLocal is true, compute local coordinates"  << endl;
//            const VecCoord& xTo = *this->toModel->getX();
//            points.resize(xTo.size());
            unsigned int i = 0, cpt = 0;
            const InVecCoord& xFrom = *this->fromModel->getX();

            for (i = 0; i < points.size(); i++)
            {
                unsigned int rigidIndex = getRigidIndex(i);
                points[i] = xFrom[rigidIndex].inverseRotate(xTo[i] - xFrom[rigidIndex].getCenter());
            }
        }
        else
        {
            for (i = 0; i < xTo.size(); i++)
            {
                points[i] = xTo[i];
            }
            //            cerr<<"globalToLocal is false, points in local coordinates : " << points << endl;
        }
    }
    else
    {
        //        cerr << "RigidMapping<TIn, TOut>::init(), points not empty or toModel is null or useX0" << endl;
    }
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::init()
{
    //    rigidMappingDummyFunction();
    if (core::behavior::BaseMechanicalState* stateFrom = dynamic_cast<core::behavior::BaseMechanicalState*>(this->fromModel.get()))
    {
        maskFrom = &stateFrom->forceMask;
    }
    if (core::behavior::BaseMechanicalState* stateTo = dynamic_cast<core::behavior::BaseMechanicalState*>(this->toModel.get()))
    {
        maskTo = &stateTo->forceMask;
    }

    if (!fileRigidMapping.getValue().empty())
        this->load(fileRigidMapping.getFullPath().c_str());


#ifdef SOFA_HAVE_EIGEN2
    eigenJacobians.resize( 1 );
    eigenJacobians[0] = &eigenJacobian;
#endif

    this->reinit();

    this->Inherit::init();
}

/*
template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::disable()
{
 if (!this->points.getValue().empty() && this->toModel!=NULL)
 {
  VecCoord& x = *this->toModel->getX();
  x.resize(points.getValue().size());
  for (unsigned int i=0;i<points.getValue().size();i++)
   x[i] = points.getValue()[i];
 }
}
*/

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::clear(int reserve)
{
    helper::WriteAccessor<Data<VecCoord> > points = this->points;
    points.clear();
    if (reserve)
        points.reserve(reserve);
    this->rigidIndexPerPoint.beginEdit()->clear();
    this->rigidIndexPerPoint.endEdit();
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::setRepartition(unsigned int value)
{
    serr<<"setRepartition: deprecated function"<<sendl;

    helper::vector<unsigned int>& rigidIndexPerPoint = *this->rigidIndexPerPoint.beginEdit();

    size_t size = this->toModel->getX()->size();

    rigidIndexPerPoint.resize( size );

    unsigned int idx = 0;
    for( size_t i=0 ; i<size ; )
         for( size_t j=0; j<value && i<size ; ++j, ++i )
            rigidIndexPerPoint[i] = idx;
         ++idx;

    this->rigidIndexPerPoint.endEdit();


}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::setRepartition(sofa::helper::vector<
                                             unsigned int> values)
{
    serr<<"setRepartition: deprecated function "<<sendl;

    helper::vector<unsigned int>& rigidIndexPerPoint = *this->rigidIndexPerPoint.beginEdit();

    size_t size = this->toModel->getX()->size();

    rigidIndexPerPoint.resize( size );

    size_t i = 0;
    for( unsigned int idx=0 ; idx<values.size() ; ++idx )
         for( size_t j=0, jend=values[idx]; j<jend ; ++j, ++i )
            rigidIndexPerPoint[i] = idx;

    this->rigidIndexPerPoint.endEdit();
}

template <class TIn, class TOut>
const typename RigidMapping<TIn, TOut>::VecCoord & RigidMapping<TIn, TOut>::getPoints()
{
    if (useX0.getValue())
    {
        const Data<VecCoord>* v = this->toModel.get()->read(core::VecCoordId::restPosition());
        if (v)
            return v->getValue();
        else
            serr
                    << "RigidMapping: ERROR useX0 can only be used in MechanicalMappings."
                    << sendl;
    }
    return points.getValue();
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::apply(const core::MechanicalParams * /*mparams*/ /* PARAMS FIRST */, Data<VecCoord>& dOut, const Data<InVecCoord>& dIn)
{
    helper::WriteAccessor< Data<VecCoord> > out = dOut;
    helper::ReadAccessor< Data<InVecCoord> > in = dIn;
    const VecCoord& pts = this->getPoints();

    updateJ = true;
#ifdef SOFA_HAVE_EIGEN2
    eigenJacobian.resizeBlocks(out.size(),in.size());
#endif

    rotatedPoints.resize(pts.size());
    out.resize(pts.size());

    for (unsigned int i = 0; i < pts.size(); i++)
    {
        unsigned int rigidIndex = getRigidIndex(i);

        rotatedPoints[i] = in[rigidIndex].rotate( pts[i] );
        out[i] = in[rigidIndex].translate( rotatedPoints[i] );
    }

    //    cerr<<"RigidMapping<TIn, TOut>::apply, " << this->getName() << endl;
    //    cerr<<"RigidMapping<TIn, TOut>::apply, in = " << dIn << endl;
    //    cerr<<"RigidMapping<TIn, TOut>::apply, points = " << pts << endl;
    //    cerr<<"RigidMapping<TIn, TOut>::apply, out = " << dOut << endl;
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::applyJ(const core::MechanicalParams * /*mparams*/ /* PARAMS FIRST */, Data<VecDeriv>& dOut, const Data<InVecDeriv>& dIn)
{
    helper::WriteAccessor< Data<VecDeriv> > out = dOut;
    helper::ReadAccessor< Data<InVecDeriv> > in = dIn;

    const VecCoord& pts = this->getPoints();
    out.resize(pts.size());

    bool isMaskInUse = maskTo && maskTo->isInUse();

    typedef helper::ParticleMask ParticleMask;
    ParticleMask::InternalStorage* indices = isMaskInUse ? &maskTo->getEntries() : NULL;
    ParticleMask::InternalStorage::const_iterator it;
    if (isMaskInUse) it = indices->begin();


    for (unsigned int i = 0; i < pts.size() && !(isMaskInUse && it == indices->end()) ; i++)
    {
        if( isMaskInUse )
        {
            if( i!=*it) continue;
            ++it;
        }
        unsigned int rigidIndex = getRigidIndex(i);
        out[i] = velocityAtRotatedPoint( in[rigidIndex], rotatedPoints[i] );
    }
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::applyJT(const core::MechanicalParams * /*mparams*/ /* PARAMS FIRST */, Data<InVecDeriv>& dOut, const Data<VecDeriv>& dIn)
{
    helper::WriteAccessor< Data<InVecDeriv> > out = dOut;
    helper::ReadAccessor< Data<VecDeriv> > in = dIn;

    const VecCoord& pts = this->getPoints();

    bool isMaskInUse = maskTo && maskTo->isInUse();
    if (maskFrom) maskFrom->setInUse(isMaskInUse);


    typedef helper::ParticleMask ParticleMask;
    ParticleMask::InternalStorage* indices = isMaskInUse ? &maskTo->getEntries() : NULL;
    ParticleMask::InternalStorage::const_iterator it;
    if (isMaskInUse) it = indices->begin();


    for (unsigned int i = 0; i < pts.size() && !(isMaskInUse && it == indices->end()) ; i++)
    {
        if( isMaskInUse )
        {
            if( i!=*it ) continue;
            ++it;
        }
        unsigned int rigidIndex = getRigidIndex(i);

        getVCenter(out[rigidIndex]) += in[i];
        getVOrientation(out[rigidIndex]) += (typename InDeriv::Rot)cross(rotatedPoints[i], in[i]);

        if (isMaskInUse)
        {
            maskFrom->insertEntry(rigidIndex);
        }
    }

}

//            using defaulttype::Vec;
//
//            /** Symmetric cross cross product.
//              Let [a×(.×c)] be the linear operator such that: a×(b×c) = [a×(.×c)]b, where × denotes the cross product.
//              This operator is not symmetric, and can mess up conjugate gradient solutions.
//              This method computes sym([a×(.×c)])b , where sym(M) = (M+M^T)/2
//              */
//            template<class Rp, class Rc>  // p for parent, c for child
//            Vec<3,Rp> symCrossCross( const Vec<3,Rc>& a,  const Vec<3,Rp>& b,  const Vec<3,Rc>& c  )
//            {
////                Rp m00 = a[1]*c[1]+a[2]*c[2], m01= -0.5*(a[1]*c[0]+a[0]*c[1]), m02 = -0.5*(a[2]*c[0]+a[0]*c[2]) ;
////                Rp                            m11=  a[0]*c[0]+a[2]*c[2],       m12 = -0.5*(a[2]*c[1]+a[1]*c[2]) ;
////                Rp                                                             m22=  a[0]*c[0]+a[1]*c[1];
//                Rp m00 = a[1]*c[1]+a[2]*c[2], m01= 0, m02 = 0 ;
//                Rp                            m11=  a[0]*c[0]+a[2]*c[2],       m12 = 0 ;
//                Rp                                                             m22=  a[0]*c[0]+a[1]*c[1];
//                return Vec<3,Rp>(
//                        m00*b[0] + m01*b[1] + m02*b[2],
//                        m01*b[0] + m11*b[1] + m12*b[2],
//                        m02*b[0] + m12*b[1] + m22*b[2]
//                        );
//            }
//
//            /** Symmetric cross cross product in 2D (see doc in 3D)
//              In 2D, this operator is a scalar so it is symmetric.
//              */
//            template<class Rp, class Rc> // p for parent, c for child
//            Rp symCrossCross( const Vec<2,Rc>& a,  const Rp& b,  const Vec<2,Rc>& c  )
//            {
//                return (a*c)*b;
//            }


template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::applyDJT(const core::MechanicalParams* mparams /* PARAMS FIRST */, core::MultiVecDerivId parentForceChangeId, core::ConstMultiVecDerivId )
{
    if( mparams->symmetricMatrix() )
        return;  // This method corresponds to a non-symmetric matrix, due to the non-commutativity of the group of rotations.

    helper::ReadAccessor<Data<VecDeriv> > childForces (*mparams->readF(this->toModel));
    helper::WriteAccessor<Data<InVecDeriv> > parentForces (*parentForceChangeId[this->fromModel.get(mparams)].write());
    helper::ReadAccessor<Data<InVecDeriv> > parentDisplacements (*mparams->readDx(this->fromModel));
    //    cerr<<"RigidMapping<TIn, TOut>::applyDJT, parent displacements = "<< parentDisplacements << endl;
    //    cerr<<"RigidMapping<TIn, TOut>::applyDJT, parent forces = "<< parentForces << endl;

    InReal kfactor = (InReal)mparams->kFactor();
    //    cerr<<"RigidMapping<TIn, TOut>::applyDJT, kfactor = "<< kfactor << endl;

    const VecCoord& pts = this->getPoints();

    bool isMaskInUse = maskTo && maskTo->isInUse();
    if (maskFrom) maskFrom->setInUse(isMaskInUse);
    typedef helper::ParticleMask ParticleMask;
    ParticleMask::InternalStorage* indices = isMaskInUse ? &maskTo->getEntries() : NULL;
    ParticleMask::InternalStorage::const_iterator it;
    if (isMaskInUse) it = indices->begin();


    for (unsigned int i = 0; i < pts.size() && !(isMaskInUse && it == indices->end()) ; i++)
    {
        if( isMaskInUse )
        {
            if( i!=*it ) continue;
            ++it;
        }
        unsigned int rigidIndex = getRigidIndex(i);

        typename TIn::AngularVector& parentTorque = getVOrientation(parentForces[rigidIndex]);
        const typename TIn::AngularVector& parentRotation = getVOrientation(parentDisplacements[rigidIndex]);
        //  const typename TIn::AngularVector& torqueDecrement = symCrossCross( childForces[i], parentRotation, rotatedPoints[i]) * kfactor;
        const typename TIn::AngularVector& torqueDecrement = TIn::crosscross( childForces[i], parentRotation, rotatedPoints[i]) * kfactor;
        parentTorque -=  torqueDecrement;

        if (isMaskInUse)
        {
            maskFrom->insertEntry(rigidIndex);
        }
    }

}


// RigidMapping::applyJT( InMatrixDeriv& out, const OutMatrixDeriv& in ) //
// this function propagate the constraint through the rigid mapping :
// if one constraint along (vector n) with a value (v) is applied on the childModel (like collision model)
// then this constraint is transformed by (Jt.n) with value (v) for the rigid model
// There is a specificity of this propagateConstraint: we have to find the application point on the childModel
// in order to compute the right constaint on the rigidModel.
template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::applyJT(const core::ConstraintParams * /*cparams*/ /* PARAMS FIRST */, Data<InMatrixDeriv>& dOut, const Data<OutMatrixDeriv>& dIn)
{
    InMatrixDeriv& out = *dOut.beginEdit();
    const OutMatrixDeriv& in = dIn.getValue();

    if (this->f_printLog.getValue())
    {
        sout << "J on mapped DOFs == " << in << sendl;
        sout << "J on input  DOFs == " << out << sendl;
    }

    const unsigned int numDofs = this->getFromModel()->getX()->size();


    // TODO the implementation on the new data structure could maybe be optimized

    typename Out::MatrixDeriv::RowConstIterator rowItEnd = in.end();

    for (typename Out::MatrixDeriv::RowConstIterator rowIt = in.begin(); rowIt != rowItEnd; ++rowIt)
    {
        typename Out::MatrixDeriv::ColConstIterator colIt = rowIt.begin();
        typename Out::MatrixDeriv::ColConstIterator colItEnd = rowIt.end();

        for (unsigned int ito = 0; ito < numDofs; ito++)
        {
            DPos v;
            DRot omega = DRot();
            bool needToInsert = false;

            for (unsigned int cpt = 0; cpt < points.getValue().size() && colIt != colItEnd; cpt++)
            {
                unsigned int rigidIndex = getRigidIndex( cpt );
                if( rigidIndex != ito )
                    continue;
                if (colIt.index() != cpt)
                    continue;

                needToInsert = true;
                const Deriv f = colIt.val();
                v += f;
                omega += (DRot) cross(rotatedPoints[cpt], f);

                ++colIt;
            }

            if (needToInsert)
            {
                const InDeriv result(v, omega);

                typename InMatrixDeriv::RowIterator o = out.writeLine(rowIt.index());
                o.addCol(ito, result);
            }
        }
    }



    if (this->f_printLog.getValue())
    {
        sout << "new J on input  DOFs = " << out << sendl;
    }

    dOut.endEdit();
}


namespace impl {

template<class U, class Coord>
static void fill_block(Eigen::Matrix<U, 3, 6>& block, const Coord& v) {
	U x = v[0];
	U y = v[1];
	U z = v[2];
				
	// note: this is -hat(v)
	block.template rightCols<3>() <<
					
		0,   z,  -y,
		-z,  0,   x,
		y,  -x,   0;
}

template<class U, class Coord>
void fill_block(Eigen::Matrix<U, 2, 3>& block, const Coord& v) {
	U x = v[0];
	U y = v[1];
				
	// note: this is -hat(v)
	block.template rightCols<1>() <<
		-y,
		x;
}


}

#ifdef SOFA_HAVE_EIGEN2
template <class TIn, class TOut>
const helper::vector<sofa::defaulttype::BaseMatrix*>* RigidMapping<TIn, TOut>::getJs()
{
	const VecCoord& out = *this->toModel->getX();
    const InVecCoord& in = *this->fromModel->getX();
    const VecCoord& pts = this->getPoints();

	typename SparseMatrixEigen::CompressedMatrix& J = eigenJacobian.compressedMatrix;
	
	if( updateJ || J.size() == 0 ) {
		
		J.resize(out.size() * NOut, in.size() * NIn);
		J.setZero();

		// matrix chunk
		typedef typename TOut::Real real;
		typedef Eigen::Matrix<real, NOut, NIn> block_type;
		block_type block;
		
		// translation part
		block.template leftCols<NOut>().setIdentity();




        for (unsigned int outIdx = 0; outIdx < pts.size() ; outIdx++)
        {
            unsigned int inIdx = getRigidIndex(outIdx);

            const Coord& v = rotatedPoints[outIdx];

            impl::fill_block(block, v);

            // block is set, now insert it in sparse matrix
            for(unsigned i = 0; i < NOut; ++i){
                unsigned row = outIdx * NOut + i;

                J.startVec( row );

                // TODO optimize identity off-diagonal and
                // skew-symmetric diagonal
                for(unsigned j = 0; j < NIn; ++j) {
                    unsigned col = inIdx * NIn + j;

                    if( block(i, j) != 0 ) {

                        J.insertBack(row, col) = block(i, j);

                    }

                }
            }
        }

		J.finalize();		
	}
												
    return &eigenJacobians;
}
#endif

template <class TIn, class TOut>
const sofa::defaulttype::BaseMatrix* RigidMapping<TIn, TOut>::getJ()
{
    const VecCoord& out = *this->toModel->getX();
    const InVecCoord& in = *this->fromModel->getX();
    const VecCoord& pts = this->getPoints();
    assert(pts.size() == out.size());

    if (matrixJ.get() == 0 || updateJ)
    {
        updateJ = false;
        if (matrixJ.get() == 0 ||
                (unsigned int)matrixJ->rowBSize() != out.size() ||
                (unsigned int)matrixJ->colBSize() != in.size())
        {
            matrixJ.reset(new MatrixType(out.size() * NOut, in.size() * NIn));
        }
        else
        {
            matrixJ->clear();
        }


        for (unsigned int outIdx = 0; outIdx < pts.size() ; outIdx++)
        {
            unsigned int inIdx = getRigidIndex(outIdx);

            setJMatrixBlock(outIdx, inIdx);
        }
    }
    matrixJ->compress();
    return matrixJ.get();
}

template<class Real>
struct RigidMappingMatrixHelper<2, Real>
{
    template <class Matrix, class Vector>
    static void setMatrix(Matrix& mat,
                          const Vector& vec)
    {
        mat[0][0] = (Real) 1     ;    mat[1][0] = (Real) 0     ;
        mat[0][1] = (Real) 0     ;    mat[1][1] = (Real) 1     ;
        mat[0][2] = (Real)-vec[1];    mat[1][2] = (Real) vec[0];
    }
};

template<class Real>
struct RigidMappingMatrixHelper<3, Real>
{
    template <class Matrix, class Vector>
    static void setMatrix(Matrix& mat,
                          const Vector& vec)
    {
        // out = J in
        // J = [ I -OM^ ]
        mat[0][0] = (Real) 1     ;    mat[1][0] = (Real) 0     ;    mat[2][0] = (Real) 0     ;
        mat[0][1] = (Real) 0     ;    mat[1][1] = (Real) 1     ;    mat[2][1] = (Real) 0     ;
        mat[0][2] = (Real) 0     ;    mat[1][2] = (Real) 0     ;    mat[2][2] = (Real) 1     ;
        mat[0][3] = (Real) 0     ;    mat[1][3] = (Real)-vec[2];    mat[2][3] = (Real) vec[1];
        mat[0][4] = (Real) vec[2];    mat[1][4] = (Real) 0     ;    mat[2][4] = (Real)-vec[0];
        mat[0][5] = (Real)-vec[1];    mat[1][5] = (Real) vec[0];    mat[2][5] = (Real) 0     ;
    }
};

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::setJMatrixBlock(unsigned outIdx, unsigned inIdx)
{
    //    cerr<<"RigidMapping<TIn, TOut>::setJMatrixBlock, outIdx = " << outIdx << ", inIdx = " << inIdx << endl;
    MBloc& block = *matrixJ->wbloc(outIdx, inIdx, true);
    RigidMappingMatrixHelper<N, Real>::setMatrix(block, rotatedPoints[outIdx]);
}

template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::draw(const core::visual::VisualParams* vparams)
{
    if (!vparams->displayFlags().getShowMappings() || this->toModel==NULL )
        return;
    std::vector<Vector3> points;
    Vector3 point;

    const VecCoord& x = *this->toModel->getX();
    for (unsigned int i = 0; i < x.size(); i++)
    {
        point = OutDataTypes::getCPos(x[i]);
        points.push_back(point);
    }
    vparams->drawTool()->drawPoints(points, 7, Vec<4, float>(1, 1, 0,1) );
}


template <class TIn, class TOut>
void RigidMapping<TIn, TOut>::parse(core::objectmodel::BaseObjectDescription* arg)
{
    Inherit::parse(arg);

    // to be backward compatible with previous data structure
    const char* repartitionChar = arg->getAttribute("repartition");
    if( repartitionChar )
    {
        serr<<"parse: You are using a deprecated Data 'repartition', please use the new structure data rigidIndexPerPoint"<<sendl;

        helper::vector< unsigned int > repartition;
        std::istringstream ss( repartitionChar );
        repartition.read( ss );
        setRepartition( repartition );
    }
}

} // namespace mapping

} // namespace component

} // namespace sofa

#endif

