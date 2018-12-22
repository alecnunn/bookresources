// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include <windows.h>
#include "WmifToBmp24.h"
using namespace Wm3;
using namespace std;

WM3_CONSOLE_APPLICATION(WmifToBmp24);

//----------------------------------------------------------------------------
void WmifToBmp24::Usage ()
{
    cout << endl;
    cout << "To convert from the Wild Magic Image Format (wmif) to" << endl;
    cout << "one (or two) Windows BITMAP images, use" << endl;
    cout << "    WmifToBmp24 myfile.wmif" << endl;
    cout << "If the wmif file is RGB888, the output file is a" << endl;
    cout << "24-bit BITMAP image named myfile.bmp.  If the wmif" << endl;
    cout << "file is RGBA8888, two output files are generated." << endl;
    cout << "The file myfile.bmp is a 24-bit BITMAP image that" << endl;
    cout << "contains the RGB portion of myfile.wmif.  The file" << endl;
    cout << "myfile.alpha.bmp is also a 24-bit BITMAP image, is" << endl;
    cout << "gray scale (R = G = B), and contains the A portion" << endl;
    cout << "of myfile.wmif.  The converter only supports Wild" << endl;
    cout << "Magic RGB888 and RBGA8888 for now." << endl;
}
//----------------------------------------------------------------------------
int WmifToBmp24::Main (int iArgQuantity, char** apcArgument)
{
    if ( iArgQuantity <= 1 )
    {
        Usage();
        return -1;
    }

    // input file name
    const char* acInputFile = apcArgument[1];

    // split the path to obtain the filename
    char acDrive[_MAX_DRIVE], acDir[_MAX_DIR], acName[_MAX_FNAME];
    char acExt[_MAX_EXT];
    _splitpath(acInputFile,acDrive,acDir,acName,acExt);

    Image* pkImage = Image::Load(acInputFile);
    if ( !pkImage )
        return -3;

    switch ( pkImage->GetFormat() )
    {
    case Image::IT_RGB888:
    {
        char acRGBFile[_MAX_PATH];
        sprintf(acRGBFile,"%s.bmp",acName);
        ofstream kRGBStr(acRGBFile,ios::out|ios::binary);
        if ( !kRGBStr )
        {
            delete pkImage;
            return -4;
        }
        SaveRGBImage(kRGBStr,pkImage);
        kRGBStr.close();
        break;
    }
    case Image::IT_RGBA8888:
    {
        char acRGBFile[_MAX_PATH];
        sprintf(acRGBFile,"%s.bmp",acName);
        ofstream kRGBStr(acRGBFile,ios::out|ios::binary);
        if ( !kRGBStr )
        {
            delete pkImage;
            return -4;
        }

        char acAFile[_MAX_PATH];
        sprintf(acAFile,"%s.alpha.bmp",acName);
        ofstream kAStr(acAFile,ios::out|ios::binary);
        if ( !kAStr )
        {
            delete pkImage;
            return -5;
        }

        SaveRGBAImage(kRGBStr,kAStr,pkImage);
        kRGBStr.close();
        kAStr.close();
        break;
    }
    default:
        return -6;
    }

    delete pkImage;
    return 0;
}
//----------------------------------------------------------------------------
void WmifToBmp24::SaveRGBImage (ofstream& rkRGBStr, const Image* pkImage)
{
    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iQuantity = iWidth*iHeight;
    int iDataBytes = 3*iQuantity;
    const unsigned char* aucData = pkImage->GetData();

    // Reverse byte order (Windows stores BGR (lowest byte to highest byte,
    // MIF stores RGB).
    unsigned char* aucRGBData = new unsigned char[iDataBytes];
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = aucData[3*i  ];
        unsigned char ucG = aucData[3*i+1];
        unsigned char ucR = aucData[3*i+2];
        aucRGBData[3*i  ] = ucR;
        aucRGBData[3*i+1] = ucG;
        aucRGBData[3*i+2] = ucB;
    }

    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42;  // "BM"
    kFileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = iWidth;
    kInfoHeader.biHeight = iHeight;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    rkRGBStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkRGBStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkRGBStr.write((const char*)aucRGBData,iDataBytes*sizeof(unsigned char));
    delete[] aucRGBData;
}
//----------------------------------------------------------------------------
void WmifToBmp24::SaveRGBAImage (ofstream& rkRGBStr, ofstream& rkAStr,
    const Image* pkImage)
{
    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iQuantity = iWidth*iHeight;
    int iDataBytes = 3*iQuantity;
    const unsigned char* aucData = pkImage->GetData();

    // Reverse byte order (Windows stores BGR (lowest byte to highest byte,
    // MIF stores RGB).
    unsigned char* aucRGBData = new unsigned char[iDataBytes];
    unsigned char* aucAData = new unsigned char[iDataBytes];
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = aucData[4*i  ];
        unsigned char ucG = aucData[4*i+1];
        unsigned char ucR = aucData[4*i+2];
        unsigned char ucA = aucData[4*i+3];
        aucRGBData[3*i  ] = ucR;
        aucRGBData[3*i+1] = ucG;
        aucRGBData[3*i+2] = ucB;
        aucAData[3*i  ] = ucA;
        aucAData[3*i+1] = ucA;
        aucAData[3*i+2] = ucA;
    }

    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42;  // "BM"
    kFileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = iWidth;
    kInfoHeader.biHeight = iHeight;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    rkRGBStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkRGBStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkRGBStr.write((const char*)aucRGBData,iDataBytes*sizeof(unsigned char));
    delete[] aucRGBData;

    rkAStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkAStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkAStr.write((const char*)aucAData,iDataBytes*sizeof(unsigned char));
    delete[] aucAData;
}
//----------------------------------------------------------------------------
