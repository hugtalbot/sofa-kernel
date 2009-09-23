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
#ifndef SOFA_GUI_MOUSEOPERATIONS_H
#define SOFA_GUI_MOUSEOPERATIONS_H

#include <iostream>
#include <vector>

namespace sofa
{
namespace component
{
namespace collision
{
class InteractionPerformer;
}
}
namespace gui
{
enum MOUSE_BUTTON {LEFT, MIDDLE, RIGHT};
enum MOUSE_STATUS {PRESSED,RELEASED, ACTIVATED, DEACTIVATED};

class PickHandler;

class Operation
{
public:
    Operation(): pickHandle(NULL), performer(NULL) {};
    virtual ~Operation() {};
    virtual void configure(PickHandler *picker, MOUSE_BUTTON b) {pickHandle=picker; button=b; }
    virtual void start() =0;
    virtual void execution() =0;
    virtual void end()     =0;
    virtual void wait() {};
protected:
    PickHandler *pickHandle;
    MOUSE_BUTTON button;
    sofa::component::collision::InteractionPerformer *performer;
};

class AttachOperation : public Operation
{
public:
    AttachOperation():stiffness(1000.0) {};
    virtual ~AttachOperation() {};
    virtual void start() ;
    virtual void execution() ;
    virtual void end() ;

    void setStiffness(double s) {stiffness = s;}
    virtual double getStiffness() const { return stiffness;}
    static bool isModifiable() {return true;};

    static std::string getDescription() {return "Attach an object to the Mouse";}
protected:
    double stiffness;
};

class InciseOperation : public Operation
{
public:
    virtual ~InciseOperation() {};
    virtual void start() ;
    virtual void execution() ;
    virtual void end() ;
    static std::string getDescription() {return "Incise along a path";}
    static bool isModifiable() {return false;};
};

class RemoveOperation : public Operation
{
public:
    virtual ~RemoveOperation() {};
    virtual void start() ;
    virtual void execution() ;
    virtual void end() ;
    static std::string getDescription() {return "Remove a primitive";}
    static bool isModifiable() {return false;};
};

class FixOperation : public Operation
{
public:
    FixOperation():stiffness(10000.0) {};
    virtual ~FixOperation() {};
    virtual void start() ;
    virtual void execution() ;
    virtual void end() ;

    void setStiffness(double s) {stiffness = s;}
    virtual double getStiffness() const { return stiffness;}
    static bool isModifiable() {return true;};

    static std::string getDescription() {return "Fix Picked particle";}
protected:
    double stiffness;
};

class SculptOperation : public Operation
{
public:
    SculptOperation():force(50), scale(50) {};
    virtual ~SculptOperation();
    virtual void start() ;
    virtual void execution() ;
    virtual void end() ;
    virtual void wait() ;

    void setForce(double f) {force = f;}
    virtual double getForce() const { return force;}
    void setScale(double s) {scale = s;}
    virtual double getScale() const {return scale;}
    static bool isModifiable() {return false;};

    static std::string getDescription() {return "Sculpt an object using the Mouse";}
protected:
    double force, scale;
};

}
}

#endif
