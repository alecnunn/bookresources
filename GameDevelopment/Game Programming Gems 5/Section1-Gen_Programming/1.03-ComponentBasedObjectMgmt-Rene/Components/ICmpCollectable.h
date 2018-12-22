#ifndef __ICMPCollectable_H
#define __ICMPCollectable_H

#include "IComponent.h"

class CVector;

class ICmpCollectable : public IComponent
{
public:
	ICmpCollectable()							{}
	virtual ~ICmpCollectable()					{}

	virtual bool		IsCollected() const = 0;
	virtual CObjectId	GetHolder() const = 0;

protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPCollectable_H