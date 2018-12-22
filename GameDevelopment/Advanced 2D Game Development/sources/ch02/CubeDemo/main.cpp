/*
  Name: Cube Demo Program
  Date: 25/02/08
  Description: First attempt at 3D rendering using Advanced2D engine
*/

#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

//create some engine-class objects
Camera *camera;
Mesh *mesh;


bool game_preload() 
{
	g_engine->setAppTitle("CUBE DEMO");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(1024);
	g_engine->setScreenHeight(768);
	g_engine->setColorDepth(32);	
	return 1;
}

bool game_init(HWND) 
{
    //create a cube 
    mesh = new Mesh();
    mesh->CreateCube(2.0f, 2.0f, 2.0f);

    //set the camera and perspective
	camera = new Camera();
	camera->setPosition(0.0f, 2.0f, 6.0f);
	camera->setTarget(0.0f, 0.0f, 0.0f);
	camera->Update();
	
    //set the ambient color
    g_engine->SetAmbient(D3DCOLOR_XRGB(40,40,255));

    return 1;
}

void game_update() 
{
}

void game_end() 
{
	delete camera;
	delete mesh;
}

void game_render3d()
{
	//clear the scene using a dark blue color
    g_engine->ClearScene(D3DCOLOR_RGBA(30,30,100, 0));

	//return to the origin
    g_engine->SetIdentity();
    
    //rotate and draw the cube
    mesh->Rotate(2.0f, 0.0f, 0.0f);
    mesh->Transform();
    mesh->Draw();
}

void game_render2d()
{
}

