/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	OBJECTFACTORIES_H
#define	OBJECTFACTORIES_H

#include "gameobjects.h"
#include <vector>

// Menu IDs - not used in game.
const int k_firstDynamicID = 40000;

class ObjectFactory;
class GameWorld;

GameObject* CreateGameObject(const char* objectType);
void DestroyObject(GameObject* pObject);

// Get a list of all game objects currently in existence.
const std::vector<GameObject*>& GetGameObjects();
// Get a list of all the registered game object types.
std::vector<const char*> GetObjectTypes();
size_t GetNumObjectTypes();

// Call this once at the end of each frame to purge delete objects and do other housekeeping.
void ObjectFrameEnd();

// Call this to register your object factory. Normally you will use AutoFactory
// to do this, using the DECLARE_OBJECT_FACTORY macro.
void RegisterFactory(const ObjectFactory* pFactory);

// This macro - DECLARE_OBJECT_FACTORY - is used to easily create object
// factories. When you pass it the name of a game object class it
// automatically creates a static template AutoFactory. The object
// type is used to specialize the AutoFactory, give it a unique name,
// and pass the object type as a string to the constructor, which allows
// the object to be created by name. For instance, this:
//DECLARE_OBJECT_FACTORY(RotatingObject);
// gives you this:
//static AutoFactory<RotatingObject> s_RotatingObjectFactory("RotatingObject");
#define	DECLARE_OBJECT_FACTORY(objectType) \
		static AutoFactory<objectType> s_##objectType(#objectType)



class ObjectFactory
{
public:
	ObjectFactory() {}
	virtual GameObject* CreateGameObject() const = 0;
	virtual const char* GetObjectType() const = 0;

private:
	ObjectFactory& operator=(const ObjectFactory&);	// Not implemented.
	ObjectFactory(const ObjectFactory&);				// Not implemented.
};



template <class ObjectType>
class AutoFactory : public ObjectFactory
{
public:
	AutoFactory(const char* objectType)
		: m_name(objectType)
	{
		RegisterFactory(this);
	}
	virtual GameObject* CreateGameObject() const
	{
		return new ObjectType;
	}
	virtual const char* GetObjectType() const
	{
		return m_name;
	}
private:
	const char* m_name;
};

#endif
