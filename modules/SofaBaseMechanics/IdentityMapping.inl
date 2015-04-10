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
#ifndef SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_INL
#define SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_INL

#include <SofaBaseMechanics/IdentityMapping.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/VecTypes.h>


namespace sofa
{

namespace component
{

namespace mapping
{


template<class TIn, class TOut>
void IdentityMapping<TIn, TOut>::init()
{
    if ((stateFrom = dynamic_cast< core::behavior::BaseMechanicalState *>(this->fromModel.get())))
        maskFrom = &stateFrom->forceMask;
    if ((stateTo = dynamic_cast< core::behavior::BaseMechanicalState *>(this->toModel.get())))
        maskTo = &stateTo->forceMask;
    Inherit::init();
}

template <class TIn, class TOut>
void IdentityMapping<TIn, TOut>::apply(const core::MechanicalParams * /*mparams*/, Data<VecCoord>& dOut, const Data<InVecCoord>& dIn)
{
    helper::WriteOnlyAccessor< Data<VecCoord> > out = dOut;
    helper::ReadAccessor< Data<InVecCoord> > in = dIn;

    out.resize(in.size());

    for(unsigned int i=0; i<out.size(); i++)
    {
        helper::eq(out[i], in[i]);
    }
}

template <class TIn, class TOut>
void IdentityMapping<TIn, TOut>::applyJ(const core::MechanicalParams * /*mparams*/, Data<VecDeriv>& dOut, const Data<InVecDeriv>& dIn)
{
    helper::WriteOnlyAccessor< Data<VecDeriv> > out = dOut;
    helper::ReadAccessor< Data<InVecDeriv> > in = dIn;

    out.resize(in.size());

    if ( !(maskTo->isInUse()) )
    {
        for(unsigned int i=0; i<out.size(); i++)
        {
            helper::eq(out[i], in[i]);
        }
    }
    else
    {
        typedef helper::ParticleMask ParticleMask;
        const ParticleMask::InternalStorage &indices=maskTo->getEntries();
        ParticleMask::InternalStorage::const_iterator it;
        for (it=indices.begin(); it!=indices.end(); it++)
        {
            const int i=(int)(*it);
            helper::eq(out[i], in[i]);
        }
    }
}

template<class TIn, class TOut>
void IdentityMapping<TIn, TOut>::applyJT(const core::MechanicalParams * /*mparams*/, Data<InVecDeriv>& dOut, const Data<VecDeriv>& dIn)
{
    helper::WriteAccessor< Data<InVecDeriv> > out = dOut;
    helper::ReadAccessor< Data<VecDeriv> > in = dIn;

    if ( !(maskTo->isInUse()) )
    {
        maskFrom->setInUse(false);
        for(unsigned int i=0; i<in.size(); i++)
        {
            helper::peq(out[i], in[i]);
        }
    }
    else
    {
        typedef helper::ParticleMask ParticleMask;
        const ParticleMask::InternalStorage &indices=maskTo->getEntries();
        ParticleMask::InternalStorage::const_iterator it;
        for (it=indices.begin(); it!=indices.end(); it++)
        {
            const int i=(int)(*it);
            helper::peq(out[i], in[i]);
            maskFrom->insertEntry(i);
        }
    }
}

template <class TIn, class TOut>
void IdentityMapping<TIn, TOut>::applyJT(const core::ConstraintParams * /*cparams*/, Data<InMatrixDeriv>& dOut, const Data<MatrixDeriv>& dIn)
{
    InMatrixDeriv& out = *dOut.beginEdit();
    const MatrixDeriv& in = dIn.getValue();

    typename Out::MatrixDeriv::RowConstIterator rowItEnd = in.end();

    for (typename Out::MatrixDeriv::RowConstIterator rowIt = in.begin(); rowIt != rowItEnd; ++rowIt)
    {
        typename Out::MatrixDeriv::ColConstIterator colIt = rowIt.begin();
        typename Out::MatrixDeriv::ColConstIterator colItEnd = rowIt.end();

        // Creates a constraints if the input constraint is not empty.
        if (colIt != colItEnd)
        {
            typename In::MatrixDeriv::RowIterator o = out.writeLine(rowIt.index());

            while (colIt != colItEnd)
            {
                InDeriv data;
                helper::eq(data, colIt.val());

                o.addCol(colIt.index(), data);

                ++colIt;
            }
        }
    }

    dOut.endEdit();
}

template <class TIn, class TOut>
void IdentityMapping<TIn, TOut>::handleTopologyChange()
{
    if ( stateTo && stateFrom && stateTo->getSize() != stateFrom->getSize()) this->init();
}

template <class TIn, class TOut>
const sofa::defaulttype::BaseMatrix* IdentityMapping<TIn, TOut>::getJ()
{
    const unsigned int outStateSize = this->toModel->read(core::ConstVecCoordId::position())->getValue().size();
    const unsigned int  inStateSize = this->fromModel->read(core::ConstVecCoordId::position())->getValue().size();
    assert(outStateSize == inStateSize);

    if (matrixJ.get() == 0 || updateJ)
    {
        updateJ = false;
        if (matrixJ.get() == 0 || (unsigned int)matrixJ->rowBSize() != outStateSize || (unsigned int)matrixJ->colBSize() != inStateSize)
        {
            matrixJ.reset(new MatrixType(outStateSize * NOut, inStateSize * NIn));
        }
        else
        {
            matrixJ->clear();
        }

        bool isMaskInUse = maskTo->isInUse();

        typedef helper::ParticleMask ParticleMask;
        const ParticleMask::InternalStorage& indices = maskTo->getEntries();
        ParticleMask::InternalStorage::const_iterator it = indices.begin();

        for(unsigned i = 0; i < outStateSize && !(isMaskInUse && it == indices.end()); i++)
        {
            if (isMaskInUse)
            {
                if(i != *it)
                {
                    continue;
                }
                ++it;
            }
            MBloc& block = *matrixJ->wbloc(i, i, true);
            IdentityMappingMatrixHelper<NOut, NIn, Real>::setMatrix(block);
        }
    }
    return matrixJ.get();
}

template<int N, int M, class Real>
struct IdentityMappingMatrixHelper
{
    template <class Matrix>
    static void setMatrix(Matrix& mat)
    {
        for(int r = 0; r < N; ++r)
        {
            for(int c = 0; c < M; ++c)
            {
                mat[r][c] = (Real) 0;
            }
            if( r<M ) mat[r][r] = (Real) 1.0;
        }
    }
};


#ifdef SOFA_HAVE_EIGEN2

template <class TIn, class TOut>
const typename IdentityMapping<TIn, TOut>::js_type* IdentityMapping<TIn, TOut>::getJs()
{
	if( !eigen.compressedMatrix.nonZeros() || updateJ ) {
		updateJ = false;

		assert( this->fromModel->getSize() == this->toModel->getSize());

		const unsigned n = this->fromModel->getSize();

		// each block (input i, output j) has only its top-left
		// principal submatrix filled with identity
		
		const unsigned rows = n * NOut;
        const unsigned cols = n * NIn;

        eigen.compressedMatrix.resize( rows, cols );
		eigen.compressedMatrix.setZero();
        eigen.compressedMatrix.reserve( rows );
		
		for(unsigned i = 0 ; i < n; ++i) {

			for(unsigned r = 0; r < std::min<unsigned>(NIn, NOut); ++r) {
				const unsigned row = NOut * i + r;

				eigen.compressedMatrix.startVec( row );

				const unsigned col = NIn * i + r;
				eigen.compressedMatrix.insertBack( row, col ) = 1;
			}
			
		}
		
		eigen.compressedMatrix.finalize();
    }

	// std::cout << eigen.compressedMatrix << std::endl;
	
	js.resize( 1 );
	js[0] = &eigen;
	return &js;
}

#endif

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
