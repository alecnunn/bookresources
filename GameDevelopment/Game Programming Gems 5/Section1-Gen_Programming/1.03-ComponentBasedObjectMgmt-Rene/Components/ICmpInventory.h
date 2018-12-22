#ifndef __ICMPInventory_H
#define __ICMPInventory_H

#include "IComponent.h"

class ICmpInventory : public IComponent
{
public:
	ICmpInventory()							{}
	virtual ~ICmpInventory()					{}

	virtual int32		GetNumItems() const = 0;
	virtual CObjectId	GetItem(int index) const = 0;
	virtual bool		IsItemInInventory(CObjectId itemId) const = 0;

protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPInventory_H