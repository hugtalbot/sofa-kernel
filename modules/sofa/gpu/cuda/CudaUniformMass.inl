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
#ifndef SOFA_GPU_CUDA_CUDAUNIFORMMASS_INL
#define SOFA_GPU_CUDA_CUDAUNIFORMMASS_INL

#include "CudaUniformMass.h"
#include <sofa/component/mass/UniformMass.inl>
#include <sofa/helper/gl/Axis.h>

namespace sofa
{

namespace gpu
{

namespace cuda
{

extern "C"
{
    void UniformMassCuda3f_addMDx(unsigned int size, float mass, void* res, const void* dx);
    void UniformMassCuda3f_accFromF(unsigned int size, float mass, void* a, const void* f);
    void UniformMassCuda3f_addForce(unsigned int size, const float *mg, void* f);

    void UniformMassCuda3f1_addMDx(unsigned int size, float mass, void* res, const void* dx);
    void UniformMassCuda3f1_accFromF(unsigned int size, float mass, void* a, const void* f);
    void UniformMassCuda3f1_addForce(unsigned int size, const float *mg, void* f);

#ifdef SOFA_DEV
#ifdef SOFA_GPU_CUDA_DOUBLE

    void UniformMassCuda3d_addMDx(unsigned int size, double mass, void* res, const void* dx);
    void UniformMassCuda3d_accFromF(unsigned int size, double mass, void* a, const void* f);
    void UniformMassCuda3d_addForce(unsigned int size, const double *mg, void* f);

    void UniformMassCuda3d1_addMDx(unsigned int size, double mass, void* res, const void* dx);
    void UniformMassCuda3d1_accFromF(unsigned int size, double mass, void* a, const void* f);
    void UniformMassCuda3d1_addForce(unsigned int size, const double *mg, void* f);

#endif // SOFA_GPU_CUDA_DOUBLE
#endif // SOFA_DEV
}

} // namespace cuda

} // namespace gpu

namespace component
{

namespace mass
{

using namespace gpu::cuda;

// -- Mass interface
template <>
void UniformMass<CudaVec3fTypes, float>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{
    UniformMassCuda3f_addMDx(dx.size(), (float)(mass.getValue()*factor), res.deviceWrite(), dx.deviceRead());
}

template <>
void UniformMass<CudaVec3fTypes, float>::accFromF(VecDeriv& a, const VecDeriv& f)
{
    UniformMassCuda3f_accFromF(f.size(), mass.getValue(), a.deviceWrite(), f.deviceRead());
}

template <>
void UniformMass<CudaVec3fTypes, float>::addForce(VecDeriv& f, const VecCoord&, const VecDeriv&)
{
    // weight
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set( theGravity, g[0], g[1], g[2]);
    Deriv mg = theGravity * mass.getValue();
    UniformMassCuda3f_addForce(f.size(), mg.ptr(), f.deviceWrite());
}

template <>
bool UniformMass<gpu::cuda::CudaVec3fTypes, float>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    //if (!x.isHostValid()) return false; // Do not recompute bounding box if it requires to transfer data from device
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        const Coord& p = x.getCached(i);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template <>
void UniformMass<CudaVec3f1Types, float>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{
    UniformMassCuda3f1_addMDx(dx.size(), (float)(mass.getValue()*factor), res.deviceWrite(), dx.deviceRead());
}

template <>
void UniformMass<CudaVec3f1Types, float>::accFromF(VecDeriv& a, const VecDeriv& f)
{
    UniformMassCuda3f1_accFromF(f.size(), mass.getValue(), a.deviceWrite(), f.deviceRead());
}

template <>
void UniformMass<CudaVec3f1Types, float>::addForce(VecDeriv& f, const VecCoord&, const VecDeriv&)
{
    // weight
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set( theGravity, g[0], g[1], g[2]);
    Deriv mg = theGravity * mass.getValue();
    UniformMassCuda3f1_addForce(f.size(), mg.ptr(), f.deviceWrite());
}

template <>
bool UniformMass<gpu::cuda::CudaVec3f1Types, float>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    //if (!x.isHostValid()) return false; // Do not recompute bounding box if it requires to transfer data from device
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        const Coord& p = x.getCached(i);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template <>
double UniformMass<gpu::cuda::CudaRigid3fTypes,sofa::defaulttype::Rigid3fMass>::getPotentialEnergy( const VecCoord& x )
{
    double e = 0;
    // gravity
    Vec3d g ( this->getContext()->getLocalGravity() );
    for (unsigned int i=0; i<x.size(); i++)
    {
        e += g*mass.getValue().mass*x[i].getCenter();
    }
    return e;
}

template <>
double UniformMass<gpu::cuda::CudaRigid3fTypes,sofa::defaulttype::Rigid3fMass>::getElementMass(unsigned int )
{
    return (double)(mass.getValue().mass);
}

template <>
void UniformMass<gpu::cuda::CudaRigid3fTypes, Rigid3fMass>::draw()
{
    if (!getContext()->getShowBehaviorModels())
        return;
    const VecCoord& x = *mstate->getX();
    defaulttype::Vec3d len;

    // The moment of inertia of a box is:
    //   m->_I(0,0) = M/REAL(12.0) * (ly*ly + lz*lz);
    //   m->_I(1,1) = M/REAL(12.0) * (lx*lx + lz*lz);
    //   m->_I(2,2) = M/REAL(12.0) * (lx*lx + ly*ly);
    // So to get lx,ly,lz back we need to do
    //   lx = sqrt(12/M * (m->_I(1,1)+m->_I(2,2)-m->_I(0,0)))
    // Note that RigidMass inertiaMatrix is already divided by M
    double m00 = mass.getValue().inertiaMatrix[0][0];
    double m11 = mass.getValue().inertiaMatrix[1][1];
    double m22 = mass.getValue().inertiaMatrix[2][2];
    len[0] = sqrt(m11+m22-m00);
    len[1] = sqrt(m00+m22-m11);
    len[2] = sqrt(m00+m11-m22);

    for (unsigned int i=0; i<x.size(); i++)
    {
        helper::gl::Axis::draw(x[i].getCenter(), x[i].getOrientation(), len);
    }
}


#ifdef SOFA_DEV
#ifdef SOFA_GPU_CUDA_DOUBLE

// -- Mass interface
template <>
void UniformMass<CudaVec3dTypes, double>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{
    UniformMassCuda3d_addMDx(dx.size(), (double)(mass.getValue()*factor), res.deviceWrite(), dx.deviceRead());
}

template <>
void UniformMass<CudaVec3dTypes, double>::accFromF(VecDeriv& a, const VecDeriv& f)
{
    UniformMassCuda3d_accFromF(f.size(), mass.getValue(), a.deviceWrite(), f.deviceRead());
}

template <>
void UniformMass<CudaVec3dTypes, double>::addForce(VecDeriv& f, const VecCoord&, const VecDeriv&)
{
    // weight
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set( theGravity, g[0], g[1], g[2]);
    Deriv mg = theGravity * mass.getValue();
    UniformMassCuda3d_addForce(f.size(), mg.ptr(), f.deviceWrite());
}

template <>
bool UniformMass<gpu::cuda::CudaVec3dTypes, double>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    //if (!x.isHostValid()) return false; // Do not recompute bounding box if it requires to transfer data from device
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        const Coord& p = x.getCached(i);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template <>
void UniformMass<CudaVec3d1Types, double>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{
    UniformMassCuda3d1_addMDx(dx.size(), (double)(mass.getValue()*factor), res.deviceWrite(), dx.deviceRead());
}

template <>
void UniformMass<CudaVec3d1Types, double>::accFromF(VecDeriv& a, const VecDeriv& f)
{
    UniformMassCuda3d1_accFromF(f.size(), mass.getValue(), a.deviceWrite(), f.deviceRead());
}

template <>
void UniformMass<CudaVec3d1Types, double>::addForce(VecDeriv& f, const VecCoord&, const VecDeriv&)
{
    // weight
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set( theGravity, g[0], g[1], g[2]);
    Deriv mg = theGravity * mass.getValue();
    UniformMassCuda3d1_addForce(f.size(), mg.ptr(), f.deviceWrite());
}

template <>
bool UniformMass<gpu::cuda::CudaVec3d1Types, double>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    //if (!x.isHostValid()) return false; // Do not recompute bounding box if it requires to transfer data from device
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        const Coord& p = x.getCached(i);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template <>
double UniformMass<gpu::cuda::CudaRigid3dTypes,sofa::defaulttype::Rigid3dMass>::getPotentialEnergy( const VecCoord& x )
{
    double e = 0;
    // gravity
    Vec3d g ( this->getContext()->getLocalGravity() );
    for (unsigned int i=0; i<x.size(); i++)
    {
        e += g*mass.getValue().mass*x[i].getCenter();
    }
    return e;
}

template <>
double UniformMass<gpu::cuda::CudaRigid3dTypes,sofa::defaulttype::Rigid3dMass>::getElementMass(unsigned int )
{
    return (double)(mass.getValue().mass);
}

template <>
void UniformMass<gpu::cuda::CudaRigid3dTypes, Rigid3dMass>::draw()
{
    if (!getContext()->getShowBehaviorModels())
        return;
    const VecCoord& x = *mstate->getX();
    defaulttype::Vec3d len;

    // The moment of inertia of a box is:
    //   m->_I(0,0) = M/REAL(12.0) * (ly*ly + lz*lz);
    //   m->_I(1,1) = M/REAL(12.0) * (lx*lx + lz*lz);
    //   m->_I(2,2) = M/REAL(12.0) * (lx*lx + ly*ly);
    // So to get lx,ly,lz back we need to do
    //   lx = sqrt(12/M * (m->_I(1,1)+m->_I(2,2)-m->_I(0,0)))
    // Note that RigidMass inertiaMatrix is already divided by M
    double m00 = mass.getValue().inertiaMatrix[0][0];
    double m11 = mass.getValue().inertiaMatrix[1][1];
    double m22 = mass.getValue().inertiaMatrix[2][2];
    len[0] = sqrt(m11+m22-m00);
    len[1] = sqrt(m00+m22-m11);
    len[2] = sqrt(m00+m11-m22);

    for (unsigned int i=0; i<x.size(); i++)
    {
        helper::gl::Axis::draw(x[i].getCenter(), x[i].getOrientation(), len);
    }
}

#endif // SOFA_GPU_CUDA_DOUBLE
#endif // SOFA_DEV

} // namespace mass

} // namespace component

} // namespace sofa

#endif
