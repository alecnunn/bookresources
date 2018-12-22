/*
  Name: VelocityDemo
  Date: 04/2008
  Description: Demonstrates angular velocity
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define VELOCITY 0.0001
#define ROCKETVEL 3.0

#define OBJECT_SHIP 100
#define OBJECT_ROCKET 200

Font *font;
Console *console;
Sprite *ship;
Vector3 velocity;
Texture *rocket_image;

bool game_preload() 
{
    g_engine->setAppTitle("VELOCITY DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(SCREENW);
    g_engine->setScreenHeight(SCREENH);
    g_engine->setColorDepth(32);    
    return 1;
}

bool game_init(HWND) 
{
    //create the console
    console = new Console();
    if (!console->init()) {
        g_engine->message("Error initializing console");
        return false;
    }

    //create ship sprite
    ship = new Sprite();
    ship->setObjectType(OBJECT_SHIP);
    ship->loadImage("fatship256.tga");
    ship->setRotation( g_engine->math->toRadians(90) );
    ship->setPosition( 10, SCREENH/2-ship->getHeight()/2 );
    g_engine->addEntity(ship);

    //load rocket image
    rocket_image = new Texture();
    rocket_image->Load("fatrocket64.tga");


    //load the Verdana10 font
    font = new Font();
    if (!font->loadImage("verdana10.tga")) {
        g_engine->message("Error loading verdana10.tga");
        return false;
    }
    if (!font->loadWidthData("verdana10.dat")) {
        g_engine->message("Error loading verdana10.dat");
        return false;
    }
    font->setColumns(16);
    font->setCharSize(20,16);

    //maximize processor
    g_engine->setMaximizeProcessor(true);

    return true;
}

void updateConsole()
{
    std::ostringstream ostr;
    int y = 0;
    console->print(g_engine->getVersionText(), y++);
    ostr.str("");
    ostr << "REFRESH : " << (double)(1000.0f/g_engine->getFrameRate_core()) 
        << " ms (" << g_engine->getFrameRate_core() << " fps)";
    console->print(ostr.str(), y++);
    ostr.str("");
    ostr << "Entities: " << g_engine->getEntityCount();
    console->print(ostr.str(), y++);
}


void game_update() 
{
    updateConsole();
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
    g_engine->SetIdentity();
}

void game_render2d()
{
    font->Print(1,SCREENH-20,"Press ~ or F12 to toggle the Console");
    font->Print(1,SCREENH-40,"Press SPACE to fire!!!");

    //draw console
    if (console->isShowing()) console->draw();
}

void game_keyRelease(int key) 
{ 
    switch (key) {
        case DIK_ESCAPE:
            g_engine->Close();
            break;

        case DIK_F12:
        case DIK_GRAVE:
            console->setShowing( !console->isShowing() );
            break;

    }
}

void game_end() 
{
    delete console;
    delete font;
    delete ship;
}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    double y;
    Sprite *ship, *rocket;
    Vector3 position;

    switch(entity->getObjectType()) 
    {
        case OBJECT_SHIP:
            ship = (Sprite*)entity;
            position = ship->getPosition();
            y = position.getY() + velocity.getY();
            if (y < 0) {
                y = 0;
                velocity.setY(0);
            }
            if (y > SCREENH-128) {
                y = SCREENH-128;
                velocity.setY(0);
            }
            position.setY(y);
            ship->setPosition( position );
            break;

        case OBJECT_ROCKET:
            rocket = (Sprite*)entity;
            if (rocket->getX() > SCREENW)
                rocket->setAlive(false);
            
            break;
    }
}

void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2)
{
}

void firerocket()
{
    Sprite *ship = (Sprite*)g_engine->findEntity(OBJECT_SHIP);
    if (!ship)
    {
        g_engine->message("Error locating ship in entity manager!","ERROR");
        g_engine->Close();
    }

    Sprite *rocket = new Sprite();
    rocket->setObjectType(OBJECT_ROCKET);
    rocket->setImage(rocket_image);
    rocket->setMoveTimer(1);
    rocket->setCollidable(false);

    double randrot = rand() % 40 - 20;
    double angle = 90 + randrot;
    rocket->setRotation( g_engine->math->toRadians(angle) );

    double x = ship->getX() + ship->getWidth();
    double y = ship->getY()+ship->getHeight()/2-rocket->getHeight()/2;
    rocket->setPosition(x,y);

    double vx = g_engine->math->LinearVelocityX(angle) * ROCKETVEL;
    double vy = g_engine->math->LinearVelocityY(angle) * ROCKETVEL;
    rocket->setVelocity(vx, vy);

    g_engine->addEntity(rocket);
    
}


void game_keyPress(int key) 
{ 
    double y;

    switch(key) 
    {
        case DIK_UP:
        case DIK_W:
            y = velocity.getY() - VELOCITY;
            if (y < -3.0) y = -3.0;
            velocity.setY(y);
            break;

        case DIK_DOWN:
        case DIK_S:
            y = velocity.getY() + VELOCITY;
            if (y > 3.0) y = 3.0;
            velocity.setY(y);
            break;

        case DIK_SPACE:
        case DIK_LCONTROL:
            firerocket();
            break;
    }
}

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }

