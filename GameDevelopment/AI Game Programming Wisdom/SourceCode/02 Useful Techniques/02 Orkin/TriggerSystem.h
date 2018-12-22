#ifndef __TRIGGER_SYSTEM_H__
#define __TRIGGER_SYSTEM_H__

#include "Vector.h"

#pragma warning(disable:4786)
#include <Map>


enum EnumTriggerType
{
	kTrig_None			= 0,
	kTrig_Explosion		= (1 << 0),
	kTrig_EnemyNear		= (1 << 1),
	kTrig_Gunfire		= (1 << 2),
};


struct TriggerRecordStruct
{
	TriggerRecordStruct(EnumTriggerType _eTriggerType, 
		unsigned long _idSource, const Vector& _vPos, 
		float _fRadius, float _fDuration, bool _bDynamicSourcePos);

	EnumTriggerType		eTriggerType;		// Type of trigger.
	unsigned long		nTriggerID;			// Registration ID assigned by the TriggerSystem.
	unsigned long		idSource;			// ID of source of trigger.
	Vector				vPos;				// Position in world of trigger.
	float				fRadius;			// Distance from source that trigger affects.
	unsigned long		nTimeStamp;			// When trigger occured.
	unsigned long		nExpirationTime;	// When trigger expires.
	bool				bDynamicSourcePos;	// Lookup position of trigger source every update.

	static unsigned long s_nNextTriggerID;	// Next unique Registration ID for trigger.
};


typedef std::multimap<unsigned short /*nPriority*/, TriggerRecordStruct*, std::greater<unsigned short> > TRIGGER_MAP;


class CTriggerSystem
{
	public :

		 CTriggerSystem();
		~CTriggerSystem();

		unsigned long	RegisterTrigger(EnumTriggerType _eTriggerType, unsigned long _nPriority,
										unsigned long _idSource, const Vector& _vPos, float _fRadius, 
										float _fDuration, bool _bDynamicSourcePos);

		void	RemoveTrigger(unsigned long nTriggerID);

		void	Update();

	private : 

		TRIGGER_MAP		m_mapTriggerMap;	// List of existing triggers, sorted by priority.

		bool			m_bTriggerCriticalSection;
};

#endif
