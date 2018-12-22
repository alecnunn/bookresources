
#include "stdafx.h"
#include "RTTI.h"
#include <stdarg.h>


RTTI::RTTI(const std::string & className) : 
    m_className(className), 
    m_numParents(0),
    m_pBaseRTTI(NULL)
{}


RTTI::RTTI(const std::string & className, const RTTI & baseRTTI) :
    m_className(className), 
    m_numParents(1)
{
    m_pBaseRTTI = new const RTTI*[1];
    m_pBaseRTTI[0] = &baseRTTI;
}


RTTI::RTTI(const std::string & className, int numParents, ...) :
    m_className(className)
{
    if (numParents < 1)
    {
        m_numParents = 0;
        m_pBaseRTTI  = NULL;
    }
    else
    {
        m_numParents = numParents;
        m_pBaseRTTI = new const RTTI*[m_numParents];

        va_list v;
        va_start(v,numParents);
        for (int i=0; i < m_numParents; ++i) 
        {
            m_pBaseRTTI[i] = va_arg(v, const RTTI*);
        }
        va_end(v);
    }
}


RTTI::~RTTI()
{
    delete [] m_pBaseRTTI;
}


bool RTTI::DerivesFrom (const RTTI & rtti) const
{
    const RTTI * pCompare = this;
    if (pCompare == &rtti)
        return true;

    for (int i=0; i < m_numParents; ++i)
    {
        if (m_pBaseRTTI[i]->DerivesFrom(rtti))
            return true;
    }

    return false;
}
