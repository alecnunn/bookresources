/*
  Name: NonThreadedDemo (Engine Version)
  Date: 04/2008
  Description: Demonstrates how to use the threaded engine
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 5000
#define SCALE 20

Texture *circle_image;
Font *font;
Console *console;
std::ostringstream ostr;


bool game_preload() 
{
	g_engine->setAppTitle("NON-THREADED DEMO");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(SCREENW);
	g_engine->setScreenHeight(SCREENH);
	g_engine->setColorDepth(32);	
	return 1;
}

void add_sprite()
{
	Sprite *sprite = new Sprite();
	sprite->setObjectType(OBJECT_SPRITE);
	sprite->setImage(circle_image);
	D3DCOLOR color = D3DCOLOR_RGBA(0,rand()%255,rand()%255,rand()%100);
	sprite->setColor(color);
	sprite->setSize(128,128);
	sprite->setScale( (float)(rand() % SCALE + SCALE/4) / 100.0f );
	sprite->setPosition( rand() % SCREENW, rand() % SCREENH );
	sprite->setCollidable(false);
	sprite->setLifetime( rand() % 30000 );

	//set velocity
	float vx = (float)(rand()%30 - 15)/10.0f;
	float vy = (float)(rand()%30 - 15)/10.0f;
	sprite->setVelocity( vx, vy );

	//add sprite to the entity manager
	g_engine->addEntity(sprite);
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
	for (n=0; n < MAX; n++) 
	{
		add_sprite();
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


	//maximize processor
	g_engine->setMaximizeProcessor(true);
	
    return true;
}

void updateConsole()
{
	static Timer timer;
	if (!timer.stopwatch(50)) return;

	console->print(g_engine->getVersionText(), 0);

	ostr.str(""); 
	ostr << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) 
		<< " ms (" << g_engine->getFrameRate_core() << " fps)";
	console->print(ostr.str(), 2);

	console->print("Press F2 to toggle Processor Throttling", 27);

	ostr.str("");
	ostr << "Entities: " << g_engine->getEntityCount();
	console->print(ostr.str(), 29);
}


void game_update() 
{
	//increase size of entity list
	add_sprite();

	//update console info
	updateConsole();
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

void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2) { }

