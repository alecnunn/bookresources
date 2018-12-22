
#include "stdafx.h"
#include "RTTI.h"



bool RTTI::DerivesFrom (const RTTI & rtti) const
{
    const RTTI * pCompare = this;

    while (pCompare != NULL)
    {
        if (pCompare == &rtti)
            return true;
        pCompare = pCompare->m_pBaseRTTI;
    }
    return false;
}
