#include "Agent.h"
#include "TriggerSystem.h"


CAgent::CAgent()
{
	m_fTriggerDistance = 50.f;

	// Update 15 times per second.
	m_fTriggerUpdateRate = 0.066f; 
	
	m_lastTriggerTime = 0;
}

bool CAgent::HandleTrigger(TriggerRecordStruct* pRecord)
{ 
	// Only handle more recent triggers.
	if(pRecord->nTimeStamp <= m_lastTriggerTime)
	{
		return false;
	}

	switch(pRecord->eTriggerType)
	{
		case kTrig_Explosion:	printf("Enemy%u handling trigger: Explosion\n", m_id);
								break;
		case kTrig_EnemyNear:	printf("Enemy%u handling trigger: EnemyNear\n", m_id);
								break;
		case kTrig_Gunfire:		printf("Enemy%u handling trigger: Gunfire\n", m_id);
								break;
	}

	m_lastTriggerTime = pRecord->nTimeStamp;

	return true; 
}
