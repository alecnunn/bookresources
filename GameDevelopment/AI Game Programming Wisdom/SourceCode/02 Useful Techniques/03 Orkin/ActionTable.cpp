#include "ActionTable.h"

//
// This is an optimized version of the ActionTable.
// It uses one multimap rather than nested maps.
//

// --------------------------------------------------------------------------

const char* CActionTable::GetAnimation(EnumAnimCondition eAnimCond, EnumAction eAction, EnumActionDescriptor* peActionDesc)
{
	unsigned long key = CREATE_KEY(eAnimCond, eAction);

	CONDITION_ACTION_MAP::iterator ca_it;
	ca_it = m_condActionMap.find(key);

	// Get list of actions for this animation condition.
	if(ca_it != m_condActionMap.end())
	{
		ActionAnimInfoStruct* pAnimInfoStruct = NULL;

		// Get number of animations listed for this action.
		long nCount = m_condActionMap.count(key);
	
		// If only one action is listed, return the animation.
		if(nCount == 1)
		{
			pAnimInfoStruct = &(ca_it->second);

			if(peActionDesc != NULL)
			{
				*peActionDesc = pAnimInfoStruct->eActionDesc;
			}
			return pAnimInfoStruct->szAnimFileName;
		}

		// Pick randomly from a list of animations for this action.
		else if(nCount > 1)
		{
			long nIndex = (long) ( ( (float)rand() / (float)RAND_MAX ) * (float)nCount );
			for(long i=0; i<nIndex; ++i, ++ca_it);
			pAnimInfoStruct = &(ca_it->second);

			if(peActionDesc != NULL)
			{
				*peActionDesc = pAnimInfoStruct->eActionDesc;
			}
			return pAnimInfoStruct->szAnimFileName;
		}
	}

	// No animation was found for the specified eAnimCond and eAction,
	// so see if a default animation exists.
	if(eAnimCond != kACond_Default)
	{
		return GetAnimation(kACond_Default, eAction, peActionDesc);
	}

	return NULL;
}

// --------------------------------------------------------------------------

void CActionTable::Read() 
{
	// Write code to read the action table data in from a file here.
	// For simplification, I'm hardcoding some entries.

	//
	// ---- BEGIN STUB CODE -------
	//

	unsigned long key;
	ActionAnimInfoStruct aaiStruct;

	// Create some default animations.

	aaiStruct.eActionDesc = kADesc_None;

	strcpy(aaiStruct.szAnimFileName, "default_idle_scratchhead.anm");
	key = CREATE_KEY(kACond_Default, kAct_Idle);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "default_idle_lookaround.anm");
	key = CREATE_KEY(kACond_Default, kAct_Idle);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "default_idle_flex.anm");
	key = CREATE_KEY(kACond_Default, kAct_Idle);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "default_walk.anm");
	key = CREATE_KEY(kACond_Default, kAct_Walk);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "default_run.anm");
	key = CREATE_KEY(kACond_Default, kAct_Run);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "default_attack.anm");
	key = CREATE_KEY(kACond_Default, kAct_Attack);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );


	// Create some one-handed weapon animations.

	aaiStruct.eActionDesc = kADesc_None;

	strcpy(aaiStruct.szAnimFileName, "one_handed_idle.anm");
	key = CREATE_KEY(kACond_OneHanded, kAct_Idle);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "one_handed_walk.anm");
	key = CREATE_KEY(kACond_OneHanded, kAct_Walk);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "one_handed_run.anm");
	key = CREATE_KEY(kACond_OneHanded, kAct_Run);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	aaiStruct.eActionDesc = kADesc_Swing;
	strcpy(aaiStruct.szAnimFileName, "one_handed_attack_swing.anm");
	key = CREATE_KEY(kACond_OneHanded, kAct_Attack);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	aaiStruct.eActionDesc = kADesc_Jab;
	strcpy(aaiStruct.szAnimFileName, "one_handed_attack_jab.anm");
	key = CREATE_KEY(kACond_OneHanded, kAct_Attack);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );


	// Create some two-handed weapon animations.

	aaiStruct.eActionDesc = kADesc_None;

	strcpy(aaiStruct.szAnimFileName, "two_handed_idle.anm");
	key = CREATE_KEY(kACond_TwoHanded, kAct_Idle);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "two_handed_walk.anm");
	key = CREATE_KEY(kACond_TwoHanded, kAct_Walk);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	strcpy(aaiStruct.szAnimFileName, "two_handed_run.anm");
	key = CREATE_KEY(kACond_TwoHanded, kAct_Run);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	aaiStruct.eActionDesc = kADesc_Swing;
	strcpy(aaiStruct.szAnimFileName, "two_handed_attack_swing.anm");
	key = CREATE_KEY(kACond_TwoHanded, kAct_Attack);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	aaiStruct.eActionDesc = kADesc_Jab;
	strcpy(aaiStruct.szAnimFileName, "two_handed_attack_jab.anm");
	key = CREATE_KEY(kACond_TwoHanded, kAct_Attack);
	m_condActionMap.insert( CONDITION_ACTION_MAP::value_type( key, aaiStruct) );

	//
	// ---- END STUB CODE -------
	//
}

// --------------------------------------------------------------------------
