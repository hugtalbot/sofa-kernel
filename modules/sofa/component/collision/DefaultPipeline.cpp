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
#include <sofa/component/collision/DefaultPipeline.h>
#include <sofa/core/CollisionModel.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/core/ObjectFactory.h>

#ifdef SOFA_DUMP_VISITOR_INFO
#include <sofa/simulation/common/Visitor.h>
#endif

#include <sofa/helper/system/gl.h>

#define VERBOSE(a) if (bVerbose.getValue()) a; else {}

namespace sofa
{

namespace component
{

namespace collision
{

using namespace core;
using namespace core::objectmodel;
using namespace core::componentmodel::collision;
using namespace sofa::defaulttype;

SOFA_DECL_CLASS(DefaultPipeline)

int DefaultPipelineClass = core::RegisterObject("The default collision detection and modeling pipeline")
        .add< DefaultPipeline >()
        .addAlias("CollisionPipeline")
        ;

DefaultPipeline::DefaultPipeline()
    : bVerbose(initData(&bVerbose, false, "verbose","Display current step information"))
    , bDraw(initData(&bDraw, false, "draw","Draw detected collisions"))
    , depth(initData(&depth, 6, "depth","Max depth of bounding trees"))
{
}

typedef simulation::Node::ctime_t ctime_t;

void DefaultPipeline::doCollisionReset()
{
    core::objectmodel::BaseContext* scene = getContext();
    simulation::Node* node = dynamic_cast<simulation::Node*>(scene);
    if (node && !node->getLogTime()) node=NULL; // Only use node for time logging
    ctime_t t0 = 0;
    const std::string category = "collision";

    VERBOSE(sout << "DefaultPipeline::doCollisionReset, Reset collisions"<<sendl);
    // clear all contacts
    if (contactManager!=NULL)
    {
        const sofa::helper::vector<Contact*>& contacts = contactManager->getContacts();
        for (sofa::helper::vector<Contact*>::const_iterator it = contacts.begin(); it!=contacts.end(); it++)
        {
            (*it)->removeResponse();
        }
    }
    // clear all collision groups
    if (groupManager!=NULL)
    {
        if (node) t0 = node->startTime();
        groupManager->clearGroups(scene);
        if (node) t0 = node->endTime(t0, category, groupManager, this);
    }
}

void DefaultPipeline::doCollisionDetection(const sofa::helper::vector<core::CollisionModel*>& collisionModels)
{
    //serr<<"DefaultPipeline::doCollisionDetection"<<sendl;

    core::objectmodel::BaseContext* scene = getContext();
    simulation::Node* node = dynamic_cast<simulation::Node*>(scene);
    if (node && !node->getLogTime()) node=NULL; // Only use node for time logging
    ctime_t t0 = 0;
    const std::string category = "collision";

    VERBOSE(sout << "DefaultPipeline::doCollisionDetection, Compute Bounding Trees"<<sendl);
    // First, we compute a bounding volume for the collision model (for example bounding sphere)
    // or we have loaded a collision model that knows its other model

    sofa::helper::vector<CollisionModel*> vectBoundingVolume;
    {
        if (node) t0 = node->startTime();
#ifdef SOFA_DUMP_VISITOR_INFO
        simulation::Visitor::printNode("ComputeBoundingTree");
#endif
        const bool continuous = intersectionMethod->useContinuous();
        const double dt       = getContext()->getDt();

        sofa::helper::vector<CollisionModel*>::const_iterator it = collisionModels.begin();
        sofa::helper::vector<CollisionModel*>::const_iterator itEnd = collisionModels.end();
        int nActive = 0;
        for (; it != itEnd; it++)
        {
            VERBOSE(sout << "DefaultPipeline::doCollisionDetection, consider model "<<(*it)->getName()<<sendl);
            if (!(*it)->isActive()) continue;
            if (continuous)
                (*it)->computeContinuousBoundingTree(dt, depth.getValue());
            else
                (*it)->computeBoundingTree(depth.getValue());
            vectBoundingVolume.push_back ((*it)->getFirst());
            ++nActive;
        }
        if (node) t0 = node->endTime(t0, "collision/bbox", this);
#ifdef SOFA_DUMP_VISITOR_INFO
        simulation::Visitor::printCloseNode("ComputeBoundingTree");
#endif
        VERBOSE(sout << "DefaultPipeline::doCollisionDetection, Computed "<<nActive<<" BBoxs"<<sendl);
    }
    // then we start the broad phase
    if (broadPhaseDetection==NULL) return; // can't go further
    VERBOSE(sout << "DefaultPipeline::doCollisionDetection, BroadPhaseDetection "<<broadPhaseDetection->getName()<<sendl);
    if (node) t0 = node->startTime();
#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printNode("BroadPhase");
#endif
    intersectionMethod->beginBroadPhase();
    broadPhaseDetection->beginBroadPhase();
    broadPhaseDetection->addCollisionModels(vectBoundingVolume);  // detection is done there
    broadPhaseDetection->endBroadPhase();
    intersectionMethod->endBroadPhase();

#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printCloseNode("BroadPhase");
#endif
    if (node) t0 = node->endTime(t0, category, broadPhaseDetection, this);

    // then we start the narrow phase
    if (narrowPhaseDetection==NULL) return; // can't go further
    VERBOSE(sout << "DefaultPipeline::doCollisionDetection, NarrowPhaseDetection "<<narrowPhaseDetection->getName()<<sendl);
    if (node) t0 = node->startTime();

#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printNode("NarrowPhase");
#endif
    intersectionMethod->beginNarrowPhase();
    narrowPhaseDetection->beginNarrowPhase();
    sofa::helper::vector<std::pair<CollisionModel*, CollisionModel*> >& vectCMPair = broadPhaseDetection->getCollisionModelPairs();
    VERBOSE(sout << "DefaultPipeline::doCollisionDetection, "<< vectCMPair.size()<<" colliding model pairs"<<sendl);
    narrowPhaseDetection->addCollisionPairs(vectCMPair);
    narrowPhaseDetection->endNarrowPhase();
    intersectionMethod->endNarrowPhase();

#ifdef SOFA_DUMP_VISITOR_INFO
    simulation::Visitor::printCloseNode("NarrowPhase");
#endif
    if (node) t0 = node->endTime(t0, category, narrowPhaseDetection, this);
}

void DefaultPipeline::doCollisionResponse()
{
    core::objectmodel::BaseContext* scene = getContext();
    simulation::Node* node = dynamic_cast<simulation::Node*>(scene);
    if (node && !node->getLogTime()) node=NULL; // Only use node for time logging
    ctime_t t0 = 0;
    const std::string category = "collision";

    // then we start the creation of contacts
    if (contactManager==NULL) return; // can't go further
    VERBOSE(sout << "Create Contacts "<<contactManager->getName()<<sendl);
    if (node) t0 = node->startTime();
    contactManager->createContacts(narrowPhaseDetection->getDetectionOutputs());
    if (node) t0 = node->endTime(t0, category, contactManager, this);

    // finally we start the creation of collisionGroup

    const sofa::helper::vector<Contact*>& contacts = contactManager->getContacts();

    // First we remove all contacts with non-simulated objects and directly add them
    sofa::helper::vector<Contact*> notStaticContacts;

    for (sofa::helper::vector<Contact*>::const_iterator it = contacts.begin(); it!=contacts.end(); it++)
    {
        Contact* c = *it;
        if (!c->getCollisionModels().first->isSimulated())
        {
            c->createResponse(c->getCollisionModels().second->getContext());
        }
        else if (!c->getCollisionModels().second->isSimulated())
        {
            c->createResponse(c->getCollisionModels().first->getContext());
        }
        else
            notStaticContacts.push_back(c);
    }


    if (groupManager==NULL)
    {
        VERBOSE(sout << "Linking all contacts to Scene"<<sendl);
        for (sofa::helper::vector<Contact*>::const_iterator it = notStaticContacts.begin(); it!=notStaticContacts.end(); it++)
        {
            (*it)->createResponse(scene);
        }
    }
    else
    {
        VERBOSE(sout << "Create Groups "<<groupManager->getName()<<sendl);
        if (node) t0 = node->startTime();
        groupManager->createGroups(scene, notStaticContacts);
        if (node) t0 = node->endTime(t0, category, groupManager, this);
    }
}

void DefaultPipeline::draw()
{
    if (!bDraw.getValue()) return;
    if (!narrowPhaseDetection) return;
#if 0
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINES);
    DetectionOutputMap& outputsMap = narrowPhaseDetection->getDetectionOutputs();
    for (DetectionOutputMap::iterator it = outputsMap.begin(); it!=outputsMap.end(); it++)
    {
        /*
        DetectionOutputVector& outputs = it->second;
        for (DetectionOutputVector::iterator it2 = outputs.begin(); it2!=outputs.end(); it2++)
        {
            DetectionOutput* d = &*it2;
            if (d->distance<0)
                glColor3f(1.0f,0.5f,0.5f);
            else
                glColor3f(0.5f,1.0f,0.5f);
            glVertex3dv(d->point[0].ptr());
            glVertex3dv(d->point[1].ptr());
        }
        */
    }
    glEnd();
    glLineWidth(1);
#endif
}
} // namespace collision

} // namespace component

} // namespace sofa

