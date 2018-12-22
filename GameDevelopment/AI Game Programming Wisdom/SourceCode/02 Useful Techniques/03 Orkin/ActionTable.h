#ifndef ActionTable_H
#define ActionTable_H

//
// This is an optimized version of the ActionTable.
// It uses one multimap rather than nested maps.
//

#pragma warning(disable:4786)
#include <Map>

#define MAX_PATH 80

#define CREATE_KEY(condition, action) (condition << 16) | action


enum EnumAction
{
	kAct_Invalid	= -1,
	kAct_Default	=  0,
	kAct_Idle,
	kAct_Walk,
	kAct_Run,
	kAct_Attack,
};

enum EnumAnimCondition
{
	kACond_Invalid	= -1,
	kACond_Default	=  0,
	kACond_OneHanded,
	kACond_TwoHanded,
	kACond_Bow,
	kACond_Staff,
};

enum EnumActionDescriptor
{
	kADesc_Invalid	= -1,
	kADesc_None		=  0,
	kADesc_Swing,
	kADesc_Jab,
};

// --------------------------------------------------------------------------

struct ActionAnimInfoStruct
{
	char					szAnimFileName[MAX_PATH];
	EnumActionDescriptor	eActionDesc;
};

// --------------------------------------------------------------------------

typedef	std::multimap<unsigned long, ActionAnimInfoStruct> CONDITION_ACTION_MAP;	// <ActionConditionKey, (szAnimFileName, eActionDesc)>

// --------------------------------------------------------------------------

class CActionTable {

public:
	
	void		Read(void);

	const char*	GetAnimation(EnumAnimCondition eAnimCond, EnumAction eAction, EnumActionDescriptor* peActionDesc);

protected:

	CONDITION_ACTION_MAP	m_condActionMap;
};

// --------------------------------------------------------------------------

#endif // ActionTable_H

