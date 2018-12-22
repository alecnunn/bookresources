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
TCachedArray<T>::TCachedArray (int iQuantity, T* atArray)
    :
    TSharedArray<T>(iQuantity,atArray),
    BIArray(1,1)
{
}
//----------------------------------------------------------------------------
template <class T>
TCachedArray<T>::TCachedArray (const TCachedArray&)
{
    // Copying of cached data is not allowed.
    assert( false );
}
//----------------------------------------------------------------------------
template <class T>
TCachedArray<T>& TCachedArray<T>::operator= (const TCachedArray&)
{
    // Copying of cached data is not allowed.
    assert( false );
    return this;
}
//----------------------------------------------------------------------------
template <class T>
TCachedArray<T>::~TCachedArray ()
{
    // Inform all renderers using this array that it is being destroyed.
    // This allows the renderer to free up any associated resources.
    const TArray<BindInfo>& rkArray = BIArray.GetArray();
    for (int i = 0; i < rkArray.GetQuantity(); i++)
        rkArray[i].User->ReleaseArray(this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
template <class T>
Object* TCachedArray<T>::GetObjectByName (const String& rkName)
{
    return TSharedArray<T>::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
template <class T>
void TCachedArray<T>::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    TSharedArray<T>::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
template <class T>
Object* TCachedArray<T>::GetObjectByID (unsigned int uiID)
{
    return TSharedArray<T>::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
template <class T> 
Object* TCachedArray<T>::Factory (Stream& rkStream) 
{ 
    TCachedArray<T>* pkObject = new TCachedArray<T>; 
    Stream::Link* pkLink = new Stream::Link(pkObject); 
    pkObject->Load(rkStream,pkLink); 
    return pkObject; 
} 
//----------------------------------------------------------------------------
template <class T> 
void TCachedArray<T>::InitializeFactory () 
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
bool TCachedArray<T>::RegisterFactory () 
{ 
    if ( !ms_bStreamRegistered ) 
    { 
        Main::AddInitializer(TCachedArray<T>::InitializeFactory); 
        ms_bStreamRegistered = true; 
    } 
    return ms_bStreamRegistered; 
}
//----------------------------------------------------------------------------
template <class T>
void TCachedArray<T>::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TSharedArray<T>::Load(rkStream,pkLink);

    WM3_END_DEBUG_STREAM_LOAD(TCachedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
void TCachedArray<T>::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TSharedArray<T>::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
template <class T>
bool TCachedArray<T>::Register (Stream& rkStream) const
{
    return TSharedArray<T>::Register(rkStream);
}
//----------------------------------------------------------------------------
template <class T>
void TCachedArray<T>::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    TSharedArray<T>::Save(rkStream);

    WM3_END_DEBUG_STREAM_SAVE(TCachedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
StringTree* TCachedArray<T>::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
template <class T>
int TCachedArray<T>::GetMemoryUsed () const
{
    return sizeof(TCachedArray<T>) - sizeof(TSharedArray<T>) +
        TSharedArray<T>::GetMemoryUsed() +
        BIArray.GetArray().GetMaxQuantity()*sizeof(BindInfo);
}
//----------------------------------------------------------------------------
template <class T>
int TCachedArray<T>::GetDiskUsed () const
{
    return TSharedArray<T>::GetDiskUsed();
}
//----------------------------------------------------------------------------
