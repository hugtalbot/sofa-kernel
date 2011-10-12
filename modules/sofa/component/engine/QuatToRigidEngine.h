#ifndef SOFA_COMPONENT_ENGINE_QUATTORIGIDENGINE_H
#define SOFA_COMPONENT_ENGINE_QUATTORIGIDENGINE_H

#include <sofa/core/DataEngine.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/Quater.h>
#include <sofa/defaulttype/RigidTypes.h>

#include <sofa/component/component.h>


namespace sofa
{

namespace component
{

namespace engine
{

/*
 * Engine which converts (vectors of) Vec3 + Quaternion to give a (vector of) Rigid
 *
 */

template <class DataTypes>
class QuatToRigidEngine : public sofa::core::DataEngine
{
public:
    SOFA_CLASS(QuatToRigidEngine,sofa::core::DataEngine);

    typedef typename DataTypes::Real Real;
    typedef sofa::defaulttype::Vec<3,Real> Vec3;
    typedef sofa::helper::Quater<Real> Quat;
    typedef typename sofa::defaulttype::StdRigidTypes<3,Real>::Coord RigidVec3;

    QuatToRigidEngine();
    virtual ~QuatToRigidEngine();

    void update();
    void init();
    void reinit();

    /// Construction method called by ObjectFactory.
    template<class T>
    static typename T::SPtr create(T* tObj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        return core::objectmodel::BaseObject::create(tObj, context, arg);
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const QuatToRigidEngine<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

    //
    Data<helper::vector<Vec3 > > f_positions;
    Data<helper::vector<Quat> > f_orientations;
    Data<helper::vector<Vec3 > > f_colinearPositions;
    Data<helper::vector<RigidVec3> > f_rigids;
};

#if defined(WIN32) && !defined(QUATTORIGIDENGINE_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
template class SOFA_ENGINE_API QuatToRigidEngine<defaulttype::Vec3dTypes>;
#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_ENGINE_API QuatToRigidEngine<defaulttype::Vec3fTypes>;
#endif //SOFA_DOUBLE
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_ENGINE_QUATTORIGIDENGINE_H
