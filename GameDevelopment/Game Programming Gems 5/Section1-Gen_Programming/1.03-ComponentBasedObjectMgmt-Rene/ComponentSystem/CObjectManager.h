#ifndef __COBJECTMANAGER_H
#define __COBJECTMANAGER_H

#include "types.h"
#include "MessageEnums.h"
#include "ComponentEnums.h"
#include "CObjectId.h"

class IComponent;
class CMessage;
class CParameterNode;
class CComponentByInterfaceIterator;
class CComponentByTypeIterator;
class CComponentByObjectIterator;
class CPostMessageIterator;
class CBroadcastMessageIterator;
struct SObjectManagerDB;

typedef IComponent* (*ComponentCreationMethod)(void);
typedef bool (*ComponentDestructionMethod)(IComponent *);

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	void Init(void);
	void DeInit(void);

	bool	LoadObjectsFromFile(const char *fileName);
	bool	LoadObjects(CParameterNode *);

	// If an interface is registered with an object, returns a pointer to the component
	// that implements it
	IComponent *QueryInterface(CObjectId, EInterfaceId);
	IComponent *CreateComponent(CObjectId, CParameterNode *);

	// Creates an object, using the name it was given in the parameter node as the object id
	CObjectId	CreateObject(CParameterNode *);
	// Does exactly what it says on the tin.
	void		DestroyAllComponents();

	// The following method get iterators that iterate over all the components that fulfill the criteria passed into the method.
	CComponentByInterfaceIterator	GetComponentsByInterfaceId(EInterfaceId);
	CComponentByTypeIterator		GetComponentsByComponentTypeId(EComponentTypeId);
	CComponentByObjectIterator		GetComponentsByObjectId(CObjectId);

	// This needs to be called once for each component type. This sets up the class factory that creates a component instance
	void			RegisterComponentType(EComponentTypeId, ComponentCreationMethod, ComponentDestructionMethod, CHash typeHash);
	// This lets the object manager know that the given component type implements the interface of the given interface type
	void			RegisterInterfaceWithComponent(EInterfaceId, EComponentTypeId);

	// Sets up a subscription to a particular message type for the given component type. Whenever the message gets sent, it will
	// go out to any components of the types that have subscribed to the message type.
	void			SubscribeToMessageType(EComponentTypeId, EMessageType);
	// Sends a message to all subscribing components of one object (as defined by the first parameter).
	void			PostMessage(CObjectId oid, const CMessage &msg);
	void			PostMessage(CObjectId, CMessage &);
	// Sends a message to all subscribing components of all objects
	void			BroadcastMessage(const CMessage &msg);
	void			BroadcastMessage(CMessage &);

	EComponentTypeId	GetComponentTypeFromHash(CHash typeName) const;
	// Goes through all the interfaces implemented by the component and finds out if the interface id passed in is one of them.
	bool				IsInterfaceImplementedByComponent(EInterfaceId, IComponent *) const;
private:
	void		RegisterAllComponentTypes(void);
	bool		AddComponentToDb(CObjectId, IComponent*);

	// Creates an object using the object id passed in as a parameter
	CObjectId	CreateObject(CParameterNode *, CObjectId);

	// Checks for the existence of an object (one or more component instances with the given object id)
	bool		ObjectExists(CObjectId oId) const;

	// Removes the component from the database. Does not delete the component.
	bool		RemoveComponentFromDb(CObjectId oId, IComponent* pComp);
private:
	SObjectManagerDB	*mDB;

	friend CComponentByTypeIterator;
	friend CComponentByInterfaceIterator;
	friend CComponentByObjectIterator;
	friend CPostMessageIterator;
	friend CBroadcastMessageIterator;
};

#endif //__COBJECTMANAGER_H