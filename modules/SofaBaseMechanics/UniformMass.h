/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_MASS_UNIFORMMASS_H
#define SOFA_COMPONENT_MASS_UNIFORMMASS_H
#include "config.h"

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/Mass.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/defaulttype/BaseVector.h>
#include <sofa/core/objectmodel/DataFileName.h>

namespace sofa
{

namespace component
{

namespace mass
{

template <class DataTypes, class TMassType>
class UniformMass : public core::behavior::Mass<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(UniformMass,DataTypes,TMassType), SOFA_TEMPLATE(core::behavior::Mass,DataTypes));

    typedef core::behavior::Mass<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;
    typedef TMassType MassType;


    Data<MassType>                        mass;         ///< the mass of each particle
    Data<SReal>                           totalMass;    ///< if >0 : total mass of this body
    sofa::core::objectmodel::DataFileName filenameMass; ///< a .rigid file to automatically load the inertia matrix and other parameters

    Data<bool>                        showCenterOfGravity; /// to display the center of gravity of the system
    Data<float>                       showAxisSize;        /// to display the center of gravity of the system

    Data<bool>  computeMappingInertia;
    Data<bool>  showInitialCenterOfGravity;


    Data<bool>  showX0; /// to display the rest positions

    /// optional range of local DOF indices. Any computation involving only indices outside of this range are discarded (useful for parallelization using mesh partitionning)
    Data< defaulttype::Vec<2,int> > localRange;
    Data< helper::vector<int> >     d_indices;

    Data<bool> handleTopoChange;
    Data<bool> preserveTotalMass;

    ////////////////////////// Inherited attributes ////////////////////////////
    /// https://gcc.gnu.org/onlinedocs/gcc/Name-lookup.html
    /// Bring inherited attributes and function in the current lookup context.
    /// otherwise any access to the base::attribute would require
    /// the "this->" approach.
    using core::behavior::ForceField<DataTypes>::mstate ;
    using core::objectmodel::BaseObject::getContext;
    ////////////////////////////////////////////////////////////////////////////

    bool m_doesTopoChangeAffect;


protected:
    UniformMass();

    ~UniformMass();

    /// @internal fonction called in the constructor that can be specialized (does nothing by default)
    void constructor_message() {}

public:
    void setMass(const MassType& mass);
    const MassType& getMass() const { return mass.getValue(); }

    SReal getTotalMass() const { return totalMass.getValue(); }
    void setTotalMass(SReal m);

    void setFileMass(const std::string& file) {filenameMass.setValue(file);}
    std::string getFileMass() const {return filenameMass.getFullPath();}

    void loadRigidMass(std::string filename);
    // -- Mass interface

    void reinit();
    void init();

    void handleTopologyChange();

    void addMDx(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecDeriv& dx, SReal factor);

    void accFromF(const core::MechanicalParams* mparams, DataVecDeriv& a, const DataVecDeriv& f);

    void addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v);

    SReal getKineticEnergy(const core::MechanicalParams* mparams, const DataVecDeriv& d_v) const;  ///< vMv/2 using dof->getV()

    SReal getPotentialEnergy(const core::MechanicalParams* mparams, const DataVecCoord& x) const;   ///< Mgx potential in a uniform gravity field, null at origin

    defaulttype::Vector6 getMomentum(const core::MechanicalParams* mparams, const DataVecCoord& x, const DataVecDeriv& v) const;  ///< (Mv,cross(x,Mv)+Iw)

    void addMDxToVector(defaulttype::BaseVector *resVect, const VecDeriv *dx, SReal mFact, unsigned int& offset);

    void addGravityToV(const core::MechanicalParams* mparams, DataVecDeriv& d_v);

    void addMToMatrix(const core::MechanicalParams *mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix); /// Add Mass contribution to global Matrix assembling

    SReal getElementMass(unsigned int index) const;
    void getElementMass(unsigned int index, defaulttype::BaseMatrix *m) const;

    bool isDiagonal() {return true;}

    void draw(const core::visual::VisualParams* vparams);
};

//Specialization for rigids
#ifndef SOFA_FLOAT
template <>
void UniformMass<defaulttype::Rigid3dTypes, defaulttype::Rigid3dMass>::reinit();
template <>
void UniformMass<defaulttype::Rigid3dTypes, defaulttype::Rigid3dMass>::loadRigidMass ( std::string );
template <>
void UniformMass<defaulttype::Rigid3dTypes, defaulttype::Rigid3dMass>::draw(const core::visual::VisualParams* vparams);
template <>
void UniformMass<defaulttype::Rigid2dTypes, defaulttype::Rigid2dMass>::draw(const core::visual::VisualParams* vparams);
template <>
double UniformMass<defaulttype::Rigid3dTypes,defaulttype::Rigid3dMass>::getPotentialEnergy ( const core::MechanicalParams*, const DataVecCoord& x ) const;
template <>
double UniformMass<defaulttype::Rigid2dTypes,defaulttype::Rigid2dMass>::getPotentialEnergy ( const core::MechanicalParams*, const DataVecCoord& x ) const;
template <>
void UniformMass<defaulttype::Vec6dTypes,double>::draw(const core::visual::VisualParams* vparams);
#endif
#ifndef SOFA_DOUBLE
template<>
void UniformMass<defaulttype::Rigid3fTypes, defaulttype::Rigid3fMass>::reinit();
template<>
void UniformMass<defaulttype::Rigid3fTypes, defaulttype::Rigid3fMass>::loadRigidMass ( std::string );
template <>
void UniformMass<defaulttype::Rigid3fTypes, defaulttype::Rigid3fMass>::draw(const core::visual::VisualParams* vparams);
template <>
void UniformMass<defaulttype::Rigid2fTypes, defaulttype::Rigid2fMass>::draw(const core::visual::VisualParams* vparams);
template <>
SReal UniformMass<defaulttype::Rigid3fTypes,defaulttype::Rigid3fMass>::getPotentialEnergy ( const core::MechanicalParams*, const DataVecCoord& x ) const;
template <>
SReal UniformMass<defaulttype::Rigid2fTypes,defaulttype::Rigid2fMass>::getPotentialEnergy ( const core::MechanicalParams*, const DataVecCoord& x ) const;
template <>
void UniformMass<defaulttype::Vec6fTypes,float>::draw(const core::visual::VisualParams* vparams);
#endif

#if defined(SOFA_EXTERN_TEMPLATE) && !defined(SOFA_COMPONENT_MASS_UNIFORMMASS_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec3dTypes, double>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec2dTypes, double>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec1dTypes, double>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec6dTypes, double>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Rigid3dTypes, defaulttype::Rigid3dMass>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Rigid2dTypes, defaulttype::Rigid2dMass>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec3fTypes, float>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec2fTypes, float>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec1fTypes, float>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Vec6fTypes, float>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Rigid3fTypes, defaulttype::Rigid3fMass>;
extern template class SOFA_BASE_MECHANICS_API UniformMass<defaulttype::Rigid2fTypes, defaulttype::Rigid2fMass>;
#endif
#endif

} // namespace mass

} // namespace component

} // namespace sofa

#endif

