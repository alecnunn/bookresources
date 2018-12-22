#ifndef __CCMPENTITY_H
#define __CCMPENTITY_H

#include "ICmpEntity.h"
#include "CVector.h"
#include <set>

class CCmpEntity : public ICmpEntity
{
public:
	CCmpEntity();
	virtual ~CCmpEntity();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	// ICmpEntity methods
	virtual CHash	GetPosition() const;
	virtual void	SetPosition(CHash room);
	virtual bool	GetVisible() const;
	virtual void	SetVisible(bool visible);
	// Adds a name this object can be referred to as from text input
	virtual bool	AddInteractionName(CHash name);
	// Is name one of the interaction names of this object?
	virtual bool	IsInteractionName(CHash name) const;
	// Tests whether this object can be seen by the viewer taking its position, and whether this object is
	// in an inventory into account.
	virtual bool	CanThisObjectBeSeenBy(CObjectId viewer, bool includeInventoryObject) const;
private:
	CHash	mRoom;
	bool	mVisible;

	typedef std::set<CHash>		InteractionNameSet;
	InteractionNameSet			mInteractionNameSet;

};

#endif //__CCMPENTITY_H