#include "RTTITypeID.hpp"
#include "Component.hpp"
#include "Property.hpp"
#include "Factory.hpp"


typedef ComponentLibrary::RTTITypeID TypeID;
typedef ComponentLibrary::Property Property;


struct Component : ComponentLibrary::Component<TypeID, TypeID>, Property
{};


typedef ComponentLibrary::Factory<TypeID, Component> Factory;
 

struct Player : Base
{
	float x, y, z;


	Player()
	{
		AddChildProperty("x", &x);
		AddChildProperty("y", &y);
		AddChildProperty("z", &z);
	}
};


void m()
{
	Factory::Singleton().Support<Player>();


	Component* component = Factory::Singleton().Create("class Player");


	component["x"] = 3.0f;
	component["y"] = 2.0f;
	component["z"] = 2.0f;
}