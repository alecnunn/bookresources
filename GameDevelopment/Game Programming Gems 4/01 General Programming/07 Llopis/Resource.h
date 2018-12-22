
#ifndef IRESOURCE_H_
#define IRESOURCE_H_

#include "ResPtr.h"
#include "RCResPtr.h"


class IResource
{
public:
    virtual ~IResource() {};
    virtual const char * GetResourceType() const = 0;
};


typedef ResPtr<IResource> ResourcePtr;
typedef RCResPtr<IResource> RCResourcePtr;



#endif 
