// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3KEYFRAMECONTROLLER_H
#define WM3KEYFRAMECONTROLLER_H

#include "Wm3Controller.h"
#include "Wm3FloatArray.h"
#include "Wm3QuaternionArray.h"
#include "Wm3Vector3Array.h"

namespace Wm3
{

class WM3_ITEM KeyframeController : public Controller
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    KeyframeController ();
    virtual ~KeyframeController ();

    FloatArrayPtr TranslationTimes;
    Vector3fArrayPtr TranslationData;

    FloatArrayPtr RotationTimes;
    QuaternionfArrayPtr RotationData;

    FloatArrayPtr ScaleTimes;
    FloatArrayPtr ScaleData;

    // animation update
    virtual bool Update (double dAppTime);

protected:
    static void GetKeyInfo (float fCtrlTime, int iQuantity, float* afTime,
        int& riLastIndex, float& rfNormTime, int& ri0, int& ri1);

    Vector3f GetTranslate (float fNormTime, int i0, int i1);
    Matrix3f GetRotate (float fNormTime, int i0, int i1);
    float GetScale (float fNormTime, int i0, int i1);

    // Cached indices for the last found pair of keys used for interpolation.
    // For a sequence of times, this guarantees an O(1) lookup.
    int m_iTLastIndex;
    int m_iRLastIndex;
    int m_iSLastIndex;
};

WM3_REGISTER_STREAM(KeyframeController);
typedef Pointer<KeyframeController> KeyframeControllerPtr;

}

#endif
