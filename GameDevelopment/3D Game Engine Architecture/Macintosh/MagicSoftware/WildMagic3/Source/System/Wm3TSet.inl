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
TSet<T>::TSet ()
{
    m_iMaxQuantity = 1;
    m_iGrowBy = 1;
    m_iQuantity = 0;
    m_atElement = new T[1];
}
//----------------------------------------------------------------------------
template <class T>
TSet<T>::TSet (int iMaxQuantity, int iGrowBy)
{
    assert( iMaxQuantity > 0 && iGrowBy > 0 );
    if ( iMaxQuantity <= 0 )
        iMaxQuantity = 1;
    if ( iGrowBy <= 0 )
        iGrowBy = 1;

    m_iMaxQuantity = iMaxQuantity;
    m_iGrowBy = iGrowBy;
    m_iQuantity = 0;
    m_atElement = new T[iMaxQuantity];
}
//----------------------------------------------------------------------------
template <class T>
TSet<T>::TSet (const TSet& rkSet)
{
    m_iMaxQuantity = rkSet.m_iMaxQuantity;
    m_iGrowBy = rkSet.m_iGrowBy;
    m_iQuantity = rkSet.m_iQuantity;
    m_atElement = new T[m_iMaxQuantity];
    for (int i = 0; i < m_iMaxQuantity; i++)
        m_atElement[i] = rkSet.m_atElement[i];
}
//----------------------------------------------------------------------------
template <class T>
TSet<T>::~TSet ()
{
    delete[] m_atElement;
}
//----------------------------------------------------------------------------
template <class T>
TSet<T>& TSet<T>::operator= (const TSet& rkSet)
{
    delete[] m_atElement;
    m_iMaxQuantity = rkSet.m_iMaxQuantity;
    m_iGrowBy = rkSet.m_iGrowBy;
    m_iQuantity = rkSet.m_iQuantity;
    m_atElement = new T[m_iMaxQuantity];
    for (int i = 0; i < m_iMaxQuantity; i++)
        m_atElement[i] = rkSet.m_atElement[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class T>
int TSet<T>::GetMaxQuantity () const
{
    return m_iMaxQuantity;
}
//----------------------------------------------------------------------------
template <class T>
int TSet<T>::GetGrowBy () const
{
    return m_iGrowBy;
}
//----------------------------------------------------------------------------
template <class T>
int TSet<T>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class T>
T* TSet<T>::GetElements ()
{
    return m_atElement;
}
//----------------------------------------------------------------------------
template <class T>
const T* TSet<T>::GetElements () const
{
    return m_atElement;
}
//----------------------------------------------------------------------------
template <class T>
T& TSet<T>::operator[] (int i)
{
    assert( 0 <= i && i < m_iQuantity );
    if ( i < 0 )
        i = 0;
    if ( i >= m_iQuantity )
        i = m_iQuantity-1;

    return m_atElement[i];
}
//----------------------------------------------------------------------------
template <class T>
const T& TSet<T>::operator[] (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    if ( i < 0 )
        i = 0;
    if ( i >= m_iQuantity )
        i = m_iQuantity-1;

    return m_atElement[i];
}
//----------------------------------------------------------------------------
template <class T>
bool TSet<T>::Insert (const T& rkElement)
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if ( rkElement == m_atElement[i] )
            return false;
    }

    if ( m_iQuantity == m_iMaxQuantity )
    {
        // array is full, resize it
        int iNewMaxQuantity = m_iMaxQuantity + m_iGrowBy;
        T* atNewElement = new T[iNewMaxQuantity];
        for (int i = 0; i < m_iMaxQuantity; i++)
            atNewElement[i] = m_atElement[i];
        delete[] m_atElement;
        m_atElement = atNewElement;
        m_iMaxQuantity = iNewMaxQuantity;
    }

    m_atElement[m_iQuantity++] = rkElement;
    return true;
}
//----------------------------------------------------------------------------
template <class T>
void TSet<T>::InsertNoCheck (const T& rkElement)
{
    if ( m_iQuantity == m_iMaxQuantity )
    {
        // array is full, resize it
        int iNewMaxQuantity = m_iMaxQuantity + m_iGrowBy;
        T* atNewElement = new T[iNewMaxQuantity];
        for (int i = 0; i < m_iMaxQuantity; i++)
            atNewElement[i] = m_atElement[i];
        delete[] m_atElement;
        m_atElement = atNewElement;
        m_iMaxQuantity = iNewMaxQuantity;
    }

    m_atElement[m_iQuantity++] = rkElement;
}
//----------------------------------------------------------------------------
template <class T>
bool TSet<T>::Remove (const T& rkElement)
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if ( rkElement == m_atElement[i] )
        {
            // element exists, shift array to fill in empty slot
            for (int j = i+1; j < m_iQuantity; j++, i++)
                m_atElement[i] = m_atElement[j];

            m_atElement[m_iQuantity-1] = T();
            m_iQuantity--;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class T>
bool TSet<T>::Exists (const T& rkElement)
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if ( rkElement == m_atElement[i] )
            return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class T>
void TSet<T>::Clear ()
{
    for (int i = 0; i < m_iQuantity; i++)
        m_atElement[i] = T();

    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class T>
void TSet<T>::Clear (int iMaxQuantity, int iGrowBy)
{
    assert( iMaxQuantity > 0 && iGrowBy > 0 );
    if ( iMaxQuantity <= 0 )
        iMaxQuantity = 1;
    if ( iGrowBy <= 0 )
        iGrowBy = 1;

    delete[] m_atElement;
    m_iMaxQuantity = iMaxQuantity;
    m_iGrowBy = iGrowBy;
    m_iQuantity = 0;
    m_atElement = new T[iMaxQuantity];
}
//----------------------------------------------------------------------------
