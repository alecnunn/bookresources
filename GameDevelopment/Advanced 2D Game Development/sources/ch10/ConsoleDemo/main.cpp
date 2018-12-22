/*
  Name: ConsoleDemo
  Date: 03/2008
  Description: Demonstrates the dropdown console
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 50

Texture *asteroid_image;
Font *font;
Console *console;

std::ostringstream ostr, ostrScreen, ostrCore;


bool game_preload() 
{
	g_engine->setAppTitle("CONSOLE DEMO");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(SCREENW);
	g_engine->setScreenHeight(SCREENH);
	g_engine->setColorDepth(32);	
	return 1;
}

bool game_init(HWND) 
{
	//load background image
	Sprite *background = new Sprite();
	background->loadImage("orion.bmp");
	background->setObjectType(999);
	g_engine->addEntity(background);

	//create the console
	console = new Console();
	if (!console->init()) {
		g_engine->message("Error initializing console");
		return false;
	}
	for (int n = 0; n < 55; n++) {
		ostr.str("");
		ostr << "CONSOLE LINE " << n;
		console->print(ostr.str());
	}


	//load asteroid image
	asteroid_image = new Texture();
	asteroid_image->Load("asteroid.tga");

	//create asteroid sprites
	Sprite *asteroid;
	for (int n=0; n < MAX; n++) 
	{
		//create a new asteroid sprite
		asteroid = new Sprite();
		asteroid->setObjectType(OBJECT_SPRITE);
		asteroid->setImage(asteroid_image);
		asteroid->setTotalFrames(64);
		asteroid->setColumns(8);
		asteroid->setSize(60,60);
		asteroid->setPosition( rand() % SCREENW, rand() % SCREENH );
		asteroid->setFrameTimer( rand() % 100 );
		asteroid->setCurrentFrame( rand() % 64 );
		if (rand()%2==0) asteroid->setAnimationDirection(-1);
		
		asteroid->setVelocity( (float)(rand()%10)/10.0f, (float)(rand()%10)/10.0f );

		//add asteroid to the entity manager
		g_engine->addEntity(asteroid);
	}


	//load the Verdana10 font
	font = new Font();
	if (!font->loadImage("verdana10.tga")) {
		g_engine->message("Error loading verdana10.tga");
		return false;
	}
	font->setColumns(16);
	font->setCharSize(20,16);

	if (!font->loadWidthData("verdana10.dat")) {
		g_engine->message("Error loading verdana10.dat");
		return false;
	}


    return true;
}


void game_update() 
{
	console->print(g_engine->getVersionText(), 0);

	ostrScreen.str(""); 
	ostrScreen << "SCREEN : " << (float)(1000.0f/g_engine->getFrameRate_real()) << " ms";
	console->print(ostrScreen.str(), 1);

	ostrCore.str("");
	ostrCore << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) << " ms";
	console->print(ostrCore.str(), 2);
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
    g_engine->SetIdentity();

}

void game_render2d()
{
	font->Print(1,1,"Press ~ or F12 to toggle the Console");

	//draw the console
	if (console->isShowing()) console->draw();

}

void game_entityUpdate(Advanced2D::Entity* entity) 
{ 
	switch(entity->getObjectType()) 
	{
		case OBJECT_SPRITE:
			Sprite* spr = (Sprite*)entity;
			if (spr->getX() < -60) spr->setX(SCREENW);
			if (spr->getX() > SCREENW) spr->setX(-60);
			if (spr->getY() < -60) spr->setY(SCREENH);
			if (spr->getY() > SCREENH) spr->setY(-60);
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

void game_end() 
{
	delete console;
	delete asteroid_image;
	delete font;
}

void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2) { }

