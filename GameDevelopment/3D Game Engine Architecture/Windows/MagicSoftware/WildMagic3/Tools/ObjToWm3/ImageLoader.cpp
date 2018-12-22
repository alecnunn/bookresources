// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "ImageLoader.h"

//----------------------------------------------------------------------------
ImageLoader::ImageLoader (const char* acFileName)
{
    InitLoader(acFileName);
}
//----------------------------------------------------------------------------
ImageLoader::ImageLoader (const string& rkFileName)
{
    InitLoader(rkFileName.c_str());
}
//----------------------------------------------------------------------------
ImageLoader::~ImageLoader ()
{
    Delete(m_pkFile);
}
//----------------------------------------------------------------------------
bool ImageLoader::Load (int& iWidth, int& iHeight, int& iBitsPerPixel,
    char*& racImageData)
{
    switch ( m_eType )
    {
        case TGA:
            return LoadTGA(iWidth,iHeight,iBitsPerPixel,racImageData);
        case BMP:
            return LoadBMP(iWidth,iHeight,iBitsPerPixel,racImageData);
    }
    return false;
}
//----------------------------------------------------------------------------
void ImageLoader::InitLoader (const char* acFileName)
{
    if ( strstr(acFileName,".tga") || strstr(acFileName,".TGA") )
    {
        m_eType = TGA;
        m_pkFile = new MemFile(acFileName);
    }
    else if ( strstr(acFileName,".bmp") || strstr(acFileName,".BMP") )
    {
        m_eType = BMP;
        m_pkFile = new MemFile(acFileName,false);
    }
    else
    {
        m_eType = UNKNOWN;
        m_pkFile = 0;
    }
}
//----------------------------------------------------------------------------
bool ImageLoader::LoadTGA (int& iWidth, int& iHeight, int& iBitsPerPixel,
    char*& racImageData)
{
    // Uncompressed TARGA only.

    UInt8 aucFileHeader[12];
    UInt8 aucTGAHeader[12]= {0,0,2,0,0,0,0,0,0,0,0,0};
    UInt8 aucTGADesc[6];

    // Make sure we have enough data, then read and verify TARGA header and
    // image description.
    int iDiff = m_pkFile->Length() - m_pkFile->FilePos();
    if ( iDiff < (sizeof(aucFileHeader) + sizeof(aucTGADesc)) )
        return false;
    m_pkFile->ReadBytes(aucFileHeader,sizeof(aucFileHeader));
    if ( memcmp(aucFileHeader,aucTGAHeader,12) )
        return false;
    m_pkFile->ReadBytes(aucTGADesc,sizeof(aucTGADesc));

    // Extract image description and read in image data.
    iWidth = 256*aucTGADesc[1] + aucTGADesc[0];
    iHeight = 256*aucTGADesc[3] + aucTGADesc[2];
    iBitsPerPixel = aucTGADesc[4];
    int iBytesPerPixel = iBitsPerPixel >> 3;
    int iImageSize = iWidth*iHeight*iBytesPerPixel;

    racImageData = new char[iImageSize];
    if ( !racImageData )
        return false;
    if ( !m_pkFile->ReadBytes((UInt8*)racImageData,iImageSize) )
        return false;

    // Swap R and B components.
    UInt8* pcBuffer = (UInt8*)racImageData;
    for (int i = 0; i < iImageSize; i += iBytesPerPixel)
    {
        UInt8 uiByte = pcBuffer[i];
        pcBuffer[i] = pcBuffer[i+2];
        pcBuffer[i+2] = uiByte;
    }

    return true;
}
//----------------------------------------------------------------------------
bool ImageLoader::LoadBMP (int& iWidth, int& iHeight, int& iBitsPerPixel,
    char*& racImageData)
{
    UInt8 uiMarker1 = m_pkFile->ReadUInt8();
    UInt8 uiMarker2 = m_pkFile->ReadUInt8();
    if ( uiMarker1 != 66 || uiMarker2 != 77 )
        return false;

    UInt32 uiFileSize = m_pkFile->ReadUInt32();
    m_pkFile->ReadUInt16();
    m_pkFile->ReadUInt16();
    UInt32 uiOffset = m_pkFile->ReadUInt32();

    UInt32 uiHeaderSize = m_pkFile->ReadUInt32();
    if ( uiHeaderSize < 40 )
        return false;

    iWidth = m_pkFile->ReadUInt32();
    iHeight = m_pkFile->ReadUInt32();
    UInt16 uiPlanes = m_pkFile->ReadUInt16();
    if ( uiPlanes != 1 )
        return false;

    iBitsPerPixel = m_pkFile->ReadUInt16();
    if ( (iBitsPerPixel == 1 || iBitsPerPixel == 4 || iBitsPerPixel == 8 ||
         iBitsPerPixel == 24) == false )
    {
        return false;
    }
        
    m_eCompression = (CompressionType)m_pkFile->ReadUInt32();
    if ( (m_eCompression == IL_BI_RGB || m_eCompression == IL_BI_RLE8 ||
         m_eCompression == IL_BI_RLE4) == false )
    {
        return false;
    }

    UInt32 uiImageSize = m_pkFile->ReadUInt32();
    m_pkFile->ReadUInt32();
    m_pkFile->ReadUInt32();
    m_pkFile->ReadUInt32();
    m_pkFile->ReadUInt32();

    if ( iBitsPerPixel != 24 )
    {
        int iNumColors = 1 << iBitsPerPixel;
        for (int i = 0; i < iNumColors; i++)
        {
            m_aacBMPPalette[0][i] = m_pkFile->ReadUInt8();
            m_aacBMPPalette[1][i] = m_pkFile->ReadUInt8();
            m_aacBMPPalette[2][i] = m_pkFile->ReadUInt8();
            m_pkFile->ReadUInt8();
        }
    }

    racImageData = new char[3*iWidth*iHeight];
    if ( !racImageData )
        return false;

    while ( m_pkFile->FilePos() < (int)uiOffset )
        m_pkFile->ReadUInt8();

    switch ( iBitsPerPixel )
    {
        case 1:
            return LoadBMP1(iWidth,iHeight,racImageData);
        case 24:
            return LoadBMP24(iWidth,iHeight,racImageData);
    }

    return false;
}
//----------------------------------------------------------------------------
bool ImageLoader::LoadBMP1 (int iWidth, int iHeight, char* acImageData)
{
    if ( m_eCompression != IL_BI_RGB )
        return false;

    int iThreeWidth = 3*iWidth;
    for (int iY = 0; iY < iHeight; iY++)
    {
        int iIndex = iThreeWidth*iY;
        for (int iX = 0; iX < iWidth; iX++)
        {
            UInt8 uiByteRead;
            if ( iX % 8 == 0 )
                uiByteRead = m_pkFile->ReadUInt8();

            UInt8 uiColor = (uiByteRead >> (7 - (iX % 8))) & 1;
            int iBase = iIndex + 3*iX;
            acImageData[iBase++] = m_aacBMPPalette[0][uiColor];
            acImageData[iBase++] = m_aacBMPPalette[1][uiColor];
            acImageData[iBase  ] = m_aacBMPPalette[2][uiColor]; 
        }

        while ( m_pkFile->FilePos() % 4 != 0 )
            m_pkFile->ReadUInt8();
    }

    return false;
}
//----------------------------------------------------------------------------
bool ImageLoader::LoadBMP24 (int iWidth, int iHeight, char* acImageData)
{
    if ( m_eCompression != IL_BI_RGB )
        return false;

    int iThreeWidth = 3*iWidth;
    int iSkip = iThreeWidth % 4;
    for (int iY = 0; iY < iHeight; iY++)
    {
        int iIndex = iThreeWidth*iY;
        for (int iX = 0; iX < iWidth; iX++)
        {
            int iBase = iIndex + 3*iX + 2;
            acImageData[iBase--] = m_pkFile->ReadUInt8();
            acImageData[iBase--] = m_pkFile->ReadUInt8();
            acImageData[iBase  ] = m_pkFile->ReadUInt8();
        }                                             

        for (int i = 0; i < iSkip; i++)
            m_pkFile->ReadUInt8();
    }
    return true;
}
//----------------------------------------------------------------------------

