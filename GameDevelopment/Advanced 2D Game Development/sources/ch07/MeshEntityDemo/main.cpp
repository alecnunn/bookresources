/*
  Name: MeshEntityDemo
  Date: 25/02/08
  Description: Demonstrates managed mesh entities
*/

#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

//camera object
Camera *camera;
Light *light;

#define MAX 10

bool game_preload() 
{
    g_engine->setAppTitle("MESH ENTITY DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(1024);
    g_engine->setScreenHeight(768);
    g_engine->setColorDepth(32);    
    return 1;
}

bool game_init(HWND) 
{
    //set the camera and perspective
    camera = new Camera();
    camera->setPosition(0.0f, 2.0f, 50.0f);
    camera->setTarget(0.0f, 0.0f, 0.0f);
    camera->Update();

    //create a directional light
    D3DXVECTOR3 pos(0.0f,0.0f,0.0f);
    D3DXVECTOR3 dir(1.0f,0.0f,0.0f);
    light = new Light(0, D3DLIGHT_DIRECTIONAL, pos, dir, 100);
    light->setColor(D3DXCOLOR(1,0,0,0));

    g_engine->SetAmbient(D3DCOLOR_RGBA(0,0,0,0));

    //load meshes
    Mesh *mesh;
    for (int n=0; n<MAX; n++) 
    {
        mesh = new Mesh();
        mesh->Load("cytovirus.x");
        mesh->SetScale(0.02f,0.02f,0.02f);

        float x = rand() % 40 - 20;
        float y = rand() % 40 - 20;
        float z = rand() % 10 - 5;
        mesh->SetPosition(x,y,z);

        //add mesh to entity manager
        g_engine->addEntity(mesh);
    }

    return 1;
}

void game_update() 
{
    //nothing to do!
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_RGBA(0,0,60,0));
    g_engine->SetIdentity();
}

void game_keyRelease(int key) 
{ 
    if (key == DIK_ESCAPE) g_engine->Close();
}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    if (entity->getRenderType() == RENDER3D)
    {
        //type-cast Entity to a Mesh
        Mesh* mesh = (Mesh*)entity;

        //perform a simple rotation
        mesh->Rotate(0,0.2f,0);
    }
}

void game_entityRender(Advanced2D::Entity* entity) 
{ 
    //type-cast Entity to a Mesh
    Mesh* mesh = (Mesh*)entity;

    //engine automatically renders each entity
    //but we can respond to each render event here
}

void game_end() 
{
    delete camera;
    delete light;
}

void game_render2d() { }
void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }

