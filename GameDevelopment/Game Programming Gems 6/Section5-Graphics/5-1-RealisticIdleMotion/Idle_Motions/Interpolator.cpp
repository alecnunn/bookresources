#include "Interpolator.h"
#include <math.h>

#define PI 3.14159265358979323846

float Interpolator::twoPointInterpol(const float& x1, const float& y1, 
								  const float& x2, const float& y2,
								  const float& x,
								  const interpolTp& i) {

	if (i == linear)
		// linear interpolation
		return  (y1 + (y2-y1)/(x2-x1) * (x-x1));
	else if (i == cosine)
		// goniometric interpolation
		return ((y1-y2)/2.0f * (float)cos(PI*(x-x1)/(x2-x1)) + (y1 + y2)/2.0f);
	else
		return 0.0f;
								  
}
