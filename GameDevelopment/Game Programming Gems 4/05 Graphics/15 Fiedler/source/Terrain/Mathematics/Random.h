// Random routines

#pragma once

#include "Float.h"
#include "Vector.h"


namespace Mathematics
{
    /// random number in range [0,max-1]

    inline int random(int max)
    {
	    if (!max) return 0;
        else return rand() % max;
    }
    
    /// random float in range [0,1]

    inline float randomFloat(float scale = 1.0f)
    {
	    return scale * ((float)(rand()) * (1.0f/(float)RAND_MAX));
    }      

    /// random float in range [min,max]

    inline float randomFloat(float min, float max)
    {
	    if (min==max) return min;
	    float r = ((float)(rand()) * (1.0f/(float)RAND_MAX));
	    return r * (max-min) + min;
    }

    /// random vector in range [(-1,-1,-1), (1,1,1)]

    inline Vector randomVector()
    {
	    return Vector( randomFloat(-1,1), randomFloat(-1,1), randomFloat(-1,1) );
    }    
}
