/*
  Name: TargetingDemo
  Date: 04/2008
  Description: Demonstrates angle to target function
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define BULLET_VEL 12.0
#define ASTEROID_VEL 3.0

#define OBJECT_BACKGROUND 1
#define OBJECT_SHIP 10
#define OBJECT_BULLET 20
#define OBJECT_ASTEROID 30
#define OBJECT_EXPLOSION 40

Font *font;
Console *console;
Texture *bullet_image;
Texture *asteroid_image;
Texture *explosion_image;
Vector3 ship_position;
Vector3 nearest_asteroid;
Vector3 target_lead;
double ship_angle = 90;
double nearest_distance;
Timer fireTimer;

bool game_preload() 
{
    g_engine->setAppTitle("TARGETING DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(SCREENW);
    g_engine->setScreenHeight(SCREENH);
    g_engine->setColorDepth(32);
    return true;
}

bool game_init(HWND) 
{
    //create the background
    Sprite *background = new Sprite();
    background->setObjectType(OBJECT_BACKGROUND);
    if (!background->loadImage("craters.tga")) {
        g_engine->message("Error loading craters.tga");
        return false;
    }
    g_engine->addEntity( background );

    //create the console
    console = new Console();
    if (!console->init()) {
        g_engine->message("Error initializing console");
        return false;
    }

    //create ship sprite
    Sprite *ship = new Sprite();
    ship->setObjectType(OBJECT_SHIP);
    if (!ship->loadImage("spaceship80.tga")) {
        g_engine->message("Error loading spaceship.tga");
        return false;
    }
    ship->setRotation( g_engine->math->toRadians(90) );
    ship->setPosition( 10, SCREENH/2-32 );
    g_engine->addEntity(ship);

    //load bullet image
    bullet_image = new Texture();
    if (!bullet_image->Load("plasma.tga")) {
        g_engine->message("Error loading plasma.tga");
        return false;
    }

    //load asteroid image
    asteroid_image = new Texture();
    if (!asteroid_image->Load("asteroid.tga")) {
        g_engine->message("Error loading asteroid.tga");
        return false;
    }

    //load the explosion image
    explosion_image = new Texture();
    if (!explosion_image->Load("explosion_30_128.tga")) {
        g_engine->message("Error loading explosion");
        return false;
    }

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

    //load sound effects
    if (!g_engine->audio->Load("fire.wav","fire")) {
        g_engine->message("Error loading fire.wav");
        return false;
    }

    if (!g_engine->audio->Load("boom.wav","boom")) {
        g_engine->message("Error loading boom.wav");
        return false;
    }

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

    ostr.str("");
    ostr << "Nearest asteroid: " << nearest_asteroid.getX() << "," << nearest_asteroid.getY();
    console->print(ostr.str(), y++);
    
    ostr.str("");
    ostr << "Nearest distance: " << nearest_distance;
    console->print(ostr.str(), y++);

    ostr.str("");
    ostr << "Leading target: " << target_lead.getX() << "," << target_lead.getY();
    console->print(ostr.str(), y++);

    ostr.str("");
    ostr << "Angle to target: " << ship_angle;
    console->print(ostr.str(), y++);
}

void addAsteroid()
{
    //add an asteroid
    Sprite *asteroid = new Sprite();
    asteroid->setObjectType(OBJECT_ASTEROID);
    asteroid->setVelocity(-ASTEROID_VEL, 0);
    asteroid->setPosition(SCREENW,50+rand()%(SCREENH-150));
    asteroid->setImage(asteroid_image);
    asteroid->setTotalFrames(64);
    asteroid->setColumns(8);
    asteroid->setSize(60,60);
    asteroid->setFrameTimer( rand() % 100 );
    asteroid->setCurrentFrame( rand() % 64 );
    if (rand()%2==0) asteroid->setAnimationDirection(-1);
    g_engine->addEntity( asteroid );
}

void firebullet()
{
    //get the ship from the entity manager
    Sprite *ship = (Sprite*)g_engine->findEntity(OBJECT_SHIP);
    if (!ship)
    {
        g_engine->message("Error locating ship in entity manager!","ERROR");
        g_engine->Close();
    }

    //create bullet sprite
    Sprite *bullet = new Sprite();
    bullet->setObjectType(OBJECT_BULLET);
    bullet->setImage(bullet_image);
    bullet->setMoveTimer(1);
    bullet->setLifetime(5000);

    //set bullet equal to ship's rotation angle
    double angle = g_engine->math->toRadians(ship_angle);
    bullet->setRotation( angle );

    //set bullet's starting position
    double x = ship->getX() + ship->getWidth()/2;
    double y = ship->getY() + ship->getHeight()/2-8;
    bullet->setPosition(x,y);

    //set bullet's velocity
    double vx = g_engine->math->LinearVelocityX(ship_angle) * BULLET_VEL;
    double vy = g_engine->math->LinearVelocityY(ship_angle) * BULLET_VEL;
    bullet->setVelocity(vx, vy);

    //fire bullet    
    g_engine->addEntity(bullet);
    g_engine->audio->Play("fire");
}


void targetNearestAsteroid(Sprite *asteroid)
{
    //get asteroid's position
    Vector3 target = asteroid->getPosition();

    //calculate distance to target
    double dist = ship_position.Distance( target );
    if (dist < nearest_distance) {
        nearest_asteroid = target;
        nearest_distance = dist;

        //lead the target for better accuracy
        target_lead.setX(asteroid->getVelocity().getX() * 0.01f);
        target_lead.setY(asteroid->getVelocity().getY() * 0.01f);
        nearest_asteroid.setX(nearest_asteroid.getX() + target_lead.getX());
        nearest_asteroid.setY(nearest_asteroid.getY() + target_lead.getY());
    
        //calculate angle to target
        ship_angle = g_engine->math->AngleToTarget(ship_position,nearest_asteroid);
        ship_angle = 90 + g_engine->math->toDegrees( ship_angle );
    
    }

    //is there a target to shoot at?
    if (nearest_distance < 1200) {
        if (fireTimer.stopwatch(100)) {
            firebullet();
        }
    }

}

void game_update() 
{
    updateConsole();
}

void game_render2d()
{
    font->Print(1,SCREENH-20,"Press ~ or F12 to toggle the Console");
    font->Print(1,SCREENH-40,"Press SPACE to launch an asteroid!!!");

    //draw console
    if (console->isShowing()) console->draw();

    nearest_distance = 999999;
}

void game_end() 
{
    delete console;
    delete font;
    delete bullet_image;
    delete asteroid_image;
    delete explosion_image;
}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    Sprite *ship, *bullet, *asteroid;
    Vector3 position;

    switch(entity->getObjectType()) 
    {
        case OBJECT_SHIP:
            ship = (Sprite*)entity;
            ship_position = ship->getPosition();
            ship->setRotation( g_engine->math->toRadians(ship_angle) );
            break;

        case OBJECT_BULLET:
            bullet = (Sprite*)entity;
            if (bullet->getX() > SCREENW)
                bullet->setAlive(false);
            break;
        
        case OBJECT_ASTEROID:
            asteroid = (Sprite*)entity;
            if (asteroid->getX() < -64) asteroid->setX(SCREENW);
            targetNearestAsteroid( asteroid );
            break;
    }
}

void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2)
{
    if (entity1->getObjectType() == OBJECT_ASTEROID)
    {
        Sprite *asteroid = (Sprite*)entity1;

        if (entity2->getObjectType() == OBJECT_BULLET)
        {
            //create an explosion
            Sprite *expl = new Sprite();
            expl->setObjectType(OBJECT_EXPLOSION);
            expl->setImage(explosion_image);
            expl->setColumns(6);
            expl->setCollidable(false);
            expl->setSize(128,128);
            double x = asteroid->getPosition().getX();
            double y = asteroid->getPosition().getY();
            expl->setPosition(x-32,y-32);
            expl->setTotalFrames(30);
            expl->setFrameTimer(40);
            expl->setLifetime(1000);
            g_engine->addEntity( expl );

            //remove the asteroid
            entity2->setAlive(false);

            //remove the bullet
            entity1->setAlive(false);

            //play explosion sound
            g_engine->audio->Play("boom");
        }
    }
}



void game_keyPress(int key) 
{ 
    switch (key) {
        case DIK_SPACE:
            addAsteroid();
            break;
    }
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

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }

