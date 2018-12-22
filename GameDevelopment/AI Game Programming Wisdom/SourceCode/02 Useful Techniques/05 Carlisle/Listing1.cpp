/* Copyright (C) Phil Carlisle, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Phil Carlisle, 2001"
 */
CreatureMachine::CreatureMachine()
{
	// add all variables to the variable list
	AddVariable("Health",VAR_INTEGER,&m_Health);
	AddVariable("EnemyLocation",VAR_POSITION,&m_Enemy;
	
	// now add conditions (may reference variables)
	AddCondition("InRange",LESSTHAN,"EnemyLocation",100);
	AddCondition("LowHealth",LESSTHAN,"Health", 50);
	
	// now add all the actions (may be referenced by the // states)	
	AddAction("IdleAction");
	AddAction("FightAction");
	AddAction("FleeAction");
	
	// now add all the states
	AddState("Idle","IdleAction");
	AddState("Fight","FightAction");
	AddState("Flee","FleeAction");

	// now add all the transitions (may reference states 
	// and variables)
	// transitions syntax : <condition-name> <start state>
	// <end state>
	AddTransition("In Range","Idle","Fight");
	AddTransition("Low Health","Fight","Flee");

};
