/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Terrain Generator Class Functions

#include "tgen.h"
#include <stdlib.h>
#include <math.h>

void TerrainGenerator::SetSeed(int nSeed)
{
	this->starting_seed = nSeed;
}

void TerrainGenerator::SetMaximumHeight(int nHeight)
{
	this->maximum_height = nHeight;
}

void TerrainGenerator::SetQuadrantSize(int nQuad)
{
	this->quad_size = nQuad;
}

void TerrainGenerator::SetWaterLevel(int nPercentWater)	    // Set the amount of water
{
	this->percent_water = nPercentWater;
}

void TerrainGenerator::SetPlainsLevel(int nPercentPlains)	    // Set the amount of plains
{
	this->percent_plains = nPercentPlains;
}

void TerrainGenerator::SetHillsLevel(int nPercentHills)	    // Set the amount of hills
{
	this->percent_hills = nPercentHills;
}

void TerrainGenerator::SetMountainsLevel(int nPercentMountains)	// Set the amount of mountains
{
	this->percent_mountains = nPercentMountains;
}

void TerrainGenerator::BuildBlankTerrain()
{
	int x_pos, y_pos;							// Terrain Co-ordinates

	for (y_pos = 0; y_pos < this->terrain_height; y_pos++)
	{
		for (x_pos = 0; x_pos < this->terrain_width; x_pos++)
		{
			this->terrain[x_pos][y_pos] = 0;
		}
	}
}

void TerrainGenerator::BuildFuzzyTerrain()
{
	int nCurrentRandomValue;				// The last selected random value
	int x_pos, y_pos;							// Terrain Co-ordinates

	srand(this->starting_seed);			// Seed the random number generator

	for (y_pos = 0; y_pos < this->terrain_height; y_pos++)
	{
		srand(y_pos);
		for (x_pos = 0; x_pos < this->terrain_width; x_pos++)
		{						
			// srand(x_pos + (x_pos * y_pos));
			nCurrentRandomValue = rand() % this->maximum_height;
			this->terrain[x_pos][y_pos] = nCurrentRandomValue;
		}
	}

	// this->quad_size = 0;
}

void TerrainGenerator::BuildFaultLines(int nLines)
{
	int nCurrentRandomValue;				// The last selected random value
	int x_pos, y_pos;							// Terrain Co-ordinates
	int i;
	float x_start, y_start, x_end, y_end;
	float x_diff, y_diff;
	double pi = 3.1415926535;
	
	srand(this->starting_seed);			// Seed the random number generator

	for (y_pos = 0; y_pos < this->terrain_height; y_pos++)
	{
		for (x_pos = 0; x_pos < this->terrain_width; x_pos++)
		{
			this->terrain[x_pos][y_pos] = 0;
		}
	}

	for (i = 0; i < nLines; i++)
	{
		
		if ((rand() % 100) < 50)
		{
			if ((rand() % 100) < 50)
			{
				x_start = (float) (rand() % (this->terrain_height / 2));
				x_end   = x_start + (rand() % (this->terrain_height / 2));
			}
			else
			{
				x_start = (float) this->terrain_height - (rand() % (this->terrain_height / 2));
				x_end   = x_start - (rand() % (this->terrain_height / 2));
			}
			y_start = 0.0;
			y_end   = (float)this->terrain_height-1;

			if (x_start < (this->terrain_height / 2))
			{
				x_diff = (max(x_start,x_end) - min(x_start,x_end)) / (float)this->terrain_height;
			}
			else
			{	
				x_diff = (min(x_start,x_end) - max(x_start,x_end)) / (float)this->terrain_height;
			}
			
			y_diff = 1.0;
		}
		else
		{
			if ((rand() % 100) < 50)
			{
				y_start = (float)(rand() % (this->terrain_height / 2));
				y_end = y_start + (rand() % (this->terrain_height / 2));
			}
			else
			{
				y_start = (float)this->terrain_height - (rand() % (this->terrain_height / 2));
				y_end = y_start - (rand() % (this->terrain_height / 2));
			}
			x_start = 0;
			x_end = (float)this->terrain_height-1;

			if (y_start < (this->terrain_height / 2))
			{
				y_diff = (max(y_start,y_end) - min(y_start,y_end)) / (float)this->terrain_width;
			}
			else
			{
				y_diff = (min(y_start,y_end) - max(y_start,y_end)) / (float)this->terrain_width;
			}
			
			x_diff = 1.0;
		}

      // nCurrentRandomValue = rand() % 255;
		nCurrentRandomValue = 0;
		do
		{
			this->terrain[(int)x_start][(int)y_start] = nCurrentRandomValue;
			x_start = x_start + x_diff;
			y_start = y_start + y_diff;
			// Apply a sine function oscillating between 0 and 255
			// the sin function should be called with values from -pi/2 to pi/2
			// nCurrentRandomValue = ((nCurrentRandomValue + (int)(((sin(((pi / 50.0) * x_start) - (pi / 2.0))) * 128) + 128)) % 255);
			if (x_diff < y_diff)
			{
				nCurrentRandomValue = (int)((sin(x_start) * 128.0) + 128.0);
			}
			else
			{
				nCurrentRandomValue = (int)((sin(y_start) * 128.0) + 128.0);
			}

		} while (((y_start < (float)this->terrain_width) && (y_start > 0.0)) && ((x_start < (float)this->terrain_height) && (x_start > 0.0)));
	}
   // this->quad_size = 0;
}

void TerrainGenerator::DisplayTerrain(HDC hDC, RECT * rcCanvas)
{
	HPEN hPen;
	HBRUSH hBrush;
	int x_pos, y_pos;
	int nHeight, nHeightTest;
	COLORREF nColor;
	RECT rcSquare;
	double x_diff, y_diff;
	int nMin, nMax;
	unsigned long ulAverage;

	x_diff = ((double) (rcCanvas->right - rcCanvas->left)) / (float)this->terrain_width;
	y_diff = ((double) (rcCanvas->bottom - rcCanvas->top)) / (float)this->terrain_height;

	rcSquare.top = 0;
	nMin = 255;
	nMax = 0;
	ulAverage = 0;

	for (y_pos = 0; y_pos < this->terrain_height; y_pos++)
	{
		for (x_pos = 0; x_pos < this->terrain_width; x_pos++)
		{
			nHeight = this->terrain[x_pos][y_pos];
			if (nHeight > nMax) nMax = nHeight;
			if (nHeight < nMin) nMin = nHeight;
			ulAverage = ulAverage + nHeight;
		}
	}

	ulAverage = ulAverage / (x_pos * y_pos);

	for (y_pos = 0; y_pos < this->terrain_height; y_pos++)
	{
		rcSquare.bottom = (int)((double) rcSquare.top + y_diff);
		rcSquare.left = 0;
		for (x_pos = 0; x_pos < this->terrain_width; x_pos++)
		{
			rcSquare.right = (int)((double) rcSquare.left + x_diff);

			nHeight = this->terrain[x_pos][y_pos];

			// nColor = PALETTERGB(nHeight,nHeight,nHeight);
			nHeight = (int)((nHeight * 100.0)	/ (nMax - nMin));
			nHeightTest = this->percent_water;
			if (nHeight < nHeightTest) 
			{
				nColor = PALETTERGB(0,0,255);
			}
			else
			{
				nHeightTest = nHeightTest + this->percent_plains;
				if (nHeight < nHeightTest) 
				{
					nColor = PALETTERGB(0,255,0);
				}
				else
				{
					nHeightTest = nHeightTest + this->percent_hills;
					if (nHeight < nHeightTest) 
					{
						nColor = PALETTERGB(128,128,0);
					}
					else
					{
						nColor = PALETTERGB(255,255,255);
					}
				}
			}

			hPen   = CreatePen(PS_SOLID,1,nColor);
			hBrush = CreateSolidBrush(nColor);

			SelectObject(hDC,hPen);
			SelectObject(hDC,hBrush);

			Rectangle(hDC,rcSquare.left,rcSquare.top,rcSquare.right,rcSquare.bottom);

			SelectObject(hDC,GetStockObject(BLACK_PEN));
			SelectObject(hDC,GetStockObject(WHITE_BRUSH));

			DeleteObject(hPen);
			DeleteObject(hBrush);

			rcSquare.left = (int)((double) rcSquare.left + x_diff);
		}
		rcSquare.top = (int)((double) rcSquare.top + y_diff);
	}
}

void  TerrainGenerator::SmoothTerrain(int nQuad)
{
	int row_offset = 0;  // start at zero for first row
	int width = this->terrain_width;
	int passes = this->quad_size;
	/* Get the memory (widthxwidth bytes) */
	// heightfield = new int[width][width];
	/* base value = 0, byte values are -128..127 */
	// for (int i=0;i<width;i++) for (int j=0;j<width;j++) this->terrain[i][j]=0;
    
	while (passes > 0)
	{

	for ( int square_size = width; square_size > 1; square_size /= 2 )
	{
		int random_range = square_size;
		
		for ( int x1 = row_offset; x1 < width; x1 += square_size )
		{
			for ( int y1 = row_offset; y1 < width; y1 += square_size )
			{
				// Get the four corner points.
				int x2 = (x1 + square_size) % width;
				int y2 = (y1 + square_size) % width;

				int i1 = this->terrain[x1][y1];
				int i2 = this->terrain[x2][y1];
				int i3 = this->terrain[x1][y2];
				int i4 = this->terrain[x2][y2];
				
				// Obtain new points by averaging the corner points.
				
				int p1 = ((i1 * 9) + (i2 * 3) + (i3 * 3) + (i4)) / 16;
				int p2 = ((i1 * 3) + (i2 * 9) + (i3) + (i4 * 3)) / 16;
				int p3 = ((i1 * 3) + (i2) + (i3 * 9) + (i4 * 3)) / 16;
				int p4 = ((i1) + (i2 * 3) + (i3 * 3) + (i4 * 9)) / 16;
				
				// Add a random offset to each new point.
				
				/* p1 = Rand1(p1, random_range);
				p2 = Rand1(p2, random_range);
				p3 = Rand1(p3, random_range);
				p4 = Rand1(p4, random_range); */

				/*p1 = p1 + (rand() % 255) % 255;
				p2 = p2 + (rand() % 255) % 255;
				p3 = p3 + (rand() % 255) % 255;
				p4 = p4 + (rand() % 255) % 255;*/
				
				// Write out the generated points.
				
				int x3 = (x1 + square_size/4) % width;
				int y3 = (y1 + square_size/4) % width;
				x2 = (x3 + square_size/2) % width;
				y2 = (y3 + square_size/2) % width;
				
				this->terrain [x3][y3]   = p1;
				this->terrain [x2][y3]   = p2;
				this->terrain [x3][y2]   = p3;
				this->terrain [x2][y2]   = p4;
			}
		}
		row_offset = square_size/4;  // set offset for next row
	}
	passes--;
	}
}

int  TerrainGenerator::GetDimension()
{
	return this->terrain_width;
}

void TerrainGenerator::SetDimension(int w)
{
	this->terrain_width = w;
	this->terrain_height = w;
}
	
void  TerrainGenerator::QuadTerrain(int nQuad)
{
	float fDivide;
	int nDivide;
	int nStartX, nStartY;
	int nX, nY;
	int nAverage, nSamples;

	if (nQuad <= 0) return;

	fDivide = (float)this->terrain_width / (float) nQuad;
	nDivide = this->terrain_width / nQuad;
	if ((float)nDivide != fDivide) nQuad++;
	if (nQuad > this->terrain_width) return;

	nStartX = 0;
	nStartY = 0;

	while (nStartY < this->terrain_width)
	{
		nStartX = 0;		
		while (nStartX < this->terrain_width)
		{
			nSamples = 0;
			nAverage = 0;
			for (nY = nStartY; nY < nStartY + nQuad; nY++)
			{
				for (nX = nStartX; nX < nStartX + nQuad; nX++)
				{
					nAverage = nAverage + this->terrain[nX][nY];
					nSamples++;
				}
			}
			nAverage = nAverage / nSamples;
			for (nY = nStartY; nY < nStartY + nQuad; nY++)
			{
				for (nX = nStartX; nX < nStartX + nQuad; nX++)
				{
					this->terrain[nX][nY] = nAverage;
				}
			}
			nStartX = nStartX + nQuad;
		}
		nStartY = nStartY + nQuad;
	}

}

