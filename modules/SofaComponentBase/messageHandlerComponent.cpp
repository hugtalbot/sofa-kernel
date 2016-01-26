/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2015 INRIA, USTL, UJF, CNRS, MGH                    *
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
* This component is open-source                                               *
*                                                                             *
* Authors: Damien Marchal                                                     *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
/*****************************************************************************
* User of this library should read the documentation
* in the logging.h file.
******************************************************************************/
#include <sofa/core/ObjectFactory.h>
using sofa::core::RegisterObject ;

#include <sofa/helper/logging/LoggerMessageHandler.h>
using sofa::helper::logging::LoggerMessageHandler ;
#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;
#include <sofa/helper/logging/FileMessageHandler.h>
using sofa::helper::logging::FileMessageHandler ;
#include <sofa/helper/logging/ClangStyleMessageFormatter.h>
using sofa::helper::logging::ClangStyleMessageFormatter;
#include <sofa/helper/logging/DefaultStyleMessageFormatter.h>
using sofa::helper::logging::DefaultStyleMessageFormatter;

#include <sofa/helper/logging/Messaging.h>
using sofa::helper::logging::MessageDispatcher;

#include "messageHandlerComponent.h"

namespace sofa
{
namespace component
{
namespace logging
{

MessageHandlerComponent::MessageHandlerComponent() :
    d_type(initData(&d_type, "handler", "Type of the message handler to use among "
                                        "[sofa, clang, log, silent]. "))
{
    m_isValid = false ;
}

void MessageHandlerComponent::parse ( BaseObjectDescription* arg )
{
    BaseObject::parse(arg) ;

    const char* type=arg->getAttribute("handler") ;
    if(type==NULL){
        msg_info(this) << "missing the handler attribute " << type << ", the default sofa style will be used. "
                          "To suppress this message you need to specify the 'handler' attribute. "
                          "eg: handler='silent' ";
        return ;
    }

    string stype(type) ;


    if(stype=="sofa"){

        gMessageDispatcher.addHandler(new ConsoleMessageHandler()) ;
    }else if(stype=="clang"){
        gMessageDispatcher.addHandler(new ConsoleMessageHandler(ClangStyleMessageFormatter::getInstance())) ;
    }else if(stype=="log"){
        gMessageDispatcher.addHandler(new LoggerMessageHandler()) ;
    }else if(stype=="silent"){
        gMessageDispatcher.clearHandlers(true) ;
    }else{
        msg_info(this) << "[" << stype << "] is not a supported format the default sofa style will be used. "
                          "To supress this message you need to specify a valid attribute "
                          "among [clang, log, silent, sofa]." ;
        return ;
    }

    msg_info(this) << " Adding a new message handler of type: " << stype ;

    m_isValid = true ;
}

SOFA_DECL_CLASS(MessageHandlerComponent) ;

int MessageHandlerComponentClass = RegisterObject("This object controls the way Sofa print's "
                                                  "info/warning/error/fatal messages. ")
        .add< MessageHandlerComponent >()
        ;



////////////////////////// FileMessageHandlerComponent ////////////////////////////////////
FileMessageHandlerComponent::FileMessageHandlerComponent() :
    d_filename(initData(&d_filename, "filename", "Name of the file into which the message will be saved in."))
{
    m_isValid = false ;
}

FileMessageHandlerComponent::~FileMessageHandlerComponent()
{
    gMessageDispatcher.rmHandler(m_handler) ;

    delete m_handler ;
}

void FileMessageHandlerComponent::parse ( BaseObjectDescription* arg )
{
    BaseObject::parse(arg) ;

    const char* type=arg->getAttribute("filename") ;
    if(type==NULL){
        msg_warning(this) << "missing the name of the file where the logs are stored. "
                             "To fix this error you need to add the missing attribute eg:"
                             "  filename='nameOfTheLogFile.log' ";
        return ;
    }

    FileMessageHandler *handler = new FileMessageHandler(type, DefaultStyleMessageFormatter::getInstance()) ;
    if(handler==NULL){
        msg_fatal(this) << "Unable to allocate memory. This is a fatal error. To fix"
                           "this you may free more RAM before running Sofa." ;
        return ;
    }

    if(!handler->isValid()){
        msg_warning(this) << " unable to open file named [" << type << "]. "
                             " Logs will not be written. ";
        return ;
    }

    msg_info(this) << " now logging messages into [" << type << "]." ;

    m_handler = handler;
    gMessageDispatcher.addHandler(m_handler) ;

    m_isValid = true ;
}

SOFA_DECL_CLASS(FileMessageHandlerComponent) ;

int FileMessageHandlerComponentClass = RegisterObject("This component dump all the messages into"
                                                      "a file.")
        .add< FileMessageHandlerComponent >()
        ;
}
}
}


