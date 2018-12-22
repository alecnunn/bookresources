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

void MakeTerrainDeposition(height_field_t field,int size,int jumps,int peakWalk, int minParticlesPerJump,int maxParticlesPerJump,float caldera);

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


void MakeTerrainDeposition(height_field_t field,int size,int jumps,int peakWalk, int minParticlesPerJump,int maxParticlesPerJump,float caldera)
{
	int x,y,i,j,m,p,px,py,minx,miny,maxx,maxy,sx,sy,numParticles,tx,ty,done;
	int dx[] = {0,1,0,size-1,1,1,size-1,size-1};
	int dy[] = {1,0,size-1,0,size-1,1,size-1,1};
	float ch,ph;
	int *calderaMap = (int *)malloc(size*size*sizeof(int));

	/*
	Clear out height field
	*/
	for (i=0;i<size*size;i++)
	{
		field[i] = 0.0f;
	}
	
	for (p=0;p<jumps;p++)
	{
		
		/*
		Pick a random spot
		*/
		x = rand()%size;
		y = rand()%size;

		/*
		px,py will track where we form the caldera
		*/
		px = x;
		py = y;
		
		
		/*
		Determine how many particles we're going to drop
		*/
		numParticles = (int)RangedRandom((float)minParticlesPerJump,(float)maxParticlesPerJump);
		
		for (i=0;i<numParticles;i++)
		{

			/*
			If it's time to move the drop point, agitate it in a random direction
			*/
			if (peakWalk!=0 && (i%peakWalk)==0)
			{
				m = rand()%8;
				x = (x+dx[m]+size)%size;
				y = (y+dy[m]+size)%size;
			}


			/*
			Drop the particle
			*/
			field[x+y*size] += 1.0f;
			
			
			/*
			Now agitate it until it settles
			*/
			sx = x;
			sy = y;
			done = 0;
			
			while (done == 0)
			{
				done = 1;
				
				/*
				Pick a random neighbor to start inspecting
				*/
				m = rand();
				for (j=0;j<8;j++)
				{
					tx = (sx+dx[(j+m)%8])%size;
					ty = (sy+dy[(j+m)%8])%size;
					
					/*
					If we can move to this neighbor, do it
					*/
					if (field[tx+ty*size] + 1.0f < field[sx+sy*size])
					{
						field[tx+ty*size] += 1.0f;
						field[sx+sy*size] -= 1.0f;
						sx = tx;
						sy = ty;
						done = 0;
						break;
					}
				}
			}

			/*
			Check to see if our latest point is higher than the caldera point 
			If so, move the caldera point here
			*/
			if (field[sx+size*sy] > field[px+size*py])
			{
				px = sx;
				py = sy;
			}
		}

		/*
		Now that we're done with the peak, invert the caldera
		ch is the caldera cutoff altitude
		ph is the height at the caldera start point

		Note that this technique will not create a caldera at every
			peak, rather it will create one caldera per jump.  It is
			possible for the algorithm to generate multiple peaks per jump.
		*/
		ph = field[px+size*py];
		ch = ph*(1.0f-caldera);


		/*
		We're going to do a floodfill, so we use an array of integers to mark visited locations
		0 = unmarked, unvisited
		1 = marked, unvisited
		2 = marked, visited
		*/
		memset(calderaMap,0,size*size*sizeof(int));

		minx = px;
		maxx = px;
		miny = py;
		maxy = py;

		/*
		Mark the start location for the caldera
		*/
		calderaMap[px+size*py] = 1;

		done = 0;

		while (done == 0)
		{
			done = 1;
			sx = minx;
			sy = miny;
			tx = maxx;
			ty = maxy;
			
			/*
			Examine the bounding rectangle looking for unvisited neighbors
			*/
			for (x=sx;x<=tx;x++)
			for (y=sy;y<=ty;y++)
			{
				px = (x+size)%size;
				py = (y+size)%size;

				/*
				If this cell is marked but unvisited, check it out
				*/
				if(calderaMap[px+size*py] == 1)
				{
					/*
					Mark cell as visited
					*/
					calderaMap[px+size*py] = 2;

					/*
					If this cell should be inverted, invert it and inspect neighbors
					Mark any unmarked, unvisited neighbor
					Don't invert any cells whose height exceeds our initial caldera height
					This prevents small peaks from destroying large ones
					*/
					if(field[px+size*py] > ch && field[px+size*py] <= ph)
					{
						done = 0;
						field[px+size*py] = 2*ch - field[px+size*py];
					
						/*
						Left and right neighbors
						*/
						px = (px+1)%size;
						if(calderaMap[px+size*py]==0)
						{
							if (x+1 > maxx) maxx = x+1;
							calderaMap[px+size*py] = 1;
						}
						px = (px+size-2)%size;
						if(calderaMap[px+size*py]==0)
						{
							if (x-1 < minx) minx = x-1;
							calderaMap[px+size*py] = 1;
						}

						/*
						Top and bottom neighbors
						*/
						px = (x+size)%size;
						py = (py+1)%size;
						if(calderaMap[px+size*py]==0)
						{
							if (y+1 > maxy) maxy = y+1;
							calderaMap[px+size*py] = 1;
						}
						py = (py+size-2)%size;
						if(calderaMap[px+size*py]==0)
						{
							if (y-1 < miny) miny = y-1;
							calderaMap[px+size*py] = 1;
						}
					}

				}
			}
		}

	}

	free(calderaMap);
	
	/*
	Calderas increase aliasing, so erode terrain with a filter value proportional to the prominence of the caldera
	*/
	FilterHeightField(field,size,caldera);
	
	NormalizeTerrain(field,size);
	
}
