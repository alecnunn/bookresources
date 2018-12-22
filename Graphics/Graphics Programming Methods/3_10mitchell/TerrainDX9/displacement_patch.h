// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef DISPLACEMENT_PATCH_H
#define DISPLACEMENT_PATCH_H

#ifndef DEBUG_H
#include "debug.h"
#endif

#ifndef SHADER_H
#include "shader.h"
#endif

#define PATCH_POWER_OF_2 6
#define PATCH_WIDTH (1<<PATCH_POWER_OF_2)
#define PATCH_SIZE (PATCH_WIDTH+1)
#define NUM_PATCH_VERTICES (PATCH_SIZE*PATCH_SIZE)
#define PATCH_SCALE 4.0f
#define PATCH_WORLD_WIDTH (PATCH_SCALE*PATCH_WIDTH)
#define INV_PATCH_WORLD_WIDTH (1.0f/PATCH_WORLD_WIDTH)
#define HALF_PATCH_WORLD_WIDTH (PATCH_WORLD_WIDTH*0.5f)
#define HEIGHT_SCALE (0.75f/512.0f)

#define MAP_WIDTH_POWER_OF_2 9
#define MAP_WIDTH (1<<MAP_WIDTH_POWER_OF_2)
#define MAP_SIZE (MAP_WIDTH*MAP_WIDTH)
#define MAP_VERTEX_STRIDE_POWER_OF_2 (MAP_WIDTH_POWER_OF_2-PATCH_POWER_OF_2)
#define MAP_VERTEX_STRIDE (1<<MAP_VERTEX_STRIDE_POWER_OF_2)

#define TEX_COORD_RANGE 8.0f

#define MAX_VERTEX_BUFFERS 512

#define TILE_GRID_WIDTH_POWER_OF_2 3
#define TILE_GRID_WIDTH (1<<TILE_GRID_WIDTH_POWER_OF_2)
#define NUM_GRID_TILES (TILE_GRID_WIDTH*TILE_GRID_WIDTH)

class DISPLACEMENT_PATCH_VERTEX
{
public:
	float x,y,z;
};

class Displacement_Patch
{
public:
	Displacement_Patch();
	~Displacement_Patch();

	void Render();
	void Render_Tiled();

private:

	void Create_Vertex_Buffer();
	void Create_Index_Buffer();
	void Create_Displacement_Map();
	void Create_Shader();

	LPDIRECT3DVERTEXBUFFER9 Vertex_Buffer;
	DWORD							Num_Vertices;

	LPDIRECT3DINDEXBUFFER9	Index_Buffer;
	int							Num_Indices;

	Shader_Class*				Shader;

	// displacement data
	float							Displacement_Scale;
	unsigned char*				Displacement_Map;
};

#endif
