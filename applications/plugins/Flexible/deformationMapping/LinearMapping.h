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
*                               SOFA :: Plugins                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_LINEARMAPPING_H
#define SOFA_COMPONENT_MAPPING_LINEARMAPPING_H

#include "../initFlexible.h"
#include "../deformationMapping/BaseDeformationMapping.h"
#include "../deformationMapping/LinearJacobianBlock.inl"

namespace sofa
{
namespace component
{
namespace mapping
{

using helper::vector;


/** Generic linear mapping, from a variety of input types to a variety of output types.
*/


template <class TIn, class TOut>
class SOFA_Flexible_API LinearMapping : public BaseDeformationMapping<defaulttype::LinearJacobianBlock<TIn,TOut> >
{
public:
    typedef defaulttype::LinearJacobianBlock<TIn,TOut> BlockType;
    typedef BaseDeformationMapping<BlockType> Inherit;
    typedef typename Inherit::Coord Coord;
    typedef typename Inherit::VecCoord VecCoord;
    typedef typename Inherit::InVecCoord InVecCoord;

    typedef defaulttype::LinearJacobianBlock<TIn,defaulttype::Vec3Types> PointMapperType;

    SOFA_CLASS(SOFA_TEMPLATE2(LinearMapping,TIn,TOut), SOFA_TEMPLATE(BaseDeformationMapping,BlockType ));

protected:
    LinearMapping (core::State<TIn>* from = NULL, core::State<TOut>* to= NULL)
        : Inherit ( from, to )
    {
    }

    virtual ~LinearMapping()     { }

    virtual void mapPosition0()
    {
        helper::ReadAccessor<Data<InVecCoord> > in0 (*this->fromModel->read(core::ConstVecCoordId::restPosition()));
        helper::ReadAccessor<Data<InVecCoord> > in (*this->fromModel->read(core::ConstVecCoordId::position()));
        helper::ReadAccessor<Data<VecCoord> > pos0 (this->f_pos0);

        // empty vectors (not used in init)
        typename PointMapperType::OutCoord op(defaulttype::NOINIT);
        typename PointMapperType::MaterialToSpatial M(defaulttype::NOINIT);
        typename PointMapperType::Gradient dw(defaulttype::NOINIT);
        typename PointMapperType::Hessian ddw(defaulttype::NOINIT);

        PointMapperType mapper;
        this->f_pos.resize(pos0.size());
        for(unsigned int i=0; i<pos0.size(); i++ )
        {
            this->f_pos[i]=Coord();
            for(unsigned int j=0; j<this->f_index.getValue()[i].size(); j++ )
            {
                unsigned int index=this->f_index.getValue()[i][j];
                mapper.init( in0[index],op,pos0[i],M,this->f_w.getValue()[i][j],dw,ddw);
                mapper.addapply(this->f_pos[i],in[index]);
            }
        }
    }

};


} // namespace mapping
} // namespace component
} // namespace sofa

#endif

