#ifndef __CCMPCollectable_H
#define __CCMPCollectable_H

#include "ICmpCollectable.h"
#include "CVector.h"

class CCmpCollectable : public ICmpCollectable
{
public:
	CCmpCollectable();
	virtual ~CCmpCollectable();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	// ICmpCollectable methods
	virtual bool		IsCollected() const;
	virtual CObjectId	GetHolder() const;

private:
	CObjectId	mHolder;
};

#endif //__CCMPCollectable_H