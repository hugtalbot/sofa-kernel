#ifndef SOFA_COMPONENT_COLLISION_PenalityCompliantContact_H
#define SOFA_COMPONENT_COLLISION_PenalityCompliantContact_H

#include "BaseContact.h"

#include <Compliant/config.h>

#include "../compliance/DiagonalCompliance.h"

#include "../utils/edit.h"


namespace sofa
{

namespace component
{

namespace collision
{

template <class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes = sofa::defaulttype::Vec3Types>
class PenalityCompliantContact : public BaseContact<TCollisionModel1, TCollisionModel2, ResponseDataTypes>
{

public:

    SOFA_CLASS(SOFA_TEMPLATE3(PenalityCompliantContact, TCollisionModel1, TCollisionModel2, ResponseDataTypes), SOFA_TEMPLATE3(BaseContact, TCollisionModel1, TCollisionModel2, ResponseDataTypes) );

    typedef BaseContact<TCollisionModel1, TCollisionModel2, ResponseDataTypes> Inherit;
    typedef typename Inherit::node_type node_type;
    typedef typename Inherit::CollisionModel1 CollisionModel1;
    typedef typename Inherit::CollisionModel2 CollisionModel2;
    typedef typename Inherit::Intersection Intersection;


    Data< SReal > stiffness;


protected:

    typedef container::MechanicalObject<defaulttype::Vec1Types> contact_dofs_type;
    typename contact_dofs_type::SPtr contact_dofs;

    core::BaseMapping::SPtr contact_map;

    typedef forcefield::DiagonalCompliance<defaulttype::Vec1Types> compliance_type;
    compliance_type::SPtr compliance;

    PenalityCompliantContact()
        : Inherit()
        , stiffness( initData(&stiffness, SReal(10), "stiffness", "Contact Stiffness") )
    {}

    PenalityCompliantContact(CollisionModel1* model1, CollisionModel2* model2, Intersection* intersectionMethod)
        : Inherit(model1, model2, intersectionMethod)
        , stiffness( initData(&stiffness, SReal(10), "stiffness", "Contact Stiffness") )
    {}



    void create_node()
    {
        const unsigned size = this->mappedContacts.size();

        this->contact_node = node_type::create( this->getName() + "_contact_frame" );
        down_cast< node_type >(this->mstate1->getContext())->addChild( this->contact_node.get() );

        // ensure all graph context parameters (e.g. dt are well copied)
        this->contact_node->updateSimulationContext();

        // 1d contact dofs
        contact_dofs = sofa::core::objectmodel::New<contact_dofs_type>();
        contact_dofs->resize( size );
        contact_dofs->setName( this->getName() + "_contact_dofs" );
        this->contact_node->addObject( contact_dofs.get() );

        // contact mapping
        contact_map = this->template createContactMapping<defaulttype::Vec1Types>(this->contact_node, contact_dofs);

        // compliance
        compliance = sofa::core::objectmodel::New<compliance_type>( contact_dofs.get() );
        this->contact_node->addObject( compliance.get() );
        editOnly(compliance->damping)->assign(1, this->damping_ratio.getValue() );
        compliance->isCompliance.setValue( false );

        typename compliance_type::VecDeriv complianceValues( size );
        for( unsigned i = 0 ; i < size ; ++i )
        {
            // no stiffness for non-violated penetration (alarm distance)
            // TODO add a kind of projector to perform an unilateral stiffness (to add stiffness only one way).
            // This would prevent sticky contacts and would allow to add small stiffness in alarm distance to slow-down object trying to go closer.

            if( (*this->contacts)[i].value < 0 )
            {
                complianceValues[i][0] = 1.0/this->stiffness.getValue();

                // only violated penetrations will propagate forces
                this->mstate1->forceMask.insertEntry( this->mappedContacts[i][0] );
                if( !this->selfCollision ) this->mstate2->forceMask.insertEntry( this->mappedContacts[i][1] );
            }
            else
            {
                complianceValues[i][0] = std::numeric_limits<int>::max();
            }
        }
        compliance->diagonal.setValue( complianceValues );

        compliance->init();
    }



    void update_node() 
    {
        const unsigned size = this->mappedContacts.size();

        if( this->selfCollision )
        {
            typedef mapping::ContactMapping<ResponseDataTypes, defaulttype::Vec1Types> contact_mapping_type;
            core::objectmodel::SPtr_dynamic_cast<contact_mapping_type>(contact_map)->setDetectionOutput(this->contacts);
        }
        else
        {
            typedef mapping::ContactMultiMapping<ResponseDataTypes, defaulttype::Vec1Types> contact_mapping_type;
            core::objectmodel::SPtr_dynamic_cast<contact_mapping_type>(contact_map)->setDetectionOutput(this->contacts);  
        }
        contact_map->reinit();

//        // every contact points must propagate constraint forces
//        for(unsigned i = 0; i < size; ++i)
//        {
//            this->mstate1->forceMask.insertEntry( this->mappedContacts[i].index1 );
//            if( !this->selfCollision ) this->mstate2->forceMask.insertEntry( this->mappedContacts[i].index2 );
//        }

        // update compliance value
        typename compliance_type::VecDeriv complianceValues( size );
        for( unsigned i = 0 ; i < size ; ++i )
        {
            if( (*this->contacts)[i].value < 0 )
            {
                complianceValues[i][0] = 1.0/this->stiffness.getValue();

                // only violated penetrations will propagate forces
                this->mstate1->forceMask.insertEntry( this->mappedContacts[i][0] );
                if( !this->selfCollision ) this->mstate2->forceMask.insertEntry( this->mappedContacts[i][1] );
            }
            else
            {
                complianceValues[i][0] = std::numeric_limits<int>::max();
            }
        }
        compliance->diagonal.setValue( complianceValues );
        compliance->reinit();
    }



};

} // namespace collision
} // namespace component
} // namespace sofa

#endif  // SOFA_COMPONENT_COLLISION_PenalityCompliantContact_H
