

#ifndef RCRESOURCEPTRHOLDER_H_
#define RCRESOURCEPTRHOLDER_H_

#include "RefCounted.h"

class IResource;


class RCResourcePtrHolder : public RefCounted
{
public:
    RCResourcePtrHolder (IResource * p) : pRes(p) {}
    ~RCResourcePtrHolder();
    IResource * pRes;
};



#endif
