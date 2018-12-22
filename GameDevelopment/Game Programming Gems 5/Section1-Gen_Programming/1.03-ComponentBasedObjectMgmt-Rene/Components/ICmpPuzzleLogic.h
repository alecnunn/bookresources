#ifndef __ICMPPuzzleLogic_H
#define __ICMPPuzzleLogic_H

#include "IComponent.h"
class CString;

class ICmpPuzzleLogic : public IComponent
{
public:
	ICmpPuzzleLogic()							{}
	virtual ~ICmpPuzzleLogic()					{}
	
	virtual CHash	GetState() = 0;
	virtual bool	SetState(CHash targetState, bool silent) = 0;
	virtual bool	Use() = 0;
	virtual bool	UseWith(CObjectId userId, CHash object) = 0;
protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPPuzzleLogic_H