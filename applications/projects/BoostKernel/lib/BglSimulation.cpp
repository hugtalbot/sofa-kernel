/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation: BglSimulation
//
// Description:
//
//
// Author: Francois Faure in The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "BglSimulation.h"
#include "BglNode.h"

#include <sofa/core/ObjectFactory.h>



namespace sofa
{
namespace simulation
{
namespace bgl
{


Simulation* getSimulation()
{
    if ( Simulation::Simulation::theSimulation==NULL )
        setSimulation(new BglSimulation);
    return simulation::getSimulation();
}

BglSimulation::BglSimulation()
{

    //-------------------------------------------------------------------------------------------------------
    //Adding aliases to convert Tree based components to Bgl components
    sofa::core::ObjectFactory::ClassEntry* classDefaultCollisionGroupManager;
    sofa::core::ObjectFactory::AddAlias("DefaultCollisionGroupManager",
            "BglCollisionGroupManager", true, &classDefaultCollisionGroupManager);

    sofa::core::ObjectFactory::ClassEntry* classCollisionGroup;
    sofa::core::ObjectFactory::AddAlias("CollisionGroup",
            "BglCollisionGroupManager", true, &classCollisionGroup);

    //Should this alias be added?
    sofa::core::ObjectFactory::ClassEntry* classTreeCollisionGroupManager;
    sofa::core::ObjectFactory::AddAlias("TreeCollisionGroupManager",
            "BglCollisionGroupManager", true, &classTreeCollisionGroupManager);


}




/// Create a graph node and attach a new Node to it, then return the Node
Node* BglSimulation::newNode(const std::string& name)
{
    return new BglNode(name);
}


/**
Data: hgraph, rgraph
 Result: hroots, interaction groups, all nodes initialized.
    */
void BglSimulation::init(Node* root )
{
    Simulation::init(root);
    BglGraphManager::getInstance()->update();
}


/// Create a GNode tree structure using available file loaders, then convert it to a BglSimulation
Node* BglSimulation::load(const char* f)
{
    Node *root=Simulation::load(f);
    BglGraphManager::getInstance()->update();
    return root;

//         //Temporary: we need to change that: We could change getRoots by a getRoot.
//         //if several roots are found, we return a master node, above the roots of the simulation

//         std::vector< Node* > roots;
//         BglGraphManager::getInstance()->getRoots(roots);
//         if (roots.empty()) return NULL;
//         return roots.back();
}


void BglSimulation::reset(Node* root)
{
    sofa::simulation::Simulation::reset(root);
    BglGraphManager::getInstance()->reset();
}

//       void BglSimulation::unload(Node* root)
//       {
//         Simulation::unload(root);
//         //Clear should be called only if we unload the scene
//         clear();
//       }

void BglSimulation::clear()
{
    BglGraphManager::getInstance()->clear();
}
}
}
}


