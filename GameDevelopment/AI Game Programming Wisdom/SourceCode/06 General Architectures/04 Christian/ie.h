
#ifndef _IE_H_
#define _IE_H_

#include <stack>
#include <string>
#include <list>

class IEGoal;
class IEExec;
class IERule;
class IEOwner;

class IE
{

public:

    void start          ( char   * name, IEOwner * owner );
    void end            ();
    void makeGoal       ( IEExec * goalExec );
    void makeRule       ( IEExec * condition, char * lineInfo );
    void addRuleGoto    ( char   * goalName,  char * lineInfo );
    void addRuleGosub   ( char   * goalName,  char * lineInfo );
    void addRuleReturn  ( char * lineInfo );

    void update         ();
    void checkStart     ();
    bool checkFinish    ();

private:

    bool        noRule          ( char * lineInfo );
    void        linkRuleGoals   ();
    IEGoal    * findGoal        ( std::string & goalName );

    void pushGoal  ( IEGoal * goal );
    void popGoal   ( IEGoal * goal );
    
private:

    std::string           m_name;
    int                   m_type;

    IEGoal              * m_curGoal;
    IERule              * m_curRule;
    IEGoal              * m_nextGoal;

    std::list<IEGoal *>   m_goalList;
    std::list<IEGoal *>   m_goalStack;

    IEOwner             * m_owner;
};

#endif
