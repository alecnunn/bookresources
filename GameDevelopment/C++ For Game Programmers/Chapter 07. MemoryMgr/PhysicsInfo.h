

#ifndef PHYSICSINFO_H_
#define PHYSICSINFO_H_


#include "MemoryMgr.h"


// This is a class that is not part of a hierarchy


struct Vector3d
{
    float x, y, z;
};


class PhysicsInfo
{
public:
    Vector3d position;
    Vector3d velocity;
    Vector3d acceleration;
    bool bFlying;
    // More stuff here

private:
    DECLARE_HEAP;
};



#endif