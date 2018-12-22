/* Copyright (C) Jan Svarovsky, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jan Svarovsky, 2000"
 */
#include <stdio.h>
#include <assert.h>
#include "mtxlib.h"

// This is your game object base class
class GameObject;

//////////////////////////////////////////////////////////////////////////////
//
// External definitions

// If the map decides two objects are close enough together, it will call this function, which you
// have to provide
extern void process_collision(GameObject *a, GameObject *b);

//////////////////////////////////////////////////////////////////////////////
//
// A game object. Derive your own objects off this
class GameObject
{
public:
	GameObject()
	{
		NextInMap = NULL;
		MapSquare = NULL;

		MapRes = 0;
	}

	// The object is in a singly linked list hanging off one of the map squares
	GameObject *NextInMap;

	// And this is the map square that this object is hanging off, ie the start of that list
	GameObject **MapSquare;
	
	// The resolution of the map the object is sitting in
	int MapRes;

	// Take the object out of the map's linked list
	void RemoveFromMap();

	// calls "process_collision" on all the relevant other objects in the map,
	// as per the gem
	void ProcessCollisions(class Map *my_map);

private:

	// Do one resolution of map, used by ProcessCollisions()
	void ProcessOneLevel(Map *my_map, GameObject **map_who, GameObject *walker, int current_res);
};

//////////////////////////////////////////////////////////////////////////////
//
// The map.

// for efficiency's sake, the map dimensions etc are constants, you could simply
// turn them into variables if you so wished

// (1 << this) is number of map squares at highest res
#define MAP_HI_RES_SHIFT         (8)

// and the number of map squares at the lowest res
#define MAP_LO_RES_SHIFT         (4)

// smallest size of a map square
#define MAP_SMALLEST_SQUARE_SIZE_SHIFT (8)
#define MAP_SMALLEST_SQUARE_SIZE       (1 << MAP_SMALLEST_SQUARE_SIZE_SHIFT)

// largest
#define MAP_BIGGEST_SQUARE_SIZE_SHIFT  (MAP_SMALLEST_SQUARE_SIZE_SHIFT + MAP_HI_RES_SHIFT - MAP_LO_RES_SHIFT)
#define MAP_BIGGEST_SQUARE_SIZE        (1 << MAP_BIGGEST_SQUARE_SIZE_SHIFT)

// The length of one edge of the map in actual game coordinates
#define MAP_SIZE			      (1 << (MAP_SMALLEST_SQUARE_SIZE_SHIFT + MAP_HI_RES_SHIFT))

// The map.
class Map
{
public:

	// A array of pointers to the different resolutions of map.
	GameObject **Who[MAP_HI_RES_SHIFT - MAP_LO_RES_SHIFT + 1];

	Map();
	~Map();

	// Fills in the who array and clears it
	bool Init();

	// deallocate
	void Reset();

	// fills in the object's map-related information given its position and radius
	void PlaceObject(GameObject &obj, const vector3 &pos, float radius);

	// given a map square at a certain resolution, retuirns the one at the next lower
	// resolution, or NULL if that was the lowest res
	GameObject **GetLowerMapSquare(GameObject **current, int res);
};

//////////////////////////////////////////////////////////////////////////////
//
// implementation

void GameObject::ProcessCollisions(Map *my_map)
{
	// We loop through several resolutions of map, starting with the current.
	GameObject *walker = NextInMap; // first start with the objects in my map square
	int current_res = MapRes;

	GameObject **map_who = MapSquare;

	do
	{
		// Do one resolution's worth of collision
		ProcessOneLevel(my_map, map_who, walker, current_res);

		// Move to the next lower resolution
		map_who = my_map->GetLowerMapSquare(map_who, current_res);
		current_res--;
	}
	while (map_who); // until we're at the lowest resolution.
}

void GameObject::ProcessOneLevel(Map *my_map, GameObject **map_sq, GameObject *walker, int current_res_shift)
{
	int current_res_size = 1 << current_res_shift;

	// Do all the objects in the first list presented
	for (; walker; walker = walker->NextInMap)
	{
		process_collision(this, walker);
	}

	// Work out if you can go to the adjacent map squares
	int current_offset = map_sq - my_map->Who[current_res_shift - MAP_LO_RES_SHIFT];

	// Then do map squares to the east, southeast and south
	if ((current_offset & (current_res_size - 1)) != current_res_size - 1)
	{
		// Square to the east
		for (walker = map_sq[1]; walker; walker = walker->NextInMap)
		{
			process_collision(this, walker);
		}
	}

	if (current_offset + current_res_size < (1 << (current_res_shift * 2)))
	{
		// Square to the south
		for (walker = map_sq[current_res_size]; walker; walker = walker->NextInMap)
		{
			process_collision(this, walker);
		}

		// and lastly, southeast.
		if ((current_offset & (current_res_size - 1)) != current_res_size - 1)
		{
			for (walker = map_sq[current_res_size + 1]; walker; walker = walker->NextInMap)
			{
				process_collision(this, walker);
			}
		}
	}
}

void GameObject::RemoveFromMap()
{
	// Search for myself.
	GameObject **pointer_to_me;
	for (pointer_to_me = MapSquare;
		*pointer_to_me != this;
		pointer_to_me = &(*pointer_to_me)->NextInMap)
	{
		assert(*pointer_to_me && "Game object couldn't find itself in map");
	}

	// Remove myself.
	*pointer_to_me = NextInMap;
	
	// And for safety's sake, let's clear my pointers.
	NextInMap = NULL;
	MapSquare = NULL;
}

Map::Map()
{
	for (int res = MAP_LO_RES_SHIFT; res <= MAP_HI_RES_SHIFT; res++)
	{
		Who[res - MAP_LO_RES_SHIFT] = NULL;
	}
}

Map::~Map()
{
	Reset();
}

void Map::Reset()
{
	// You'd better have cleared all the objects out by now. I won't check.
	for (int res = MAP_LO_RES_SHIFT; res <= MAP_HI_RES_SHIFT; res++)
	{
		delete[] Who[res - MAP_LO_RES_SHIFT];
		Who[res - MAP_LO_RES_SHIFT] = NULL;
	}
}

bool Map::Init()
{
	Reset(); // just in case

	// allocate and clear everything.
	for (int res = MAP_LO_RES_SHIFT; res <= MAP_HI_RES_SHIFT; res++)
	{
		Who[res - MAP_LO_RES_SHIFT] = new GameObject *[1 << (res * 2)];

		if (!Who[res - MAP_LO_RES_SHIFT]) return false; // alloc failed

		for (int sq = 0; sq < (1 << (res * 2)); sq++)
		{
			Who[res - MAP_LO_RES_SHIFT][sq] = NULL;
		}
	}

	return true;
}

// fills in the object's map-related information given its position and radius
void Map::PlaceObject(GameObject &obj, const vector3 &pos, float radius)
{
	// input value checking.
	assert(radius >= 0.f && radius < MAP_SIZE);

	// If you want to allow positions off the map, change these asserts into assignments
	assert(pos.x >= 0.f && pos.x < MAP_SIZE);
	assert(pos.y >= 0.f && pos.y < MAP_SIZE);

	// Conversion into integer coordinate system needed for shifting/array maths
	// later on. Note that these conversions are often slow and may have to be replaced
	// with faster versions in some compilers. If you do replace them, preserve their
	// rounding-down nature
	int iradius = int(radius);
	int ix      = int(pos.x );
	int iy      = int(pos.y );

	// Find which resolution level of the map the object should go in.
	obj.MapRes = MAP_HI_RES_SHIFT;
	for (int map_size = MAP_SMALLEST_SQUARE_SIZE; map_size <= MAP_BIGGEST_SQUARE_SIZE; map_size <<= 1)
	{
		// Does the object fit?
		if (iradius <= map_size) goto it_fits;

		// step on...
		obj.MapRes--;
	}

	assert(!"object too large for map - some collisions may not be detected");

it_fits:

	// Put it in the map.
	int which_level = obj.MapRes - MAP_LO_RES_SHIFT;
	GameObject **which_who = Who[which_level];

	// Then add on the position
	which_who += ix >> (MAP_BIGGEST_SQUARE_SIZE_SHIFT - which_level);
	which_who += (iy >> (MAP_BIGGEST_SQUARE_SIZE_SHIFT - which_level)) << obj.MapRes;

	// Insert the object into the map square
	obj.NextInMap = *which_who;
	*which_who = &obj;

	obj.MapSquare = which_who;
}

GameObject **Map::GetLowerMapSquare(GameObject **current, int who_res)
{
	// Top of map?
	if (who_res == MAP_LO_RES_SHIFT) return NULL;

	// Cunning bit-shifting.
	int current_offset = current - Who[who_res - MAP_LO_RES_SHIFT];

	// Extract the y part of the current offset.
	int y_mask = 0xffffffff << who_res;

	// The new offset is this:
	int new_offset = ((current_offset & ~y_mask      ) >> 1) +
					 ((current_offset & (y_mask << 1)) >> 2);

	return Who[who_res - MAP_LO_RES_SHIFT - 1] + new_offset;
}

