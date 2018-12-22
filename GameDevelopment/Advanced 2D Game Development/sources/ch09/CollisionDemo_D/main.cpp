/*
  Name: CollisionDemo_D (distance version)
  Date: 03/2008
  Description: Demonstrates sprite collision using distance function
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#define SCREENW 1024
#define SCREENH 768
#define OBJECT_BACKGROUND 1
#define OBJECT_SPRITE 100
#define MAX 50
#define SCALE 70

Texture *ball_image;
Font *font;
Console *console;
std::ostringstream ostr;
int collisions;


bool game_preload() 
{
	g_engine->setAppTitle("COLLISION DEMO (DISTANCE)");
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
	if (!background->loadImage("craters.tga")) {
		g_engine->message("Error loading craters.tga");
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
	ball_image = new Texture();
	if (!ball_image->Load("lightningball.tga")) {
		g_engine->message("Error loading lightningball.tga");
		return false;
	}

	//create sprites
	Sprite *sprite;
	for (int n=0; n < MAX; n++) 
	{
		//create a new sprite
		sprite = new Sprite();
		sprite->setObjectType(OBJECT_SPRITE);
		sprite->setImage(ball_image);
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
	ostr.str("");
	ostr << "CORE : " << (float)(1000.0f/g_engine->getFrameRate_core()) 
		<< " ms (" << g_engine->getFrameRate_core() << " fps)";
	console->print(ostr.str(), y++);
	ostr.str("");
	ostr << "Entities: " << g_engine->getEntityCount();
	console->print(ostr.str(), y++);
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
	delete ball_image;
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

