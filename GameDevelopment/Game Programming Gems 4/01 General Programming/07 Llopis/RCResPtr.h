
#ifndef RCRESPTR_H_
#define RCRESPTR_H_

#include <cstddef>
#include "RCResourcePtrHolder.h"

class IResource;



template <class ResourceType>
class RCResPtr
{
public:
    RCResPtr(const RCResPtr<ResourceType> & resPtr);
    explicit RCResPtr(ResourceType * pRes);
    RCResPtr(RCResourcePtrHolder * pHolder = NULL);
    ~RCResPtr();

	RCResPtr<ResourceType> & operator= (const RCResPtr<ResourceType> & resPtr);

    ResourceType * operator->() const { return static_cast<ResourceType *>(m_pResHolder->pRes); }
    ResourceType & operator*() const { return *(static_cast<ResourceType *>(m_pResHolder->pRes)); }

    bool operator==(RCResPtr<ResourceType> res) const;
    bool operator!=(RCResPtr<ResourceType> res) const { return !operator==(res); }
    bool operator< (RCResPtr<ResourceType> res) const;


    RCResourcePtrHolder * GetResPtrHolder() const { return m_pResHolder; }
    typedef ResourceType * PointerType;

private:
    RCResourcePtrHolder * m_pResHolder;
};


template <class ResourceType>
RCResPtr<ResourceType>::RCResPtr(const RCResPtr<ResourceType> & resPtr)
{
    m_pResHolder = resPtr.GetResPtrHolder();
    if (m_pResHolder != NULL)
        m_pResHolder->AddRef();    
}


template <class ResourceType>
RCResPtr<ResourceType>::RCResPtr(ResourceType * pRes)
{
    m_pResHolder = new RCResourcePtrHolder(pRes); 
    m_pResHolder->AddRef();    
}

template <class ResourceType>
RCResPtr<ResourceType>::RCResPtr(RCResourcePtrHolder * pHolder) : 
    m_pResHolder (pHolder)
{
    if (m_pResHolder != NULL)
        m_pResHolder->AddRef();    
}


template <class ResourceType>
RCResPtr<ResourceType>::~RCResPtr()
{
    if (m_pResHolder != NULL)
        m_pResHolder->Release(); 
}


template <class ResourceType>
RCResPtr<ResourceType> & RCResPtr<ResourceType>::operator= (const RCResPtr<ResourceType> & resPtr)
{
    if (m_pResHolder != resPtr.GetResPtrHolder())
    {
        if (m_pResHolder != NULL)
            m_pResHolder->Release(); 
        m_pResHolder = resPtr.GetResPtrHolder();
        if (m_pResHolder != NULL)
            m_pResHolder->AddRef();    
    }
    return *this;
}



template <class ResourceType>
bool RCResPtr<ResourceType>::operator==(RCResPtr<ResourceType> res) const
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
bool RCResPtr<ResourceType>::operator< (RCResPtr<ResourceType> res) const
{
    if (res.m_pResHolder == NULL)
        return false;
    if (m_pResHolder == NULL)
        return true;
    return (m_pResHolder->pRes < res.m_pResHolder->pRes);
}






#endif 
