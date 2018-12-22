#ifndef __COMPONENTSTRUCTS_H
#define __COMPONENTSTRUCTS_H

#include "CHash.h"

// The following structs are useful for data passed along with messages

struct ExamineInfo
{
	CHash		examineObjectInteractionName; // Interaction name of object to be examined
	CObjectId	userId; // Object id of the examiner
};

struct UseInfo
{
	CHash useObjectInteractionName;	// Interaction name of object to be used.
	// e.g. use _thermostat_ ; use _key_ with lock
	CHash useWithInteractionName;	// Interaction name of object the above object is used with
	// e.g. use key with _lock_ ; use shark repellant with _shark_
	CHash userId;					// The object id of the object using the useObject (potentially with the useWith object) ;-)
};

struct PickupInfo
{
	CHash	collectorId;						// The object doing the collecting
	CHash	collectedObjectInteractionName;		// The interaction name of the object being collected
};

struct EventInfo
{
	EventInfo() {}
	EventInfo(CHash eventName) : mEventName(eventName) {}
	EventInfo(CHash eventName, CObjectId targetId)  : mEventName(eventName), mTargetId(targetId) {}
	EventInfo(CHash eventName, CObjectId targetId, CObjectId actorId)  : mEventName(eventName), mTargetId(targetId), mActorId(actorId) {}
	CHash		mEventName;		
	CObjectId	mTargetId;		// The object that was affected by this event
	CObjectId	mActorId;		// The object that caused this event
};

struct TellRoomInfo
{
	CObjectId	room;
	const char *pMessage;
};


#endif //__COMPONENTSTRUCTS_H