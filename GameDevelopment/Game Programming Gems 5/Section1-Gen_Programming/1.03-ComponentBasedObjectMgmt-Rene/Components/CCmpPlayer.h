#ifndef __CCMPPlayer_H
#define __CCMPPlayer_H

#include "ICmpPlayer.h"
#include "CVector.h"

class CCmpPlayer : public ICmpPlayer
{
public:
	CCmpPlayer();
	virtual ~CCmpPlayer();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

protected:
	bool	HandleCommand(const char *commandString);
};

#endif //__CCMPPlayer_H