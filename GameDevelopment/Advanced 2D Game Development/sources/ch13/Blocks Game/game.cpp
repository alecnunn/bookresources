/*
  Blocks Game
  Advanced 2D Game Development
*/

#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;
using namespace std;

#include "levels.h"

#define STATE_PLAY      1
#define STATE_TITLE     2
#define STATE_LOSER     3
#define STATE_CLEARED   5
int gameState = STATE_TITLE;

#define SPRITE_BALL 100
#define SPRITE_PADDLE 101
#define SPRITE_BLOCK 102
#define SPRITE_GLOW 103

Console *console;
Texture *block_image;
Sprite *glow;
Font *times24;

double paddleVelocity = 0.0;
int score = 0;
int level = 0;
Vector3 ballPos;
Vector3 paddlePos;
double anglePaddleToBall;
double ballPower = 1;

ParticleEmitter *peScore;
ParticleEmitter *pePower;
ParticleEmitter *peBall;
ParticleEmitter *peLevel1, *peLevel2;

bool game_preload()
{
	g_engine->setAppTitle("BLOCK ATTACK");
	g_engine->setScreenWidth(800);
	g_engine->setScreenHeight(600);
	g_engine->setColorDepth(32);
	g_engine->setFullscreen(false);
	return true;
}

void loadLevel(int level)
{
    //remove existing blocks
    Entity *entity;
    while ( (entity = g_engine->findEntity(SPRITE_BLOCK)) != NULL)
        entity->setAlive(false);

    //load new blocks
    Sprite *block;
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLUMNS; x++)
        {
            int num = levels[level][y][x];
            if (num > 0) {
                block = new Sprite();
                block->setImage(block_image);
                block->setSize(58,28);
                block->setColumns(4);
                block->setTotalFrames(8);
                block->setCurrentFrame(num-1);
                block->setAnimationDirection(0);
                block->setPosition(45 + x * 60, 70 + y * 30);
                block->setObjectType( SPRITE_BLOCK );
                block->setColor(0x77FFFFFE);
                g_engine->addEntity( block );
            }
        }
    }

}



bool initParticles()
{
    //create particle emitter for score display
    peScore = new ParticleEmitter();
    if (!peScore->loadImage("particle16.tga")) {
        g_engine->message("Error loading particle image");
        return false;
    }
    peScore->setPosition(180,10);
    peScore->setScale(4.0f);
    peScore->setMax(300);
    peScore->setAlphaRange(1,16);
    peScore->setColorRange(200,200,200,255,255,255);
    peScore->setVelocity(1.8);
    peScore->setDirection(270);
    peScore->setSpread(20);
    peScore->setLength(220);

    //create particle emitter for power display
    pePower = new ParticleEmitter();
    if (!pePower->loadImage("particle16.tga")) {
        g_engine->message("Error loading particle image");
        return false;
    }
    pePower->setPosition(560,10);
    pePower->setScale(4.0f);
    pePower->setMax(300);
    pePower->setAlphaRange(1,16);
    pePower->setColorRange(200,200,200,255,255,255);
    pePower->setVelocity(1.8);
    pePower->setDirection(90);
    pePower->setSpread(20);
    pePower->setLength(220);


    //create particle emitter that follows ball
    peBall = new ParticleEmitter();
    if (!peBall->loadImage("particle16.tga")) {
        g_engine->message("Error loading particle image");
        return false;
    }
    peBall->setPosition(0,0);
    peBall->setScale( 0.8 + (double)(rand()%10) / 10.0 );
    peBall->setMax(400);
    peBall->setAlphaRange(1,16);
    peBall->setColorRange(100,100,100,250,250,250);
    peBall->setVelocity(0.5);
    peBall->setSpread(90);
    peBall->setLength(2000);
	peBall->setLifetime(1000);


    //create left particle emitter for level display
    peLevel1 = new ParticleEmitter();
    if (!peLevel1->loadImage("particle16.tga")) {
        g_engine->message("Error loading particle image");
        return false;
    }
    peLevel1->setPosition(280,10);
    peLevel1->setScale( 4.0 );
    peLevel1->setMax(200);
    peLevel1->setAlphaRange(1,16);
    peLevel1->setColorRange(200,200,200,255,255,255);
    peLevel1->setVelocity(1.8);
	peLevel1->setDirection(90);
    peLevel1->setSpread(20);
    peLevel1->setLength(100);

    //create right particle emitter for level display
    peLevel2 = new ParticleEmitter();
    if (!peLevel2->loadImage("particle16.tga")) {
        g_engine->message("Error loading particle image");
        return false;
    }
    peLevel2->setPosition(460,10);
    peLevel2->setScale( 4.0 );
    peLevel2->setMax(200);
    peLevel2->setAlphaRange(1,16);
    peLevel2->setColorRange(200,200,200,255,255,255);
    peLevel2->setVelocity(1.8);
	peLevel2->setDirection(270);
    peLevel2->setSpread(20);
    peLevel2->setLength(100);

	return true;
}

bool initSprites()
{
    //create the paddle
    Sprite *paddle = new Sprite();
    if (!paddle->loadImage("paddle1.tga")) {
        g_engine->message("Error loading paddle");
        return false;
    }
    paddle->setPosition(g_engine->getScreenWidth()/2, g_engine->getScreenHeight()-60);
    paddle->setObjectType( SPRITE_PADDLE );
    paddle->setVisible(false);
    g_engine->addEntity( paddle );


    //load blocks
    block_image = new Texture();
    if (!block_image->Load("blocks.tga")) {
        g_engine->message("Error loading blocks");
        return false;
    }
    level = 0;
    loadLevel(level);   

    //create ball light sprite
    glow = new Sprite();
    if (!glow->loadImage("glow.tga")) {
        g_engine->message("Error loading glow");
        return false;
    }
    glow->setObjectType( SPRITE_GLOW );
    glow->setCollidable(false);

	//create the ball sprite
    Sprite *ball = new Sprite();
    if (!ball->loadImage("ball.tga")) {
      g_engine->message("Error loading ball");
      return false;
   }
    ball->setPosition(400,300);
    ball->setVelocity(1,-4);
    ball->setObjectType( SPRITE_BALL );
    ball->setVisible(false);
    g_engine->addEntity( ball );

	return true;
}

bool game_init(HWND hwnd)
{
	g_engine->setPaused(true);

    //create the console
    console = new Console();
    if (!console->init()) {
        g_engine->message("Error loading console");
        return false;
    }
    console->hide();

	//load the TimesNewRoman24 font
	times24 = new Font();
	if (!times24->loadImage("timesnewroman24.tga")) {
		g_engine->message("Error loading timesnewroman24.tga");
		return false;
	}
	if (!times24->loadWidthData("timesnewroman24.dat")) {
		g_engine->message("Error loading timesnewroman24.dat");
		return false;
	}
	times24->setColumns(16);
	times24->setCharSize(32,36);


    //load the background
    Sprite *background = new Sprite();
    if (!background->loadImage("background.tga",0)) {
	    g_engine->message("Error loading background image");
	    return false;
    }
    background->setCollidable(false);
    g_engine->addEntity( background );


    //load audio samples
    g_engine->audio->Load("hit.wav","hit");
    g_engine->audio->Load("coinflip.wav","coinflip");
    g_engine->audio->Load("music.ogg","music");


	if (!initSprites()) return false;
	if (!initParticles()) return false;


   return true;
}

void game_end()
{
    if (console) delete console;
    if (times24) delete times24;
    if (glow) delete glow;
    if (peScore) delete peScore;
    if (pePower) delete pePower;
    if (peBall) delete peBall;
	if (peLevel1) delete peLevel1;
	if (peLevel2) delete peLevel2;
}


void updatePaddle(Sprite *paddle)
{
    static Timer timer;
    if (!timer.stopwatch(10)) return;

	double right = g_engine->getScreenWidth() - paddle->getWidth() - 40;
    double px = paddle->getX() + paddleVelocity;
	if (px < 40) {
        paddleVelocity = 0.0;
        px = 40;
    }
	if (px > right) {
        paddleVelocity = 0.0;
        px = right;
    }
	paddle->setX(px); 

    ostringstream ostr;
    ostr << "Paddle pos [" << fixed << setprecision(0) <<  px << "] vel [" 
        << setprecision(2) << paddleVelocity << "]";
    console->print(ostr.str(),27);

    //save paddle pos
    paddlePos = paddle->getPosition();
}

void updateBall(Sprite *ball)
{
    static Timer timer;
    if (!timer.stopwatch(1)) return;

    //get ball position and velocity
	double vx = ball->getVelocity().getX();
	double vy = ball->getVelocity().getY();
	int rightEdge = g_engine->getScreenWidth() - ball->getWidth() - 40;
	int bottomEdge = g_engine->getScreenHeight() - ball->getHeight();

    //update ball velocity x
	if (ball->getX() < 40) {
        vx = fabs(vx);
        ball->setX(40);
	}
    if (ball->getX() > rightEdge) {
        vx = fabs(vx) * -1;
        ball->setX(rightEdge);
    }

    //update ball velocity y
	if (ball->getY() < 10) {
        vy = fabs(vy);
        ball->setY(10);
    }
    if (ball->getY() > bottomEdge) {
        vy = fabs(vy) * -1;
        ball->setY(bottomEdge);
        
        //ball loses all power when missed
        ballPower = 1.0;
    }

    //update ball's velocity
    ball->setVelocity(vx, vy);

    //print ball data
    ostringstream ostr;
    ostr << "Ball pos [" << fixed << setprecision(0)
        << ball->getX() << "," << ball->getY() << "] vel ["
        << setprecision(2)
        << ball->getVelocity().getX() << ","
        << ball->getVelocity().getY() << "]";
    console->print(ostr.str(),29);

    //save ball pos
    ballPos = ball->getPosition();
    peBall->setPosition( ballPos );

}



void game_update()
{
    //update particles
    peScore->update(5);
    pePower->update(5);
    peBall->update(5);
	peLevel1->update(5);
	peLevel2->update(5);

    switch (gameState) {
        case STATE_PLAY:    
			//calculate angle from ball to paddle for shadow rendering
			anglePaddleToBall = g_engine->math->AngleToTarget(paddlePos, ballPos);
			anglePaddleToBall = g_engine->math->toDegrees(anglePaddleToBall);
			anglePaddleToBall = g_engine->math->wrapAngleDegs(anglePaddleToBall-90);

			//update music
			if (!g_engine->audio->IsPlaying("music"))
				g_engine->audio->Play("music");


			//all blocks cleared?
			Entity *entity = g_engine->findEntity(SPRITE_BLOCK);
			if (entity == NULL) {
				gameState = STATE_CLEARED;
			}
			break;
    }

    //print status info
    if (console->isShowing()) {
    
        //print program title
        ostringstream title;
        title << "Game title [" << g_engine->getAppTitle() << "]";
        console->print(title.str(), 0);
    
        //print screen resolution
        ostringstream screen;
        screen << "Screen res [" << g_engine->getScreenWidth() 
            << "," << g_engine->getScreenHeight() 
            << "] depth [" << g_engine->getColorDepth() << "]";
        console->print(screen.str(), 2);
    
        //print engine version
        console->print(g_engine->getVersionText(), 4);
    
        //print core rate
        ostringstream core;
        core << "Core update [" << fixed << setprecision(4)
            << 1000.0 / g_engine->getFrameRate_core() << "]";
        console->print(core.str(), 6);
    
        //print screen fps
        ostringstream fps;
        fps << "Screen refresh [" << fixed << setprecision(2)
            << g_engine->getFrameRate_real() << "]";
        console->print(fps.str(), 8);
    
        //print entity count
        ostringstream entities;
        entities << "Entitiy count [" << g_engine->getEntityCount() << "]";
        console->print(entities.str(), 10);

        //print print ball to paddle angle
        ostringstream angle;
        angle << "Angle paddle to ball [" << fixed << setprecision(2) 
            << anglePaddleToBall << "]";
        console->print(angle.str(), 31);
    }

}

void game_render2d()
{
	ostringstream sc,lvl,pow;
	static Timer timer;
	static D3DCOLOR color = 0xAA333333;
	static bool toggle = false;

	switch (gameState) {
		case STATE_PLAY:
			peScore->draw();
			pePower->draw();
			peBall->draw();
			peLevel1->draw();
			peLevel2->draw();

			//toggle color
			if (timer.stopwatch(750)) {
				toggle = !toggle;
				if (toggle)
					color = 0xAA333333;
				else
					color = 0xFFFFF000;
			}

			//print score
			sc << "SCORE " << score;
			times24->Print(42,20,sc.str(), color);

			//print level #
			lvl << "LEVEL " << level+1;
			times24->Print(350,20,lvl.str(), color);

			//print ball power
			pow << "POWER " << ballPower;
			times24->Print(622,20,pow.str(), color);

			break;

        case STATE_TITLE:   
			times24->Print(300,300,"BLOCKS GAME");
			break;

        case STATE_LOSER: 
			times24->Print(300,300,"Game Over");
			break;

        case STATE_CLEARED: 
			times24->Print(300,300,"Level Clear");
			break;    
	}

    console->draw();
}

void game_entityRender(Entity* entity) 
{ 
    switch (gameState) {
        case STATE_PLAY:

        //draw solid paddle over shadow
        if (entity->getObjectType() == SPRITE_PADDLE) {
            Sprite *paddle = (Sprite*)entity;
            double x = paddle->getX();
            double y = paddle->getY();
    
            double shadowX = g_engine->math->LinearVelocityX(anglePaddleToBall);
            shadowX *= 10.0;
            double shadowY = g_engine->math->LinearVelocityY(anglePaddleToBall);
            shadowY *= 10.0;
    
            ostringstream shadow;
            shadow << "Paddle shadow [" << fixed << setprecision(2) 
                << shadowX << "," << shadowY << "]";
            console->print(shadow.str(), 32);
    
            paddle->setPosition(x+shadowX,y+shadowY);
            paddle->setColor(0x44000000);
            paddle->draw();
    
            paddle->setColor(0xFFFFFFFF);
            paddle->setPosition(x, y);
            paddle->draw();
        }

        //draw solid ball over glow
        else if (entity->getObjectType() == SPRITE_BALL) {
            Sprite *ball = (Sprite*)entity;
            double x = ball->getX();
            double y = ball->getY();
    
    
            double scale = (double)ballPower / 2.0;
            glow->setScale( 1.0 + scale );
    
            glow->setPosition(x - glow->getWidth() * scale / 2.0, 
                y - glow->getHeight() * scale / 2.0);
    
            glow->setColor(0x33FFFFFF);
            glow->draw();
    
            D3DCOLOR color = 0xFFFFFFFF;
            //for (int n=0; n<ballPower; n++) color += 0x11000000;
            ball->setColor(color);
            ball->setPosition(x,y);
            ball->setScale( 1.0 + scale / 20.0 );
            ball->draw();
        }
    
        //draw solid block over shadow
    /*    else if (entity->getObjectType() == SPRITE_BLOCK) {
            Sprite *block = (Sprite*)entity;
            double x = block->getX();
            double y = block->getY();
            block->setColor(0x44000000);
            block->setPosition(x+5, y+4);
            block->draw();
            block->setColor(0xFFFFFFFF);
            block->setPosition(x,y);
            block->draw();
        }
    */
        break;
    }

}

void game_entityUpdate(Entity* entity) 
{ 
    if (gameState == STATE_PLAY) {
        switch (entity->getObjectType()) {
            case 100:
                updateBall( (Sprite*)entity );
                break;
    
            case 101: 
                updatePaddle( (Sprite*)entity );
                break;
        }
    }

}

/*
    This function will cause sprite b to bounce away from
    sprite a based on their orientation to each other.
*/
void reboundOffPaddle(Sprite *a, Sprite *b)
{
    //get center of sprite a
    double acx = a->getX() + a->getWidth()/2;
    double acy = a->getY() + a->getHeight()/2;
    
    //get center of sprite b
    double bcx = b->getX() + b->getWidth()/2;
    double bcy = b->getY() + b->getHeight()/2;

    //get velocity of b
    double vx = b->getVelocity().getX();
    double vy = b->getVelocity().getY();

    //is b to the right?
    if (bcx > acx) {

        //b will rebound to the right
        vx = fabs(vx);

        //is b down/right?
        if (bcy > acy) {

            //b will rebound down/right
            vy = fabs(vy);
            
        }
        //no, b is up/right
        else {

            //b will rebound up/right
            vy = fabs(vy) * -1.0;

        }

    }
    //no, b is left
    else if (bcx <= acx) {

        //b will rebound to the left
        vx = fabs(vx) * -1.0;

        //is b down/left?
        if (bcy > acy) {

            //b will rebound up/left
            vy = fabs(vy);
            
        }
        //no, b is up/left
        else {

            //b will rebound down/left
            vy = fabs(vy) * -1.0;

        }
    }

    //set new velocity
    b->setVelocity(vx,vy);
}

/*
    This function will cause sprite b to bounce away from
    sprite a based on their orientation to each other.
*/
void reboundOffBlock(Sprite *block, Sprite *ball)
{
    //get center of block
    double acx = block->getX() + block->getWidth()/2;
    double acy = block->getY() + block->getHeight()/2;
    
    //get center of ball
    double bcx = ball->getX() + ball->getWidth()/2;
    double bcy = ball->getY() + ball->getHeight()/2;

    //get velocity of ball
    double vx = ball->getVelocity().getX();
    double vy = ball->getVelocity().getY();

    //is b down/right?
    if (bcy > acy) {

        //b will rebound down/right
        vy = fabs(vy);
        
    }
    //no, b is up/right
    else {

        //b will rebound up/right
        vy = fabs(vy) * -1.0;

    }


    //set new velocity
    ball->setVelocity(vx,vy);
}

void tweakBall(Sprite *ball)
{
    //add slight value to velocity
    double vx = ball->getVelocity().getX();
    double vy = ball->getVelocity().getY();
    double rx = (double)(rand()%9+1) / 100.0;
    double ry = (double)(rand()%9+1) / 100.0;
    if (vx < 0) vx -= rx; else vx += rx;
    if (vy < 0) vy -= ry; else vy += ry;

    //keep velocity within reasonable bounds
    if (vx > 6.0) vx = 6.0;
    if (vy > 6.0) vy = 6.0;
    ball->setVelocity(vx,vy);
}

void hitBlock(Sprite *block)
{
    D3DCOLOR color = block->getColor();

    //increase alpha based on ball power
    for (int n = 0; n < ballPower; n++) 
    {
        color += 0x11000000;
        if (color > 0xEEFFFFFF) {
			//destroy block
			block->setAlive(false);
            //score points!
            score += (int)ballPower;
            return;
        }
    }

    //store new alpha
    block->setColor( color );
}

void game_entityCollision(Entity* entity1,Entity* entity2) 
{ 
    Sprite *a = (Sprite*)entity1;
    Sprite *b = (Sprite*)entity2;
//    double vx,vy;

    switch (a->getObjectType()) {
        case SPRITE_PADDLE:
            if (b->getObjectType() == SPRITE_BALL) {
                //rebound ball off of the paddle
                reboundOffPaddle(a,b);

                //tweak the ball's velocity
                tweakBall(b);

                //increase ball's power output
                if (++ballPower > 9.0) ballPower = 9.0;

                g_engine->audio->Play("coinflip");
            }
            break;

        case SPRITE_BLOCK:
            if (b->getObjectType() == SPRITE_BALL) {
                //bounce ball away from block
                reboundOffBlock(a,b);

                //destroy the block
                hitBlock(a);

                //play hit sound
                g_engine->audio->Play("hit");
            }
            break;
    }

}

void game_keyPress(int key) 
{ 
    switch (gameState) {
        case STATE_PLAY:    
    		switch (key) {
    			case DIK_LEFT:	paddleVelocity -= 0.15;	break;
       			case DIK_RIGHT:	paddleVelocity += 0.15;	break;
    		}
			break;
    }
	
}

void game_keyRelease(int key) 
{ 
	switch (key) {
        case DIK_F12:
        case DIK_GRAVE:		
			console->setShowing( !console->isShowing() );  break;
	}

    switch (gameState) {
        case STATE_PLAY:    
    		switch (key) {
				case DIK_ESCAPE:	
					g_engine->Close();	break;
    		}
			break;
        case STATE_TITLE:   
    		switch (key) {
				case DIK_ESCAPE: 
					g_engine->Close();	break;
				default:
					gameState = STATE_PLAY;
					g_engine->setPaused(false);
			}
			break;
        case STATE_LOSER: 
			gameState = STATE_TITLE;	break;
			g_engine->setPaused(true);

		case STATE_CLEARED: 
			gameState = STATE_PLAY;		
			g_engine->setPaused(false);
			if (++level > LEVELS-1) level = 0;
			loadLevel(level);
			break;
	}

}

void game_render3d()
{
	g_engine->ClearScene(D3DCOLOR_XRGB(20,20,20));
}

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }

