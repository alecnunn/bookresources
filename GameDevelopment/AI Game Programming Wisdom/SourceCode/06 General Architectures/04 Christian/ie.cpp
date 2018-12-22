
#include "ie.h"
#include "iegoal.h"
#include "ieexec.h"
#include "ierule.h"

#include <assert.h>

static bool dbg = false;

//===========================================================================
// Name: 
// start 
//
// Description: 
// Start an inference engine goal set.
//
// Parameters: 
// name - The name of the goal set.
// 
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::start ( char * name, IEOwner * owner )
{
    m_name      = name;
    m_owner     = owner;
    m_curGoal   = NULL;
    m_curRule   = NULL;
    m_nextGoal  = NULL;
}

//===========================================================================
// Name: 
// end 
//
// Description: 
// End the goal set. Link up the goal pointers in the rules.
//
// Parameters: 
// None.
//
// Return Value: 
// None.
//
// Remarks: 
// 
//===========================================================================
void IE::end ()
{
    // link the rules to their goals via the goal name
    linkRuleGoals();

    // set current goal to the first goal
    m_curGoal = m_goalList.front();

	// push  goal on stack
	pushGoal ( m_curGoal );	

}

//===========================================================================
// Name: 
// makeGoal 
//
// Description: 
// Make a new goal and add it to the end of the goal list.
//
// Parameters: 
// name - The name of the goal.
//
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::makeGoal ( IEExec * exec )
{    
    if ( !exec )
        return;

    // new goal
    m_curGoal = new IEGoal ( exec );

    // initialize goal
    exec->init( m_owner );

    // add it to the end of the goal list
    m_goalList.push_back ( m_curGoal );
}

//===========================================================================
// Name: 
// makeRule 
//
// Description: 
// Make a new rule and add it to the current goal.
//
// Parameters: 
// condition - Pointer to the conditional object for the rule.
// lineInfo  - Description of the line the makeRule macro is on. (for debug)
//
// Return Value: 
// None.
//
// Remarks: 
// This method expects a 'current' goal to have been established by a call
// to makeGoal.
//===========================================================================
void IE::makeRule ( IEExec * condition, char * lineInfo )
{
    // make sure there is a current goal established
    if ( ! m_curGoal )
    {
        printf ( "ERROR: No goal specified for rule at %s.\n", lineInfo );
        return;
    }

    // check the type of the conditional object to make
    // sure it matches the type of the goal set
#if 0
    if ( condition->getType() != m_type )
    {
        printf ( "ERROR: Condition %s at %s invalid type of %d.\n",
            condition->getName(), lineInfo, condition->getType() );
        return;
    }
#endif

    // make a new rule
    m_curRule = new IERule( condition );

    // initialize rule condition exec
    condition->init( m_owner );

    // add the rule to the current goal
    m_curGoal->addRule ( m_curRule );
}

//===========================================================================
// Name: 
// addRuleGoto 
//
// Description: 
// Make the current rule a goto rule.
//
// Parameters: 
// goalName - The name of the goal to goto to.
// lineInfo - Debug info for the macro setting the goto portion. (debug)
//
// Return Value: 
// None.
//
// Remarks: 
// This method expects a current rule to have been established via makeRule.
//===========================================================================
void IE::addRuleGoto ( char * goalName, char * lineInfo )
{
    if ( noRule ( lineInfo ) )
        return;

    m_curRule->setGoal ( goalName, lineInfo );
    m_curRule->setType ( IERule::Goto );

    m_curRule->m_name = "If_";
    m_curRule->m_name += m_curRule->m_condition->getName();
    m_curRule->m_name += "_Goto_";
    m_curRule->m_name += goalName;
}

//===========================================================================
// Name: 
// addRuleGosub
//
// Description: 
// Make the current rule a gosub rule.
//
// Parameters: 
// goalName - The name of the goal to gosub.
// lineInfo - Debug info for the macro setting the gosub portion. (debug)
//
// Return Value: 
// None.
//
// Remarks: 
// This method expects a current rule to have been established via makeRule.
//===========================================================================
void IE::addRuleGosub ( char * goalName, char * lineInfo )
{
    if ( noRule ( lineInfo ) )
        return;

    m_curRule->setGoal ( goalName, lineInfo );
    m_curRule->setType ( IERule::Gosub );

    m_curRule->m_name = "If_";
    m_curRule->m_name += m_curRule->m_condition->getName();
    m_curRule->m_name += "_Gosub_";
    m_curRule->m_name += goalName;
}

//===========================================================================
// Name: 
// addRuleReturn 
//
// Description: 
// Make the current rule a return rule.
//
// Parameters: 
// lineInfo - Debug info for the macro setting the return portion. (debug)
//
// Return Value: 
// None.
//
// Remarks: 
// This method expects a current rule to have been established via makeRule.
// A goal name is not need because a return rule will return to the goal
// that transfered to the current goal, whether through a goto or gosub.
//===========================================================================
void IE::addRuleReturn ( char * lineInfo )
{
    if ( noRule ( lineInfo ) )
        return;

    m_curRule->setType ( IERule::Return );
    m_curRule->setGoal ( m_curGoal );

    m_curRule->m_name = "If_";
    m_curRule->m_name += m_curRule->m_condition->getName();
    m_curRule->m_name += "_Return";
}


//===========================================================================
// Name: 
// update 
//
// Description: 
// Main update for the inference engine. Processes goal start, finish, and
// rule sets. Pushs, Pops, and transfers goal control based on returns from
// rule conditionals.
//
// Parameters: 
// None.
//
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::update ()
{
	assert ( m_curGoal );

	// if at the start of the current goal
    checkStart();

	// if trying to finish the current goal
    if ( ! checkFinish() )
        return;

	// process all rules for all goals on the stack
    std::list<IEGoal *>::iterator goalItr;
    std::list<IERule *>::iterator ruleItr;

    IEGoal * goal;
    IERule * rule;

    // first, see if goal alread on the stack
    goalItr = m_goalStack.begin();

    bool ruleFired = false;

    while ( goalItr != m_goalStack.end() && ! ruleFired )
	{
        goal = *goalItr++;

        ruleItr = goal->m_rules.begin();

        while ( ruleItr != goal->m_rules.end() )
        {			
            rule = *ruleItr++;

            // if rule already fired then skip (for rules on stack)
            if ( rule->m_fired )
                continue;

            if ( dbg ) printf ( "Rule %s tested.\n", rule->getName() );

			if ( rule->m_condition->update() == true )
			{
                if ( dbg ) printf ( "\tFired.\n" );

                rule->m_fired = true;

				// get the goal for this rule
				m_nextGoal = rule->getGoal();
				assert( m_nextGoal );

                if ( dbg ) printf ( "\tNext goal %s.\n", m_nextGoal->getName() );

                // set finish for the current goal to allow
                // cleanup (call of finish())
				m_curGoal->setFinish( true );

				// pop current goal off the stack if not gosub rule,
				// next goal will be replacing it
                if ( rule->m_type != IERule::Gosub )
				    popGoal( m_curGoal );

                ruleFired = true;

                break;
			}
		}
	}

	// excute main goal update, unless rule fired
    if ( ! ruleFired )
    {
	    m_curGoal->update();
    }
    // go ahead and check finish this frame to possibly be ready for the next
    else
    {
        checkFinish();
    }

    if ( dbg ) printf ( "Goal %s updated.\n", m_curGoal->getName() );
}

//===========================================================================
// Name: 
// checkStart 
//
// Description: 
// Checks for start condition and performs start if it needs to.
//
// Parameters: 
// None.
//
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::checkStart ()
{
	if ( m_curGoal->atStart() )
	{
		// call start
		m_curGoal->start();

        m_curGoal->setStart ( false );

        if ( dbg ) printf ( "Goal %s start.\n", m_curGoal->getName() );

        m_curGoal->resetFiredRules();
	}
}

//===========================================================================
// Name: 
// checkFinish 
//
// Description: 
// Checks for finish condition
//
// Parameters: 
// None.
//
// Return Value: 
// Returns false if not finished, else true.
//
// Remarks: 
//
//===========================================================================
bool IE::checkFinish ()
{
	if ( m_curGoal->atFinish() )
	{
        if ( dbg ) printf ( "Goal %s at finish.\n", m_curGoal->getName() );

		// call finish method, if not finished
		// then return and process no more
		if ( m_curGoal->finish() == false )
        {
            if ( dbg ) printf ( "Goal %s net yet finished.\n", m_curGoal->getName() );
			return false;
        }

		// goal is finished, reset it
		m_curGoal->reset();

        // set last goal of next goal so it will know who 
        // transferred control to it
        m_nextGoal->m_lastGoal = m_curGoal;

        if ( dbg ) printf ( "Goal %s finished.\n", m_curGoal->getName() );

		// prepare next goal
		m_curGoal = m_nextGoal;

		// push new goal on stack, or if it is already
        // on the stack then simply set the stack to it
		pushGoal ( m_curGoal );	
	}

    return true;
}

//===========================================================================
// Name: 
// noRule 
//
// Description: 
// Tests for a current rule and prints error info if there is not one.
//
// Parameters: 
// lineInfo - Debug info about the current line of a macro.
//
// Return Value: 
// True if there is no current rule. False otherwise.
//
// Remarks: 
//
//===========================================================================
bool IE::noRule ( char * lineInfo )
{
    if ( ! m_curRule )
    {
        printf ( "ERROR: No rule specified at %s.\n", lineInfo );
        return true;
    }

    return false;
}

//===========================================================================
// Name: 
// linkRuleGoals 
//
// Description: 
// Links rules with the goals they are referencing via name. 
//
// Parameters: 
// None.
//
// Return Value: 
// None.
//
// Remarks: 
// If a rule refers to a goal that is not found then error info is printed
// about the line the rule macro was on.
//===========================================================================
void IE::linkRuleGoals ()
{
    std::list<IEGoal *>::iterator goalItr;
    std::list<IERule *>::iterator ruleItr;

    IEGoal * goal;
    IERule * rule;

    goalItr = m_goalList.begin();

    // iterate through goals
    while ( goalItr != m_goalList.end() )
    {
        goal = *goalItr++;

        // for every rule in the goal
        ruleItr = goal->m_rules.begin();

        while ( ruleItr != goal->m_rules.end() )
        {
            rule = *ruleItr++;

            // if there is a goal name then find it
            if ( rule->m_goalName.length() > 0 )
            {
                IEGoal * ruleGoal = findGoal ( rule->m_goalName );

                if ( ruleGoal == NULL )
                {
                    printf ( "ERROR: Goal %s at %s does not exist.\n",
                        rule->m_goalName, rule->m_lineInfo );                    
                }
                else
                {
                    rule->setGoal ( ruleGoal );
                }
            }
        }
    }
}

//===========================================================================
// Name: 
// findGoal 
//
// Description: 
// Finds a goal in the goal list given a name.
//
// Parameters: 
// goalName - The name of the goal being searched for.
//
// Return Value: 
// A pointer to the found goal or NULL.
//
// Remarks: 
//
//===========================================================================
IEGoal * IE::findGoal ( std::string & goalName )
{
    std::list<IEGoal *>::iterator goalItr;

    IEGoal * goal;

    goalItr = m_goalList.begin();

    while ( goalItr != m_goalList.end() )
    {
        goal = *goalItr;

        if ( goal->m_name == goalName )
            return goal;

        ++goalItr;
    }

    return NULL;
}

//===========================================================================
// Name: 
// pushGoal 
//
// Description: 
// Pushes a goal onto the goal stack, but first looks to see if it is already
// on the stack and pops everybody above it if it is.
//
// Parameters: 
// addGoal - The goal to put on the top of the stack.
//
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::pushGoal ( IEGoal * addGoal )
{
    std::list<IEGoal *>::iterator goalItr;

    IEGoal * goal;

    // first, see if goal alread on the stack
    goalItr = m_goalStack.begin();

    while ( goalItr != m_goalStack.end() )
    {
        goal = *goalItr;

        // if found
        if ( goal == addGoal )
        {
            goalItr = m_goalStack.end();

            --goalItr;

            // pop stack off above goal
            while ( goal != *goalItr )
            {
                m_goalStack.pop_back ();

                --goalItr;
            }

            // done, quit
            return;
        }

        ++goalItr;
    }

    // not on stack so put it there
    m_goalStack.push_back ( addGoal );
}

//===========================================================================
// Name: 
// popGoal  
//
// Description: 
// Pops a goal and every goal above it off the stack.
//
// Parameters: 
// goal - The goal to take off the stack.
//
// Return Value: 
// None.
//
// Remarks: 
//
//===========================================================================
void IE::popGoal  ( IEGoal * goal2Pop )
{
    std::list<IEGoal *>::iterator goalItr;

    IEGoal * goal;

    goalItr = m_goalStack.end();

    --goalItr;

    // pop upto and including goal2Pop
    while ( !m_goalStack.empty() )
    {
        goal = *goalItr--;

        m_goalStack.pop_back();

        // quit if at goal2Pop
        if ( goal == goal2Pop )
            return;
    }

    // shouldn't reach here
    printf ( "ERROR: Goal %s does not exsit on the goal stack.\n",
        goal2Pop->getName() );
}
