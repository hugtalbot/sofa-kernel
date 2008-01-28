/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This program is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation; either version 2 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program; if not, write to the Free Software Foundation, Inc., 51        *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                    *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef ADDOBECJT_H
#define ADDOBECJT_H

#include <vector>
#include "DialogAddObject.h"

namespace sofa
{

namespace gui
{

namespace qt
{

class AddObject : public DialogAddObject
{
    Q_OBJECT
public:

    AddObject( std::vector< std::string > *list_object_, QWidget* parent, const char* name= 0, bool  modal= FALSE, Qt::WFlags f= 0 );


    void setPath(const std::string path);

public slots:
    void fileOpen();
    void buttonUpdate(int);
    void accept();

signals:
    void loadObject(std::string, double, double, double, double, double, double,double);


protected:
    std::string fileName;
    std::vector< std::string > *list_object;

};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif
