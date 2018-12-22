/*
  Name: ThreadDemo Program
  Date: 042008
  Description: Demonstrates how to use the pthreads-win32 library
*/
#include <pthread.h>
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;


#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 50
#define SCALE 70

Texture *circle_image;
Font *font;
Console *console;
std::ostringstream ostr;
int collisions;

//THREAD STUFF
#define MAXTHREADS 2
bool done = false;
unsigned long thread_counter = 0;
void* thread_function(void* data);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_waits = 0;


void* thread_function(void* data)
{
    while(!done)
    {
        //lock the mutex 
        if (pthread_mutex_lock(&mutex) != 0) thread_waits++;

        //increment thread counter
        thread_counter++;

        //do something more intensive
        Vector3 vector1(100,200,300);
        Vector3 vector2(400,500,600);
        double dist = g_engine->math->Distance(vector1,vector2);
        double dist_squared = dist*dist;
        double square_root_of_dist = sqrt(dist);
        double answer = square_root_of_dist;
        
        //unlock the mutex
        if (pthread_mutex_unlock(&mutex) != 0) thread_waits++;
    }
    pthread_exit(NULL);
    return NULL;
}

bool game_preload() 
{
    g_engine->setAppTitle("THREAD DEMO");
    g_engine->setFullscreen(false);
    g_engine->setScreenWidth(SCREENW);
    g_engine->setScreenHeight(SCREENH);
    g_engine->setColorDepth(32);    
    return 1;
}

bool game_init(HWND) 
{
    int n;

    //load background image
    Sprite *background = new Sprite();
    if (!background->loadImage("galaxies.tga")) {
        g_engine->message("Error loading galaxies.tga");
        return false;
    }
    background->setObjectType(OBJECT_BACKGROUND);
    background->setCollidable(false);
    g_engine->addEntity(background);

    //create the console
    console = new Console();
    if (!console->init()) {
        g_engine->message("Error initializing console");
        return false;
    }
    console->setShowing(true);


    //load sprite image
    circle_image = new Texture();
    if (!circle_image->Load("circle.tga")) {
        g_engine->message("Error loading circle.tga");
        return false;
    }

    //create sprites
    Sprite *sprite;
    for (n=0; n < MAX; n++) 
    {
        //create a new sprite
        sprite = new Sprite();
        sprite->setObjectType(OBJECT_SPRITE);
        sprite->setImage(circle_image);
        sprite->setColor(D3DCOLOR_RGBA(255,255,255,50));
        sprite->setSize(128,128);
        sprite->setScale( (float)(rand() % SCALE + SCALE/4) / 100.0f );
        sprite->setPosition( rand() % SCREENW, rand() % SCREENH );
        sprite->setCollisionMethod(COLLISION_DIST);

        //set velocity
        float vx = (float)(rand()%30 - 15)/10.0f;
        float vy = (float)(rand()%30 - 15)/10.0f;
        sprite->setVelocity( vx, vy );

        //add sprite to the entity manager
        g_engine->addEntity(sprite);
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


    //create the thread(s)
    int id;
    for (n = 0; n < MAXTHREADS; n++) {
        pthread_t mythread;
        int mythread_id = n;
        id = pthread_create(&mythread, NULL, thread_function, (void*)&mythread_id);
    }
    
    return true;
}

void updateConsole()
{
    int y = 0;
    console->print(g_engine->getVersionText(), 1);

    ostr.str(""); 
    ostr << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) 
        << " ms (" << g_engine->getFrameRate_core() << " fps)";
    console->print(ostr.str(), 3);
    ostr.str("");
    ostr << "THREAD_COUNTER: " << thread_counter;
    console->print(ostr.str(), 5);
    ostr.str("");
    ostr << "THREAD_WAITS: " << thread_waits;
    console->print(ostr.str(), 7);

    console->print("Press F2 to toggle Processor Throttling", 27);

    ostr.str("");
    ostr << "Entities: " << g_engine->getEntityCount();
    console->print(ostr.str(), 29);
}


void game_update() 
{
    //any code that touches thread variables must be wrapped in a mutex
    pthread_mutex_lock(&mutex);
    updateConsole();
    collisions = 0;
    pthread_mutex_unlock(&mutex);
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_render2d()
{
    font->Print(1,SCREENH-20,"Press ~ or F12 to toggle the Console");

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

        case DIK_F2:
            g_engine->setMaximizeProcessor( !g_engine->getMaximizeProcessor() );
            break;
    }
}

void game_end() 
{
    //kill the persistent thread
    done = true;
    pthread_mutex_destroy(&mutex);

    //delete objects
    delete console;
    delete circle_image;
    delete font;
}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
    switch(entity->getObjectType()) 
    {
        case OBJECT_SPRITE:
            Sprite* spr = (Sprite*)entity;
            double w = (double)spr->getWidth() * spr->getScale();
            double h = (double)spr->getHeight() * spr->getScale();
            double vx = spr->getVelocity().getX();
            double vy = spr->getVelocity().getY();

            if (spr->getX() < 0) {
                spr->setX(0);
                vx = fabs(vx);
            }
            else if (spr->getX() > SCREENW-w) {
                spr->setX(SCREENW-w);
                vx = fabs(vx) * -1;
            }
            if (spr->getY() < 0) {
                spr->setY(0);
                vy = fabs(vy);
            }
            else if (spr->getY() > SCREENH-h) {
                spr->setY(SCREENH-h);
                vy = fabs(vy) * -1;
            }

            spr->setVelocity(vx,vy);
            break;
    }
}

void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2)
{
    Sprite *a = (Sprite*)entity1;
    Sprite *b = (Sprite*)entity2;

    if (a->getObjectType() == OBJECT_SPRITE && b->getObjectType() == OBJECT_SPRITE)
    {
        collisions++;

        double x1 = a->getX();
        double y1 = a->getY();
        double x2 = b->getX();
        double y2 = b->getY();

        double vx1 = a->getVelocity().getX();
        double vy1 = a->getVelocity().getY();
        double vx2 = b->getVelocity().getX();
        double vy2 = b->getVelocity().getY();

        if (x1 < x2) {
            vx1 = fabs(vx1) * -1;
            vx2 = fabs(vx1);
        }
        else if (x1 > x2) {
            vx1 = fabs(vx1);
            vx2 = fabs(vx2) * -1;
        }
        if (y1 < y2) {
            vy1 = fabs(vy1) * -1;
            vy2 = fabs(vy2);
        } 
        else {
            vy1 = fabs(vy1);
            vy2 = fabs(vy2) * -1;
        }

        a->setVelocity(vx1,vy1);
        b->setVelocity(vx2,vy2);
    }
}


void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }

