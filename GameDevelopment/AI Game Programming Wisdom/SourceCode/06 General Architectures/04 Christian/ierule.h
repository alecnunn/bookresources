

#ifndef _IERULE_H_
#define _IERULE_H_

#include <string>

class IE;
class IEExec;
class IEGoal;

class IERule
{

friend IE;
friend IEGoal;

public:

	enum RuleType
	{
		Goto,		// Goto rule   - if condition met, goes to new goal 
		Gosub,	    // Push rule   - if condition met, keeps current goal's 
		            //               rules on a stack and goes to new goal
        Return      // Return rule - if condition met, return to previous goal                  
	};

public:

    IERule  ( IEExec * condition );
    ~IERule ();

    void     setGoal ( IEGoal * goal );
    void     setGoal ( char * name, char * lineInfo );

    IEGoal * getGoal ();

    void     setType ( RuleType type );

    const char * getName () { return m_name.c_str(); }


private:

    std::string m_name;
    std::string m_goalName;
    std::string m_lineInfo;

    IEGoal * m_goal;
    IEExec * m_condition;

    RuleType m_type;

    bool     m_fired; // flag for rule that has been fired (triggered)
};

#endif