// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WMaterialTree.h"
using namespace Wm3;
using namespace std;

//----------------------------------------------------------------------------
WMaterialTree::WMaterialTree (Wm3::MaterialState* pkMState)
{
    m_spkMState = pkMState;
}
//----------------------------------------------------------------------------
void WMaterialTree::SetMChildQuantity (int iQuantity)
{
    m_kMChild.resize(iQuantity);
}
//----------------------------------------------------------------------------
int WMaterialTree::GetMChildQuantity () const
{
    return (int)m_kMChild.size();
}
//----------------------------------------------------------------------------
void WMaterialTree::SetMaterial (Wm3::MaterialStatePtr spkMState)
{
    m_spkMState = spkMState;
}
//----------------------------------------------------------------------------
MaterialStatePtr WMaterialTree::GetMaterial () const
{
    return m_spkMState;
}
//----------------------------------------------------------------------------
WMaterialTree& WMaterialTree::GetMChild (int i)
{
    return m_kMChild[i];
}
//----------------------------------------------------------------------------
void WMaterialTree::SetTChildQuantity (int iQuantity)
{
    m_kTChild.resize(iQuantity);
}
//----------------------------------------------------------------------------
int WMaterialTree::GetTChildQuantity () const
{
    return (int)m_kTChild.size();
}
//----------------------------------------------------------------------------
TexturePtr WMaterialTree::GetTexture (int i)
{
    return m_kTChild[i].GetTexture();
}
//----------------------------------------------------------------------------
WTextureTree& WMaterialTree::GetTChild (int i)
{
    return m_kTChild[i];
}
//----------------------------------------------------------------------------
