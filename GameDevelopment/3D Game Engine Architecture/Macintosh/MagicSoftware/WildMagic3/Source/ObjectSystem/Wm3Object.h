// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3OBJECT_H
#define WM3OBJECT_H

#include "Wm3Main.h"
#include "Wm3Rtti.h"
#include "Wm3NameID.mcr"
#include "Wm3SmartPointer.h"
#include "Wm3Stream.h"
#include "Wm3StringTree.h"

namespace Wm3
{

class Controller;

class WM3_ITEM Object
{
// root class of object system
public:
    virtual ~Object ();
protected:
    Object ();

// run-time type information system
public:
    static const Rtti TYPE;
    virtual const Rtti& GetType () const;
    bool IsExactly (const Rtti& rkType) const;
    bool IsDerived (const Rtti& rkType) const;
    bool IsExactlyTypeOf (const Object* pkObject) const;
    bool IsDerivedTypeOf (const Object* pkObject) const;

// controller system
public:
    void SetController (Controller* pkController);
    int GetControllerQuantity () const;
    Controller* GetController (int i) const;
    void RemoveController (Controller* pkController);
    void RemoveAllControllers ();
    bool UpdateControllers (double dAppTime);
private:
    // controllers (Pointer used directly to avoid circular headers)
    TList<Pointer<Controller> >* m_pkControllerList;

// copying system
public:
    Pointer<Object> Copy () const;
    static char NameAppend;

// name-ID system
public:
    void SetName (const String& rkName);
    const String& GetName () const;
    unsigned int GetID () const;
    static unsigned int GetNextID ();
    virtual Object* GetObjectByName (const String& rkName);
    virtual void GetAllObjectsByName (const String& rkName,
        TArray<Object*>& rkObjects);
    virtual Object* GetObjectByID (unsigned int uiID);
private:
    String m_kName;
    unsigned int m_uiID;
    static unsigned int ms_uiNextID;

// smart pointer system
public:
    void IncrementReferences ();
    void DecrementReferences ();
    int GetReferences () const;
    static THashTable<unsigned int,Object*> InUse;
    static void PrintInUse (const char* acFilename, const char* acMessage);
private:
    int m_iReferences;

// streaming system
public:
    enum { FACTORY_MAP_SIZE = 256 };
    static THashTable<String,FactoryFunction>* ms_pkFactory;
    static bool RegisterFactory ();
    static void InitializeFactory ();
    static void TerminateFactory ();
    static Object* Factory (Stream& rkStream);
    virtual void Load (Stream& rkStream, Stream::Link* pkLink);
    virtual void Link (Stream& rkStream, Stream::Link* pkLink);
    virtual bool Register (Stream& rkStream) const;
    virtual void Save (Stream& rkStream) const;
    virtual StringTree* SaveStrings (const char* acTitle = 0);
    virtual int GetMemoryUsed () const;
    virtual int GetDiskUsed () const;
private:
    static bool ms_bStreamRegistered;
};

// static and dynamic type casting
template <class T> T* StaticCast (Object* pkObj);
template <class T> const T* StaticCast (const Object* pkObj);
template <class T> T* DynamicCast (Object* pkObj);
template <class T> const T* DynamicCast (const Object* pkObj);

WM3_REGISTER_STREAM(Object);
typedef Pointer<Object> ObjectPtr;
#include "Wm3Object.inl"

}

#endif
