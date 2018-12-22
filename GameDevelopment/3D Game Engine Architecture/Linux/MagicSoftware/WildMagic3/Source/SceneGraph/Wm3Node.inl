// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
inline int Node::GetQuantity () const
{
    return m_kChild.GetQuantity();
}
//----------------------------------------------------------------------------
inline int Node::GetUsed () const
{
    return m_iUsed;
}
//----------------------------------------------------------------------------
