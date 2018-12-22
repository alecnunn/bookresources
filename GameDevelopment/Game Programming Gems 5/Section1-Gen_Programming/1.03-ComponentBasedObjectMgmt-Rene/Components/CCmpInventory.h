#ifndef __CCMPInventory_H
#define __CCMPInventory_H

#include "ICmpInventory.h"
#include <set>

class CCmpInventory : public ICmpInventory
{
public:
	CCmpInventory();
	virtual ~CCmpInventory();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	// ICmpInventory methods
	virtual int32		GetNumItems() const;
	virtual CObjectId	GetItem(int index) const;
	virtual bool		IsItemInInventory(CObjectId itemId) const;

protected:
	bool	InsertItem(CObjectId itemId);
	bool	RemoveItem(CObjectId itemId);
	void	ExamineInventory() const;
	void	SetItemPositions(CHash newPos);
private:
	typedef std::set<CObjectId> InventorySet;
	InventorySet mInventorySet;
};

#endif //__CCMPInventory_H