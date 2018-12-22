#ifndef __CCMPActor_H
#define __CCMPActor_H

#include "ICmpActor.h"
#include "CVector.h"

struct EventInfo;

class CCmpActor : public ICmpActor
{
public:
	CCmpActor();
	virtual ~CCmpActor();

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
	// Examines the character more closely
	void DescribeCharacter(CObjectId viewer) const;
	void HandleEvent(EventInfo &evInfo);
	// This is called on the regular MT_UPDATE tick.
	void Update();
	// Tells any player object in the same room as the actor the message
	void TellRoom(const char *pMsg);
	// Sets the actors position to room, and tells the old room that it's leaving and the new room that it's entering.
	void GotoRoom(CObjectId room);

private:
	// These would all live in script if this wasn't just a demo (that lacks a scripting language).
	enum EBossState	{ IN_OFFICE, ON_WAY_TO_COOLER, DRINK, ON_WAY_TO_THERMOSTAT, SET_THERMOSTAT, ON_WAY_BACK_THROUGH_CORRIDOR, ON_WAY_TO_OFFICE };
	EBossState	mBossState;
	uint32		mNextStateTime;
	bool		mThermostatHot;
};

#endif //__CCMPActor_H