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
#ifndef SOFA_CORE_OBJECTMODEL_DDGNODE_H
#define SOFA_CORE_OBJECTMODEL_DDGNODE_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/core.h>
#include <list>

namespace sofa
{

namespace core
{

namespace objectmodel
{

/**
 *  \brief Abstract base to manage data dependencies. BaseData and DataEngine inherites from this class
 *
 */
class SOFA_CORE_API DDGNode
{
public:

    /// Constructor
    DDGNode():dirty(false) {}

    /// Destructor. Do nothing
    virtual ~DDGNode();

    /// Update the value of Datas
    virtual void update() = 0;

    /// True if the Data has been modified
    virtual void setDirty();

    /// Set dirty flag to false
    void cleanDirty();

    /// Returns true if the DDGNode has been modified. Otherwise returns false
    bool isDirty();

    /// Add a new input to this node
    void addInput(DDGNode* n);

    /// Remove an input from this node
    void delInput(DDGNode* n);

    /// Add a new output to this node
    void addOutput(DDGNode* n);

    /// Remove an output from this node
    void delOutput(DDGNode* n);

    /// Get the list of inputs for this DDGNode
    std::list<DDGNode*> getInputs();

    /// Get the list of outputs for this DDGNode
    std::list<DDGNode*> getOutputs();

protected:

    std::list<DDGNode*> inputs;
    std::list<DDGNode*> outputs;
    bool dirty;
};

} // namespace objectmodel

} // namespace core

} // namespace sofa

#endif
