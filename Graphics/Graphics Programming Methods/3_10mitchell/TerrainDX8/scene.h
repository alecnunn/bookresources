// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef SCENE_H
#define SCENE_H

#ifndef DEBUG_H
#include "debug.h"
#endif

#ifndef DISPLACEMENT_PATCH_H
#include "displacement_patch.h"
#endif

class Scene_Class
{
public:
	Scene_Class();
	~Scene_Class();

	void Render();

private:

//	Height_Patch*	Height_Terrain;
	Displacement_Patch* Displacement_Terrain;
};

#endif
