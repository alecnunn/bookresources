#include "Spline.h"
#include <assert.h>


float clamp(float x, float a, float b)
{
	return (x < a ? a : (x > b ? b : x));
};

/* Coefficients of basis matrix for spline() */
#define CR00 -0.5  
#define CR01  1.5
#define CR02 -1.5
#define CR03  0.5
#define CR10  1.0 
#define CR11 -2.5
#define CR12  2.0
#define CR13 -0.5
#define CR20 -0.5
#define CR21  0 
#define CR22  0.5
#define CR23  0
#define CR30  0 
#define CR31  1.0
#define CR32  0
#define CR33  0 


State spline (float x, int nknots, State *knot)
{
	int span;
	int nspans  = nknots - 3;
	State c0, c1, c2, c3; /* coeff of the cubic */

	if (nspans < 1) 
	{
		assert(false);
		/* illegal */
		return c0;
	}

	/* Find the approx 4-point span of the spline */
	x = clamp(x, 0, 1)*nspans;
	span = (int) x;
	if (span >= nknots-3)
		span = nknots-3;
	x-=span;
	knot+=span;

	/*Evaluate the span cubit at x using Horner's rule */
	c3 = CR00*knot[0] + CR01*knot[1] + CR02*knot[2] + CR03*knot[3];
	c2 = CR10*knot[0] + CR11*knot[1] + CR12*knot[2] + CR13*knot[3];
	c1 = CR20*knot[0] + CR21*knot[1] + CR22*knot[2] + CR23*knot[3];
	c0 = CR30*knot[0] + CR31*knot[1] + CR32*knot[2] + CR33*knot[3];

	return (((c3*x+c2)*x+c1)*x+c0);

}