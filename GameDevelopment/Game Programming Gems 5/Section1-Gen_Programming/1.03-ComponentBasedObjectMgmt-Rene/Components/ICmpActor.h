#ifndef __ICMPActor_H
#define __ICMPActor_H

#include "IComponent.h"

class CVector;

class ICmpActor : public IComponent
{
public:
	ICmpActor()							{}
	virtual ~ICmpActor()					{}


protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPActor_H