#include "hammingwindow.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Hamming(float *src, float *dst, int size)
{
	static int windowSize = 0;
	static float *window = 0;

	//Only recompute window if the size has changed.
	if ( windowSize != size)
	{
		delete[] window;
		windowSize = size;
		window = new float[windowSize];

		//w(i) = 0.54 - 0.46*cos(2*pi*(i)'/(M-1));

		float invM_1 = 1/float(windowSize-1);
		for(int i=0; i< windowSize; i++)
		{
			window[i] = 0.54f - 0.46f*cosf( 2*float(M_PI)*i*invM_1);
		}
	}

	for(int i=0; i<windowSize; i++)
	{
		*dst = *src * window[i];
		dst++; src++;
	}
}