/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "priv_precompiled.h"

#include "objectfactories.h"

#include <map>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

typedef map<string, const ObjectFactory*>	FactoryMap;
typedef vector<GameObject*>	ObjectList;

FactoryMap& GetFactoryMap()
{
	// This object must be statically declared within a function to guarantee that it will
	// be initialized before it used for the first time.
	static FactoryMap s_factoryMap;
	return s_factoryMap;
}

// Memory leak - this never actually gets freed.
static ObjectList s_objectList;

static ObjectList s_justDiedList;

GameObject* CreateGameObject(const char* objectType)
{
#ifdef	_DEBUG
	// Make sure a factory has been registered for this object type.
	FactoryMap::iterator p = GetFactoryMap().find(objectType);
	if (p == GetFactoryMap().end())
	{
		dprintf("Type '%s' is not registered\n", objectType);
		assert(!"Error - tried creating object with unregistered type.");
	}
#endif
	const ObjectFactory* pFactory = GetFactoryMap()[objectType];
	GameObject* pObject = pFactory->CreateGameObject();
	s_objectList.push_back(pObject);
	pObject->PostConstructorInit();
	return pObject;
}

void ObjectFrameEnd()
{
	// When an object dies it is set to one. At the end of that frame it is
	// set to two. At the end of the next frame, every thread has had a chance
	// to notice that it is dead, so it can be deleted then. So, when the
	// dead count is greater than two you can kill it.
	const int k_maxDeadFrames = 2;
	for (ObjectList::iterator p = s_justDiedList.begin(); p != s_justDiedList.end(); /**/)
	{
		if (++((*p)->m_deadFrames) > k_maxDeadFrames)
		{
			delete *p;
			p = s_justDiedList.erase(p);
		}
		else
			++p;
	}
}

void GameObject::DestroyObject(GameObject* pObject)
{
	ObjectList::iterator p = find(s_objectList.begin(), s_objectList.end(), pObject);
	assert(p != s_objectList.end());
	s_objectList.erase(p);
	// Mark the object as dead.
	pObject->m_deadFrames = 1;
	s_justDiedList.push_back(pObject);
}

const vector<GameObject*>& GetGameObjects()
{
	return s_objectList;
}

void RegisterFactory(const ObjectFactory* pFactory)
{
	const char* objectType = pFactory->GetObjectType();
#ifdef	_DEBUG
	// Make sure a factory has NOT been registered for this object type.
	FactoryMap::iterator p = GetFactoryMap().find(objectType);
	assert(p == GetFactoryMap().end());
#endif
	GetFactoryMap()[objectType] = pFactory;
}

std::vector<const char*> GetObjectTypes()
{
	vector<const char*>	objectList;
	for (FactoryMap::iterator p = GetFactoryMap().begin(); p != GetFactoryMap().end(); ++p)
	{
		objectList.push_back(p->second->GetObjectType());
	}
	return objectList;
}

size_t GetNumObjectTypes()
{
	return GetFactoryMap().size();
}
