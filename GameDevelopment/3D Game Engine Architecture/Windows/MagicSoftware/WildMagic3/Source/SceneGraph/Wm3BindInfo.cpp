// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BindInfo.h"
using namespace Wm3;

//----------------------------------------------------------------------------
BindInfo::BindInfo ()
{
    User = 0;
    memset(ID,0,8*sizeof(char));
}
//----------------------------------------------------------------------------
BindInfoArray::BindInfoArray (int iQuantity, int iGrowBy)
    :
    m_kBind(iQuantity,iGrowBy)
{
}
//----------------------------------------------------------------------------
const TArray<BindInfo>& BindInfoArray::GetArray () const
{
    return m_kBind;
}
//----------------------------------------------------------------------------
void BindInfoArray::Bind (Renderer* pkUser, int iSize, const void* pvID)
{
    assert( 1 <= iSize && iSize <= 8 );

    BindInfo kInfo;
    kInfo.User = pkUser;
    memcpy(kInfo.ID,pvID,iSize*sizeof(char));
    m_kBind.Append(kInfo);
}
//----------------------------------------------------------------------------
void BindInfoArray::Unbind (Renderer* pkUser)
{
    int i;
    for (i = 0; i < m_kBind.GetQuantity(); i++)
    {
        if ( m_kBind[i].User == pkUser )
        {
            m_kBind.Remove(i);
            break;
        }
    }
}
//----------------------------------------------------------------------------
void BindInfoArray::GetID (Renderer* pkUser, int iSize, void* pvID)
{
    assert( 1 <= iSize && iSize <= 8 );

    int i;
    for (i = 0; i < m_kBind.GetQuantity(); i++)
    {
        if ( m_kBind[i].User == pkUser )
        {
            memcpy(pvID,m_kBind[i].ID,iSize*sizeof(char));
            break;
        }
    }

    // The resource is not yet bound to the renderer.
    if ( i == m_kBind.GetQuantity() )
        memset(pvID,0,iSize*sizeof(char));
}
//----------------------------------------------------------------------------
