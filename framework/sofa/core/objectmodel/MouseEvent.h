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
//
// C++ Interface: MouseEvent
//
// Description:
//
//
// Author: Pierre-Jean Bensoussan, Digital Trainers (2008)
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_CORE_OBJECTMODEL_MOUSEEVENT_H
#define SOFA_CORE_OBJECTMODEL_MOUSEEVENT_H

#include <sofa/core/objectmodel/Event.h>

namespace sofa
{

namespace core
{

namespace objectmodel
{

/**
 * @brief MouseEvent Class
 *
 * Implements an Event that notifies about a Mouse Interaction.
 */
class MouseEvent : public sofa::core::objectmodel::Event
{
public:

    /**
     * @brief Defines possible Mouse states.
     */
    typedef enum
    {
        Move=0,
        LeftPressed,
        LeftReleased,
        RightPressed,
        RightReleased,
        Wheel,
        Reset
    } State;

    /**
     * @brief Wheel Mouse Event constructor.
     */
    MouseEvent(State state, int wheelDelta = 0);

    /**
     * @brief Default constructor.
     */
    MouseEvent(State state, int posX, int posY);

    /**
     * @brief Default destructor.
     */
    virtual ~MouseEvent();

    /**
     * @name Accessors
     */
    //@{
    int getPosX(void) const {return m_posX;};
    int getPosY(void) const {return m_posY;};
    int getWheelDelta(void) const {return m_wheelDelta;};
    State getState(void) const {return m_state;};
    //}@

private:

    State m_state; ///< Mouse State on the event propagation.
    int m_wheelDelta; ///< Mouse wheel delta.
    int m_posX, m_posY; ///< Mouse coordinates.
};

} // namespace objectmodel

} // namespace core

} // namespace sofa

#endif // SOFA_CORE_OBJECTMODEL_MOUSEEVENT_H
