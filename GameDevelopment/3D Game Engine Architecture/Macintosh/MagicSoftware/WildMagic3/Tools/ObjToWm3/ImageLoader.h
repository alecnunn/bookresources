// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "PlatformSupport.h"
#include "MemFile.h"

// A simple image loader.

class ImageLoader
{
public:
    ImageLoader (const char* acFileName);
    ImageLoader (const std::string& rkFileName);
    ~ImageLoader ();

    enum FileType
    {
        TGA,
        BMP,
        UNKNOWN
    };

    bool Load (int& iWidth, int& iHeight, int& iBitsPerPixel,
        char*& racImageData);

private:
    enum CompressionType
    {
        IL_BI_RGB,
        IL_BI_RLE8,
        IL_BI_RLE4
    };

    void InitLoader (const char* acFileName);

    bool LoadTGA (int& iWidth, int& iHeight, int& iBitsPerPixel,
        char*& rpcImageData);

    bool LoadBMP (int& iWidth, int& iHeight, int& iBitsPerPixel,
        char*& racImageData);

    bool LoadBMP1 (int iWidth, int iHeight, char* acImageData); 
    
    bool LoadBMP24 (int iWidth, int iHeight, char* acImageData);

    FileType m_eType;
    CompressionType m_eCompression;
    MemFile* m_pkFile;
    char m_aacBMPPalette[3][256];
};

#endif

