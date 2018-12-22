/*
  Name: SpriteEntityDemo
  Date: 03/2008
  Description: Demonstrates entity management with sprites
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define OBJECT_SPRITE 100
#define MAX 10000

Texture *image;

bool game_preload() 
{
    g_engine->setAppTitle("SPRITE ENTITY DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(1024);
    g_engine->setScreenHeight(768);
    g_engine->setColorDepth(32);
    return true;
}

bool game_init(HWND) 
{
    Sprite *asteroid;

    image = new Texture();
    image->Load("asteroid.tga");

    for (int n=0; n < MAX; n++) 
    {
        //create a new asteroid sprite
        asteroid = new Sprite();
        asteroid->setObjectType(OBJECT_SPRITE);
        asteroid->setImage(image);
        asteroid->setTotalFrames(64);
        asteroid->setColumns(8);
        asteroid->setSize(60,60);
        asteroid->setPosition( rand() % 950, rand() % 700 );
        asteroid->setFrameTimer( rand() % 100 );
        asteroid->setCurrentFrame( rand() % 64 );
        if (rand()%2==0) asteroid->setAnimationDirection(-1);

        //add sprite to the entity manager
        g_engine->addEntity(asteroid);
    }

    std::ostringstream s;
    s << "Entities: " << g_engine->getEntityCount();
    g_engine->message(s.str());

    return true;
}


void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_keyRelease(int key) 
{ 
    //exit when escape key is pressed
    if (key == DIK_ESCAPE) g_engine->Close();
}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    //type-cast Entity to a Sprite
    Sprite* sprite = (Sprite*)entity;

    //this is where you can update sprite properties
}

void game_entityRender(Advanced2D::Entity* entity) 
{ 
    //engine automatically renders each entity
    //but we can respond to each render event here
}

void game_end() 
{ 
    delete image;
}

void game_update() { }
void game_render2d() { }
void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
