// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WTextureTree.h"
using namespace Wm3;
using namespace std;

//----------------------------------------------------------------------------
WTextureTree::WTextureTree ()
{
}
//----------------------------------------------------------------------------
void WTextureTree::SetTexture (TexturePtr spkTexture)
{
    m_spkTexture = spkTexture;
}
//----------------------------------------------------------------------------
TexturePtr WTextureTree::GetTexture ()
{
    return m_spkTexture;
}
//----------------------------------------------------------------------------
void WTextureTree::SetChildQuantity (int iQuantity)
{
    m_kChild.resize(iQuantity);
}
//----------------------------------------------------------------------------
int WTextureTree::GetChildQuantity () const
{
    return (int)m_kChild.size();
}
//----------------------------------------------------------------------------
WTextureTree& WTextureTree::GetChild (int i)
{
    return m_kChild[i];
}
//----------------------------------------------------------------------------
