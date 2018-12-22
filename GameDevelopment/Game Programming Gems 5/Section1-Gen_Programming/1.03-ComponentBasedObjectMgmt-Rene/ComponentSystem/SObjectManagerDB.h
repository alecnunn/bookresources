#ifndef __SOBJECTMANAGERDB_H
#define __SOBJECTMANAGERDB_H

#include <set>
#include <map>
#include "ComponentEnums.h"
#include "MessageEnums.h"

class IComponent;

typedef std::map<CObjectId, IComponent*> CComponentMap;
typedef IComponent* (*ComponentCreationMethod)(void);
typedef bool (*ComponentDestructionMethod)(IComponent *);

struct SComponentTypeInfo
{
	ComponentCreationMethod     mCreationMethod;
	ComponentDestructionMethod  mDestructionMethod;
	CHash                       mTypeHash;
}; 

struct SObjectManagerDB
{
	//////////////////////////////////////////////////////////////////
	// Static component type data
	SComponentTypeInfo					mComponentTypeInfo[NUM_COMPONENT_TYPE_IDS];
	std::set<EComponentTypeId>			mInterfaceTypeToComponentTypes[NUM_INTERFACE_IDS];
	
	//////////////////////////////////////////////////////////////////
	// Dynamic component data
	std::map<CObjectId, IComponent*>	mComponentTypeToComponentMap[NUM_COMPONENT_TYPE_IDS];
	
	//////////////////////////////////////////////////////////////////
	// Message data
	std::set<EComponentTypeId>			mMessageTypeToComponentTypes[NUM_MESSAGE_TYPES];
};

#endif //__SOBJECTMANAGERDB_H