// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Object.h"
#include "Wm3Stream.h"
using namespace Wm3;

const String Stream::ms_kTopLevel("Top Level");

//----------------------------------------------------------------------------
// construction and destruction
//----------------------------------------------------------------------------
Stream::Stream ()
    :
    m_apkTopLevel(16,16),
    m_kMap(1024),
    m_kOrdered(1024,1024)
{
    m_iBufferSize = 0;
    m_iBufferNext = 0;
    m_acBuffer = 0;
}
//----------------------------------------------------------------------------
Stream::~Stream ()
{
    m_kMap.RemoveAll();
    m_kOrdered.RemoveAll();
    RemoveAll();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// top-level object management
//----------------------------------------------------------------------------
bool Stream::Insert (Object* pkObject)
{
    if ( pkObject  )
    {
        // an object can only be inserted once
        for (int i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        {
            if ( pkObject == m_apkTopLevel[i] )
                return false;
        }

        m_apkTopLevel.Append(pkObject);
        pkObject->IncrementReferences();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Stream::Remove (Object* pkObject)
{
    if ( pkObject )
    {
        for (int i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        {
            if ( pkObject == m_apkTopLevel[i] )
            {
                m_apkTopLevel.Remove(i);
                pkObject->DecrementReferences();
                return true;
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void Stream::RemoveAll ()
{
    for (int i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        m_apkTopLevel[i]->DecrementReferences();

    m_apkTopLevel.RemoveAll();
}
//----------------------------------------------------------------------------
int Stream::GetObjectCount ()
{
    return m_apkTopLevel.GetQuantity();
}
//----------------------------------------------------------------------------
Object* Stream::GetObjectAt (int i) const
{
    if ( i < m_apkTopLevel.GetQuantity() )
        return m_apkTopLevel[i];
    else
        return 0;
}
//----------------------------------------------------------------------------
bool Stream::IsTopLevel (const Object* pkObject)
{
    for (int i = 0; i < m_apkTopLevel.GetQuantity(); i++)
    {
        if ( pkObject == m_apkTopLevel[i] )
            return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// memory streaming
//----------------------------------------------------------------------------
bool Stream::Load (char* acBuffer, int iSize)
{
    if ( !Object::ms_pkFactory )
    {
        // If this assert fails, make sure you have called Main::Initialize()
        // in your 'main' or 'WinMain' before you use the engine code.  The
        // initialization builds the hash table of factory functions.
        assert( false );
        return false;
    }

    m_kMap.RemoveAll();
    m_kOrdered.RemoveAll();

    // clear out all previous top level objects
    RemoveAll();

    // Initialize the internal Stream buffer.  The m_iBufferNext value is
    // already initialized to zero.
    m_iBufferSize = iSize;
    m_acBuffer = acBuffer;

    // load list of unique objects
    Object* pkObject;
    while ( m_iBufferNext < m_iBufferSize )
    {
        // read "Top Level" or RTTI name
        String kString;
        Read(kString);
        bool bTopLevel = (kString == ms_kTopLevel);
        if ( bTopLevel )
        {
            // read RTTI name
            Read(kString);
        }

        // get the factory function for the type of object about to be read
        FactoryFunction* poFactory = Object::ms_pkFactory->Find(kString);
        if ( !poFactory )
        {
            // Cannot find the factory function.  Make sure you have added
            // WM3_REGISTER_STREAM(someclass) to the header file for each
            // 'someclass'.  This macro registers the factory function for
            // the class.
            assert( false );
            return false;
        }

        // load the object
        pkObject = (*poFactory)(*this);

        // keep track of all top level objects for application use
        if ( bTopLevel )
            Insert(pkObject);
    }

    // Link the objects.  Maintain an array of objects for use by the
    // application (if so desired).
    m_kOrdered.SetMaxQuantity(m_kMap.GetQuantity(),false);
    Link** ppkLink = m_kMap.GetFirst(&pkObject);
    while ( ppkLink )
    {
        pkObject = (*ppkLink)->GetObject();
        pkObject->Link(*this,*ppkLink);
        m_kOrdered.Append(pkObject);
        ppkLink = m_kMap.GetNext(&pkObject);
    }

    // delete the stream link records
    ppkLink = m_kMap.GetFirst(&pkObject);
    while ( ppkLink )
    {
        delete *ppkLink;
        ppkLink = m_kMap.GetNext(&pkObject);
    }

    // reset internal management structures
    m_acBuffer = 0;
    m_iBufferSize = 0;
    m_iBufferNext = 0;
    return true;
}
//----------------------------------------------------------------------------
bool Stream::Save (char*& racBuffer, int& riSize)
{
    m_kMap.RemoveAll();
    m_kOrdered.RemoveAll();

    // build list of unique objects
    int i;
    for (i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        m_apkTopLevel[i]->Register(*this);

    // accumulate number of bytes used by object on disk
    m_iBufferSize = m_apkTopLevel.GetQuantity()*ms_kTopLevel.GetDiskUsed();
    for (i = 0; i < m_kOrdered.GetQuantity(); i++)
        m_iBufferSize += m_kOrdered[i]->GetDiskUsed();

    m_acBuffer = new char[m_iBufferSize];
    m_iBufferNext = 0;

    // save list of unique objects
    for (i = 0; i < m_kOrdered.GetQuantity(); i++)
    {
        const Object* pkObject = m_kOrdered[i];
        if ( IsTopLevel(pkObject) )
            Write(ms_kTopLevel);
        pkObject->Save(*this);
    }

    // make sure the buffer is exactly filled
    assert( m_iBufferNext == m_iBufferSize );
    if ( m_iBufferNext != m_iBufferSize )
    {
        // The buffer is not exactly filled.  This is an indication that some
        // class is either incorrectly reporting the number of disk bytes
        // used (via the GetDiskUsed function) or incorrectly writing the
        // member data to disk.
        assert( false );
        delete[] m_acBuffer;
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    // transfer ownership of buffer to caller
    racBuffer = m_acBuffer;
    riSize = m_iBufferSize;

    // reset internal management structures
    m_acBuffer = 0;
    m_iBufferSize = 0;
    m_iBufferNext = 0;
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// file streaming
//----------------------------------------------------------------------------
bool Stream::Load (const char* acFilename)
{
    // load the stream from disk to memory
    char* acBuffer;
    int iSize;
    if ( !System::Load(acFilename,acBuffer,iSize) )
        return false;

    // get the file version
    if ( iSize < StreamVersion::LENGTH )
    {
        // file does not exist or not large enough to store version string
        delete[] acBuffer;
        return false;
    }
    m_kVersion = StreamVersion(acBuffer);
    if ( !m_kVersion.IsValid() )
    {
        delete[] acBuffer;
        return false;
    }

    // reconstruct the scene graph from the buffer
    iSize -= StreamVersion::LENGTH;
    if ( !Load(acBuffer+StreamVersion::LENGTH,iSize) )
    {
        delete[] acBuffer;
        return false;
    }

    delete[] acBuffer;
    return true;
}
//----------------------------------------------------------------------------
bool Stream::Save (const char* acFilename)
{
    // save the file version to disk
    if ( !System::Save(acFilename,StreamVersion::CURRENT,
         StreamVersion::LENGTH) )
    {
        return false;
    }

    // decompose the scene graph into the buffer
    char* acBuffer;
    int iSize;
    if ( !Save(acBuffer,iSize) )
        return false;

    // save the scene graph from memory to disk
    if ( !System::Append(acFilename,acBuffer,iSize) )
    {
        delete[] acBuffer;
        return false;
    }

    delete[] acBuffer;
    return true;
}
//----------------------------------------------------------------------------
bool Stream::SaveText (const char* acFilename, int iTabSize)
{
    int iCQuantity = GetObjectCount();
    StringTree kRoot(1,iCQuantity);
    kRoot.SetString(0,StringTree::Format(acFilename));

    int iSlot = 0;
    for (int i = 0; i < iCQuantity; i++)
    {
        Object* pkObject = m_apkTopLevel[i];
        assert( pkObject );
        kRoot.SetChild(iSlot++,pkObject->SaveStrings());
    }

    return kRoot.Save(acFilename,iTabSize);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// support functions
//----------------------------------------------------------------------------
bool Stream::InsertInMap (Object* pkObject, Link* pkLink)
{
    return m_kMap.Insert(pkObject,pkLink);
}
//----------------------------------------------------------------------------
Object* Stream::GetFromMap (Object* pkLinkID)
{
    Link** ppkLink = m_kMap.Find(pkLinkID);
    return ( ppkLink ? (*ppkLink)->GetObject() : 0 );
}
//----------------------------------------------------------------------------
void Stream::InsertInOrdered (Object* pkObject)
{
    m_kOrdered.Append(pkObject);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// memory and disk usage
//----------------------------------------------------------------------------
int Stream::GetMemoryUsed () const
{
    // build list of unique objects
    int i;
    for (i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        m_apkTopLevel[i]->Register((Stream&)*this);

    // accumulate object bytes used
    int iSize = 0;
    for (i = 0; i < m_kOrdered.GetQuantity(); i++)
        iSize += m_kOrdered[i]->GetMemoryUsed();

    // clear out the structures (reduce memory, prepare for other calls)
    m_kMap.RemoveAll();
    m_kOrdered.RemoveAll();
    return iSize;
}
//----------------------------------------------------------------------------
int Stream::GetDiskUsed () const
{
    // build list of unique objects
    int i;
    for (i = 0; i < m_apkTopLevel.GetQuantity(); i++)
        m_apkTopLevel[i]->Register((Stream&)*this);

    // accumulate object bytes used
    int iSize = 0;
    for (i = 0; i < m_kOrdered.GetQuantity(); i++)
        iSize += m_kOrdered[i]->GetDiskUsed();

    // clear out the structures (reduce memory, prepare for other calls)
    m_kMap.RemoveAll();
    m_kOrdered.RemoveAll();
    return iSize;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Stream::Link member functions
//----------------------------------------------------------------------------
Stream::Link::Link (Object* pkObject)
{
    m_pkObject = pkObject;
    m_iCurrent = 0;
}
//----------------------------------------------------------------------------
void Stream::Link::Add (Object* pkLinkID)
{
    m_kLinkID.Append(pkLinkID);
}
//----------------------------------------------------------------------------
void Stream::Link::SetObject (Object* pkObject)
{
    m_pkObject = pkObject;
}
//----------------------------------------------------------------------------
Object* Stream::Link::GetObject ()
{
    return m_pkObject;
}
//----------------------------------------------------------------------------
int Stream::Link::GetQuantity () const
{
    return m_kLinkID.GetQuantity();
}
//----------------------------------------------------------------------------
Object* Stream::Link::GetLinkID ()
{
    assert( m_iCurrent < m_kLinkID.GetQuantity() );
    return m_kLinkID[m_iCurrent++];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// read functions
//----------------------------------------------------------------------------
void Stream::Read (Object*& rpkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&rpkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Object** apkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        apkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (bool& rbValue)
{
    char* pcSrc = m_acBuffer + m_iBufferNext;
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    rbValue = (*pcSrc != 0 ? true : false);
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, bool* abValue)
{
    if ( iQuantity > 0 )
    {
        char* acSrc = m_acBuffer + m_iBufferNext;
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        for (int i = 0; i < iQuantity; i++)
            abValue[i] = (acSrc[i] != 0 ? true : false);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (char& rcValue)
{
    char* pcSrc = m_acBuffer + m_iBufferNext;
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    rcValue = *pcSrc;
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, char* acValue)
{
    if ( iQuantity > 0 )
    {
        char* acSrc = m_acBuffer + m_iBufferNext;
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        memcpy(acValue,acSrc,iQuantity);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (unsigned char& rucValue)
{
    unsigned char* pucSrc = (unsigned char*)(m_acBuffer + m_iBufferNext);
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    rucValue = *pucSrc;
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, unsigned char* aucValue)
{
    if ( iQuantity > 0 )
    {
        unsigned char* aucSrc = (unsigned char*)(m_acBuffer + m_iBufferNext);
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        memcpy(aucValue,aucSrc,iQuantity);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (short& rsValue)
{
    m_iBufferNext += System::Read2(m_acBuffer+m_iBufferNext,1,&rsValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, short* asValue)
{
    m_iBufferNext += System::Read2(m_acBuffer+m_iBufferNext,iQuantity,
        asValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (unsigned short& rusValue)
{
    m_iBufferNext += System::Read2(m_acBuffer+m_iBufferNext,1,&rusValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, unsigned short* ausValue)
{
    m_iBufferNext += System::Read2(m_acBuffer+m_iBufferNext,iQuantity,
        ausValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int& riValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&riValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, int* aiValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        aiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (unsigned int& ruiValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&ruiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, unsigned int* auiValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        auiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (long& rlValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&rlValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, long* alValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        alValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (unsigned long& rulValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&rulValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, unsigned long* aulValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        aulValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (float& rfValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,1,&rfValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, float* afValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,iQuantity,
        afValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (double& rdValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,1,&rdValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, double* adValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,iQuantity,
        adValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (String& rkValue)
{
    int iLength;
    Read(iLength);

    if ( iLength > 0 )
    {
        char* acText = m_acBuffer + m_iBufferNext;
        m_iBufferNext += iLength;
        assert( m_iBufferNext <= m_iBufferSize );
        rkValue = String(iLength,acText);
    }
    else
    {
        rkValue = String(0);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, String* akValue)
{
    for (int i = 0; i < iQuantity; i++)
        Read(akValue[i]);
}
//----------------------------------------------------------------------------
void Stream::Read (ColorRGB& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,3,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, ColorRGB* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,3*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (ColorRGBA& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, ColorRGBA* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix2f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix2f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix3f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,9,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix3f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,9*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix4f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,16,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix4f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,16*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Plane3f& rkValue)
{
    Read(rkValue.Normal);
    Read(rkValue.Constant);
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Plane3f* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Read(akValue[i].Normal);
        Read(akValue[i].Constant);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (Quaternionf& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Quaternionf* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector2f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,2,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector2f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,2*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector3f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,3,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector3f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,3*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector4f& rkValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4,
        (float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector4f* akValue)
{
    m_iBufferNext += System::Read4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix2d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix2d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix3d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,9,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix3d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,9*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Matrix4d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,16,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Matrix4d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,16*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Plane3d& rkValue)
{
    Read(rkValue.Normal);
    Read(rkValue.Constant);
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Plane3d* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Read(akValue[i].Normal);
        Read(akValue[i].Constant);
    }
}
//----------------------------------------------------------------------------
void Stream::Read (Quaterniond& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Quaterniond* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector2d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,2,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector2d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,2*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector3d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,3,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector3d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,3*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Vector4d& rkValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4,
        (double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Vector4d* akValue)
{
    m_iBufferNext += System::Read8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Read (Transformation& rkValue)
{
    Read(rkValue.m_kMatrix);
    Read(rkValue.m_kTranslate);
    Read(rkValue.m_kScale);
    Read(rkValue.m_bIsIdentity);
    Read(rkValue.m_bIsRSMatrix);
    Read(rkValue.m_bIsUniformScale);
}
//----------------------------------------------------------------------------
void Stream::Read (int iQuantity, Transformation* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Read(akValue[i].m_kMatrix);
        Read(akValue[i].m_kTranslate);
        Read(akValue[i].m_kScale);
        Read(akValue[i].m_bIsIdentity);
        Read(akValue[i].m_bIsRSMatrix);
        Read(akValue[i].m_bIsUniformScale);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// write functions
//----------------------------------------------------------------------------
void Stream::Write (const Object* pkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&pkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, Object** const apkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        apkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Pointer<Object>& rspkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&rspkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Pointer<Object>* aspkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        aspkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (bool bValue)
{
    char* pcDst = m_acBuffer + m_iBufferNext;
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    *pcDst = (bValue ? 1 : 0);
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const bool* abValue)
{
    if ( iQuantity > 0 )
    {
        char* acDst = m_acBuffer + m_iBufferNext;
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        for (int i = 0; i < iQuantity; i++)
            acDst[i] = (abValue[i] ? 1 : 0);
    }
}
//----------------------------------------------------------------------------
void Stream::Write (char cValue)
{
    char* pcDst = m_acBuffer + m_iBufferNext;
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    *pcDst = cValue;
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const char* acValue)
{
    if ( iQuantity > 0 )
    {
        char* acDst = m_acBuffer + m_iBufferNext;
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        memcpy(acDst,acValue,iQuantity);
    }
}
//----------------------------------------------------------------------------
void Stream::Write (unsigned char ucValue)
{
    unsigned char* pucDst = (unsigned char*)(m_acBuffer + m_iBufferNext);
    m_iBufferNext++;
    assert( m_iBufferNext <= m_iBufferSize );
    *pucDst = ucValue;
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const unsigned char* aucValue)
{
    if ( iQuantity > 0 )
    {
        unsigned char* aucDst = (unsigned char*)(m_acBuffer + m_iBufferNext);
        m_iBufferNext += iQuantity;
        assert( m_iBufferNext <= m_iBufferSize );
        memcpy(aucDst,aucValue,iQuantity);
    }
}
//----------------------------------------------------------------------------
void Stream::Write (short sValue)
{
    m_iBufferNext += System::Write2(m_acBuffer+m_iBufferNext,1,&sValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const short* asValue)
{
    m_iBufferNext += System::Write2(m_acBuffer+m_iBufferNext,iQuantity,
        asValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (unsigned short usValue)
{
    m_iBufferNext += System::Write2(m_acBuffer+m_iBufferNext,1,&usValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const unsigned short* ausValue)
{
    m_iBufferNext += System::Write2(m_acBuffer+m_iBufferNext,iQuantity,
        ausValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&iValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const int* aiValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        aiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (unsigned int uiValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&uiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const unsigned int* auiValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        auiValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (long lValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&lValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const long* alValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        alValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (unsigned long ulValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&ulValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const unsigned long* aulValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        aulValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (float fValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,1,&fValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const float* afValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,iQuantity,
        afValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (double dValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,1,&dValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const double* adValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,iQuantity,
        adValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const String& rkValue)
{
    Write(rkValue.GetLength());
    Write(rkValue.GetLength(),(const char*)rkValue);
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const String* akValue)
{
    for (int i = 0; i < iQuantity; i++)
        Write(akValue[i]);
}
//----------------------------------------------------------------------------
void Stream::Write (const ColorRGB& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,3,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const ColorRGB* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,3*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const ColorRGBA& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const ColorRGBA* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix2f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix2f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix3f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,9,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix3f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,9*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix4f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,16,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix4f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,16*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Plane3f& rkValue)
{
    Write(rkValue.Normal);
    Write(rkValue.Constant);
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Plane3f* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Write(akValue[i].Normal);
        Write(akValue[i].Constant);
    }
}
//----------------------------------------------------------------------------
void Stream::Write (const Quaternionf& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Quaternionf* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector2f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,2,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector2f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,2*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector3f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,3,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector3f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,3*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector4f& rkValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4,
        (const float*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector4f* akValue)
{
    m_iBufferNext += System::Write4(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const float*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix2d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix2d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix3d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,9,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix3d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,9*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Matrix4d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,16,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Matrix4d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,16*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Plane3d& rkValue)
{
    Write(rkValue.Normal);
    Write(rkValue.Constant);
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Plane3d* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Write(akValue[i].Normal);
        Write(akValue[i].Constant);
    }
}
//----------------------------------------------------------------------------
void Stream::Write (const Quaterniond& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Quaterniond* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector2d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,2,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector2d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,2*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector3d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,3,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector3d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,3*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Vector4d& rkValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4,
        (const double*)rkValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Vector4d* akValue)
{
    m_iBufferNext += System::Write8(m_acBuffer+m_iBufferNext,4*iQuantity,
        (const double*)akValue);
    assert( m_iBufferNext <= m_iBufferSize );
}
//----------------------------------------------------------------------------
void Stream::Write (const Transformation& rkValue)
{
    Write(rkValue.m_kMatrix);
    Write(rkValue.m_kTranslate);
    Write(rkValue.m_kScale);
    Write(rkValue.m_bIsIdentity);
    Write(rkValue.m_bIsRSMatrix);
    Write(rkValue.m_bIsUniformScale);
}
//----------------------------------------------------------------------------
void Stream::Write (int iQuantity, const Transformation* akValue)
{
    for (int i = 0; i < iQuantity; i++)
    {
        Write(akValue[i].m_kMatrix);
        Write(akValue[i].m_kTranslate);
        Write(akValue[i].m_kScale);
        Write(akValue[i].m_bIsIdentity);
        Write(akValue[i].m_bIsRSMatrix);
        Write(akValue[i].m_bIsUniformScale);
    }
}
//----------------------------------------------------------------------------
