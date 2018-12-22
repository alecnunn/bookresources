/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
#ifndef _TGEN_H
#define _TGEN_H

#ifdef _WIN32
#include <windows.h>
#endif

class TerrainGenerator {
	int starting_seed;
	int maximum_height;
	int quad_size;
	int percent_water;
	int percent_plains;
	int percent_hills;
	int percent_mountains;
	int terrain_width, terrain_height;
	int	terrain[256][256];				// Mark's random numebrs

public:
	void SetSeed(int nSeed);
	void SetMaximumHeight(int nHeight);
	void SetQuadrantSize(int nQuad);
	void SetWaterLevel(int nPercentWater);
	void SetPlainsLevel(int nPercentPlains);
	void SetHillsLevel(int nPercentHills);
	void SetMountainsLevel(int nPercentMountains);
	void SetDimension(int w);

	int GetSeed() {return starting_seed;};
	int GetMaximumHeight() {return maximum_height;};
	int GetQuadrantSize() {return quad_size;};
	int GetWaterLevel() {return percent_water;};
	int GetPlainsLevel() {return percent_plains;};
	int GetHillsLevel() {return percent_hills;};
	int GetMountainsLevel() {return percent_mountains;};
	int GetDimension();

	void BuildBlankTerrain();
	void BuildFuzzyTerrain();
	void BuildFaultLines(int nLines);
	void DisplayTerrain(HDC hDC, RECT * rcCanvas);
	void SmoothTerrain(int nQuad);
	void QuadTerrain(int nQuad);
};


#endif