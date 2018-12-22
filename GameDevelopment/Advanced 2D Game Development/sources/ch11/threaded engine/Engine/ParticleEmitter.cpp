
#include "ParticleEmitter.h"

//***mod--floats to doubles

namespace Advanced2D {

	ParticleEmitter::ParticleEmitter()
	{
		image = NULL;
		max = 100;
		length = 100;
		direction = 0;
		alphaMin = 254;	alphaMax = 255;
		minR = 0; maxR = 255;
		minG = 0; maxG = 255;
		minB = 0; maxB = 255;
		spread = 10;
		velocity = 1.0f;
		scale = 1.0f;
	}
	
	bool ParticleEmitter::loadImage(std::string imageFile)
	{
		image = new Texture();
//fixed return value
		return image->Load(imageFile);
	}
	
	
	ParticleEmitter::~ParticleEmitter()
	{
		delete image;
	
		for (iter i = particles.begin(); i != particles.end(); ++i)
		{
			delete *i;
		}
		particles.clear();
	}
	
	void ParticleEmitter::add()
	{
//***mod--defined in Math.h
		//static float PI_DIV_180 = 3.1415926535f / 180.0f;
		double vx,vy;

		Sprite *p = new Sprite();
		p->setImage(image);
		p->setPosition(position.getX(), position.getY());

		double variation = (rand() % spread - spread/2) / 100.0f;

		//wrap angles
		double dir = direction-90;
		if (dir < 0 || dir > 359) dir = 360 - dir;

//***mod--use math.h def
		//vx = cos( dir * PI_DIV_180) + variation;
		vx = cos( dir * PI_under_180) + variation;
		//vy = sin( dir * PI_DIV_180) + variation;
		vy = sin( dir * PI_under_180) + variation;

		p->setVelocity(vx * velocity,vy * velocity);

		//set random color based on ranges
		int r = rand()%(maxR-minR)+minR;
		int g = rand()%(maxG-minG)+minG;
		int b = rand()%(maxB-minB)+minB;
		int a = rand()%(alphaMax-alphaMin)+alphaMin;
		p->setColor(D3DCOLOR_RGBA(r,g,b,a));
	
		p->setScale( scale );
	
		particles.push_back(p);
	
	}
	
	void ParticleEmitter::draw()
	{
		//grow particle list
//***mod
		if ((int)particles.size() < max)
			add();
	
		//draw particles
		for (iter i = particles.begin(); i != particles.end(); ++i)
		{
			(*i)->draw();
		}
	}
	
	void ParticleEmitter::update()
	{
		for (iter i = particles.begin(); i != particles.end(); ++i)
		{
			(*i)->move();
			if ( (*i)->getPosition().Distance(this->position) > length)
			{
				(*i)->setX(position.getX());
				(*i)->setY(position.getY());
			}
		}
	}

	void ParticleEmitter::setAlphaRange(int min,int max) 
	{ 
		alphaMin=min; 
		alphaMax=max; 
	}

	void ParticleEmitter::setColorRange(int r1,int g1,int b1,int r2,int g2,int b2)
	{
		minR = r1; maxR = r2;
		minG = g1; maxG = g2;
		minB = b1; maxB = b2;
	}


	
}


