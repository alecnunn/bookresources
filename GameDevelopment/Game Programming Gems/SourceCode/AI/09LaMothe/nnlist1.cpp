/* Copyright (C) Andre' LaMothe, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andre' LaMothe, 2000"
 */
// MCULLOCCH PITTS SIMULATOR /////////////////////////////////////////////////////

// INCLUDES //////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <ctype.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>

// MAIN //////////////////////////////////////////////////////////////////////////

void main(void)
{
float	threshold,	// this is the theta term used to threshold the summation 
		w1,w2,		// these hold the weights
		x1,x2,		// inputs to the neurode		
		y_in,		// summed input activation
		y_out;		// final output of neurode
		
printf("\nMcCulloch-Pitts Single Neurode Simulator.\n");
printf("\nPlease Enter Threshold?");
scanf("%f",&threshold);

printf("\nEnter value for weight w1?");
scanf("%f",&w1);

printf("\nEnter value for weight w2?");
scanf("%f",&w2);

printf("\n\nBegining Simulation:");

// enter main event loop

while(1)
	{
	printf("\n\nSimulation Parms: threshold=%f, W=(%f,%f)\n",threshold,w1,w2);

	// request inputs from user
	printf("\nEnter input for X1?");
	scanf("%f",&x1);

	printf("\nEnter input for X2?");
	scanf("%f",&x2);

	// compute activation
	y_in = x1*w1 + x2*w2;

	// input result to activation function (simple binary step)
	if (y_in>=threshold)
		y_out = (float)1.0;
	else
		y_out = (float)0.0;

	// print out result
	printf("\nNeurode Output is %f\n",y_out);

	// try again
	printf("\nDo you wish to continue Y or N?");
	char ans[8];
	scanf("%s",ans);
	if (toupper(ans[0])!='Y')
		break;
		
	} // end while

printf("\n\nSimulation Complete.\n");

} // end main
