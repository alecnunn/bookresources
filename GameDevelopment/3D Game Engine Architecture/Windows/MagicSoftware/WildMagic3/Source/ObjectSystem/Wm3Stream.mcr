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
#define WM3_DECLARE_STREAM \
public: \
    static bool RegisterFactory (); \
    static void InitializeFactory (); \
    static Object* Factory (Stream& rkStream); \
    virtual void Load (Stream& rkStream, Stream::Link* pkLink); \
    virtual void Link (Stream& rkStream, Stream::Link* pkLink); \
    virtual bool Register (Stream& rkStream) const; \
    virtual void Save (Stream& rkStream) const; \
    virtual StringTree* SaveStrings (const char* acTitle = NULL); \
    virtual int GetMemoryUsed () const; \
    virtual int GetDiskUsed () const; \
private: \
    static bool ms_bStreamRegistered
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_STREAM(classname) \
bool classname::ms_bStreamRegistered = false; \
\
bool classname::RegisterFactory () \
{ \
    if ( !ms_bStreamRegistered ) \
    { \
        Main::AddInitializer(classname::InitializeFactory); \
        ms_bStreamRegistered = true; \
    } \
    return ms_bStreamRegistered; \
} \
\
void classname::InitializeFactory () \
{ \
    if ( !ms_pkFactory ) \
    { \
        ms_pkFactory = \
            new THashTable<String,FactoryFunction>(FACTORY_MAP_SIZE); \
    } \
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); \
} \
\
Object* classname::Factory (Stream& rkStream) \
{ \
    classname* pkObject = new classname; \
    Stream::Link* pkLink = new Stream::Link(pkObject); \
    pkObject->Load(rkStream,pkLink); \
    return pkObject; \
}
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_ABSTRACT_STREAM(classname) \
bool classname::ms_bStreamRegistered = false; \
\
bool classname::RegisterFactory () \
{ \
    if ( !ms_bStreamRegistered ) \
    { \
        Main::AddInitializer(classname::InitializeFactory); \
        ms_bStreamRegistered = true; \
    } \
    return ms_bStreamRegistered; \
} \
\
void classname::InitializeFactory () \
{ \
    if ( !ms_pkFactory ) \
    { \
        ms_pkFactory = \
            new THashTable<String,FactoryFunction>(FACTORY_MAP_SIZE); \
    } \
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); \
} \
\
Object* classname::Factory (Stream&) \
{ \
    assert( false ); \
    return NULL; \
}
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_TEMPLATE_STREAM(classname) \
template <> \
bool classname::ms_bStreamRegistered = false;
//----------------------------------------------------------------------------
#define WM3_IMPLEMENT_DEFAULT_STREAM(classname,baseclassname) \
void classname::Load (Stream& rkStream, Stream::Link* pkLink) \
{ \
    WM3_BEGIN_DEBUG_STREAM_LOAD; \
    baseclassname::Load(rkStream,pkLink); \
    WM3_END_DEBUG_STREAM_LOAD(classname); \
} \
\
void classname::Link (Stream& rkStream, Stream::Link* pkLink) \
{ \
    baseclassname::Link(rkStream,pkLink); \
} \
\
bool classname::Register (Stream& rkStream) const \
{ \
    return baseclassname::Register(rkStream); \
} \
\
void classname::Save (Stream& rkStream) const \
{ \
    WM3_BEGIN_DEBUG_STREAM_SAVE; \
    baseclassname::Save(rkStream); \
    WM3_END_DEBUG_STREAM_SAVE(classname); \
} \
\
StringTree* classname::SaveStrings (const char*) \
{ \
    StringTree* pkTree = new StringTree(1,1); \
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName())); \
    pkTree->SetChild(0,baseclassname::SaveStrings()); \
    return pkTree; \
} \
\
int classname::GetMemoryUsed () const \
{ \
    return sizeof(classname) - sizeof(baseclassname) + \
        baseclassname::GetMemoryUsed(); \
} \
\
int classname::GetDiskUsed () const \
{ \
    return baseclassname::GetDiskUsed(); \
}
//----------------------------------------------------------------------------
#define WM3_REGISTER_STREAM(classname) \
static bool gs_bStreamRegistered_##classname = \
    classname::RegisterFactory ()
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM3_BEGIN_DEBUG_STREAM_LOAD \
    int iBeginLoad_ = rkStream.GetBufferNext()
#else
#define WM3_BEGIN_DEBUG_STREAM_LOAD
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM3_END_DEBUG_STREAM_LOAD(classname) \
    int iEndLoad_ = rkStream.GetBufferNext(); \
    int iUsedLoad_ = iEndLoad_ - iBeginLoad_ + \
        GetType().GetName().GetDiskUsed(); \
    int iUsedReported_ = classname::GetDiskUsed(); \
    assert( iUsedLoad_ == iUsedReported_ )
#else
#define WM3_END_DEBUG_STREAM_LOAD(classname)
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM3_BEGIN_DEBUG_STREAM_SAVE \
    int iBeginSave_ = rkStream.GetBufferNext()
#else
#define WM3_BEGIN_DEBUG_STREAM_SAVE
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM3_END_DEBUG_STREAM_SAVE(classname) \
    int iEndSave_ = rkStream.GetBufferNext(); \
    int iUsedSave_ = iEndSave_ - iBeginSave_; \
    int iUsedReported_ = classname::GetDiskUsed(); \
    assert( iUsedSave_ == iUsedReported_ )
#else
#define WM3_END_DEBUG_STREAM_SAVE(classname)
#endif
//----------------------------------------------------------------------------
