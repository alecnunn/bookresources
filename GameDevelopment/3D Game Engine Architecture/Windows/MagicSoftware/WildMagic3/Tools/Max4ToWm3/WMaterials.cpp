// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WMaterials.h"
#include "max.h"
using namespace std;

//----------------------------------------------------------------------------
WMaterials::WMaterials ()
{
}
//----------------------------------------------------------------------------
bool WMaterials::Add (Mtl* pkMtl)
{
    if ( pkMtl == NULL ) 
        return false;

    for (int i = 0; i < (int)m_kMaterial.size(); i++)
    {
        if ( m_kMaterial[i] == pkMtl )
            return false;
    }
    
    m_kMaterial.push_back(pkMtl);
    return true;
}
//----------------------------------------------------------------------------
void WMaterials::SetQuantity (int iQuantity)
{
    m_kMaterial.resize(iQuantity);
}
//----------------------------------------------------------------------------
int WMaterials::GetQuantity () const
{
    return (int)m_kMaterial.size();
}
//----------------------------------------------------------------------------
Mtl* WMaterials::Get (int iMtlID) const
{
    if ( 0 <= iMtlID && iMtlID < (int)m_kMaterial.size() )
        return m_kMaterial[iMtlID];

    return NULL;
}
//----------------------------------------------------------------------------
int WMaterials::GetID (Mtl* pkMtl) const
{
    for (int i = 0; i < (int)m_kMaterial.size(); i++)
    {
        if ( m_kMaterial[i] == pkMtl )
            return i;
    }

    return -1;
}
//----------------------------------------------------------------------------
