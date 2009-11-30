/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_ENGINE_TRANSFORMENGINE_H
#define SOFA_COMPONENT_ENGINE_TRANSFORMENGINE_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/component/component.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/BaseObject.h>

#include <sofa/defaulttype/Quat.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace component
{

namespace engine
{

/**
 * This class transforms the positions of one DataFields into new positions after applying a transformation
This transformation can be either translation, rotation, scale
 */
template <class DataTypes>
class TransformEngine : public core::DataEngine
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(TransformEngine,DataTypes),core::DataEngine);
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Real Real;

public:

    TransformEngine();

    ~TransformEngine() {}

    void init();

    void reinit();

    void update();

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const TransformEngine<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

protected:
    Data<VecCoord> f_inputX; // input position
    Data<VecCoord> f_outputX; // ouput position
    Data<defaulttype::Vector3> translation; // translation
    Data<defaulttype::Vector3> rotation; // rotation
    Data<defaulttype::Vector3> scale; // scale
};

#if defined(WIN32) && !defined(SOFA_COMPONENT_ENGINE_TRANSFORMPOSITION_CPP)
#pragma warning(disable : 4231)

#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec1dTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec2dTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec3dTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Rigid2dTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Rigid3dTypes>;
#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec1fTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec2fTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Vec3fTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Rigid2fTypes>;
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::Rigid3fTypes>;
#endif //SOFA_DOUBLE
template class SOFA_COMPONENT_ENGINE_API TransformEngine<defaulttype::ExtVec3fTypes>;
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif
