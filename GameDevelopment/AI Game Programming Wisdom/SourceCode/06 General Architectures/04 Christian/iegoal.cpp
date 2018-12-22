
#include "iegoal.h"
#include "ieexec.h"
#include "ierule.h"

IEGoal::IEGoal  ( IEExec * goalExec )
{
    m_goalExec = goalExec;
    m_name     = goalExec->getName();
    m_start    = true;
    m_finish   = false;
}

IEGoal::~IEGoal ()
{
    destroyRules();
}

void IEGoal::addRule ( IERule * rule )
{
    // add new rule
    m_rules.push_back ( rule );
}

void IEGoal::setStart ( bool start )
{
    m_start = start;
}

bool IEGoal::atStart ()
{
    return m_start;
}

bool IEGoal::start ()
{
    return m_goalExec->start();
}

void IEGoal::setFinish ( bool finish )
{
    m_finish = finish;;
}

bool IEGoal::atFinish ()
{
    return m_finish;
}

bool IEGoal::finish ()
{
    return m_goalExec->finish();
}

void IEGoal::reset ()
{
    m_start  = true;
    m_finish = false;

    m_goalExec->reset();
}

void IEGoal::destroyRules ()
{
    m_rules.clear();
}

bool IEGoal::update ()
{
    return m_goalExec->update();
}

const char * IEGoal::getName ()
{
    return m_name.c_str();
}

void IEGoal::resetFiredRules ()
{
    std::list<IERule *>::iterator ruleItr;

    IERule * rule;

    ruleItr = m_rules.begin();

    while ( ruleItr != m_rules.end() )
    {			
        rule = *ruleItr++;

        rule->m_fired = false;
    }

}