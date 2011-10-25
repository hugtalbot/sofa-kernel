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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_OBJECTMODEL_LINK_H
#define SOFA_CORE_OBJECTMODEL_LINK_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/objectmodel/BaseLink.h>
#include <sofa/core/ExecParams.h>
#include <sofa/helper/vector.h>
#include <string>

namespace sofa
{

namespace core
{

namespace objectmodel
{

class DDGNode;

template<class TDestType, bool strongLink>
class LinkTraitsDestPtr;

template<class TDestType>
class LinkTraitsDestPtr<TDestType, false>
{
public:
    typedef TDestType* T;
    static TDestType* get(T p) { return p; }
};

template<class TDestType>
class LinkTraitsDestPtr<TDestType, true>
{
public:
    typedef typename TDestType::SPtr T;
    static TDestType* get(const T& p) { return p.get(); }
};

template<class TDestType, class TDestPtr, bool storePath>
class LinkTraitsValueType;

template<class TDestType, class TDestPtr>
class LinkTraitsValueType<TDestType,TDestPtr, false>
{
public:
    typedef TDestPtr T;
    static std::string name(const T& ptr)
    {
        if (!ptr) return std::string();
        else return ptr->getName();
    }
};

template<class TDestType, class TDestPtr>
class LinkTraitsValueType<TDestType,TDestPtr, true>
{
public:
    struct T
    {
        TDestPtr ptr;
        std::string path;
        operator TDestPtr() const { return ptr; }
        TDestType* operator*() const { return &(*ptr); }
        TDestType* operator->() const { return &(*ptr); }
        bool operator == (TDestType* p) { return ptr == p; }
        bool operator != (TDestType* p) { return ptr != p; }
    };
    static std::string name(const T& v)
    {
        if (v.path) return v.path;
        else if (!v.ptr) return std::string();
        else return v.ptr->getName();
    }
};

template<class TDestType, class TValueType, bool multiLink>
class LinkTraitsContainer;


/// Class to hold 0-or-1 pointer. The interface is similar to std::vector (size/[]/begin/end), plus an automatic convertion to one pointer.
template < class T, class TPtr = T* >
class SinglePtr
{
protected:
    TPtr elems[1];
public:
    typedef T pointed_type;
    typedef TPtr value_type;
    typedef value_type const * const_iterator;
    typedef value_type const * const_reverse_iterator;

    SinglePtr()
    {
        elems[0] = TPtr();
    }
    const_iterator begin() const
    {
        return elems;
    }
    const_iterator end() const
    {
        return (!elems[0])?elems:elems+1;
    }
    const_reverse_iterator rbegin() const
    {
        return begin();
    }
    const_reverse_iterator rend() const
    {
        return end();
    }
    unsigned int size() const
    {
        return (!elems[0])?0:1;
    }
    bool empty() const
    {
        return !elems[0];
    }
    void clear()
    {
        elems[0] = TPtr();
    }
    const TPtr& get() const
    {
        return elems[0];
    }
    TPtr& get()
    {
        return elems[0];
    }
    const TPtr& operator[](unsigned int i) const
    {
        return elems[i];
    }
    TPtr& operator[](unsigned int i)
    {
        return elems[i];
    }
    const TPtr& operator()(unsigned int i) const
    {
        return elems[i];
    }
    TPtr& operator()(unsigned int i)
    {
        return elems[i];
    }
    operator T*() const
    {
        return elems[0];
    }
    T* operator->() const
    {
        return elems[0];
    }
};

template<class TDestType, class TValueType>
class LinkTraitsContainer<TDestType, TValueType, false>
{
public:
    typedef SinglePtr<TDestType, TValueType> T;
    //typedef helper::fixed_array<TValueType,1> T;
    static void clear(T& c)
    {
        c.clear();
    }
    static unsigned int add(T& c, TDestType* v)
    {
        c.get() = v;
        return 0;
    }
    static unsigned int find(const T& c, TDestType* v)
    {
        if (c.get() == v) return 0;
        else return 1;
    }
    static void remove(T& c, unsigned index)
    {
        if (!index)
            c.clear();
    }
};

template<class TDestType, class TValueType>
class LinkTraitsContainer<TDestType, TValueType, true>
{
public:
    typedef helper::vector<TValueType> T;
    static void clear(T& c)
    {
        c.clear();
    }
    static unsigned int add(T& c, TValueType v)
    {
        unsigned int index = c.size();
        c.push_back(v);
        return index;
    }
    static unsigned int find(const T& c, TValueType v)
    {
        unsigned int s = c.size();
        for (unsigned int i=0; i<s; ++i)
            if (c[i] == v) return i;
        return s;
    }
    static void remove(T& c, unsigned index)
    {
        unsigned int s = c.size();
        for (unsigned int i=index+1; i < s; ++i)
            c[i-1] = c[i];
        c.resize(s-1);
    }
};

template<class Type>
class LinkTraitsPtrCasts;

/**
 *  \brief Container of all links in the scenegraph, from a given type of object (Owner) to another (Dest)
 *
 */
template<class TOwnerType, class TDestType, unsigned TFlags>
class TLink : public BaseLink
{
public:
    typedef TOwnerType OwnerType;
    typedef TDestType DestType;
    enum { ActiveFlags = TFlags };
#define ACTIVEFLAG(f) ((ActiveFlags & (f)) != 0)
    typedef LinkTraitsDestPtr<DestType, ACTIVEFLAG(FLAG_STRONGLINK)> TraitsDestPtr;
    typedef typename TraitsDestPtr::T DestPtr;
    typedef LinkTraitsValueType<DestType, DestPtr, ACTIVEFLAG(FLAG_STOREPATH)> TraitsValueType;
    typedef typename TraitsValueType::T ValueType;
    typedef LinkTraitsContainer<DestType, ValueType, ACTIVEFLAG(FLAG_MULTILINK)> TraitsContainer;
    typedef typename TraitsContainer::T Container;
    typedef typename Container::const_iterator const_iterator;
    typedef typename Container::const_reverse_iterator const_reverse_iterator;
    typedef LinkTraitsPtrCasts<TOwnerType> TraitsOwnerCasts;
    typedef LinkTraitsPtrCasts<TDestType> TraitsDestCasts;
#undef ACTIVEFLAG

    TLink(const InitLink<OwnerType>& init)
        : BaseLink(init, ActiveFlags), m_owner(init.owner)
    {
    }

    virtual ~TLink()
    {
    }

    unsigned int size(const core::ExecParams* params = 0) const
    {
        return (unsigned int)m_value[core::ExecParams::currentAspect(params)].size();
    }

    bool empty(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)].empty();
    }

    const Container& getValue(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)];
    }

    const_iterator begin(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)].begin();
    }

    const_iterator end(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)].end();
    }

    const_reverse_iterator rbegin(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)].rbegin();
    }

    const_reverse_iterator rend(const core::ExecParams* params = 0) const
    {
        return m_value[core::ExecParams::currentAspect(params)].rend();
    }

    bool add(DestPtr v)
    {
        if (!v) return false;
        const int aspect = core::ExecParams::currentAspect();
        unsigned int index = TraitsContainer::add(m_value[aspect],&*v);
        changed(DestPtr(), m_value[aspect][index], index);
        return true;
    }

    bool remove(DestPtr v)
    {
        if (!v) return false;
        const int aspect = core::ExecParams::currentAspect();
        unsigned int index = TraitsContainer::find(m_value[aspect],v);
        if (index >= m_value[aspect].size()) return false;
        m_value[aspect][index] = NULL;
        changed(v, m_value[aspect][index], index);
        TraitsContainer::remove(m_value[aspect],index);
        return true;
    }

    unsigned int getSize() const
    {
        return size();
    }

    Base* getLinkedBase(unsigned int index=0) const
    {
        return TraitsDestCasts::getBase(getIndex(index));
    }
    BaseData* getLinkedData(unsigned int index=0) const
    {
        return TraitsDestCasts::getData(getIndex(index));
    }
    std::string getLinkedName(unsigned int index=0) const
    {
        return TraitsValueType::name(getIndex(index));
    }

    /// Copy the value of an aspect into another one.
    virtual void copyAspect(int destAspect, int srcAspect)
    {
        BaseLink::copyAspect(destAspect, srcAspect);
        m_value[destAspect] = m_value[srcAspect];
    }

    /// Release memory allocated for the specified aspect.
    virtual void releaseAspect(int aspect)
    {
        BaseLink::releaseAspect(aspect);
        TraitsContainer::clear(m_value[aspect]);
    }

    sofa::core::objectmodel::Base* getOwnerBase() const
    {
        return TraitsOwnerCasts::getBase(m_owner);
    }
    sofa::core::objectmodel::BaseData* getOwnerData() const
    {
        return TraitsOwnerCasts::getData(m_owner);
    }

protected:
    OwnerType* m_owner;
    helper::fixed_array<Container, SOFA_DATA_MAX_ASPECTS> m_value;

    DestType* getIndex(unsigned int index) const
    {
        const int aspect = core::ExecParams::currentAspect();
        if (index < m_value[aspect].size())
            return TraitsDestPtr::get(m_value[aspect][index]);
        else
            return NULL;
    }

    virtual void changed(DestPtr before, DestPtr& after, unsigned int index) = 0;
};

/**
 *  \brief Container of vectors of links in the scenegraph, from a given type of object (Owner) to another (Dest)
 *
 */
template<class TOwnerType, class TDestType, unsigned TFlags>
class MultiLink : public TLink<TOwnerType,TDestType,TFlags|BaseLink::FLAG_MULTILINK>
{
public:
    typedef TLink<TOwnerType,TDestType,TFlags|BaseLink::FLAG_MULTILINK> Inherit;
    typedef TOwnerType OwnerType;
    typedef TDestType DestType;
    typedef typename Inherit::TraitsDestPtr TraitsDestPtr;
    typedef typename Inherit::DestPtr DestPtr;
    typedef typename Inherit::TraitsValueType TraitsValueType;
    typedef typename Inherit::ValueType ValueType;
    typedef typename Inherit::TraitsContainer TraitsContainer;
    typedef typename Inherit::Container Container;
    typedef typename Inherit::TraitsOwnerCasts TraitsOwnerCasts;
    typedef typename Inherit::TraitsDestCasts TraitsDestCasts;

    //typedef void (OwnerType::*ValidatorFn)(DestPtr, DestPtr&);
    typedef void (OwnerType::*ValidatorFn)(DestPtr, DestPtr&, unsigned int);

    MultiLink(const BaseLink::InitLink<OwnerType>& init)
        : Inherit(init), m_validator(NULL)
    {
    }

    MultiLink(const BaseLink::InitLink<OwnerType>& init, DestPtr val)
        : Inherit(init), m_validator(NULL)
    {
        if (val) this->add(val);
    }

    virtual ~MultiLink()
    {
    }

    void setValidator(ValidatorFn fn)
    {
        m_validator = fn;
    }

    DestType* get(unsigned int index, const core::ExecParams* params = 0) const
    {
        const int aspect = core::ExecParams::currentAspect(params);
        if (index < this->m_value[aspect].size())
            return TraitsDestPtr::get(this->m_value[aspect][index]);
        else
            return NULL;
    }
    /*
        void reset(unsigned int index)
        {
            const int aspect = core::ExecParams::currentAspect();
            if (index >= this->m_value[aspect].size()) return;
            DestPtr v = this->m_value[aspect][index];
            this->m_value[aspect][index] = NULL;
            changed(v, this->m_value[aspect][index], index);
            TraitsContainer::remove(this->m_value[aspect],index);
        }

        void set(DestPtr v, unsigned int index)
        {
            if (!v) { reset(index); return; }
            const int aspect = core::ExecParams::currentAspect();
            if (index >= this->m_value[aspect].size()) { add(v); return; }
            DestPtr& val = this->m_value[aspect][index];
            DestPtr before = val;
            val = v;
            changed(before, val, index);
        }
    */
protected:
    ValidatorFn m_validator;

    void changed(DestPtr before, DestPtr& after, unsigned int index)
    {
        if (m_validator)
            (this->m_owner->*m_validator)(before, after, index);
    }
};

/**
 *  \brief Container of single links in the scenegraph, from a given type of object (Owner) to another (Dest)
 *
 */
template<class TOwnerType, class TDestType, unsigned TFlags>
class SingleLink : public TLink<TOwnerType,TDestType,TFlags&~BaseLink::FLAG_MULTILINK>
{
public:
    typedef TLink<TOwnerType,TDestType,TFlags&~BaseLink::FLAG_MULTILINK> Inherit;
    typedef TOwnerType OwnerType;
    typedef TDestType DestType;
    typedef typename Inherit::TraitsDestPtr TraitsDestPtr;
    typedef typename Inherit::DestPtr DestPtr;
    typedef typename Inherit::TraitsValueType TraitsValueType;
    typedef typename Inherit::ValueType ValueType;
    typedef typename Inherit::TraitsContainer TraitsContainer;
    typedef typename Inherit::Container Container;
    typedef typename Inherit::TraitsOwnerCasts TraitsOwnerCasts;
    typedef typename Inherit::TraitsDestCasts TraitsDestCasts;

    typedef void (OwnerType::*ValidatorFn)(DestPtr, DestPtr&);

    SingleLink(const BaseLink::InitLink<OwnerType>& init)
        : Inherit(init), m_validator(NULL)
    {
    }

    SingleLink(const BaseLink::InitLink<OwnerType>& init, DestPtr val)
        : Inherit(init), m_validator(NULL)
    {
        if (val) this->add(val);
    }

    virtual ~SingleLink()
    {
    }

    void setValidator(ValidatorFn fn)
    {
        m_validator = fn;
    }

    DestType* get(const core::ExecParams* params = 0) const
    {
        const int aspect = core::ExecParams::currentAspect(params);
        return TraitsDestPtr::get(this->m_value[aspect].get());
    }

    void reset()
    {
        const int aspect = core::ExecParams::currentAspect();
        DestPtr v = this->m_value[aspect].get();
        if (!v) return;
        this->m_value[aspect].get() = NULL;
        changed(v, this->m_value[aspect].get(), 0);
        TraitsContainer::remove(this->m_value[aspect],0);
    }

    void set(DestPtr v)
    {
        if (!v) { reset(); return; }
        const int aspect = core::ExecParams::currentAspect();
        DestPtr& val = this->m_value[aspect].get();
        DestPtr before = val;
        val = v;
        changed(before, val, 0);
    }

#ifndef SOFA_DEPRECATE_OLD_API
    // Convenient operators to make a SingleLink appear as a regular pointer
    operator DestType*() const
    {
        return get();
    }
    DestType* operator->() const
    {
        return get();
    }
    DestType& operator*() const
    {
        return *get();
    }

    void operator=(DestPtr v)
    {
        set(v);
    }
#endif

protected:
    ValidatorFn m_validator;

    void changed(DestPtr before, DestPtr& after, unsigned int /*index*/)
    {
        if (m_validator)
            (this->m_owner->*m_validator)(before, after);
    }
};

} // namespace objectmodel

} // namespace core

// the SingleLink class is used everywhere
using core::objectmodel::SingleLink;

// the MultiLink class is used everywhere
using core::objectmodel::MultiLink;

} // namespace sofa

#endif
