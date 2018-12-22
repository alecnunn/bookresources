#include "Advanced2D.h"
#include "Entity.h"

#pragma once

//***mod--change all doubles to double
namespace Advanced2D {

	enum CollisionType {
		COLLISION_NONE = 0,
		COLLISION_RECT = 1,
		COLLISION_DIST = 2
	};

//*****ADDED IN CHAPTER 7
	class Sprite : public Entity {
	private:

//moved to entity
//		bool visible;
//		bool alive;
//		int lifetimeLength;
//		Timer lifetimeTimer;
//		int objecttype;

		Vector3 position;
		Vector3 velocity;
		bool imageLoaded;
		int state;
		int direction;

//*****CHAPTER 8 - moved some properties to protected to support Font subclass
	protected:
		Texture *image;
		int width,height;
		int animcolumns;

//*****CHAPTER 7 - MOVED TO ENTITY.H
		//std::string name;

//******UPDATE CHAPTER 3 LISTING, NOW USING TIMER-BASED ANIMATION
		//int framecount,frametimer;
		int framestart,frametimer;

//*****CHAPTER 9 -- ADDED TIMER-BASED MOVEMENT, NEED TO UPDATE CHAPTER 3
		int movestart, movetimer;

//*****CHAPTER 9 -- ADDED COLLISION PROPERTY, NEED TO UPDATE CHAPTER 3
		bool collidable;
		enum CollisionType collisionMethod;

		int curframe,totalframes,animdir;
		double faceAngle, moveAngle;
		int animstartx, animstarty;

//******UPDATE CHAPTER 3 LISTING TO SUPPORT ROTATION/SCALING
		double rotation, scaling;
		D3DXMATRIX matRotate;
		D3DXMATRIX matScale;
		void transform();
		D3DCOLOR color;
	
	public:

//*****REMOVE FROM CHAPTER 3 LISTING, MOVED TO ENTITY.H
		//int id; //used by entity mgr
	
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

//moved to entity
		//accessors/mutators
//		int getObjectType() { return objecttype; }
//		void setObjectType(int value) { objecttype = value; }

//****moved to entity
/*		bool getVisible() { return visible; }
		void setVisible(bool value) { visible = value; }

		bool getAlive() { return alive; }
		void setAlive(bool value) { alive = value; }
*/
		int getState() { return state; }
		void setState(int value) { state = value; }

//****moved to entity
		//std::string getName() { return name; }
		//void setName(std::string value) { name = value; }

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

//****moved to entity
//		int getLifetime() { return lifetimeLength; }
//		void setLifetime(int milliseconds) { lifetimeLength = milliseconds; lifetimeTimer.reset(); }
//		bool lifetimeExpired() { return lifetimeTimer.stopwatch(lifetimeLength); }

//******UPDATE CHAPTER 3 LISTING TO SUPPORT ROTATION/SCALING
		double getRotation() { return rotation; }
		void setRotation(double value) { rotation = value; }
		double getScale() { return scaling; }
		void setScale(double value) { scaling = value; }
		void setColor(D3DCOLOR col) { color = col; }

//*****CHAPTER 9 -- ADDED TIMER-BASED MOVEMENT, NEED TO UPDATE CHAPTER 3
		int getMoveTimer() { return movetimer; }
		void setMoveTimer(int value) { movetimer = value; }

//*****CHAPTER 9 -- ADDED COLLISION PROPERTY, NEED TO UPDATE CHAPTER 3
		bool isCollidable() { return collidable; }
		void setCollidable(bool value) { collidable = value; }
		CollisionType getCollisionMethod() { return collisionMethod; }
		void setCollisionMethod(CollisionType type) { collisionMethod = type; }

	public:
		Sprite();
		virtual ~Sprite();
		bool loadImage(std::string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(255,0,255));
		void setImage(Texture *);

//declared as pure virtual in Entity.h
		void move();
		void animate();

//******UPDATE CHAPTER 3 LISTING
//declared pure virtual in entity
		void draw();

//deleted in ch7--use only draw
		//void drawframe();
	}; //class
};

