// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IMAGE_H
#define WM3IMAGE_H

#include "Wm3Object.h"

namespace Wm3
{

class WM3_ITEM Image : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    enum // Format
    {
        IT_RGBA4444,
        IT_RGB888,
        IT_RGBA5551,
        IT_RGBA8888,
        IT_QUANTITY
    };

    // Construction and destruction.  Wm3Image accepts responsibility for
    // deleting the input array.  The Wm3Object::m_acName field is used as a
    // unique identifier for the image for purposes of sharing.  The caller of
    // the constructor may provided a name.  If not, the constructor generates
    // a unique name "imageN.wmif" where N is the Wm3Object::m_uiID field. A
    // global map of images is maintained for sharing purposes.
    Image (int eFormat, int iWidth, int iHeight, unsigned char* aucData,
        const char* acImageName = 0, bool bRequirePowerOfTwo = true);
    virtual ~Image ();

    // member access
    int GetFormat () const;
    int GetBytesPerPixel () const;
    int GetWidth () const;
    int GetHeight () const;
    int GetQuantity () const;
    unsigned char* GetData () const;
    unsigned char* operator() (int i);

    // Streaming support.  The sharing system is automatically invoked by
    // these calls.  In Load, if an image corresponding to the filename is
    // already in memory, then that image is returned (i.e. shared).
    // Otherwise, a new image is created and returned.  The filename is used
    // as the image name.
    static Image* Load (const char* acFilename,
        bool bRequirePowerOfTwo = true);

    // This is intended to support saving procedurally generated images or
    // for utilities that convert to WMIF from another format.  The filename
    // in this call does not replace the image name that might already exist.
    bool Save (const char* acFilename);

protected:
    // support for streaming
    Image ();

    static bool IsPowerOfTwo (int iValue);

    int m_eFormat;
    int m_iWidth, m_iHeight, m_iQuantity;
    unsigned char* m_aucData;

    static int ms_aiBytesPerPixel[IT_QUANTITY];
};

WM3_REGISTER_STREAM(Image);
typedef Pointer<Image> ImagePtr;
#include "Wm3Image.inl"

}

#endif
