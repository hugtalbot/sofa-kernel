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
#include <sofa/helper/system/config.h>
#include <sofa/component/initComponentMisc.h>
#include <sofa/component/initMiscTopology.h>
#include <sofa/component/initMiscMapping.h>
#include <sofa/component/initMiscForcefield.h>
#include <sofa/component/initMiscFEM.h>
#include <sofa/component/initMiscEngine.h>
#include <sofa/component/initMiscCollision.h>
#include <sofa/component/initMiscSolver.h>
#include <sofa/component/initMisc.h>
#ifdef SOFA_DEV
#include <sofa/component/initMiscTopologyDev.h>
#include <sofa/component/initMiscMappingDev.h>
#include <sofa/component/initMiscForcefieldDev.h>
#include <sofa/component/initMiscFEMDev.h>
#include <sofa/component/initMiscCollisionDev.h>
#include <sofa/component/initMiscSolverDev.h>
#include <sofa/component/initMiscDev.h>
#include <sofa/component/initAdvancedInteraction.h>
#endif

namespace sofa
{

namespace component
{


void initComponentMisc()
{
    static bool first = true;
    if (first)
    {
        first = false;
    }

    initMiscTopology();
    initMiscMapping();
    initMiscForcefield();
    initMiscFEM();
    initMiscEngine();
    initMiscCollision();
    initMiscSolver();
    initMisc();
#ifdef SOFA_DEV
    initMiscTopologyDev();
    initMiscMappingDev();
    initMiscForcefieldDev();
    initMiscFEMDev();
    initMiscCollisionDev();
    initMiscSolverDev();
    initMiscDev();
    initAdvancedInteraction();
#endif
}

} // namespace component

} // namespace sofa
