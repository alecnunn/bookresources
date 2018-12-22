
#ifndef _IEEXEC_H_
#define _IEEXEC_H_

#include <string>

class IEOwner;

class IEExec
{
public:

    IEExec();

    virtual void init   ( IEOwner * owner ) = 0;
    virtual bool start  ();
    virtual bool update () = 0;
    virtual bool finish ();
    virtual void reset  ();

    virtual IEOwner * getOwner  () = 0;

    virtual const char * getName() = 0;

protected:
    int m_state;

    bool m_done;
};


#endif