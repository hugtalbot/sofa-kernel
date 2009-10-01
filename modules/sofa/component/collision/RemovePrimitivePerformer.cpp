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
#include <sofa/component/collision/RemovePrimitivePerformer.h>

#include <sofa/helper/Factory.inl>

namespace sofa
{

namespace component
{

namespace collision
{
#ifndef WIN32
helper::Creator<InteractionPerformer::InteractionPerformerFactory, RemovePrimitivePerformer >  RemovePrimitivePerformerClass("RemovePrimitive");
#endif
void RemovePrimitivePerformer::execute()
{
    BodyPicked picked=this->interactor->getBodyPicked();
    if (!picked.body) return;
    core::CollisionElementIterator collisionElement( picked.body, picked.indexCollisionElement);

    sofa::core::componentmodel::topology::TopologyModifier* topologyModifier;
    picked.body->getContext()->get(topologyModifier);

    // Handle Removing of topological element (from any type of topology)
    if(topologyModifier) topologyChangeManager.removeItemsFromCollisionModel(collisionElement);
    picked.body=NULL;
    this->interactor->setBodyPicked(picked);
}
}
}
}

