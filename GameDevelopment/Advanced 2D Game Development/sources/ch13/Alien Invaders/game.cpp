/**
  * ALIEN INVADERS
  */


#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;
using namespace std;

#define BLACK D3DCOLOR_XRGB(0,0,0)
#define RED D3DCOLOR_XRGB(255,0,0)

#define ENTITY_PLAYER 10
#define ENTITY_BLOCK 20
#define ENTITY_INVADER 30
#define ENTITY_EXPLOSION 40
#define ENTITY_PLAYER_BULLET 100
#define ENTITY_ALIEN_BULLET 200

const int COLUMNS = 9;
const int ROWS = 6;
Sprite *invaders[ROWS][COLUMNS];
Texture *invaderImages[8];

Mesh *earth;
Camera *camera;
Console *console;
Texture *fontImg;
Font *font24;
Font *font10;
Texture *bulletUp;
Texture *bulletDown;
Texture *block_image;
Texture *explosion_image;
Timer stepTime;
Timer shotTime;
bool readyToFire = false;

double alienDir = 12.0;
double alienX = 20.0;
double alienY = 40.0;
int stepSound = 1;

int score = 0;
int lives = 5;
int state = 0;
double angle = 90.0;
int lightDir = -1;
double shipvel = 0.0;
double playerx, playery;
bool alienFire;
double alienFireX, alienFireY;


bool game_preload()
{
	g_engine->setAppTitle("ALIEN INVADERS");
	g_engine->setFullscreen(false);
	g_engine->setScreenWidth(1024);
	g_engine->setScreenHeight(768);
	g_engine->setColorDepth(32);
	return 1;
}


void resetGame()
{
	char s[255];
	
	//reset variables
	lives = 5;
	score = 0;

	//clear out all existing blocks
	Entity *entity;
	while ( (entity = g_engine->findEntity(ENTITY_BLOCK)) != NULL)
		entity->setAlive(false);
	
	//add the blocks
	Sprite *block;
	for (int n=0; n<5; n++)
	{
		block = new Sprite();
		block->setObjectType(ENTITY_BLOCK);
		block->setImage(block_image);
		block->setColumns(5);
		block->setTotalFrames(4);
		block->setAnimationDirection(0);
		block->setSize(134,68);
		block->setPosition(30.0f + n * 210, 610.0f);
		g_engine->addEntity(block);
	}

	//create the invaders
	Sprite *invader;
	for (int y=0; y<ROWS; y++) {
		for (int x=0; x<COLUMNS; x++) {
			if (invaders[y][x]) delete invaders[y][x];
			invaders[y][x] = new Sprite();
			invaders[y][x]->setImage(invaderImages[y]);
			invaders[y][x]->setSize(68,48);
			invaders[y][x]->setColumns(2);
			invaders[y][x]->setTotalFrames(2);
			invaders[y][x]->setFrameTimer(250);
		}
	}

	//clear out all existing bullets
	while ( (entity = g_engine->findEntity(ENTITY_PLAYER_BULLET)) != NULL)
		entity->setAlive(false);
	while ( (entity = g_engine->findEntity(ENTITY_ALIEN_BULLET)) != NULL)
		entity->setAlive(false);
	
}


bool game_init(HWND hwnd)
{
	//create the console
	console = new Console();
	if (!console->init()) {
		g_engine->message("Error creating console");
		return false;
	}
	console->hide();

	//load the Verdana10 font
	font10 = new Font();
	if (!font10->loadImage("verdana10.tga")) {
		g_engine->message("Error loading verdana10.tga");
		return false;
	}
	if (!font10->loadWidthData("verdana10.dat")) {
		g_engine->message("Error loading verdana10.dat");
		return false;
	}
	font10->setColumns(16);
	font10->setCharSize(20,16);

	//load the TimesNewRoman24 font
	font24 = new Font();
	if (!font24->loadImage("timesnewroman24.tga")) {
		g_engine->message("Error loading timesnewroman24.tga");
		return false;
	}
	if (!font24->loadWidthData("timesnewroman24.dat")) {
		g_engine->message("Error loading timesnewroman24.dat");
		return false;
	}
	font24->setColumns(16);
	font24->setCharSize(32,36);

	//load the block sprite image
	block_image = new Texture();
	if (!block_image->Load("blocks.bmp")) {
		g_engine->message("Error loading blocks");
		return false;
	}

	//load the invader images
	ostringstream filename;
	for (int n=0; n<ROWS; n++) {
		invaderImages[n] = new Texture();
		filename.str("");
		filename << "invader" << n << ".bmp";
		if (!invaderImages[n]->Load(filename.str(), BLACK)) {
			g_engine->message("Error loading invader image");
			g_engine->Close();
		}
	}



	//load audio files
	g_engine->audio->Load("fire1.wav","fire1");
	g_engine->audio->Load("fire2.wav","fire2");
	g_engine->audio->Load("explode1.wav","explode1");
	g_engine->audio->Load("explode2.wav","explode2");
	g_engine->audio->Load("step1.wav","step1");
	g_engine->audio->Load("step2.wav","step2");
	g_engine->audio->Load("step3.wav","step3");
	g_engine->audio->Load("step4.wav","step4");


    //set the camera and perspective
	camera = new Camera();
	camera->setPosition(0.0f, 6.0f, 20.0f);
	camera->setTarget(0.0f, 2.0f, 0.0f);
	camera->Update();

	//set up lighting
	g_engine->SetAmbient(D3DCOLOR_RGBA(80,80,80, 0));
	
	//load Earth for background 
	earth = new Mesh();
	earth->Load("earth.x");
	earth->SetPosition(0.0f,-40.0f,-200.0f);
	earth->SetScale(0.9f, 1.0f, 1.0f);

	//load the ship
	Sprite *ship = new Sprite();
	ship->setObjectType(ENTITY_PLAYER);
	ship->loadImage("ship.bmp", BLACK);
	double x = (double)g_engine->getScreenWidth()/2;
	double y = (double)g_engine->getScreenHeight()-50;
	ship->setPosition(x,y);
	ship->setMoveTimer(1);
	g_engine->addEntity(ship);

	//load the bullets
	bulletUp = new Texture();
	bulletUp->Load("bulletup.bmp", BLACK);
	bulletDown = new Texture();
	bulletDown->Load("bulletdown.bmp", BLACK);

	//load exlosion
	explosion_image = new Texture();
	if (!explosion_image->Load("explosion_30_128.tga")) {
		g_engine->message("Error loading explosion image");
		return false;
	}


	//reset the game
	resetGame();

	//begin in pause mode
	g_engine->setPaused(true);

    return true;
}


void firebulletdown(double x,double y)
{
	Sprite *bullet = new Sprite();
	bullet->setImage(bulletDown);
	bullet->setObjectType(ENTITY_ALIEN_BULLET);
	bullet->setX(x);
	bullet->setY(y);
	bullet->setVelocity(0.0f, 8.0f);
	bullet->setLifetime(4000);
	g_engine->addEntity(bullet);
	g_engine->audio->Play("fire2");
}

void firebulletup()
{
	if (!readyToFire) return;
	readyToFire = false;
	
	//get the ship
	Sprite *ship = (Sprite*)g_engine->findEntity(ENTITY_PLAYER);
	if (!ship) return;
	
	Sprite *bullet = new Sprite();
	bullet->setImage(bulletUp);
	bullet->setObjectType(ENTITY_PLAYER_BULLET);
	bullet->setX(ship->getX() + 14);
	bullet->setY(ship->getY() - 16);
	bullet->setVelocity(0.0f, -8.0f);
	bullet->setLifetime(4000);
	g_engine->addEntity(bullet);
	g_engine->audio->Play("fire1");
}


void updateInvaders()
{
	int w = invaders[0][0]->getWidth();
	int h = invaders[0][0]->getHeight();
	bool flag = false;

	//update horde position
	alienX += alienDir;

	//examine horde edges 
	if (alienX < -80 || alienX > 350) {
		alienDir *= -1;
		alienX += alienDir;
		alienY += 10;
	}
	
	//update invaders
	for (int y=0; y<ROWS; y++) 
	{
		for (int x=0; x<COLUMNS; x++) 
		{
			if (invaders[y][x]->getAlive()) 
			{
				flag = true;

				//update animation
				invaders[y][x]->animate();

				//update position
				double px = alienX + x * 85.0f;
				double py = alienY + y * 65.0f;
				invaders[y][x]->setPosition(px,py);

				//did invader reach the bottom?
				if (py > playery-h) state = 2;

				//small chance of firing from this invader
				if (rand()%100 == 1) {
					alienFireX = px + 14;
					alienFireY = py + 16;
				}
			}
		} 
	} 

	//did player win?
	if (!flag) state = 2;

}

void game_update()
{
	static std::ostringstream s;
	static Timer alienShot;

	//game state
	switch(state)
	{
		case 0: //title
			g_engine->setPaused(true);
			break;

		case 1: //normal
			//move invaders one step at a time
			if (stepTime.stopwatch(500))
			{
				//play step sound
				s.str("");
				s << "step" << stepSound;
				g_engine->audio->Play(s.str());
				if (++stepSound > 4) stepSound = 1;

				//update invaders
				updateInvaders();

			} //if

							
			//update fire delay
			if (shotTime.stopwatch(700)) readyToFire = true;
			
			//any aliens firing?
			if (alienShot.stopwatch(500)) {
				if (alienFireX > 0.0) {
					firebulletdown(alienFireX,alienFireY);
					alienFireX = alienFireY = 0.0;
				}
			}

			break;

		case 2: //gameover
			g_engine->setPaused(true);
			break;
	}
}

//3D rendering with timing
void game_render3d()
{
    g_engine->ClearScene(BLACK);
    g_engine->SetIdentity();
	camera->Update();

	angle += 0.02f;
	earth->SetRotation(angle,0.0f,0.0f);
	earth->Transform();
	earth->draw();


}

//2D rendering with timing
void game_render2d()
{
	static ostringstream ostr;

	//game state
	switch(state)
	{
		case 0: //title
			font24->Print(320,300,"A L I E N    I N V A D E R S", RED);
			font24->Print(380,600,"press space to start", RED);
			break;

		case 1: //normal
			for (int y=0; y<ROWS; y++) {
				for (int x=0; x<COLUMNS; x++) {
					if (invaders[y][x]->getAlive())
						invaders[y][x]->draw();
				}
			}

			ostr.str("");
			ostr << "SCORE " << score;
			font24->Print(40,1,ostr.str(),0xDDEE4444);

			ostr.str("");
			ostr << "LIVES " << lives;
			font24->Print(850,1,ostr.str(),0xDDEE4444);

			break;

		case 2: //gameover
			font24->Print(370,300,"G A M E    O V E R");
			font24->Print(400,600,"press any key");
			break;
			
		case 3: //paused
			font24->Print(350,300,"G A M E    P A U S E D");
			font24->Print(380,650,"press p to resume");
			break;
	}

	//update console
	if (console->isShowing()) {
		ostr.str("");
		ostr << "Core refresh: " << fixed << setprecision(4) 
			<< 1000.0 / (double)g_engine->getFrameRate_core() << " ms";
		console->print(ostr.str(),1);

		ostr.str("");
		ostr << "Screen refresh: "
			<< 1000.0 / (double)g_engine->getFrameRate_real() << " ms";
		console->print(ostr.str(),2);

		ostr.str("");
		ostr << "Entities: " << fixed << setprecision(0) 
			<< g_engine->getEntityCount();
		console->print(ostr.str(),3);

		ostr.str("");
		ostr << "Horde position: " << alienX << "," << alienY;
		console->print(ostr.str(),4);

		ostr.str("");
		ostr << "Player: " << playerx << "," << playery;
		console->print(ostr.str(),27);

		console->draw();
	}
}

void game_keyPress(int key)
{

	switch(key) {
		case DIK_LEFT:
			shipvel -= 0.01;
			if (shipvel < -0.5) shipvel = -0.5;
			break;
			
		case DIK_RIGHT:
			shipvel += 0.01f;
			if (shipvel > 0.5) shipvel = 0.5;
			break;
	}
}

void game_keyRelease(int key)
{
	static Timer delay;
	
	switch(key) {
		case DIK_GRAVE:
		case DIK_F12:
			console->setShowing(!console->isShowing());
			break;

		case DIK_ESCAPE: 
			if (state == 1) 
			{
				while(!delay.stopwatch(500));
				state = 2;
				g_engine->setPaused(true);
			}
			else if (state == 2) {
				g_engine->Close();
			}
			break;
			
		case DIK_P: //pause
			if (state == 1)
			{
				g_engine->setPaused(true);
				state = 3;
			}
			else if (state == 3)
			{
				g_engine->setPaused(false);
				state = 1;
			}
			break;
			
		case DIK_LCONTROL:
			if (state == 1)
			{
				firebulletup();
			}
			break;
			
		case DIK_SPACE: 
			if (state == 0)
			{
				while(!delay.stopwatch(500));
				g_engine->setPaused(false);
				state = 1;
			}
			
			if (state == 2)
			{
				while(!delay.stopwatch(500));
				resetGame();
				g_engine->setPaused(false);
				state = 0;
			}
			
			break;
	}
}

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }

void game_end()
{
	if (console) delete console;
	if (bulletUp) delete bulletUp;
	if (bulletDown) delete bulletDown;
	if (camera) delete camera;
	if (earth) delete earth;
//	if (font10) delete font10;
	if (font24) delete font24;

	for (int n=0; n<ROWS; n++) 
		if (invaderImages[n]) delete invaderImages[n];

	for (int y=0; y<ROWS; y++) 
		for (int x=0; x<COLUMNS; x++) 
			if (invaders[y][x]) delete invaders[y][x];

}


void doExplosion(Sprite *target)
{
	Sprite *expl = new Sprite();
	expl->setObjectType(ENTITY_EXPLOSION);
	expl->setImage(explosion_image);
	expl->setColumns(6);
	expl->setTotalFrames(30);
	expl->setSize(128,128);
	expl->setCollidable(false);
	expl->setFrameTimer(20);
	expl->setLifetime(500);
	double x = target->getX() - target->getWidth()/2 + 16;
	double y = target->getY() - target->getHeight()/2;
	expl->setPosition(x,y);
	g_engine->addEntity( expl );
}

void game_entityUpdate(Entity *entity)
{
	double w;

	//exit if this is not a sprite
	if (entity->getRenderType() != RENDER2D) return;
	Sprite *sprite = (Sprite*)entity;

	if (state == 1)
	{
		switch(entity->getObjectType())
		{
			case ENTITY_PLAYER: 
				w = (double)(g_engine->getScreenWidth() - sprite->getWidth()-5 );
				sprite->setVelocity(shipvel,0);
				if (sprite->getX() < 5) {
					sprite->setVelocity(0,0);
					sprite->setX(5);
				}
				if (sprite->getX() > w) {
					sprite->setVelocity(0,0);
					sprite->setX(w);
				}

				break;

			case ENTITY_PLAYER_BULLET: //bullet
				if (sprite->getY() < 0)	sprite->setAlive(false);

				//check for collisions
				for (int y=0; y<ROWS; y++) {
					for (int x=0; x<COLUMNS; x++) {
						if (invaders[y][x]->getAlive()) {
							if (g_engine->collision(invaders[y][x], sprite)) 
							{
								invaders[y][x]->setAlive(false);
								sprite->setAlive(false);
								g_engine->audio->Play("explode2");
								doExplosion(invaders[y][x]);
								score += 10;
								break;
							}
						}
					}
				}
				break;

		}
	}
}

void game_entityRender(Entity *entity)
{
	static char s[80];

	//exit if this is not a sprite
	if (entity->getRenderType() != RENDER2D) return;
	Sprite *sprite = (Sprite*)entity;

	if (state == 1)
	{
		switch(sprite->getObjectType()) {
			case 10: //ship
				playerx = sprite->getX();
				playery = sprite->getY();
				break;
		}
	}
}

void game_entityCollision(Entity *entity1, Entity *entity2)
{
	int frame;

	//exit if these entities are not sprites
	if (entity1->getRenderType() != RENDER2D) return;
	if (entity2->getRenderType() != RENDER2D) return;

	//normal gameplay mode?
	if (state == 1)
	{
		Sprite *sprite1 = (Sprite*)entity1;
		Sprite *sprite2 = (Sprite*)entity2;

		switch(sprite1->getObjectType())
		{
			case ENTITY_BLOCK: 

				switch (sprite2->getObjectType())
				{
					case ENTITY_PLAYER_BULLET: //player bullet
						g_engine->audio->Play("explode1");
						sprite2->setAlive(false);
						frame = sprite1->getCurrentFrame() + 1;
						if (frame < sprite1->getTotalFrames())
							sprite1->setCurrentFrame(frame);
						else
							sprite1->setAlive(false);
						break;
			 		case ENTITY_ALIEN_BULLET: 
						g_engine->audio->Play("explode1");
						sprite2->setAlive(false);
						frame = sprite1->getCurrentFrame() + 1;
						if (frame < sprite1->getTotalFrames())
							sprite1->setCurrentFrame(frame);
						else
							sprite1->setAlive(false);
						break;
				}
 				break;
	 			
			case ENTITY_PLAYER:
				switch (sprite2->getObjectType())
				{
			 		case ENTITY_ALIEN_BULLET: 
						g_engine->audio->Play("explode2");
						//sprite1->setAlive(false);
						sprite2->setAlive(false);
						if (--lives < 1) state = 2;
						break;
				}
		}
	}
}

