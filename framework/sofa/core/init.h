/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: The SOFA Team (see Authors.txt)                                    *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_INIT_H
#define SOFA_CORE_INIT_H

#include <sofa/core/core.h>

namespace sofa
{

namespace core
{

/// @brief Initialize the SofaCore library, as well as its dependencies:
/// SofaDefaultType, SofaHelper.
void SOFA_CORE_API init();

/// @brief Return true if and only if the SofaCore library has been initialized.
bool SOFA_CORE_API isInitialized();

/// @brief Clean up the resources used by the SofaCore library, as well as its
/// dependencies: SofaDefaultType, SofaHelper.
void SOFA_CORE_API cleanup();

/// @brief Return true if and only if the SofaCore library has been cleaned
/// up.
bool SOFA_CORE_API isCleanedUp();

/// @brief Print a warning if the SofaCore library is not initialized.
void SOFA_CORE_API checkIfInitialized();

} // namespace core

} // namespace sofa

#endif
