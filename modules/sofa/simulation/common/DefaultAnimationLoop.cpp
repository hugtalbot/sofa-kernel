/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
#include <sofa/simulation/common/DefaultAnimationLoop.h>
#include <sofa/core/ObjectFactory.h>

#include <sofa/simulation/common/AnimateVisitor.h>
#include <sofa/simulation/common/UpdateContextVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>
#include <sofa/simulation/common/PropagateEventVisitor.h>
#include <sofa/simulation/common/BehaviorUpdatePositionVisitor.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>
#include <sofa/simulation/common/UpdateMappingEndEvent.h>
#include <sofa/simulation/common/UpdateBoundingBoxVisitor.h>
#include <sofa/simulation/common/xml/NodeElement.h>

#include <sofa/helper/system/SetDirectory.h>
//#include <sofa/helper/system/PipeProcess.h>
#include <sofa/helper/AdvancedTimer.h>

#include <sofa/core/visual/VisualParams.h>

#include <stdlib.h>
#include <math.h>
#include <algorithm>

namespace sofa
{

namespace simulation
{

SOFA_DECL_CLASS(DefaultAnimationLoop)

int DefaultAnimationLoopClass = core::RegisterObject("The simplest animation loop, created by default when user do not put on scene")
        .add< DefaultAnimationLoop >()
        ;



DefaultAnimationLoop::DefaultAnimationLoop(simulation::Node* _gnode)
    : Inherit()
    , gnode(_gnode)
{
    //assert(gnode);
}

DefaultAnimationLoop::~DefaultAnimationLoop()
{

}

void DefaultAnimationLoop::init()
{
    if (!gnode)
        gnode = dynamic_cast<simulation::Node*>(this->getContext());
}

void DefaultAnimationLoop::setNode( simulation::Node* n )
{
    gnode=n;
}

void DefaultAnimationLoop::step(const core::ExecParams* params, SReal dt)
{
    if (dt == 0)
        dt = this->gnode->getDt();


#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printNode("Step");
#endif

    {
        AnimateBeginEvent ev ( dt );
        PropagateEventVisitor act ( params, &ev );
        gnode->execute ( act );
    }

    SReal startTime = gnode->getTime();


    sofa::helper::AdvancedTimer::stepBegin("BehaviorUpdatePositionVisitor");
    BehaviorUpdatePositionVisitor beh(params , dt);
    gnode->execute ( beh );
    sofa::helper::AdvancedTimer::stepEnd("BehaviorUpdatePositionVisitor");


    sofa::helper::AdvancedTimer::stepBegin("AnimateVisitor");
    AnimateVisitor act(params, dt);
    gnode->execute ( act );
    sofa::helper::AdvancedTimer::stepEnd("AnimateVisitor");


    sofa::helper::AdvancedTimer::stepBegin("UpdateSimulationContextVisitor");
    gnode->setTime ( startTime + dt );
    gnode->execute< UpdateSimulationContextVisitor >(params);
    sofa::helper::AdvancedTimer::stepEnd("UpdateSimulationContextVisitor");

    {
        AnimateEndEvent ev ( dt );
        PropagateEventVisitor act ( params, &ev );
        gnode->execute ( act );
    }

    sofa::helper::AdvancedTimer::stepBegin("UpdateMapping");
    //Visual Information update: Ray Pick add a MechanicalMapping used as VisualMapping
    gnode->execute< UpdateMappingVisitor >(params);
    {
        UpdateMappingEndEvent ev ( dt );
        PropagateEventVisitor act ( params , &ev );
        gnode->execute ( act );
    }
    sofa::helper::AdvancedTimer::stepEnd("UpdateMapping");

#ifndef SOFA_NO_UPDATE_BBOX
    sofa::helper::AdvancedTimer::stepBegin("UpdateBBox");
    gnode->execute< UpdateBoundingBoxVisitor >(params);
    sofa::helper::AdvancedTimer::stepEnd("UpdateBBox");
#endif
#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printCloseNode("Step");
#endif


}


} // namespace simulation

} // namespace sofa
