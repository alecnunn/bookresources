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
TArray<T>::TArray (int iQuantity, int iGrowBy)
{
    assert( iQuantity >= 0 && iGrowBy >= 0 );

    if ( iQuantity < 0 )
        iQuantity = 0;

    if ( iGrowBy < 0 )
        iGrowBy = 0;

    m_iQuantity = 0;
    m_iMaxQuantity = iQuantity;
    m_iGrowBy = iGrowBy;
    m_atArray = ( m_iMaxQuantity > 0 ? new T[m_iMaxQuantity] : 0 );
}
//----------------------------------------------------------------------------
template <class T>
TArray<T>::TArray (const TArray& rkObject)
{
    m_atArray = 0;
    *this = rkObject;
}
//----------------------------------------------------------------------------
template <class T>
TArray<T>::~TArray ()
{
    delete[] m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TArray<T>& TArray<T>::operator= (const TArray& rkObject)
{
    m_iQuantity = rkObject.m_iQuantity;
    m_iMaxQuantity = rkObject.m_iMaxQuantity;
    m_iGrowBy = rkObject.m_iGrowBy;

    delete[] m_atArray;
    if ( m_iMaxQuantity > 0 )
    {
        m_atArray = new T[m_iMaxQuantity];
        for (int i = 0; i < m_iMaxQuantity; i++)
            m_atArray[i] = rkObject.m_atArray[i];
    }
    else
    {
        m_atArray = 0;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
int TArray<T>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class T>
T* TArray<T>::GetArray ()
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
const T* TArray<T>::GetArray () const
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
T& TArray<T>::operator[] (int i)
{
    assert( 0 <= i && i < m_iQuantity );
    if ( i < 0 )
        i = 0;
    if ( i >= m_iQuantity )
        i = m_iQuantity-1;

    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
const T& TArray<T>::operator[] (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    if ( i < 0 )
        i = 0;
    if ( i >= m_iQuantity )
        i = m_iQuantity-1;

    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::Append (const T& rtElement)
{
    if ( ++m_iQuantity > m_iMaxQuantity )
    {
        assert( m_iGrowBy > 0 );
        if ( m_iGrowBy > 0 )
        {
            // increase the size of the array
            SetMaxQuantity(m_iMaxQuantity+m_iGrowBy,true);
        }
        else
        {
            // cannot grow the array, overwrite the last element
            --m_iQuantity;
        }
    }

    m_atArray[m_iQuantity-1] = rtElement;
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::SetElement (int i, const T& rtElement)
{
    assert( i >= 0 );
    if ( i < 0 )
        i = 0;

    if ( i >= m_iQuantity )
    {
        if ( i >= m_iMaxQuantity )
        {
            assert( m_iGrowBy > 0 );
            if ( m_iGrowBy > 0 )
            {
                // increase the size of the array
                if ( i+1 >= m_iMaxQuantity )
                {
                    int iN =
                        (int)(0.5f+(i+1-m_iMaxQuantity)/(float)m_iGrowBy);
                    iN++;
                    SetMaxQuantity(m_iMaxQuantity+iN*m_iGrowBy,true);
                }
            }
            else
            {
                // cannot grow the array, overwrite the last element
                i = m_iQuantity-1;
            }
        }
        m_iQuantity = i+1;
    }

    m_atArray[i] = rtElement;
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::Remove (int i)
{
    assert( 0 <= i && i < m_iQuantity );
    if ( i < 0 || i >= m_iQuantity )
        return;

    for (int j = i+1; j < m_iQuantity; i = j++)
        m_atArray[i] = m_atArray[j];
    m_atArray[m_iQuantity-1] = T();
    m_iQuantity--;
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::RemoveAll ()
{
    for (int i = 0; i < m_iQuantity; i++)
        m_atArray[i] = T();
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::SetMaxQuantity (int iNewMaxQuantity, bool bCopy)
{
    assert( iNewMaxQuantity >= 0 );
    if ( iNewMaxQuantity < 0 )
        iNewMaxQuantity = 0;

    if ( iNewMaxQuantity == 0 )
    {
        delete[] m_atArray;
        m_iQuantity = 0;
        m_iMaxQuantity = 0;
        return;
    }

    if ( iNewMaxQuantity != m_iMaxQuantity )
    {
        T* atNewArray = new T[iNewMaxQuantity];

        if ( bCopy )
        {
            int iCopyQuantity;
            if ( iNewMaxQuantity > m_iMaxQuantity )
                iCopyQuantity = m_iMaxQuantity;
            else
                iCopyQuantity = iNewMaxQuantity;

            for (int i = 0; i < iCopyQuantity; i++)
                atNewArray[i] = m_atArray[i];

            if ( m_iQuantity > iNewMaxQuantity )
                m_iQuantity = iNewMaxQuantity;
        }
        else
        {
            m_iQuantity = 0;
        }

        delete[] m_atArray;
        m_atArray = atNewArray;
        m_iMaxQuantity = iNewMaxQuantity;
    }
}
//----------------------------------------------------------------------------
template <class T>
int TArray<T>::GetMaxQuantity () const
{
    return m_iMaxQuantity;
}
//----------------------------------------------------------------------------
template <class T>
void TArray<T>::SetGrowBy (int iGrowBy)
{
    assert( iGrowBy >= 0 );
    if ( iGrowBy >= 0 )
        m_iGrowBy = iGrowBy;
}
//----------------------------------------------------------------------------
template <class T>
int TArray<T>::GetGrowBy () const
{
    return m_iGrowBy;
}
//----------------------------------------------------------------------------
