#include "ActionTable.h"

void main()
{
	// Sample usage of the action table.

	CActionTable myActionTable;
	myActionTable.Read();

	const char* szAnimFileName;

	// Play an idle animation for the default condition.
	// The action table will randomly choose between the idle animations:
	// "default_idle_scratchhead.anm", "default_idle_lookaround.anm", and "default_idle_flex".

	szAnimFileName = myActionTable.GetAnimation(kACond_Default, kAct_Idle, NULL);
	szAnimFileName = myActionTable.GetAnimation(kACond_Default, kAct_Idle, NULL);
	szAnimFileName = myActionTable.GetAnimation(kACond_Default, kAct_Idle, NULL);
	szAnimFileName = myActionTable.GetAnimation(kACond_Default, kAct_Idle, NULL);

	// Play a run animation for the two-handed weapon condition.
	// The action table will choose the only available animation:
	// "two_handed_run.anm".

	szAnimFileName = myActionTable.GetAnimation(kACond_TwoHanded, kAct_Run, NULL);

	// Play an attack animation for the one-handed weapon condition.
	// The action table will randomly choose between the attack animations:
	// "one_handed_attack_swing.anm", and "one_handed_attack_jab".
	// The action descriptor can be checked in code to determine which type
	// of attack was chosen.

	EnumActionDescriptor eActDesc;
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);
	szAnimFileName = myActionTable.GetAnimation(kACond_OneHanded, kAct_Attack, &eActDesc);

	// Play a walk animation for the bow weapon condition.
	// There are no bow animations, so the action table will choose the default walk animation:
	// "default_walk.anm".

	szAnimFileName = myActionTable.GetAnimation(kACond_Bow, kAct_Walk, NULL);
}
