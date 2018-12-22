/* Copyright (C) Jason Shankel, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jason Shankel, 2000"
 */
#include <stdlib.h>
#include <math.h>
#include <memory.h>

/*
height field typedef
*/
typedef float *height_field_t;

/*
Function prototypes
*/

float RangedRandom(float v1,float v2);
void NormalizeTerrain(height_field_t field,int size);
void FilterHeightBand(float *band,int stride,int count,float filter);
void FilterHeightField(height_field_t field,int size,float filter);

void MakeTerrainPlasma(height_field_t field,int size,float rough);

/*
Returns a random number between v1 and v2
*/
float RangedRandom(float v1,float v2)
{
	return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
}

/*
Given a height field, normalize it so that the minimum altitude
is 0.0 and the maximum altitude is 1.0
*/
void NormalizeTerrain(height_field_t field,int size)
{
	float maxVal,minVal,dh;
	int i;

	/*
	Find the maximum and minimum values in the height field
	*/ 
	maxVal = field[0];
	minVal = field[0];

	for (i=1;i<size*size;i++)
	{
		if (field[i] > maxVal) 
		{
			maxVal = field[i];
		}
		else if (field[i] < minVal) 
		{
			minVal = field[i];
		}
	}

	/*
	Find the altitude range (dh)
	*/
	if (maxVal <= minVal) return;
	dh = maxVal-minVal;

	/*
	Scale all the values so they are in the range 0-1
	*/
	for (i=0;i<size*size;i++)
	{
		field[i] = (field[i]-minVal)/dh;
	}

}

/*
Erosion filter -
FilterHeightBand applies a FIR filter across a row or column of the height field
*/
void FilterHeightBand(float *band,int stride,int count,float filter)
{
	int i,j=stride;
	float v = band[0];
	for (i=0;i<count-1;i++)
	{
		band[j] = filter*v + (1-filter)*band[j];
		v = band[j];
		j+=stride;
	}
}

/*
Erosion filter -
Erodes a terrain in all 4 directions
*/
void FilterHeightField(height_field_t field,int size,float filter)
{
	int i;

	/*
	Erode rows left to right
	*/
	for (i=0;i<size;i++)
	{
		FilterHeightBand(&field[size*i],1,size,filter);
	}
	
	/*
	Erode rows right to left
	*/
	for (i=0;i<size;i++)
	{
		FilterHeightBand(&field[size*i+size-1],-1,size,filter);
	}

	/*
	Erode columns top to bottom
	*/
	for (i=0;i<size;i++)
	{
		FilterHeightBand(&field[i],size,size,filter);
	}

	/*
	Erode columns bottom to top
	*/
	for (i=0;i<size;i++)
	{
		FilterHeightBand(&field[size*(size-1)+i],-size,size,filter);
	}
}


/*
Genereate terrain using diamond-square (plasma) algorithm
*/
void MakeTerrainPlasma(height_field_t field,int size,float rough)
{
	int i,j,ni,nj,mi,mj,pmi,pmj,rectSize = size;
	float dh = (float)rectSize/2,r = (float)pow(2,-1*rough);

	/*
	Since the terrain wraps, all 4 "corners" are represented by the value at 0,0,
		so seeding the heightfield is very straightforward
	Note that it doesn't matter what we use for a seed value, since we're going to
		renormalize the terrain after we're done
	*/
	field[0] = 0;


	while(rectSize > 0)
	{
		
		/*
		Diamond step -

		Find the values at the center of the retangles by averaging the values at 
		the corners and adding a random offset:


		a.....b
		.     .  
		.  e  .
		.     .
		c.....d   

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)

		*/
		       
		for (i=0;i<size;i+=rectSize)
		for (j=0;j<size;j+=rectSize)
		{
			
			ni = (i+rectSize)%size;
			nj = (j+rectSize)%size;

			mi = (i+rectSize/2);
			mj = (j+rectSize/2);

			field[mi+mj*size] = (field[i+j*size] + field[ni+j*size] + field[i+nj*size] + field[ni+nj*size])/4 + RangedRandom(-dh/2,dh/2);
		}

		/*
		Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		  so we don't need to calculate them

		The height field wraps, so we're never left hanging.  The right side of the last
			rectangle in a row is the left side of the first rectangle in the row.  The bottom
			side of the last rectangle in a column is the top side of the first rectangle in
			the column

              .......
		      .     .
		      .     .
		      .  d  .
		      .     .
		      .     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random
		
		In the code below:
			a = (i,j) 
			b = (ni,j) 
			c = (i,nj) 
			d = (mi,pmj) 
			e = (pmi,mj) 
			f = (mi,mj) 
			g = (mi,j)
			h = (i,mj)

		*/
		for (i=0;i<size;i+=rectSize)
		for (j=0;j<size;j+=rectSize)
		{

			ni = (i+rectSize)%size;
			nj = (j+rectSize)%size;

			mi = (i+rectSize/2);
			mj = (j+rectSize/2);

			pmi = (i-rectSize/2+size)%size;
			pmj = (j-rectSize/2+size)%size;

			
			/*
			Calculate the square value for the top side of the rectangle
			*/
			field[mi+j*size] = (field[i+j*size] + field[ni+j*size] + field[mi+pmj*size] + field[mi+mj*size])/4 + RangedRandom(-dh/2,dh/2);

			
			/*
			Calculate the square value for the left side of the rectangle
			*/
			field[i+mj*size] = (field[i+j*size] + field[i+nj*size] + field[pmi+mj*size] + field[mi+mj*size])/4 + RangedRandom(-dh/2,dh/2);

		}

		/*
		Setup values for next iteration
		At this point, the height field has valid values at each of the coordinates that fall on a rectSize/2 boundary
		*/
		rectSize /= 2;
		dh *= r;
	}
	/*
	Normalize terrain so minimum value is 0 and maximum value is 1
	*/
	NormalizeTerrain(field,size);
}


