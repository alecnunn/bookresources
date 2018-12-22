/*
  Name: CollisionDemo
  Date: 03/2008
  Description: Demonstrates sprite collision
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 40

Texture *asteroid_image;
Font *font;
Console *console;
std::ostringstream ostr;
Texture *collisionBox;
int collisions;


bool game_preload() 
{
	g_engine->setAppTitle("COLLISION DEMO");
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
	if (!background->loadImage("orion.bmp")) {
		g_engine->message("Error loading orion.bmp");
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


	//load asteroid image
	asteroid_image = new Texture();
	if (!asteroid_image->Load("asteroid.tga")) {
		g_engine->message("Error loading asteroid.tga");
		return false;
	}

	//create asteroid sprites
	Sprite *asteroid;
	for (int n=0; n < MAX; n++) 
	{
		//create a new asteroid sprite
		asteroid = new Sprite();
		asteroid->setObjectType(OBJECT_SPRITE);
		ostr.str(""); ostr << "ASTEROID" << n;
		asteroid->setName(ostr.str());
		asteroid->setImage(asteroid_image);
		asteroid->setScale( (float)(rand() % 150 + 50) / 100.0f );

		//set animation properties
		asteroid->setTotalFrames(64);
		asteroid->setColumns(8);
		asteroid->setSize(60,60);
		asteroid->setPosition( rand() % SCREENW, rand() % SCREENH );
		asteroid->setFrameTimer( rand() % 90 + 10 );
		asteroid->setCurrentFrame( rand() % 64 );
		if (rand()%2==0) asteroid->setAnimationDirection(-1);

		//set movement properties
		float vx = (float)(rand()%10 - 5)/10.0f;
		float vy = (float)(rand()%10 - 5)/10.0f;
		asteroid->setVelocity( vx, vy );

		//collision toggle
		asteroid->setCollidable(true);

		//movement timer keeps sprite consistent at any framerate
		asteroid->setMoveTimer( 16 );

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

	//load highlight image used to show collisions
	collisionBox = new Texture();
	if (!collisionBox->Load("highlight.tga")) {
		g_engine->message("Error loading highlight.tga");
		return false;
	}

    return true;
}

void updateConsole()
{
	int y = 0;
	console->print(g_engine->getVersionText(), y++);
	y++;

	ostr.str(""); 
	ostr << "SCREEN : " << (float)(1000.0f/g_engine->getFrameRate_real()) 
		<< " ms (" << g_engine->getFrameRate_real() << " fps)";
	console->print(ostr.str(), y++);
	y++;

	ostr.str("");
	ostr << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) 
		<< " ms (" << g_engine->getFrameRate_core() << " fps)";
	console->print(ostr.str(), y++);

	ostr.str("");
	ostr << "Processor throttling: " << g_engine->getMaximizeProcessor();
	console->print(ostr.str(), y++);
	y++;

	ostr.str("");
	ostr << "Screen: " << g_engine->getScreenWidth() << "," 
		<< g_engine->getScreenHeight() << "," << g_engine->getColorDepth();
	console->print(ostr.str(), y++);
	y++;

	ostr.str("");
	ostr << "Entities: " << g_engine->getEntityCount();
	console->print(ostr.str(), y++);

	ostr.str("");
	ostr << "Collisions: " << collisions;
	console->print(ostr.str(), y++);
	y++;


	ostr.str("");
	ostr << "Press F2 to toggle Processor Throttling";
	console->print(ostr.str(), 27);

}


void game_update() 
{
	updateConsole();

	collisions = 0;
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
    g_engine->SetIdentity();
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
	delete console;
	delete asteroid_image;
	delete font;
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

void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2)
{
	Sprite *box;
	Sprite *a = (Sprite*)entity1;
	Sprite *b = (Sprite*)entity2;

	if (a->getObjectType() == OBJECT_SPRITE && b->getObjectType() == OBJECT_SPRITE)
	{
		collisions++;

		//add first collision box
		box = new Sprite();
		box->setColor(0x33DD4444);
		box->setImage(collisionBox);
		box->setPosition( a->getPosition() );
		box->setScale( a->getWidth() * a->getScale() / 100.0f );
		box->setLifetime(100);
		g_engine->addEntity( box );

		//add second collision box
		box = new Sprite();
		box->setColor(0x33DD4444);
		box->setImage(collisionBox);
		box->setPosition( b->getPosition() );
		box->setScale( b->getWidth() * b->getScale() / 100.0f );
		box->setLifetime(100);
		g_engine->addEntity( box );
	}
}


void game_keyPress(int key) { }
void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }

