

#ifndef _DamageReact_H_
#define _DamageReact_H_

#include "ieexec.h"

class Character;

class DamageReact : public IEExec
{

public:

    DamageReact();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum DamageReactStates
    {
        Go,
		Done
    };


private:
    std::string m_name;
    Character * m_owner;

	float m_clock;
    float m_time;
};

#endif
