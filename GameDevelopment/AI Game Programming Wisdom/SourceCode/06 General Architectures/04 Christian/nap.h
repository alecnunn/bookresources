

#ifndef _NAP_H_
#define _NAP_H_

#include "ieexec.h"

class Character;

class Nap : public IEExec
{

public:

    Nap();

    void init   ( IEOwner * owner );
    bool start  ();
    bool update ();
    bool finish ();

    IEOwner * getOwner ();

    const char * getName();

    enum NapStates
    {
        LieDown,
        DownYawn,
        Rest,
        OpenEyes,
        GetUp,
        UpYawn,
        Stretch,
        Done
    };

private:

    std::string m_name;
    Character * m_owner;

    float     m_clock;

    float     m_timeSnore;
};

#endif
