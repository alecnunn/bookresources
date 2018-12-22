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
TImage<T>::TImage (int iDimensions, int* aiBound, T* atData)
    :
    Lattice(iDimensions,aiBound)
{
    SetData(atData);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const TImage& rkImage)
    :
    Lattice(rkImage)
{
    m_atData = new T[m_iQuantity];
    memcpy(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T));
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const char* acFilename)
{
    Load(acFilename);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (int iDimensions)
    :
    Lattice(iDimensions)
{
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::~TImage ()
{
    delete[] m_atData;
}
//----------------------------------------------------------------------------
template <class T>
void TImage<T>::SetData (T* atData)
{
    if ( atData )
    {
        m_atData = atData;
    }
    else
    {
        assert( m_iQuantity > 0 );
        m_atData = new T[m_iQuantity];
        memset(m_atData,0,m_iQuantity*sizeof(T));
    }
}
//----------------------------------------------------------------------------
template <class T>
T* TImage<T>::GetData () const
{
    return m_atData;
}
//----------------------------------------------------------------------------
template <class T>
T& TImage<T>::operator[] (int i) const
{
    // assert:  i < m_iQuantity
    return m_atData[i];
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (const TImage& rkImage)
{
    Lattice::operator=(rkImage);

    delete[] m_atData;
    m_atData = new T[m_iQuantity];
    memcpy(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T));

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (T tValue)
{
    for (int i = 0; i < m_iQuantity; i++)
        m_atData[i] = tValue;

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator== (const TImage& rkImage) const
{
    if ( Lattice::operator!=(rkImage) )
        return false;

    return memcmp(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T)) == 0;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator!= (const TImage& rkImage) const
{
    return !operator==(rkImage);
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Load (const char* acFilename)
{
    FILE* pkIFile = fopen(acFilename,"rb");
    if ( !pkIFile )
    {
        m_iDimensions = 0;
        m_iQuantity = 0;
        m_aiBound = 0;
        m_aiOffset = 0;
        m_atData = 0;
        return false;
    }

    if ( !Lattice::Load(pkIFile) )
    {
        m_atData = 0;
        fclose(pkIFile);
        return false;
    }

    int iRTTI, iSizeOf;
    System::Read4(pkIFile,1,&iRTTI);
    System::Read4(pkIFile,1,&iSizeOf);

    m_atData = new T[m_iQuantity];
    if ( iRTTI == T::GetRTTI() )
    {
        const int iTypeSize = sizeof(T);
        switch ( iTypeSize )
        {
        case 1:
            System::Read1(pkIFile,m_iQuantity,m_atData);
            break;
        case 2:
            System::Read2(pkIFile,m_iQuantity,m_atData);
            break;
        case 4:
            System::Read4(pkIFile,m_iQuantity,m_atData);
            break;
        case 8:
            System::Read8(pkIFile,m_iQuantity,m_atData);
            break;
        default:
            assert( false );
        }
    }
    else
    {
        char* acFileData = new char[m_iQuantity*iSizeOf];
        switch ( iSizeOf )
        {
        case 1:
            System::Read1(pkIFile,m_iQuantity,acFileData);
            break;
        case 2:
            System::Read2(pkIFile,m_iQuantity,acFileData);
            break;
        case 4:
            System::Read4(pkIFile,m_iQuantity,acFileData);
            break;
        case 8:
            System::Read8(pkIFile,m_iQuantity,acFileData);
            break;
        default:
            assert( false );
        }
        ImageConvert(m_iQuantity,iRTTI,acFileData,T::GetRTTI(),m_atData);
        delete[] acFileData;
    }

    fclose(pkIFile);
    return true;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Save (const char* acFilename) const
{
    FILE* pkOFile = fopen(acFilename,"wb");
    if ( !pkOFile )
        return false;

    if ( !Lattice::Save(pkOFile) )
    {
        fclose(pkOFile);
        return false;
    }

    int iRTTI = T::GetRTTI();
    System::Write4(pkOFile,1,&iRTTI);

    int iSizeOf = (int)(sizeof(T));
    System::Write4(pkOFile,1,&iSizeOf);

    const int iTypeSize = sizeof(T);
    switch ( iTypeSize )
    {
    case 1:
        System::Write1(pkOFile,m_iQuantity,m_atData);
        break;
    case 2:
        System::Write2(pkOFile,m_iQuantity,m_atData);
        break;
    case 4:
        System::Write4(pkOFile,m_iQuantity,m_atData);
        break;
    case 8:
        System::Write8(pkOFile,m_iQuantity,m_atData);
        break;
    default:
        assert( false );
    }

    fclose(pkOFile);
    return true;
}
//----------------------------------------------------------------------------
