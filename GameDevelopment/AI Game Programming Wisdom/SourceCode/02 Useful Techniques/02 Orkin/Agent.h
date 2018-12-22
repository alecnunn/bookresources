#ifndef __AGENT_H__
#define __AGENT_H__

#pragma warning(disable:4786)
#include "Vector.h"

struct TriggerRecordStruct;


class CAgent 
{
public:

	CAgent();

	const Vector&		GetPosition() const { return m_vPos; }
	const unsigned long	GetTriggerFlags() const { return m_flagTriggerFlags; }
	const float			GetTriggerUpdateRate() const { return m_fTriggerUpdateRate; }
	const float			GetTriggerDistance() const { return m_fTriggerDistance; }
	const unsigned long	GetNextTriggerUpdate() const { return m_nNextTriggerUpdate; }

	void				SetID(unsigned long id) { m_id = id; }
	void				SetPosition(Vector& vPos) { m_vPos = vPos; }
	void				SetTriggerFlags(unsigned long flags) { m_flagTriggerFlags = flags; }
	void				SetNextTriggerUpdate(unsigned long nNextTriggerUpdate) { m_nNextTriggerUpdate = nNextTriggerUpdate; }

	virtual bool		HandleTrigger(TriggerRecordStruct* pRecord);

protected:

	unsigned long	m_id;
	Vector			m_vPos;
	float			m_fTriggerUpdateRate; 
	unsigned long	m_nNextTriggerUpdate;
	unsigned long	m_lastTriggerTime;
	float			m_fTriggerDistance;
	unsigned long	m_flagTriggerFlags;
};

extern CAgent* g_pAgentList[100];
extern unsigned long g_nNumAgents;

#endif
