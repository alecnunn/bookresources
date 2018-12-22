#ifndef TESTRES_H_
#define TESTRES_H_

#include "Resource.h"

class TestRes : public IResource
{
public:
    virtual ~TestRes() { s_nDeleted++; }
    virtual const char * GetResourceType() const { return "PointerTestResource"; }
    int num;
    int GetNum() const { return num; }

    static int s_nDeleted;
};


#endif