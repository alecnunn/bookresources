

#ifndef _IEOWNER_H_
#define _IEOWNER_H_

#include <string>

class IE;

//
// Common interface for Inference Engine owner
//
class IEOwner
{

public:
    
    virtual void makeBrain () = 0;

    void think     ();
    
    void         setName ( char * name ) { m_name = name; }
    const char * getName () { return m_name.c_str(); }

protected:

    IE * m_brain;

    std::string m_name;
};

#define IE_START(name)\
void name::makeBrain ()\
{\
    char lineInfo[256];\
    IEExec * exec;\
    IE * ie = new IE;\
	ie->start (#name, this);

#define GOAL(execClass)\
    exec = new execClass;\
	ie->makeGoal(exec); 

#define IF(execClass)\
    exec = new execClass;\
    sprintf ( lineInfo, "File:%s Line %d", __FILE__, __LINE__ );\
	ie->makeRule(exec, lineInfo);

#define IF_DONE\
    ie->makeDoneRule();

#define GOTO(goal)\
    sprintf ( lineInfo, "File:%s Line %d", __FILE__, __LINE__ );\
	ie->addRuleGoto(#goal, lineInfo);

#define GOSUB(goal)\
    sprintf ( lineInfo, "File:%s Line %d", __FILE__, __LINE__ );\
	ie->addRuleGosub(#goal, lineInfo);

#define RETURN\
    sprintf ( lineInfo, "File:%s Line %d", __FILE__, __LINE__ );\
	ie->addRuleReturn(lineInfo);

#define IE_END \
	ie->end(); \
    m_brain = ie; \
}

#endif
