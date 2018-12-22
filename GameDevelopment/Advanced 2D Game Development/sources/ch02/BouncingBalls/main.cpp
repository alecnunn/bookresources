/*
  Name: BouncingBalls Program
  Date: 25/02/08
  Description: Demonstrates loading and rendering a mesh file
*/
#include <vector>
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

//macro to read the keyboard asynchronously
#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)

//camera object
Camera *camera;

#define SPHERES 100

typedef std::vector<Mesh*>::iterator iter;
std::vector<Mesh*> entities;


bool game_preload() 
{
	g_engine->setAppTitle("BOUNCING BALLS");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(1024);
	g_engine->setScreenHeight(768);
	g_engine->setColorDepth(32);	
	return 1;
}

bool game_init(HWND) 
{
	srand(time(NULL));

    //set the camera and perspective
	camera = new Camera();
	camera->setPosition(0.0f, 2.0f, 10.0f);
	camera->setTarget(0.0f, 0.0f, 0.0f);
	camera->Update();

    //load ball meshes
	Mesh *ball;
	for (int n=0; n<SPHERES; n++) 
	{
		ball = new Mesh();
	    ball->Load("ball.x");
		ball->SetScale(0.3f,0.3f,0.3f);
		ball->SetPosition(0.0f,0.0f,0.0f);
		float x = (float)(rand()%8+1) / 100.0f;
		float y = (float)(rand()%8+1) / 100.0f;
		float z = (float)(rand()%8+1) / 100.0f;
		ball->SetVelocity(x,y,z);
		entities.push_back(ball);
	}

    return 1;
}

void game_update() 
{
	//update entity positions
	for (iter i = entities.begin(); i != entities.end(); ++i)
	{
		(*i)->Update();
		(*i)->LimitBoundary(-5,5,4,-4,4,-4);
	}

	if (KEY_DOWN(VK_ESCAPE))
		g_engine->Close();

}

void game_end() 
{
	delete camera;

	for (iter i = entities.begin(); i != entities.end(); ++i)
	{
		delete *i;
	}
	entities.clear();
}

void game_render3d()
{
    static DWORD start=0;

	//clear the scene using a dark blue color
    g_engine->ClearScene(D3DCOLOR_RGBA(30,30,100,0));

	//return to the origin
    g_engine->SetIdentity();

	//draw entities
	for (iter i = entities.begin(); i != entities.end(); ++i)
	{
		(*i)->Transform();
		(*i)->Draw();
	}

}


