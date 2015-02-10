/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
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
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/

#include "Sofa_test.h"
#include <plugins/SceneCreator/SceneCreator.h>

#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/Utils.h>

using namespace sofa::helper::system;

namespace sofa {

int BaseSofa_test::seed = (unsigned int)time(NULL);

// Fill PluginRepository at load time.
int initPluginRepository()
{
#ifdef WIN32
    const std::string pluginsDir = "bin";
#else
    const std::string pluginsDir = "lib";
#endif
    PluginRepository.addFirstPath(FileSystem::getParentDirectory(FileSystem::getParentDirectory(Utils::getExecutablePath())) + "/" + pluginsDir);
    return 42;
}
int initPluginPath = initPluginRepository();

BaseSofa_test::BaseSofa_test(){
    seed = testing::seedValue;
    modeling::initSofa();

    //if you want to generate the same sequence of pseudo-random numbers than a specific test suites
    //use the same seed (the seed value is indicated at the 2nd line of test results)
    //and pass the seed in command argument line ex: SofaTest_test.exe seed 32
    helper::srand(seed);
}

BaseSofa_test::~BaseSofa_test(){ clearSceneGraph(); }

void BaseSofa_test::clearSceneGraph(){ modeling::clearScene(); }

#ifndef SOFA_FLOAT
template struct SOFA_TestPlugin_API Sofa_test<double>;
#endif
#ifndef SOFA_DOUBLE
template struct SOFA_TestPlugin_API Sofa_test<float>;
#endif
}
