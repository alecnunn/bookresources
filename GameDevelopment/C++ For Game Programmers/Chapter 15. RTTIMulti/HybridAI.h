
#ifndef HYBRIDAI_H_
#define HYBRIDAI_H_


#include "LandAI.h"
#include "FlyingAI.h"
#include "RTTI.h"

class HybridAI : public LandAI, public FlyingAI  
{
    RTTI_DECL;
};

#endif 
