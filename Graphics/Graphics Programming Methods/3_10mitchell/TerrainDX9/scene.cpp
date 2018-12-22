// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "terrain.h"
#include "scene.h"

Scene_Class::Scene_Class()
{
	Displacement_Terrain=new Displacement_Patch;
}

Scene_Class::~Scene_Class()
{
	Safe_Delete(Displacement_Terrain);
}

void Scene_Class::Render()
{
	// wrap camera world position
	D3DXVECTOR3* eye=Application::Get_Camera()->Get_Eye();

	if (eye->x>PATCH_WORLD_WIDTH*4.5f) eye->x-=PATCH_WORLD_WIDTH;
	else if (eye->x<PATCH_WORLD_WIDTH*3.5f) eye->x+=PATCH_WORLD_WIDTH;

	if (eye->z>PATCH_WORLD_WIDTH*4.5f) eye->z-=PATCH_WORLD_WIDTH;
	else if (eye->z<PATCH_WORLD_WIDTH*3.5f) eye->z+=PATCH_WORLD_WIDTH;

	//Displacement_Terrain->Render();
	
	Displacement_Terrain->Render_Tiled();
}


