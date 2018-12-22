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
inline void SwitchNode::SetActiveChild (int iActiveChild)
{
    assert(iActiveChild == SN_INVALID_CHILD || iActiveChild < GetQuantity());
    m_iActiveChild = iActiveChild;
}
//----------------------------------------------------------------------------
inline int SwitchNode::GetActiveChild () const
{
    return m_iActiveChild;
}
//----------------------------------------------------------------------------
inline void SwitchNode::DisableAllChildren ()
{
    m_iActiveChild = SN_INVALID_CHILD;
}
//----------------------------------------------------------------------------
