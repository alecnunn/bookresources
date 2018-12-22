

#include "ieowner.h"
#include "ie.h"

void IEOwner::think ()
{
    if ( m_brain )
        m_brain->update();
}
