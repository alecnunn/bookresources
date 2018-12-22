
#include "resourceptrholder.h"
#include "resource.h"



ResourcePtrHolder::~ResourcePtrHolder() 
{ 
    delete pRes; 
}


