
#include "Advanced2D.h"

namespace Advanced2D {

	Entity::Entity(enum RenderType renderType)
	{
		this->renderType = renderType;
		this->id = -1;
		this->name = "";
		this->visible = true;
		this->alive = true;
		this->objectType = 0;
		this->lifetimeLength = 0;
		this->lifetimeTimer.reset();
	}
	
};
