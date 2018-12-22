
#include "Advanced2D.h"

#pragma once

namespace Advanced2D {

	enum RenderType {
		RENDER2D = 0,
		RENDER3D = 1
	};

	class Entity {
	private:
		int id;
		std::string name;
		bool visible;
		bool alive;
		enum RenderType renderType;
		int objectType;
		int lifetimeLength;
		Timer lifetimeTimer;

	public:
		Entity(enum RenderType renderType);
		virtual ~Entity() { };

		virtual void move() = 0;
		virtual void animate() = 0;
		virtual void draw() = 0;

		void setID(int value) { id = value; }
		int getID() { return id; }

		void setRenderType(enum RenderType type) { renderType = type; }
		enum RenderType getRenderType() { return renderType; }

		std::string getName() { return name; }
		void setName(std::string value) { name = value; }

		bool getVisible() { return visible; }
		void setVisible(bool value) { visible = value; }

		bool getAlive() { return alive; }
		void setAlive(bool value) { alive = value; }

		int getLifetime() { return lifetimeLength; }
		void setLifetime(int milliseconds) { lifetimeLength = milliseconds; lifetimeTimer.reset(); }
		bool lifetimeExpired() { return lifetimeTimer.stopwatch(lifetimeLength); }

		int getObjectType() { return objectType; }
		void setObjectType(int value) { objectType = value; }

	};


};
