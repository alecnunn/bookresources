

#include "ieexec.h"

IEExec::IEExec()
{
    m_done  = false;
    m_state = -1;
}

bool IEExec::start ()
{
    return false;
}

bool IEExec::finish ()
{
    return false;
}

void IEExec::reset ()
{
    // nothing by default
}
