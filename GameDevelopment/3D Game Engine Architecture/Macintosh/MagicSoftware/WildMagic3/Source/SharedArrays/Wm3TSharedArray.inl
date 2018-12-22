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
TSharedArray<T>::TSharedArray (int iQuantity, T* atArray)
{
    m_iQuantity = iQuantity;
    m_atArray = atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::TSharedArray (const TSharedArray& rkShared)
{
    m_atArray = 0;
    *this = rkShared;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::~TSharedArray ()
{
    delete[] m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>& TSharedArray<T>::operator= (const TSharedArray& rkShared)
{
    delete[] m_atArray;
    m_iQuantity = rkShared.m_iQuantity;
    if ( rkShared.m_atArray )
    {
        m_atArray = new T[m_iQuantity];
        for (int i = 0; i < m_iQuantity; i++)
            m_atArray[i] = rkShared.m_atArray[i];
    }
    else
    {
        m_atArray = 0;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <class T>
int TSharedArray<T>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class T>
T* TSharedArray<T>::GetData () const
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::operator const T* () const
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::operator T* ()
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
const T& TSharedArray<T>::operator[] (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
T& TSharedArray<T>::operator[] (int i)
{
    assert( 0 <= i && i < m_iQuantity );
    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::SetActiveQuantity (int iActiveQuantity)
{
    assert( iActiveQuantity >= 0 );
    m_iQuantity = iActiveQuantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
template <class T> 
Object* TSharedArray<T>::Factory (Stream& rkStream) 
{ 
    TSharedArray<T>* pkObject = new TSharedArray<T>; 
    Stream::Link* pkLink = new Stream::Link(pkObject); 
    pkObject->Load(rkStream,pkLink); 
    return pkObject; 
} 
//----------------------------------------------------------------------------
template <class T> 
void TSharedArray<T>::InitializeFactory () 
{ 
    if ( !ms_pkFactory ) 
    { 
        ms_pkFactory = 
            new THashTable<String,FactoryFunction>(FACTORY_MAP_SIZE); 
    } 
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); 
} 
//----------------------------------------------------------------------------
template <class T> 
bool TSharedArray<T>::RegisterFactory () 
{ 
    if ( !ms_bStreamRegistered ) 
    { 
        Main::AddInitializer(TSharedArray<T>::InitializeFactory); 
        ms_bStreamRegistered = true; 
    } 
    return ms_bStreamRegistered; 
}
//----------------------------------------------------------------------------
template <class T>
Object* TSharedArray<T>::GetObjectByName (const String& rkName)
{
    return Object::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
template <class T>
Object* TSharedArray<T>::GetObjectByID (unsigned int uiID)
{
    return Object::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);
    rkStream.Read(m_iQuantity);
    m_atArray = new T[m_iQuantity];
    rkStream.Read(m_iQuantity,m_atArray);

    WM3_END_DEBUG_STREAM_LOAD(TSharedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
template <class T>
bool TSharedArray<T>::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);
    rkStream.Write(m_iQuantity);
    rkStream.Write(m_iQuantity,m_atArray);

    WM3_END_DEBUG_STREAM_SAVE(TSharedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
StringTree* TSharedArray<T>::SaveStrings (const char* acTitle)
{
    StringTree* pkTree = new StringTree(1,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Object::SaveStrings());
    pkTree->SetChild(1,StringTree::Format(acTitle,m_iQuantity,m_atArray));

    return pkTree;
}
//----------------------------------------------------------------------------
template <class T>
int TSharedArray<T>::GetMemoryUsed () const
{
    return sizeof(TSharedArray<T>)-sizeof(Object)+m_iQuantity*sizeof(T);
}
//----------------------------------------------------------------------------
template <class T>
int TSharedArray<T>::GetDiskUsed () const
{
    return Object::GetDiskUsed()+sizeof(m_iQuantity)+m_iQuantity*sizeof(T);
}
//----------------------------------------------------------------------------
