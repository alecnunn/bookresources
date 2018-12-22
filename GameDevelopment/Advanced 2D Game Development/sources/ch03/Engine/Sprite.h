#include "Advanced2D.h"

#pragma once

namespace Advanced2D {

	enum CollisionType {
		COLLISION_NONE = 0,
		COLLISION_RECT = 1,
		COLLISION_DIST = 2
	};

	class Sprite {
	private:
		bool visible;
		bool alive;
    	int lifetimeLength;
		Timer lifetimeTimer;
		int objecttype;

		Vector3 position;
		Vector3 velocity;
		bool imageLoaded;
		int state;
		int direction;

	protected:
		Texture *image;
		int width,height;
		int animcolumns;
		int framestart,frametimer;
		int movestart, movetimer;
		bool collidable;
		enum CollisionType collisionMethod;
		int curframe,totalframes,animdir;
		double faceAngle, moveAngle;
		int animstartx, animstarty;
		double rotation, scaling;
		D3DXMATRIX matRotate;
		D3DXMATRIX matScale;
		void transform();
		D3DCOLOR color;
	
	public:
		//screen position
		Vector3 getPosition() { return position; }
		void setPosition(Vector3 position) { this->position = position; }
		void setPosition(double x, double y) { position.Set(x,y,0); }
		double getX() { return position.getX(); }
		double getY() { return position.getY(); }
		void setX(double x) { position.setX(x); }
		void setY(double y) { position.setY(y); }

		//movement velocity
		Vector3 getVelocity() { return velocity; }
		void setVelocity(Vector3 v) { this->velocity = v; }
		void setVelocity(double x, double y) { velocity.setX(x); velocity.setY(y); }
	
		//image size
		void setSize(int width, int height) { this->width = width; this->height = height; }
		int getWidth() { return this->width; }
		void setWidth(int value) { this->width = value; }
		int getHeight() { return this->height; }
		void setHeight(int value) { this->height = value; }

		bool getVisible() { return visible; }
		void setVisible(bool value) { visible = value; }

		bool getAlive() { return alive; }
		void setAlive(bool value) { alive = value; }

		int getState() { return state; }
		void setState(int value) { state = value; }

		int getDirection() { return direction; }
		void setDirection(int value) { direction = value; }

		int getColumns() { return animcolumns; }
		void setColumns(int value) { animcolumns = value; }

		int getFrameTimer() { return frametimer; }
		void setFrameTimer(int value) { frametimer = value; }

		int getCurrentFrame() { return curframe; }
		void setCurrentFrame(int value) { curframe = value; }

		int getTotalFrames() { return totalframes; }
		void setTotalFrames(int value) { totalframes = value; }

		int getAnimationDirection() { return animdir; }
		void setAnimationDirection(int value) { animdir = value; }

		double getRotation() { return rotation; }
		void setRotation(double value) { rotation = value; }
		double getScale() { return scaling; }
		void setScale(double value) { scaling = value; }
		void setColor(D3DCOLOR col) { color = col; }

		int getMoveTimer() { return movetimer; }
		void setMoveTimer(int value) { movetimer = value; }

		bool isCollidable() { return collidable; }
		void setCollidable(bool value) { collidable = value; }
		CollisionType getCollisionMethod() { return collisionMethod; }
		void setCollisionMethod(CollisionType type) { collisionMethod = type; }

	public:
		Sprite();
		virtual ~Sprite();
		bool loadImage(std::string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(255,0,255));
		void setImage(Texture *);
		void move();
		void animate();
		void draw();

	}; //class
}; //namespace

