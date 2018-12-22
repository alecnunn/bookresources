
#include "ierule.h"
#include "iegoal.h"

IERule::IERule ( IEExec * condition )
{
    m_condition = condition;
    m_goal      = NULL;
    m_fired     = false;
}

IERule::~IERule ()
{
}

void IERule::setGoal ( IEGoal * goal )
{
    m_goal = goal;
}

void IERule::setGoal ( char * name, char * lineInfo )
{
    m_goalName = name;
    m_lineInfo = lineInfo;
}

IEGoal * IERule::getGoal()
{
    if ( m_type == IERule::Return )
	{
		return m_goal->m_lastGoal;
	}
	else
	{
        return m_goal;
	}
}

void IERule::setType ( RuleType type )
{
    m_type = type;
}
