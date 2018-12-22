#include "TriggerSystem.h"
#include "Agent.h"
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <assert.h>


#define DIST(a, b) sqrt( (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.z - a.z)*(b.z - a.z) )

unsigned long TriggerRecordStruct::s_nNextTriggerID = 1;

// -----------------------------------------------------------------------

TriggerRecordStruct::TriggerRecordStruct(EnumTriggerType _eTriggerType, 
		unsigned long _idSource, const Vector& _vPos, 
		float _fRadius, float _fDuration, bool _bDynamicSourcePos)
{
	eTriggerType		= _eTriggerType;
	idSource			= _idSource;
	vPos				= _vPos;
	fRadius				= _fRadius;
	nTimeStamp			= timeGetTime();
	bDynamicSourcePos	= _bDynamicSourcePos;

	// Triggers with duration of 0 do not ever expire.
	if(_fDuration != 0.f)
	{
		nExpirationTime = nTimeStamp + (unsigned long)(_fDuration * 1000.f);
	}
	else {
		nExpirationTime = 0;
	}

	// Keep a unique identifier for all triggers.
	nTriggerID = s_nNextTriggerID++;
}

// -----------------------------------------------------------------------

CTriggerSystem::CTriggerSystem()
{
	m_bTriggerCriticalSection = false;
}

CTriggerSystem::~CTriggerSystem()
{
}

// -----------------------------------------------------------------------

unsigned long CTriggerSystem::RegisterTrigger(EnumTriggerType _eTriggerType, unsigned long _nPriority,
									   unsigned long _idSource, const Vector& _vPos, float _fRadius, 
									   float _fDuration, bool _bDynamicSourcePos)
{
	// Create a trigger record, and fill it in.
	TriggerRecordStruct* pTriggerRecord = 
		new	TriggerRecordStruct(_eTriggerType, _idSource, _vPos, 
								_fRadius, _fDuration, _bDynamicSourcePos);

	// Trigger records are sorted by priority.
	m_mapTriggerMap.insert( TRIGGER_MAP::value_type(_nPriority, pTriggerRecord) );

	return pTriggerRecord->nTriggerID;
}

// -----------------------------------------------------------------------

void CTriggerSystem::RemoveTrigger(unsigned long nTriggerID)
{
	assert(m_bTriggerCriticalSection == false);

	TRIGGER_MAP::iterator it = m_mapTriggerMap.begin();
	while( it != m_mapTriggerMap.end() )
	{
		if( it->second->nTriggerID == nTriggerID )
		{
			delete(it->second);
			return;
		}
		else ++it;
	}
}

// ----------------------------------------------------------------------- 

void CTriggerSystem::Update()
{
    CAgent* pAgent	= NULL;
	float fDistance	= 0.f;

	TriggerRecordStruct* pRecord;
	TRIGGER_MAP::iterator it;

	unsigned long nCurTime = timeGetTime();

	//
	// Delete expired trigger records.
	// Records with expiration time 0 never expire.
	//
	// For records that are not expired, update position
	// if the dynamic flag is set.
	//
	it = m_mapTriggerMap.begin();
	while( it != m_mapTriggerMap.end() )
	{
		pRecord = it->second;
		if( (pRecord->nExpirationTime != 0) && (pRecord->nExpirationTime < nCurTime) )
		{
			delete(pRecord);
			it = m_mapTriggerMap.erase(it);
		}
		else {
			// Update position if dynamic flag is set.
			// Reset time-stamp.
			if(pRecord->bDynamicSourcePos == true)
			{
				pRecord->vPos = g_pAgentList[pRecord->idSource]->GetPosition();
				pRecord->nTimeStamp = nCurTime;
			}

			++it;
		}		
	}


	//
	// Trigger Agents.
	//

	// Make sure triggers are not getting removed while in this loop.
	m_bTriggerCriticalSection = true;

	// Loop thru agents.
	for(unsigned long iAgent=0; iAgent < g_nNumAgents; ++iAgent)
	{
		pAgent = g_pAgentList[iAgent];

		// Check if it's time for this Agent to update.
		if( (pAgent->GetTriggerUpdateRate() > 0.f) && (nCurTime > pAgent->GetNextTriggerUpdate()) )
		{
			pAgent->SetNextTriggerUpdate(nCurTime + (unsigned long)(pAgent->GetTriggerUpdateRate() * 1000.f));

			// Loop through existing trigger records.
			for(it = m_mapTriggerMap.begin(); it != m_mapTriggerMap.end(); ++it)
			{
				pRecord = it->second;

				// Check if this Agent responds to this trigger.
				if( !(pRecord->eTriggerType & pAgent->GetTriggerFlags()) )
					continue;
				
				// Check that trigger source is not the Agent itself.
				if( pRecord->idSource == iAgent)
					continue;

				// Check radius.
				fDistance = DIST(pRecord->vPos, pAgent->GetPosition());
				if(fDistance > (pAgent->GetTriggerDistance() + pRecord->fRadius) )
					continue;
				
				// HandleTrigger returns true if the Agent responded to the trigger.
				if( pAgent->HandleTrigger(pRecord) )
				{
					// Only pay attention to the highest priority 
					// trigger at any one instant.
					break;
				}
			}
		}
	}

	m_bTriggerCriticalSection = false;
}

