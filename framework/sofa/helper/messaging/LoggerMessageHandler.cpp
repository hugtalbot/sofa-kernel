/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
* in the messaging.h file.
******************************************************************************/
#include <sstream>
using std::ostringstream ;

#include <iostream>
using std::endl ;
using std::cerr ;

#include <map>
using std::map ;

//#include <string>
//using std::string ;

#include "Message.h"
#include "LoggerMessageHandler.h"



namespace sofa
{

namespace helper
{

namespace messaging
{

// todo(damien) this should something we can change dynamically :)
const unsigned int messagequeuesize = 10000;
map<int, Message> s_m_messages ;

const Message& LoggerMessageHandler::getMessageAt(int index)
{
    if(s_m_messages.find(index) == s_m_messages.end()){
        // Nope
        // return Message::empty ;
        std::cerr << "WHARRGARBL" << std::endl;
    }

    return s_m_messages.find(index)->second ;
}

// todo(damien): implement a real way to control the size of the log.
void LoggerMessageHandler::process(Message& m)
{
    s_m_messages[m.id()] = m ;

    if(s_m_messages.size() >= messagequeuesize){
        cerr << "There is too much messages in the message queue. To remove this error you need to "
                     "1) send me an email explaining that you reach the 10K limit and that I "
                     "have no excuse to delay the proper implementation of the LoggerMessageHandler" << endl;
    }
}


} // messaging
} // helper
} // sofa

