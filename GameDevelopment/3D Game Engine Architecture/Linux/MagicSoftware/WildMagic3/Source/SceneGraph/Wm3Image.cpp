// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Image.h"
#include "Wm3ImageVersion.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Image,Object);
WM3_IMPLEMENT_STREAM(Image);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Image,Object);

int Image::ms_aiBytesPerPixel[Image::IT_QUANTITY] =
{
    2, 3, 2, 4
};

//----------------------------------------------------------------------------
Image::Image (int eFormat, int iWidth, int iHeight, unsigned char* aucData,
    const char* acImageName, bool bRequirePowerOfTwo)
{
    if ( bRequirePowerOfTwo )
    {
        assert( IsPowerOfTwo(iWidth) && IsPowerOfTwo(iHeight) );
    }

    m_eFormat = eFormat;
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_iQuantity = iWidth*iHeight;
    m_aucData = aucData;

    if ( acImageName )
    {
        SetName(acImageName);
    }
    else
    {
        // Generate the unique image name.
        char acFilename[32];
        sprintf(acFilename,"image%u.mif",GetID());
        SetName(acFilename);
    }
}
//----------------------------------------------------------------------------
Image::Image ()
{
    m_eFormat = IT_QUANTITY;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iQuantity = 0;
    m_aucData = 0;
}
//----------------------------------------------------------------------------
Image::~Image ()
{
    delete[] m_aucData;
}
//----------------------------------------------------------------------------
Image* Image::Load (const char* acFilename, bool bRequirePowerOfTwo)
{
    if ( !acFilename )
        return 0;

    char* acBuffer;
    int iSize;
    bool bLoaded = System::Load(acFilename,acBuffer,iSize);
    if ( !bLoaded )
    {
        // file does not exist
        return 0;
    }
    if ( iSize < ImageVersion::LENGTH )
    {
        // file not large enough to store version string
        delete[] acBuffer;
        return 0;
    }

    // read the file version
    ImageVersion kVersion(acBuffer);
    if ( !kVersion.IsValid() )
    {
        delete[] acBuffer;
        return 0;
    }

    char* pcCurrent = acBuffer + ImageVersion::LENGTH;

    // read the image format and dimensions
    int eFormat, iWidth, iHeight;
    pcCurrent += System::Read4(pcCurrent,1,&eFormat);
    pcCurrent += System::Read4(pcCurrent,1,&iWidth);
    pcCurrent += System::Read4(pcCurrent,1,&iHeight);

    // read the image data
    int iDataSize = ms_aiBytesPerPixel[eFormat]*iWidth*iHeight;
    unsigned char* aucData = new unsigned char[iDataSize];
    System::Read1(pcCurrent,iDataSize,aucData);

    if ( bRequirePowerOfTwo )
    {
        if ( !IsPowerOfTwo(iWidth) || !IsPowerOfTwo(iHeight) )
        {
            FILE* pkFile = fopen("ImageNotPowerOfTwo.txt","wt");
            assert( pkFile );
            fprintf(pkFile,"Image %s\n",acFilename);
            fprintf(pkFile,"An image dimension is not a power of two.\n");
            fprintf(pkFile,"Width = %d  Height = %d\n",iWidth,iHeight);
            fclose(pkFile);
        }
    }

    Image* pkImage = new Image(eFormat,iWidth,iHeight,aucData,acFilename,
        bRequirePowerOfTwo);

    delete[] acBuffer;
    return pkImage;
}
//----------------------------------------------------------------------------
bool Image::Save (const char* acFilename)
{
    if ( !acFilename )
        return false;

    FILE* pkFile = fopen(acFilename,"wb");
    if ( !pkFile )
        return false;

    // write the file version
    System::Write1(pkFile,ImageVersion::LENGTH,ImageVersion::CURRENT);

    // write the image format and dimensions
    System::Write4(pkFile,1,&m_eFormat);
    System::Write4(pkFile,1,&m_iWidth);
    System::Write4(pkFile,1,&m_iHeight);

    // write the image data
    int iDataSize = ms_aiBytesPerPixel[m_eFormat]*m_iQuantity;
    System::Write1(pkFile,iDataSize,m_aucData);

    fclose(pkFile);
    return true;
}
//----------------------------------------------------------------------------
bool Image::IsPowerOfTwo (int iValue)
{
    return (iValue != 0) && ((iValue & -iValue) == iValue);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Image::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_eFormat);
    rkStream.Read(m_iWidth);
    rkStream.Read(m_iHeight);
    rkStream.Read(m_iQuantity);

    bool bHasData;
    rkStream.Read(bHasData);
    if ( bHasData )
    {
        int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iWidth*m_iHeight;
        m_aucData = new unsigned char[iBytes];
        rkStream.Read(iBytes,m_aucData);
    }

    WM3_END_DEBUG_STREAM_LOAD(Image);
}
//----------------------------------------------------------------------------
void Image::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Image::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Image::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_eFormat);
    rkStream.Write(m_iWidth);
    rkStream.Write(m_iHeight);
    rkStream.Write(m_iQuantity);

    bool bHasData = ( m_aucData ? true : false );
    rkStream.Write(bHasData);
    if ( bHasData )
    {
        int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iWidth*m_iHeight;
        rkStream.Write(iBytes,m_aucData);
    }

    WM3_END_DEBUG_STREAM_SAVE(Image);
}
//----------------------------------------------------------------------------
StringTree* Image::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    char* acString = 0;
    switch ( m_eFormat )
    {
    case IT_RGBA4444:
        acString = StringTree::Format("type = RGBA4444");
        break;
    case IT_RGB888:
        acString = StringTree::Format("type = RGB888");
        break;
    case IT_RGBA5551:
        acString = StringTree::Format("type = RGBA5551");
        break;
    case IT_RGBA8888:
        acString = StringTree::Format("type = RGBA8888");
        break;
    default:  // IT_QUANTITY
        break;
    }
    pkTree->SetString(1,acString);

    pkTree->SetString(2,StringTree::Format("width =",m_iWidth));
    pkTree->SetString(3,StringTree::Format("height =",m_iHeight));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Image::GetMemoryUsed () const
{
    int iBaseSize = sizeof(Image) - sizeof(Object);
    int iDynaSize = m_iQuantity*sizeof(m_aucData[0]);
    int iTotalSize = iBaseSize + iDynaSize + Object::GetMemoryUsed();
    return iTotalSize;
}
//----------------------------------------------------------------------------
int Image::GetDiskUsed () const
{
    int iSize = Object::GetDiskUsed() +
        sizeof(m_eFormat) +
        sizeof(m_iWidth) +
        sizeof(m_iHeight) +
        sizeof(m_iQuantity);

    iSize++;  // existence of data stored as Boolean flag
    if ( m_aucData )
    {
        int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iWidth*m_iHeight;
        iSize += iBytes*sizeof(m_aucData[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
