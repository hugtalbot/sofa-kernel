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
#include <sofa/gpu/opencl/OpenCLSpatialGridContainer.inl>
#include <sofa/core/ObjectFactory.h>
#include <vector>
#include "oclRadixSort/CPUSortWithOpenCL.h"

#define DEBUG_TEXT(t) printf("\t%s\t %s %d\n",t,__FILE__,__LINE__);

struct GridParams
{
    float cellWidth;
    float invCellWidth;
    int cellMask;
    float halfCellWidth;
    float invHalfCellWidth;
};


namespace sofa
{

namespace component
{

namespace container
{

using namespace sofa::defaulttype;
using namespace sofa::gpu::opencl;
using namespace core::behavior;


SOFA_DECL_CLASS(OpenCLSpatialGridContainer)

int SpatialGridContainerOpenCLClass = core::RegisterObject("GPU support using OpenCL.")
        .add< SpatialGridContainer<OpenCLVec3fTypes> >()
        ;

template class SpatialGridContainer< OpenCLVec3fTypes >;
template class SpatialGrid< SpatialGridTypes< OpenCLVec3fTypes > >;



template class SpatialGridContainer< OpenCLVec3dTypes >;
template class SpatialGrid< SpatialGridTypes< OpenCLVec3dTypes > >;



} // namespace container

} // namespace component

namespace gpu
{

namespace opencl
{

int SpatialGridContainer_RadixSortTempStorage(unsigned int numElements)
{
    NOT_IMPLEMENTED();

}

typedef struct v
{
    int key;
    float value[3];
    bool operator()(v v1,v v2) {return(v1.key>v2.key);}
} e3f;

bool compare(e3f v1,e3f v2) {return (v1.key<v2.key);}

void SpatialGridContainer_RadixSort(sofa::gpu::opencl::_device_pointer keys,
        sofa::gpu::opencl::_device_pointer values,
        sofa::gpu::opencl::_device_pointer /*temp*/,
        unsigned int numElements,
        unsigned int /*keyBits*/,
        bool         /*flipBits*/)
{

    /*	int testKey[] = {32,71,12,45,26,80,53,33};
    	float value3[] = {0,0,0, 0,0,1, 0,1,0, 0,1,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1};

    	sofa::gpu::opencl::_device_pointer k;
    	sofa::gpu::opencl::_device_pointer v;

    	std::cout << 8*3*sizeof(float) << " " << 8*sizeof(Vec3f) << "\n";

    	sofa::gpu::opencl::myopenclCreateBuffer(0,&k.m,8*sizeof(int));
    	sofa::gpu::opencl::myopenclEnqueueWriteBuffer(0,k.m,0,testKey,8*sizeof(int));
    	sofa::gpu::opencl::myopenclCreateBuffer(0,&v.m,8*sizeof(Vec3f));
    	sofa::gpu::opencl::myopenclEnqueueWriteBuffer(0,v.m,0,value3,8*sizeof(Vec3f));*/

    std::cout << "radixsort\n";
    CPUSortWithOpenCL<int>::sort(keys,values,numElements);
    /*
    	sofa::gpu::opencl::myopenclEnqueueReadBuffer(0,value3,v.m,0,8*sizeof(Vec3f));
    	sofa::gpu::opencl::myopenclEnqueueReadBuffer(0,testKey,k.m,0,8*sizeof(int));
    	for(int i=0;i<8;i++)
    		std::cout << testKey[i] << " " << value3[i*3+0] <<" "<<  value3[i*3+1] <<" "<<  value3[i*3+2]<<"\n";*/

}




sofa::helper::OpenCLProgram* SpatialGridContainerOpenCLFloat_program;


void SpatialGridContainer_CreateProgramWithFloat()
{
    if(SpatialGridContainerOpenCLFloat_program==NULL)
    {
        std::map<std::string, std::string> types;
        types["Real"]="float";
        types["Real4"]="float4";

        SpatialGridContainerOpenCLFloat_program
            = new sofa::helper::OpenCLProgram(sofa::helper::OpenCLProgram::loadSource("OpenCLSpatialGridContainer.cl"),&types);

        SpatialGridContainerOpenCLFloat_program->buildProgram();

        std::cout << SpatialGridContainerOpenCLFloat_program->buildLog(0);
        sofa::gpu::opencl::myopenclShowError(__FILE__,__LINE__);
    }
}


sofa::helper::OpenCLKernel *SpatialGridContainer3f_computeHash_kernel;
void SpatialGridContainer3f_computeHash(int cellBits, float cellWidth, int nbPoints,gpu::opencl::_device_pointer particleIndex8,gpu::opencl::_device_pointer particleHash8, const gpu::opencl::_device_pointer x)
{
    DEBUG_TEXT("SpatialGridContainer3f_computeHash");
    BARRIER(x,__FILE__,__LINE__);

    std::cout << "particleHash8:" << particleHash8.offset << "\tparticleIndex8:" << particleIndex8.offset << "\tx" << x.offset << "\n";
//exit(0);

    int BSIZE = gpu::opencl::OpenCLMemoryManager<float>::BSIZE;
    SpatialGridContainer_CreateProgramWithFloat();

    GridParams p;
    p.cellWidth = cellWidth;
    p.invCellWidth = 1.0f/cellWidth;
    p.cellMask = (1<<cellBits)-1;
    p.halfCellWidth = cellWidth*0.5f;
    p.invHalfCellWidth = 2.0f/cellWidth;

    std::cout << "p.cellWidth:" << p.cellWidth << "\tp.invCellWidth:" << p.invCellWidth << "\tp.cellMask:" << p.cellMask << "\tp.halfCellWidth:" << p.halfCellWidth << "\tp.invHalfCellWidth:" << p.invHalfCellWidth << "\n";
    float d1[30];
    opencl::myopenclEnqueueReadBuffer(0,d1,x.m,x.offset,30*sizeof(float));
    std::cout << "x =(";
    for(int i=0; i<30; i++) {if(i%3==0 && i)std::cout<<")    ( "; std::cout << d1[i] << " ";}
    std::cout << ")\n";


    if(SpatialGridContainer3f_computeHash_kernel==NULL)SpatialGridContainer3f_computeHash_kernel
            = new sofa::helper::OpenCLKernel(SpatialGridContainerOpenCLFloat_program,"computeHashD");


    SpatialGridContainer3f_computeHash_kernel->setArg<_device_pointer>(0,&x);
    SpatialGridContainer3f_computeHash_kernel->setArg<_device_pointer>(1,&particleIndex8);
    int offsetIndex = particleIndex8.offset/sizeof(int);
    SpatialGridContainer3f_computeHash_kernel->setArg<int>(2,&offsetIndex);
    SpatialGridContainer3f_computeHash_kernel->setArg<_device_pointer>(3,&particleHash8);
    SpatialGridContainer3f_computeHash_kernel->setArg<int>(4,&nbPoints);
    SpatialGridContainer3f_computeHash_kernel->setArg<GridParams>(5,&p);

    size_t local_size[1];
    local_size[0]=BSIZE;

    size_t work_size[1];
    work_size[0]=((nbPoints%BSIZE)==0)?nbPoints:BSIZE*(nbPoints/BSIZE+1);

    SpatialGridContainer3f_computeHash_kernel->execute(0,1,NULL,work_size,local_size);	//note: num_device = const = 0




    opencl::myopenclEnqueueReadBuffer(0,d1,x.m,x.offset,30*sizeof(float));
    std::cout << "x =(";
    for(int i=0; i<30; i++) {if(i%3==0 && i)std::cout<<")    ( "; std::cout << d1[i] << " ";}
    std::cout << ")\n";



    DEBUG_TEXT("~SpatialGridContainer3f_computeHash");
    BARRIER(x,__FILE__,__LINE__);
//NOT_IMPLEMENTED();
}

sofa::helper::OpenCLKernel *SpatialGridContainer3f_findCellRange_kernel;
void SpatialGridContainer_findCellRange(int cellBits, int index0, float cellWidth, int nbPoints, const gpu::opencl::_device_pointer particleHash8,gpu::opencl::_device_pointer cellRange,gpu::opencl::_device_pointer cellGhost)
{
    DEBUG_TEXT("SpatialGridContainer_findCellRange");
    BARRIER(particleHash8,__FILE__,__LINE__);
    std::cout << index0;

    opencl::myopenclMemsetDevice(0,cellRange, 0, ((1<<cellBits)+1)*sizeof(int));

    int BSIZE = gpu::opencl::OpenCLMemoryManager<float>::BSIZE;
    SpatialGridContainer_CreateProgramWithFloat();

    if(SpatialGridContainer3f_findCellRange_kernel==NULL)SpatialGridContainer3f_findCellRange_kernel
            = new sofa::helper::OpenCLKernel(SpatialGridContainerOpenCLFloat_program,"findCellRangeD");


    SpatialGridContainer3f_findCellRange_kernel->setArg<int>(0,&index0);
    SpatialGridContainer3f_findCellRange_kernel->setArg<_device_pointer>(1,&particleHash8);
    SpatialGridContainer3f_findCellRange_kernel->setArg<_device_pointer>(2,&cellRange);
    SpatialGridContainer3f_findCellRange_kernel->setArg<_device_pointer>(3,&cellGhost);
    int nbPoints8 = nbPoints*8;
    SpatialGridContainer3f_findCellRange_kernel->setArg<int>(4,&nbPoints8);

    size_t local_size[1];
    local_size[0]=BSIZE;

    size_t work_size[1];
    work_size[0]=((nbPoints8%BSIZE)==0)?nbPoints8:BSIZE*(nbPoints8/BSIZE+1);

    SpatialGridContainer3f_findCellRange_kernel->execute(0,1,NULL,work_size,local_size);	//note: num_device = const = 0

    DEBUG_TEXT("~SpatialGridContainer_findCellRange");
    BARRIER(particleHash8,__FILE__,__LINE__);
}








void SpatialGridContainer3f1_computeHash(int /*cellBits*/, float /*cellWidth*/, int /*nbPoints*/,gpu::opencl::_device_pointer /*particleIndex8*/,gpu::opencl::_device_pointer /*particleHash8*/, const gpu::opencl::_device_pointer /*x*/) {NOT_IMPLEMENTED();}
//void SpatialGridContainer3f_reorderData(int nbPoints, const gpu::opencl::_device_pointer particleHash,gpu::opencl::_device_pointer sorted, const gpu::opencl::_device_pointer x){NOT_IMPLEMENTED();}
//void SpatialGridContainer3f1_reorderData(int nbPoints, const gpu::opencl::_device_pointer particleHash,gpu::opencl::_device_pointer sorted, const gpu::opencl::_device_pointer x){NOT_IMPLEMENTED();}


}	//opencl
}	//gpu

} // namespace sofa
