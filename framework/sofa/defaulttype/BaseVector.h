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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_DEFAULTTYPE_BASEVECTOR_H
#define SOFA_DEFAULTTYPE_BASEVECTOR_H

#include <sofa/helper/system/config.h>
namespace sofa
{

namespace defaulttype
{

/// Generic vector API, allowing to fill and use a vector independently of the linear algebra library in use.
///
/// Note that accessing values using this class is rather slow and should only be used in codes where the
/// provided genericity is necessary.
class BaseVector
{
public:
    virtual ~BaseVector() {}

    /// Number of elements
    virtual unsigned int size(void) const = 0;
    /// Read the value of element i
    virtual SReal element(int i) const = 0;

    /// Resize the vector, and reset all values to 0
    virtual void resize(int dim) = 0;
    /// Reset all values to 0
    virtual void clear() = 0;

    /// Write the value of element i
    virtual void set(int i, SReal v) = 0;
    /// Add v to the existing value of element i
    virtual void add(int i, SReal v) = 0;
    /*
        /// Write the value of element i
        virtual void set(int i, SReal v) { set(i,(SReal)v); }
        /// Add v to the existing value of element i
        virtual void add(int i, SReal v) { add(i,(SReal)v); }
    */
    /// Reset the value of element i to 0
    virtual void clear(int i) { set(i,0.0); }
};

} // nampespace defaulttype

} // nampespace sofa


#endif
