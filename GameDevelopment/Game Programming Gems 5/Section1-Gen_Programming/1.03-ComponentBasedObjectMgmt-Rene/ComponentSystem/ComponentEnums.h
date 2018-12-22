#ifndef __COMPONENTENUMS_H
#define __COMPONENTENUMS_H

// All the component types available to the game. Any new component type needs a corresponding enum value in here
enum EComponentTypeId
{
	CID_ENTITY = 0,
	CID_ROOM,
	CID_ACTOR,
	CID_COLLECTABLE,
	CID_INVENTORY,
	CID_PLAYER,
	CID_PUZZLE_LOGIC,
	CID_DESCRIPTION,
	NUM_COMPONENT_TYPE_IDS,
	CID_INVALID
};

// All the interface types available to the game. Any new interface type needs a corresponding enum value in here
enum EInterfaceId
{
	IID_ENTITY = 0,
	IID_ROOM,
	IID_ACTOR,
	IID_COLLECTABLE,
	IID_INVENTORY,
	IID_PLAYER,
	IID_PUZZLE_LOGIC,
	IID_DESCRIPTION,
	NUM_INTERFACE_IDS
};

#endif //__COMPONENTENUMS_H