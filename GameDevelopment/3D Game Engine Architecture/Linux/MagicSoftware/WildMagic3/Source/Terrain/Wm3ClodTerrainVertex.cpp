// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ClodTerrainVertex.h"
using namespace Wm3;

//----------------------------------------------------------------------------
ClodTerrainVertex::ClodTerrainVertex ()
{
    m_akDependent[0] = 0;
    m_akDependent[1] = 0;
    m_bEnabled = false;
}
//----------------------------------------------------------------------------
void ClodTerrainVertex::Enable ()
{
    m_bEnabled = true;

    if ( m_akDependent[0] )
    {
        if ( !m_akDependent[0]->m_bEnabled )
            m_akDependent[0]->Enable();
    }
    
    if ( m_akDependent[1] )
    {
        if ( !m_akDependent[1]->m_bEnabled )
            m_akDependent[1]->Enable();
    }
}
//----------------------------------------------------------------------------
void ClodTerrainVertex::Disable ()
{
    m_bEnabled = false;
    
    if ( m_akDependent[0] )
    {
        if ( m_akDependent[0]->m_bEnabled )
            m_akDependent[0]->Disable();
    }

    if ( m_akDependent[1] )
    {
        if ( m_akDependent[1]->m_bEnabled )
            m_akDependent[1]->Disable();
    }
}
//----------------------------------------------------------------------------
