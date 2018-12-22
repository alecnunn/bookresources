
#ifndef _IEGOAL_H_
#define _IEGOAL_H_

#include <list>
#include <string>

class IERule;
class IEExec;
class IE;

class IEGoal
{

friend IE;
friend IERule;

public:
    IEGoal  ( IEExec * goalExec);
    ~IEGoal ();

    void         addRule         ( IERule * rule );
    void         setStart        ( bool start );
    bool         atStart         ();
    bool         start           ();
    void         setFinish       ( bool finish );
    bool         atFinish        ();
    bool         finish          ();
    void         reset           ();
    void         destroyRules    ();
    bool         update          ();
    const char * getName         ();
    void         resetFiredRules ();

private:
    std::string m_name;             // goal name

    IEExec    * m_goalExec;         // goal executor

    std::list<IERule *> m_rules;    // list of rules for this goal

    IEGoal    * m_lastGoal;         // goal that had control before this goal

    bool        m_start;            // start  flag
    bool        m_finish;           // finish flag
};

#endif