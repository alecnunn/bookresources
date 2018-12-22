#ifndef __ICMPENTITY_H
#define __ICMPENTITY_H

#include "IComponent.h"

class CVector;

class ICmpEntity : public IComponent
{
public:
	ICmpEntity()							{}
	virtual ~ICmpEntity()					{}

	virtual CHash	GetPosition() const = 0;
	virtual void	SetPosition(CHash room) = 0;
	virtual bool	GetVisible() const = 0;
	virtual void	SetVisible(bool visible) = 0;
	virtual bool	AddInteractionName(CHash name) = 0;
	virtual bool	IsInteractionName(CHash name) const = 0;
	virtual bool	CanThisObjectBeSeenBy(CObjectId viewer, bool includeInventoryObject) const = 0;
protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPENTITY_H