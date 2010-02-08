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
// Author: Hadrien Courtecuisse
//
// Copyright: See COPYING file that comes with this distribution
#ifndef SOFA_COMPONENT_COLLISION_PRECOMPUTEDLINEARSOLVER_INL
#define SOFA_COMPONENT_COLLISION_PRECOMPUTEDLINEARSOLVER_INL

#include "PrecomputedLinearSolver.h"
#include <sofa/component/linearsolver/NewMatMatrix.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/component/linearsolver/SparseMatrix.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <math.h>
#include <sofa/helper/system/thread/CTime.h>
#include <sofa/component/forcefield/TetrahedronFEMForceField.h>
#include <sofa/gpu/cuda/CudaTetrahedronFEMForceField.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/component/linearsolver/MatrixLinearSolver.h>
#include <sofa/helper/system/thread/CTime.h>
#include <sofa/component/container/RotationFinder.h>
#include <sofa/core/componentmodel/behavior/LinearSolver.h>

#include <sofa/component/odesolver/EulerImplicitSolver.h>
#include <sofa/component/linearsolver/CGLinearSolver.h>
#include <sofa/component/linearsolver/PCGLinearSolver.h>

#ifdef SOFA_HAVE_CSPARSE
#include <sofa/component/linearsolver/SparseCholeskySolver.h>
#include <sofa/component/linearsolver/CompressedRowSparseMatrix.h>
#else
#include <sofa/component/linearsolver/CholeskySolver.h>
#endif

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::component::odesolver;
using namespace sofa::component::linearsolver;

template<class TMatrix,class TVector>
PrecomputedLinearSolver<TMatrix,TVector>::PrecomputedLinearSolver()
    : f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )
    , use_file( initData(&use_file,true,"use_file","Dump system matrix in a file") )
    , solverName(initData(&solverName, std::string(""), "solverName", "Name of the solver to use to precompute the first matrix"))
    , init_MaxIter( initData(&init_MaxIter,5000,"init_MaxIter","Max Iter use to precompute the first matrix") )
    , init_Tolerance( initData(&init_Tolerance,1e-20,"init_Tolerance","Tolerance use to precompute the first matrix") )
    , init_Threshold( initData(&init_Threshold,1e-35,"init_Threshold","Threshold use to precompute the first matrix") )

{
    first = true;
}

template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector>::setSystemMBKMatrix(double mFact, double bFact, double kFact)
{
    // Update the matrix only the first time

    if (first)
    {
        init_mFact = mFact;
        init_bFact = bFact;
        init_kFact = kFact;
        Inherit::setSystemMBKMatrix(mFact,bFact,kFact);
        loadMatrix();
        first = false;
    }
}

//Solve x = R * M^-1 * R^t * b
template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector>::solve (TMatrix& /*M*/, TVector& z, TVector& r)
{
    z = *this->currentGroup->systemMatrix * r;
}

template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector >::loadMatrix()
{
    unsigned systemSize = this->currentGroup->systemMatrix->rowSize();
    dt = this->getContext()->getDt();

    EulerImplicitSolver* EulerSolver;
    this->getContext()->get(EulerSolver);
    factInt = 1.0; // christian : it is not a compliance... but an admittance that is computed !
    if (EulerSolver) factInt = EulerSolver->getPositionIntegrationFactor(); // here, we compute a compliance

    std::stringstream ss;
    ss << this->getContext()->getName() << "-" << systemSize << "-" << dt << ".comp";
    std::ifstream compFileIn(ss.str().c_str(), std::ifstream::binary);

    if(compFileIn.good() && use_file.getValue())
    {
        cout << "file open : " << ss.str() << " compliance being loaded" << endl;
        compFileIn.read((char*) (*this->currentGroup->systemMatrix)[0], systemSize * systemSize * sizeof(Real));
        compFileIn.close();
    }
    else
    {
        if (solverName.getValue().empty()) loadMatrixWithCSparse();
        else loadMatrixWithSolver();

        if (use_file.getValue())
        {
            std::ofstream compFileOut(ss.str().c_str(), std::fstream::out | std::fstream::binary);
            compFileOut.write((char*)(*this->currentGroup->systemMatrix)[0], systemSize * systemSize*sizeof(Real));
            compFileOut.close();
        }
    }

    for (unsigned int j=0; j<systemSize; j++)
    {
        for (unsigned i=0; i<systemSize; i++)
        {
            this->currentGroup->systemMatrix->set(j,i,this->currentGroup->systemMatrix->element(j,i)/factInt);
        }
    }
}

template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector>::loadMatrixWithCSparse()
{
#ifdef SOFA_HAVE_CSPARSE
    cout << "Compute the initial invert matrix with CS_PARSE" << endl;

    CompressedRowSparseMatrix<double> matSolv;
    FullVector<double> r;
    FullVector<double> b;

    unsigned systemSize = this->currentGroup->systemMatrix->colSize();

    matSolv.resize(systemSize,systemSize);
    r.resize(systemSize);
    b.resize(systemSize);
    SparseCholeskySolver<CompressedRowSparseMatrix<double>, FullVector<double> > solver;

    for (unsigned int j=0; j<systemSize; j++)
    {
        for (unsigned int i=0; i<systemSize; i++)
        {
            if (this->currentGroup->systemMatrix->element(j,i)!=0) matSolv.set(j,i,(double)this->currentGroup->systemMatrix->element(j,i));
        }
        b.set(j,0.0);
    }

    std::cout << "Precomputing constraint correction LU decomposition " << std::endl;
    solver.invert(matSolv);

    for (unsigned int j=0; j<systemSize; j++)
    {
        std::cout.precision(2);
        std::cout << "Precomputing constraint correction : " << std::fixed << (float)j/(float)systemSize*100.0f << " %   " << '\xd';
        std::cout.flush();

        if (j>0) b.set(j-1,0.0);
        b.set(j,1.0);

        solver.solve(matSolv,r,b);
        for (unsigned int i=0; i<systemSize; i++)
        {
            this->currentGroup->systemMatrix->set(j,i,r.element(i) * factInt);
        }
    }
    std::cout << "Precomputing constraint correction : " << std::fixed << 100.0f << " %   " << '\xd';
    std::cout.flush();

#else
    std::cout << "WARNING ; you don't have CS_parse solvername will be use" << std::endl;
    loadMatrixWithSolver();
#endif
}

template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector >::loadMatrixWithSolver()
{
// 	usePrecond = false;//Don'Use precond during precomputing
//
// 	cout << "Compute the initial invert matrix with solver" << endl;
//
// 	behavior::MechanicalState<DataTypes>* mstate = dynamic_cast< behavior::MechanicalState<DataTypes>* >(this->getContext()->getMechanicalState());
// 	if (mstate==NULL) {
// 		serr << "PrecomputedWarpPreconditioner can't find Mstate" << sendl;
// 		return;
// 	}
// 	const VecDeriv& v0 = *mstate->getV();
// 	unsigned dof_on_node = v0[0].size();
// 	unsigned nbNodes = v0.size();
// 	unsigned systemSize = nbNodes*dof_on_node;
//
// 	std::stringstream ss;
// 	//ss << this->getContext()->getName() << "_CPP.comp";
// 	ss << this->getContext()->getName() << "-" << systemSize << "-" << dt << ".comp";
// 	std::ifstream compFileIn(ss.str().c_str(), std::ifstream::binary);
//
// 	EulerImplicitSolver* EulerSolver;
// 	this->getContext()->get(EulerSolver);
//
// 	// for the initial computation, the gravity has to be put at 0
// 	const Vec3d gravity = this->getContext()->getGravityInWorld();
// 	const Vec3d gravity_zero(0.0,0.0,0.0);
// 	this->getContext()->setGravityInWorld(gravity_zero);
//
// 	PCGLinearSolver<GraphScatteredMatrix,GraphScatteredVector>* PCGlinearSolver;
// 	CGLinearSolver<GraphScatteredMatrix,GraphScatteredVector>* CGlinearSolver;
// 	core::componentmodel::behavior::LinearSolver* linearSolver;
//
//         if (solverName.getValue().empty()) {
//             this->getContext()->get(CGlinearSolver);
//             this->getContext()->get(PCGlinearSolver);
//             this->getContext()->get(linearSolver);
//         } else {
//             core::objectmodel::BaseObject* ptr = NULL;
//             this->getContext()->get(ptr, solverName.getValue());
//             PCGlinearSolver = dynamic_cast<PCGLinearSolver<GraphScatteredMatrix,GraphScatteredVector>*>(ptr);
//             CGlinearSolver = dynamic_cast<CGLinearSolver<GraphScatteredMatrix,GraphScatteredVector>*>(ptr);
//             linearSolver = dynamic_cast<core::componentmodel::behavior::LinearSolver*>(ptr);
//         }
//
//         if(EulerSolver && CGlinearSolver)
// 		sout << "use EulerImplicitSolver &  CGLinearSolver" << sendl;
// 	else if(EulerSolver && PCGlinearSolver)
// 		sout << "use EulerImplicitSolver &  PCGLinearSolver" << sendl;
// 	else if(EulerSolver && linearSolver)
// 		sout << "use EulerImplicitSolver &  LinearSolver" << sendl;
// 	else if(EulerSolver) {
// 		sout << "use EulerImplicitSolver" << sendl;
// 	} else {
// 		serr<<"PrecomputedContactCorrection must be associated with EulerImplicitSolver+LinearSolver for the precomputation\nNo Precomputation" << sendl;
// 		return;
// 	}
// 	VecId lhId = core::componentmodel::behavior::BaseMechanicalState::VecId::velocity();
// 	VecId rhId = core::componentmodel::behavior::BaseMechanicalState::VecId::force();
//
//
// 	mstate->vAvail(lhId);
// 	mstate->vAlloc(lhId);
// 	mstate->vAvail(rhId);
// 	mstate->vAlloc(rhId);
// 	std::cout << "System: (" << init_mFact << " * M + " << init_bFact << " * B + " << init_kFact << " * K) " << lhId << " = " << rhId << std::endl;
// 	if (linearSolver) {
// 	    std::cout << "System Init Solver: " << linearSolver->getName() << " (" << linearSolver->getClassName() << ")" << std::endl;
// 	    linearSolver->setSystemMBKMatrix(init_mFact, init_bFact, init_kFact);
// 	}
//
// 	VecDeriv& force = *mstate->getVecDeriv(rhId.index);
// 	force.clear();
// 	force.resize(nbNodes);
//
// 	///////////////////////// CHANGE THE PARAMETERS OF THE SOLVER /////////////////////////////////
// 	double buf_tolerance=0, buf_threshold=0;
// 	int buf_maxIter=0;
// 	if(CGlinearSolver) {
// 		buf_tolerance = (double) CGlinearSolver->f_tolerance.getValue();
// 		buf_maxIter   = (int) CGlinearSolver->f_maxIter.getValue();
// 		buf_threshold = (double) CGlinearSolver->f_smallDenominatorThreshold.getValue();
// 		CGlinearSolver->f_tolerance.setValue(init_Tolerance.getValue());
// 		CGlinearSolver->f_maxIter.setValue(init_MaxIter.getValue());
// 		CGlinearSolver->f_smallDenominatorThreshold.setValue(init_Threshold.getValue());
// 	} else if(PCGlinearSolver) {
// 		buf_tolerance = (double) PCGlinearSolver->f_tolerance.getValue();
// 		buf_maxIter   = (int) PCGlinearSolver->f_maxIter.getValue();
// 		buf_threshold = (double) PCGlinearSolver->f_smallDenominatorThreshold.getValue();
// 		PCGlinearSolver->f_tolerance.setValue(init_Tolerance.getValue());
// 		PCGlinearSolver->f_maxIter.setValue(init_MaxIter.getValue());
// 		PCGlinearSolver->f_smallDenominatorThreshold.setValue(init_Threshold.getValue());
// 	}
// 	///////////////////////////////////////////////////////////////////////////////////////////////
//
// 	VecDeriv& velocity = *mstate->getVecDeriv(lhId.index);
// 	VecDeriv velocity0 = velocity;
// 	VecCoord& pos = *mstate->getX();
// 	VecCoord pos0 = pos;
//
// 	for(unsigned int f = 0 ; f < nbNodes ; f++) {
// 		std::cout.precision(2);
// 		std::cout << "Precomputing constraint correction : " << std::fixed << (float)f/(float)nbNodes*100.0f << " %   " << '\xd';
// 		std::cout.flush();
// 		Deriv unitary_force;
//
// 		for (unsigned int i=0; i<dof_on_node; i++) {
// 			unitary_force.clear();
// 			unitary_force[i]=1.0;
// 			force[f] = unitary_force;
//
// 			velocity.clear();
// 			velocity.resize(nbNodes);
//
// 			if(f*dof_on_node+i <2 ) {
// 				EulerSolver->f_verbose.setValue(true);
// 				EulerSolver->f_printLog.setValue(true);
// 				serr<<"getF : "<<force<<sendl;
// 			}
//
// 			if (linearSolver) {
// 				linearSolver->setSystemRHVector(rhId);
// 				linearSolver->setSystemLHVector(lhId);
// 				linearSolver->solveSystem();
// 			}
//
// 			if (linearSolver && f*dof_on_node+i == 0) linearSolver->freezeSystemMatrix(); // do not recompute the matrix for the rest of the precomputation
//
// 			//velocity = *mstate->getV();
// 			double fact = factInt / unitary_force[i];
//
// 			if(f*dof_on_node+i < 2) {
// 				EulerSolver->f_verbose.setValue(false);
// 				EulerSolver->f_printLog.setValue(false);
// 				serr<<"getV : "<<velocity<<sendl;
// 			}
// 			for (unsigned int v=0; v<nbNodes; v++) {
// 				for (unsigned int j=0; j<dof_on_node; j++) {
// 					this->currentGroup->systemMatrix->set(v*dof_on_node+j,f*dof_on_node+i,(Real)(fact * velocity[v][j]));
// 				}
// 			}
// 		}
// 		unitary_force.clear();
// 		force[f] = unitary_force;
// 	}
// 	std::cout << "Precomputing constraint correction : " << std::fixed << 100.0f << " %   " << '\xd';
// 	std::cout.flush();
//
// 	///////////////////////////////////////////////////////////////////////////////////////////////
//
// 	if (linearSolver) linearSolver->updateSystemMatrix(); // do not recompute the matrix for the rest of the precomputation
//
// 	///////////////////////// RESET PARAMETERS AT THEIR PREVIOUS VALUE /////////////////////////////////
// 	// gravity is reset at its previous value
// 	this->getContext()->setGravityInWorld(gravity);
//
// 	if(CGlinearSolver) {
// 		CGlinearSolver->f_tolerance.setValue(buf_tolerance);
// 		CGlinearSolver->f_maxIter.setValue(buf_maxIter);
// 		CGlinearSolver->f_smallDenominatorThreshold.setValue(buf_threshold);
// 	} else if(PCGlinearSolver) {
// 		PCGlinearSolver->f_tolerance.setValue(buf_tolerance);
// 		PCGlinearSolver->f_maxIter.setValue(buf_maxIter);
// 		PCGlinearSolver->f_smallDenominatorThreshold.setValue(buf_threshold);
// 	}
//
// 	//Reset the velocity
// 	for (unsigned int i=0; i<velocity0.size(); i++) velocity[i]=velocity0[i];
// 	//Reset the position
// 	for (unsigned int i=0; i<pos0.size(); i++) pos[i]=pos0[i];
//
// 	mstate->vFree(lhId);
// 	mstate->vFree(rhId);
//
// 	usePrecond = true;
}

template<class TMatrix,class TVector>
void PrecomputedLinearSolver<TMatrix,TVector>::invert(TMatrix& /*M*/) {}

template<class TMatrix,class TVector>
bool PrecomputedLinearSolver<TMatrix,TVector>::addJMInvJt(defaulttype::BaseMatrix* result, defaulttype::BaseMatrix* J, double fact)
{
    if (SparseMatrix<double>* j = dynamic_cast<SparseMatrix<double>*>(J))
    {
        ComputeResult(result, *j, (float) fact);
    }
    else if (SparseMatrix<float>* j = dynamic_cast<SparseMatrix<float>*>(J))
    {
        ComputeResult(result, *j, (float) fact);
    } return false;

    return true;
}

template<class TMatrix,class TVector> template<class JMatrix>
void PrecomputedLinearSolver<TMatrix,TVector>::ComputeResult(defaulttype::BaseMatrix * result,JMatrix& J, float fact)
{
    unsigned nl = 0;
    for (typename JMatrix::LineConstIterator jit1 = J.begin(); jit1 != J.end(); jit1++) nl++;

    internalData.JMinv.clear();
    internalData.JMinv.resize(nl,this->currentGroup->systemMatrix->rowSize());

    nl = 0;
    for (typename JMatrix::LineConstIterator jit1 = J.begin(); jit1 != J.end(); jit1++)
    {
        for (unsigned c = 0; c<this->currentGroup->systemMatrix->rowSize(); c++)
        {
            Real v = 0.0;
            for (typename JMatrix::LElementConstIterator i1 = jit1->second.begin(); i1 != jit1->second.end(); i1++)
            {
                v += this->currentGroup->systemMatrix->element(i1->first,c) * i1->second;
            }
            internalData.JMinv.add(nl,c,v);
        }
        nl++;
    }

    //compute Result = JRMinv * Jt

    nl = 0;
    for (typename JMatrix::LineConstIterator jit1 = J.begin(); jit1 != J.end(); jit1++)
    {
        int l = jit1->first;
        for (typename JMatrix::LineConstIterator jit2 = J.begin(); jit2 != J.end(); jit2++)
        {
            int c = jit2->first;
            Real res = 0.0;
            for (typename JMatrix::LElementConstIterator i1 = jit2->second.begin(); i1 != jit2->second.end(); i1++)
            {
                res += internalData.JMinv.element(nl,i1->first) * i1->second;
            }
            result->add(l,c,res*fact);
        }
        nl++;
    }
}

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
