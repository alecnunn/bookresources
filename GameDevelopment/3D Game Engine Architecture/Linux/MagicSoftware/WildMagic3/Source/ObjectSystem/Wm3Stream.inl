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
inline int Stream::GetOrderedQuantity () const
{
    return m_kOrdered.GetQuantity();
}
//----------------------------------------------------------------------------
inline Object* Stream::GetOrderedObject (int i) const
{
    assert( 0 <= i && i < m_kOrdered.GetQuantity() );
    if ( i < 0 || i >= m_kOrdered.GetQuantity() )
        return 0;

    return m_kOrdered[i];
}
//----------------------------------------------------------------------------
inline int Stream::GetBufferSize () const
{
    return m_iBufferSize;
}
//----------------------------------------------------------------------------
inline int Stream::GetBufferNext () const
{
    return m_iBufferNext;
}
//----------------------------------------------------------------------------
