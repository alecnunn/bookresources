#ifndef __ICMPPlayer_H
#define __ICMPPlayer_H

#include "IComponent.h"

class CVector;

class ICmpPlayer : public IComponent
{
public:
	ICmpPlayer()							{}
	virtual ~ICmpPlayer()					{}

protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPPlayer_H