/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_GUI_QGLVIEWER_GENGRAPHFORM_H
#define SOFA_GUI_QGLVIEWER_GENGRAPHFORM_H

#include "BaseGenGraphForm.h"
#include <sofa/simulation/tree/GNode.h>

#ifdef SOFA_QT4
#include "QProcess"
#include "QStringList"
#else
#include "qprocess.h"
#include "qstringlist.h"
#endif

#include <list>
#include <map>
#include <set>

namespace sofa
{

namespace gui
{

namespace qt
{

class GenGraphForm : public BaseGenGraphForm
{
    Q_OBJECT
public:
    GenGraphForm();

    void setScene(sofa::simulation::tree::GNode* scene);

public slots:
    virtual void change();
    virtual void doBrowse();
    virtual void doExport();
    virtual void doDisplay();
    virtual void doClose();
    virtual void taskFinished();
    virtual void changeFilter();
    virtual void setFilter();

protected:
    QString exportedFile;
    sofa::simulation::tree::GNode* graph;
    std::list<QStringList> tasks;
    QProcess* currentTask;

    void addTask(QStringList argv);
    void runTask();
    void killAllTasks();

    std::map<std::string, std::set<std::string> > presetFilters;
    bool settingFilter;
    std::set<std::string> getCurrentFilter();
};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif
