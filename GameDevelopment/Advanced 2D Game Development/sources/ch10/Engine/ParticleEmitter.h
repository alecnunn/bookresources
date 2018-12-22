#include "Advanced2D.h"

#ifndef _PARTICLEEMITTER_H
#define _PARTICLEEMITTER_H 1


//***mod--floats to doubles

namespace Advanced2D {

	class ParticleEmitter
	{
	private:
		typedef std::vector<Sprite*>::iterator iter;
		std::vector<Sprite*> particles;
		Texture *image;
		Vector3 position;
		double direction;
		double length;
		int max;
		int alphaMin,alphaMax;
		int minR,minG,minB,maxR,maxG,maxB;
		int spread;
		double velocity;
		double scale;

//*****MOVE TO PUBLIC IN CHAPTER 4
		//void add();

	public:
		void setPosition(double x, double y) { position.Set(x,y,0); }
		void setPosition(Vector3 vec) { position = vec; }
		Vector3 getPosition() { return position; }
		void setDirection(double angle) { direction = angle; }
		double getDirection() { return direction; }
		void setMax(int num) { max = num; }
		void setAlphaRange(int min,int max);
		void setColorRange(int r1,int g1,int b1,int r2,int g2,int b2);
		void setSpread(int value) { spread = value; }
		void setLength(double value) { length = value; }
		void setVelocity(double value) { velocity = value; }
		void setScale(double value) { scale = value; }

		ParticleEmitter();
		bool loadImage(std::string imageFile);
		virtual ~ParticleEmitter();
		void draw();
		void update();

//*****MOVED FROM PRIVATE
		void add();


	};

};

#endif
