#ifndef __ICOMPONENT_H
#define __ICOMPONENT_H

#include "types.h"
#include "ComponentEnums.h"
#include "MessageEnums.h"
#include "CObjectId.h"

// Forward declarations
class CParameterNode;
class CMessage;
class ICmpEntity;
class CObjectManager;

// The IComponent class. All our component classes inherit from this
class IComponent
{
public:
	IComponent();
	virtual ~IComponent();
	virtual bool    Init(CObjectId, CParameterNode &) = 0;
	virtual void    Deinit(void) = 0;
	CObjectId       GetObjectId(void) const     { return mObjectId; }
	ICmpEntity      *GetEntity() const;
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void) = 0;
private:
	void            SetObjectId(CObjectId oId)  { mObjectId = oId; }
	CObjectId       mObjectId;

	friend CObjectManager;
};
#endif //__ICOMPONENT_H