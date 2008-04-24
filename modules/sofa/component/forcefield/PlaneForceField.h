#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_H
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/Data.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class PlaneForceFieldInternalData
{
public:
};

template<class DataTypes>
class PlaneForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef typename sofa::defaulttype::Vector3::value_type Real_Sofa;

protected:
    sofa::helper::vector<unsigned int> contacts;

    PlaneForceFieldInternalData<DataTypes> data;

public:

    Data<Deriv> planeNormal;
    Data<Real> planeD;
    Data<Real> stiffness;
    Data<Real> damping;
    Data<defaulttype::Vec3f> color;
    Data<bool> bDraw;

    PlaneForceField()
        : planeNormal(initData(&planeNormal, "normal", "plane normal"))
        , planeD(initData(&planeD, (Real)0, "d", "plane d coef"))
        , stiffness(initData(&stiffness, (Real)500, "stiffness", "force stiffness"))
        , damping(initData(&damping, (Real)5, "damping", "force damping"))
        , color(initData(&color, defaulttype::Vec3f(0.0f,.5f,.2f), "color", "plane color"))
        , bDraw(initData(&bDraw, false, "draw", "enable/disable drawing of plane"))
    {
        Deriv n;
        DataTypes::set(n, 0, 1, 0);
        planeNormal.setValue(n);
    }

    void setPlane(const Deriv& normal, Real d)
    {
        Real n = normal.norm();
        planeNormal.setValue( normal / n);
        planeD.setValue( d / n );
    }

    void setStiffness(Real stiff)
    {
        stiffness.setValue( stiff );
    }

    void setDamping(Real damp)
    {
        damping.setValue( damp );
    }

    void rotate( Deriv axe, Real angle ); // around the origin (0,0,0)

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    virtual sofa::defaulttype::Vector3::value_type getPotentialEnergy(const VecCoord& x);

    virtual void updateStiffness( const VecCoord& x );

    void draw();
    void draw2(float size=10.0f);
    bool addBBox(Real_Sofa* minBBox, Real_Sofa* maxBBox);

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
