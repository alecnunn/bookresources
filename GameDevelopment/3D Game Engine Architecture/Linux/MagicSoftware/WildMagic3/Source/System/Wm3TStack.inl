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
template <class T>
TStack<T>::TStack (int iMaxQuantity)
{
    assert( iMaxQuantity > 0 );
    m_iMaxQuantity = iMaxQuantity;
    m_iTop = -1;
    m_atStack = new T[m_iMaxQuantity];
}
//----------------------------------------------------------------------------
template <class T>
TStack<T>::~TStack ()
{
    delete[] m_atStack;
}
//----------------------------------------------------------------------------
template <class T>
bool TStack<T>::IsEmpty () const
{
    return m_iTop == -1;
}
//----------------------------------------------------------------------------
template <class T>
bool TStack<T>::IsFull () const
{
    return m_iTop >= m_iMaxQuantity-1;
}
//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Push (const T& rkItem)
{
    m_iTop++;
    if ( m_iTop <= m_iMaxQuantity-1 )
    {
        m_atStack[m_iTop] = rkItem;
    }
    else
    {
        // stack full, should not happen
        assert( false );
    }
}
//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Pop (T& rkItem)
{
    if ( m_iTop >= 0 )
    {
        if ( m_iTop <= m_iMaxQuantity-1 )
        {
            rkItem = m_atStack[m_iTop];
        }
        else
        {
            // stack full, should not happen
            assert( false );
        }
        m_iTop--;
    }
    else
    {
        // stack empty, should not happen
        assert( false );
    }
}
//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Clear ()
{
    m_iTop = -1;
}
//----------------------------------------------------------------------------
template <class T>
bool TStack<T>::GetTop (T& rkItem) const
{
    if ( 0 <= m_iTop && m_iTop <= m_iMaxQuantity-1 )
    {
        rkItem = m_atStack[m_iTop];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class T>
int TStack<T>::GetQuantity () const
{
    return m_iTop+1;
}
//----------------------------------------------------------------------------
template <class T>
const T* TStack<T>::GetData () const
{
    return m_atStack;
}
//----------------------------------------------------------------------------
