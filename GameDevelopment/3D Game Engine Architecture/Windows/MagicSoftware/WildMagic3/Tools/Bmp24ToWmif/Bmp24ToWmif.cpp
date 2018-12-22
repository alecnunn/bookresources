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
#include "Bmp24ToWmif.h"
using namespace Wm3;
using namespace std;

WM3_CONSOLE_APPLICATION(Bmp24ToWmif);

//----------------------------------------------------------------------------
void Bmp24ToWmif::Usage ()
{
    cout << endl;
    cout << "To convert Windows BITMAP format to Wild Magic Image" << endl;
    cout << "Format (wmif), use" << endl;
    cout << "    Bmp24ToWmif myfile.bmp [myfile.alpha.bmp]" << endl;
    cout << "The BITMAP myfile.bmp is a 24-bit image that stores RGB" << endl;
    cout << "components.  If you want an alpha channel, supply" << endl;
    cout << "myfile.alpha.bmp, a 24-bit BITMAP image that is gray" << endl;
    cout << "scale (R = G = B).  The common channel value is used as" << endl;
    cout << "the alpha channel of the output image, myfile.wmif." << endl;
}
//----------------------------------------------------------------------------
int Bmp24ToWmif::Main (int iArgQuantity, char** apcArgument)
{
    if ( iArgQuantity <= 1 )
    {
        Usage();
        return -1;
    }

    // get RGB input file
    const char* acRGBFile = apcArgument[1];
    int iRGBWidth, iRGBHeight;
    unsigned char* aucRGBData;
    int iErrorCode = GetImage(acRGBFile,iRGBWidth,iRGBHeight,aucRGBData);
    if ( iErrorCode != 0 )
    {
        delete[] aucRGBData;
        return iErrorCode;
    }

    // split the path to get the file name
    char acDrive[_MAX_DRIVE], acDir[_MAX_DIR], acName[_MAX_FNAME];
    char acExt[_MAX_EXT], acWmifName[_MAX_PATH];
    _splitpath(acRGBFile,acDrive,acDir,acName,acExt);
    sprintf(acWmifName,"%s.wmif",acName);

    if ( iArgQuantity <= 2 )
    {
        // create and save the RGB image to disk
        Image* pkRGBImage = new Image(Image::IT_RGB888,iRGBWidth,iRGBHeight,
            aucRGBData,NULL,false);
        if ( !pkRGBImage )
        {
            delete[] aucRGBData;
            return -4;
        }
        if ( !pkRGBImage->Save(acWmifName) )
        {
            delete pkRGBImage;
            return -5;
        }
        delete pkRGBImage;
        return 0;
    }

    // get A input file
    const char* acAFile = apcArgument[2];
    int iAWidth, iAHeight;
    unsigned char* aucAData;
    iErrorCode = GetImage(acAFile,iAWidth,iAHeight,aucAData);
    if ( iErrorCode != 0 )
    {
        delete[] aucRGBData;
        delete[] aucAData;
        return iErrorCode;
    }
    if ( iAWidth != iRGBWidth || iAHeight != iRGBHeight )
    {
        delete[] aucRGBData;
        delete[] aucAData;
        return -6;
    }

    int iWidth = iRGBWidth, iHeight = iRGBHeight;
    int iQuantity = iWidth*iHeight;
    unsigned char* aucData = new unsigned char[4*iQuantity];
    for (int i = 0; i < iQuantity; i++)
    {
        aucData[4*i  ] = aucRGBData[3*i  ];
        aucData[4*i+1] = aucRGBData[3*i+1];
        aucData[4*i+2] = aucRGBData[3*i+2];
        aucData[4*i+3] = aucAData[3*i];  // use the R channel for alpha
    }
    delete[] aucRGBData;
    delete[] aucAData;

    Image* pkRGBAImage = new Image(Image::IT_RGBA8888,iWidth,iHeight,
        aucData,NULL,false);
    if ( !pkRGBAImage )
    {
        delete[] aucData;
        return -7;
    }
    if ( !pkRGBAImage->Save(acWmifName) )
    {
        delete pkRGBAImage;
        return -8;
    }
    delete pkRGBAImage;
    return 0;
}
//----------------------------------------------------------------------------
int Bmp24ToWmif::GetImage (const char* acBMPName, int& riWidth,
    int& riHeight, unsigned char*& raucData)
{
    riWidth = 0;
    riHeight = 0;
    raucData = NULL;

    HBITMAP hImage = (HBITMAP) LoadImage(NULL,acBMPName,IMAGE_BITMAP,0,0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if ( !hImage )
        return -2;

    DIBSECTION dibSection;
    GetObject(hImage,sizeof(DIBSECTION),&dibSection);

    riWidth = dibSection.dsBm.bmWidth;
    riHeight = dibSection.dsBm.bmHeight;
    int iQuantity = dibSection.dsBm.bmWidth*dibSection.dsBm.bmHeight;
    if ( dibSection.dsBm.bmBitsPixel != 24 )
        return -3;

    // allocate the WMIF image
    raucData = new unsigned char[3*iQuantity];

    // Windows BMP stores BGR, need to invert to RGB.
    unsigned char* aucSrc = (unsigned char*)dibSection.dsBm.bmBits;
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = aucSrc[3*i  ];
        unsigned char ucG = aucSrc[3*i+1];
        unsigned char ucR = aucSrc[3*i+2];
        raucData[3*i  ] = ucR;
        raucData[3*i+1] = ucG;
        raucData[3*i+2] = ucB;
    }

    DeleteObject(hImage);

    return 0;
}
//----------------------------------------------------------------------------
