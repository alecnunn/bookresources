/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// main.cpp
//
// program to test polynomial stuff

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "DifferentiableFunction.h"
#include "TrigFunction.h"
#include "Poly.h"

int main(int argc, char** argv)
{
	// make Taylor Series

	Poly	taylor;
	TrigFunction	sinefn;

	taylor.MakeTaylorSeries(&sinefn, 0.0, 11);
	taylor.Print();

	// make Lagrange series

	Poly	lagrange;

	double pi = 4.0 * atan(1.0);

	Number	x[11];
	Number	y[11];

	x[0] = 0;
	x[1] = (float)pi * 0.25f;
	x[2] = (float)pi * 0.33333333333333f;
	x[3] = (float)pi * 0.5f;
	x[4] = (float)pi * 0.75f;
	x[5] = (float)pi;
	x[6] = -x[1];
	x[7] = -x[2];
	x[8] = -x[3];
	x[9] = -x[4];
	x[10] = -x[5];

	int ii;
	for (ii = 0; ii < 11; ii++)
	{
		y[ii] = (float)sin(x[ii]);
	}

	lagrange.MakeLagrangeSeries(11, x, y);
	lagrange.Print();

	// calculate errors

	Number	maxerr_taylor = 0.0;
	Number	maxerr_lagrange = 0.0;
	Number	toterr_taylor = 0.0;
	Number	toterr_lagrange = 0.0;

	for (ii = -256; ii <= 256; ii++)
	{
		Number	number = (float)pi * float(ii) / 256;

		Number	exact = (float)sin(number);

		Number	taylor_approx = taylor.Evaluate(number);
		Number	lagrange_approx = lagrange.Evaluate(number);

		Number	taylor_error = (float)fabs(taylor_approx - exact);
		Number	lagrange_error = (float)fabs(lagrange_approx - exact);

		if (taylor_error > maxerr_taylor)		maxerr_taylor = taylor_error;
		if (lagrange_error > maxerr_lagrange)	maxerr_lagrange = lagrange_error;
		toterr_taylor += taylor_error;
		toterr_lagrange += lagrange_error;
	}

	toterr_taylor /= 513.0;
	toterr_lagrange /= 513.0;

	printf("Taylor Series:\nMax err = %f\nAvg err = %f\n\n", maxerr_taylor, toterr_taylor);
	printf("Lagrange series:\nMax err = %f\nAvg err = %f\n\n", maxerr_lagrange, toterr_lagrange);
		
	return 0;
}
