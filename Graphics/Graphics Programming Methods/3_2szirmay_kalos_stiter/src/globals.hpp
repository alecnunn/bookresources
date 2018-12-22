#pragma once

#include <stdlib.h>
#include <float.h>
#include <math.h>

typedef enum {X_DOMINANT, Y_DOMINANT, Z_DOMINANT} DominantAxis;

#define EPSILON				1e-5F 
#define MAX_DISTANCE		1e+7f

#define M_PI				3.14159265358979323846f
#define	M_1_PI				0.31830988618379067154f
#define PI2					((float)(2.0f*M_PI))

/* Get minimum/maximum of two values. */

#ifndef MIN
#define MIN(x,y) (((x)>(y))?(y):(x))
#endif

#ifndef MAX
#define MAX(x,y) (((x)<(y))?(y):(x))
#endif

/* Get minimum/maximum of three values. */

#define MAX3(x,y,z) (MAX(MAX((x),(y)),(z)))
#define MIN3(x,y,z) (MIN(MIN((x),(y)),(z)))

// rand() is in the range [0..RAND_MAX], both get 0 and RAND_MAX (I have tested it)
// so RAND generates in [0,1)
#define RAND  ((float)rand () / (RAND_MAX + 1))	
