#ifndef __CCMPROOM_H
#define __CCMPROOM_H

#include "ICmpRoom.h"
#include <set>

class CCmpRoom : public ICmpRoom
{
public:
	CCmpRoom();
	virtual ~CCmpRoom();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool				Init(CObjectId, CParameterNode &);
	virtual void				Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	virtual bool		AddConnectedRoom(CObjectId room);
	virtual bool		IsRoomConnected(CObjectId room) const;
	virtual CObjectId	GetConnectedRoom(CHash interactionName) const;

protected:
	void				PrintLookDescription();

protected:
	typedef std::set<CObjectId> ConnectedRoomSet;
	ConnectedRoomSet			mConnectedRoomsSet;
};

#endif //__CCMPROOM_H