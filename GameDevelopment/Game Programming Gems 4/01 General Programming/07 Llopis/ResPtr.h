
#ifndef RESPTR_H_
#define RESPTR_H_

#include <cstddef>
#include "ResourcePtrHolder.h"

class IResource;



template <class ResourceType>
class ResPtr
{
public:
    ResPtr(const ResPtr<ResourceType> & resPtr);
    explicit ResPtr(ResourceType * pRes);
    ResPtr(ResourcePtrHolder * pHolder = NULL);
    ~ResPtr();

	ResPtr<ResourceType> & operator= (const ResPtr<ResourceType> & resPtr);

    ResourceType * operator->() const { return static_cast<ResourceType *>(m_pResHolder->pRes); }
    ResourceType & operator*() const { return *(static_cast<ResourceType *>(m_pResHolder->pRes)); }

    bool operator==(ResPtr<ResourceType> res) const;
    bool operator!=(ResPtr<ResourceType> res) const { return !operator==(res); }
    bool operator< (ResPtr<ResourceType> res) const;


    ResourcePtrHolder * GetResPtrHolder() const { return m_pResHolder; }
    typedef ResourceType * PointerType;

private:
    ResourcePtrHolder * m_pResHolder;
};


template <class ResourceType>
ResPtr<ResourceType>::ResPtr(const ResPtr<ResourceType> & resPtr)
{
    m_pResHolder = resPtr.GetResPtrHolder();
}


template <class ResourceType>
ResPtr<ResourceType>::ResPtr(ResourceType * pRes)
{
    m_pResHolder = new ResourcePtrHolder(pRes); 
}

template <class ResourceType>
ResPtr<ResourceType>::ResPtr(ResourcePtrHolder * pHolder) : 
    m_pResHolder (pHolder)
{
}


template <class ResourceType>
ResPtr<ResourceType>::~ResPtr()
{
}


template <class ResourceType>
ResPtr<ResourceType> & ResPtr<ResourceType>::operator= (const ResPtr<ResourceType> & resPtr)
{
    if (m_pResHolder != resPtr.GetResPtrHolder())
    {
        m_pResHolder = resPtr.GetResPtrHolder();
    }
    return *this;
}



template <class ResourceType>
bool ResPtr<ResourceType>::operator==(ResPtr<ResourceType> res) const
{
    if (m_pResHolder == res.m_pResHolder)
        return true;
    // At this point, both holders cannot be NULL
    if (res.m_pResHolder == NULL && m_pResHolder->pRes == NULL)
        return true;
    if (m_pResHolder == NULL && res.m_pResHolder->pRes == NULL)
        return true;
    if (m_pResHolder != NULL && res.m_pResHolder != NULL &&
        m_pResHolder->pRes == res.m_pResHolder->pRes)
        return true;
    return false;
}


template <class ResourceType>
bool ResPtr<ResourceType>::operator< (ResPtr<ResourceType> res) const
{
    if (res.m_pResHolder == NULL)
        return false;
    if (m_pResHolder == NULL)
        return true;
    return (m_pResHolder->pRes < res.m_pResHolder->pRes);
}



template <class TargetT, class SourceT>
TargetT rp_static_cast(SourceT rpSource)
{
    SourceT::PointerType pSource = NULL;
    TargetT::PointerType pTarget = static_cast<TargetT::PointerType>(pSource);
    pTarget;
    return TargetT(rpSource.GetResPtrHolder());
}



#endif 
