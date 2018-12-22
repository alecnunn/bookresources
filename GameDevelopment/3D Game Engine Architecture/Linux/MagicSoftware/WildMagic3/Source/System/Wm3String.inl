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
inline int String::GetLength () const
{
    return m_iLength;
}
//----------------------------------------------------------------------------
inline String::operator const char* () const
{
    return m_acText;
}
//----------------------------------------------------------------------------
inline int String::GetMemoryUsed () const
{
    return sizeof(m_iLength) + (m_iLength+1)*sizeof(char);
}
//----------------------------------------------------------------------------
inline int String::GetDiskUsed () const
{
    return sizeof(m_iLength) + m_iLength*sizeof(char);
}
//----------------------------------------------------------------------------
