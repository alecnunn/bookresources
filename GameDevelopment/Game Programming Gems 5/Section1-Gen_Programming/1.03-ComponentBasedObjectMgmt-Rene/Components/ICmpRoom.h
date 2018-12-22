#ifndef __ICMPROOM_H
#define __ICMPROOM_H

#include "IComponent.h"

class ICmpRoom : public IComponent
{
public:
	ICmpRoom()					{}
	virtual ~ICmpRoom()			{}

	virtual bool		AddConnectedRoom(CObjectId room) = 0;
	virtual bool		IsRoomConnected(CObjectId room) const = 0;
	virtual CObjectId	GetConnectedRoom(CHash interactionName) const = 0;
protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPROOM_H