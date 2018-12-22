// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef STREAM3DS_H
#define STREAM3DS_H

#include "Wm3System.h"
#include "Wm3String.h"
#include "Wm3ColorRGB.h"
#include "Wm3ColorRGBA.h"
#include "Wm3Vector3.h"
#include "Wm3Vector2.h"
#include "Wm3Matrix3.h"
using namespace Wm3;

// Loads a 3DS file and treats it logically like a stream.
//
// Reading past the end will just give errors.  Reading before the beginning
// will bump the pointer to the end of the buffer.  This is so that
// the general method of processing a 3ds file (i.e. currentPos < someLimit)
// will fail as elegantly as possible if currentPos gets set to something
// very wrong.

class Stream3ds
{
public:
    Stream3ds ();
    ~Stream3ds ();

    bool Load (const char* acFilename);
    
    // positioning for reads
    void SetRelativePos (int iPos);
    void SetAbsolutePos (int iPos);

    // position information
    bool InvalidPos ();
    int GetPos ();
    int GetMaxPos ();

    // methods to read data at current position
    char ReadChar ();
    String ReadString ();     // null-terminated string read
    int ReadUInt1 ();         // 1 unsigned byte
    int ReadUInt2 ();         // 2 unsigned bytes
    int ReadInt2 ();          // 2 signed bytes
    int ReadInt4 ();          // 4 signed bytes
    int ReadChunkPointer ();  // 4 bytes (absolute pointer)
    Matrix3f ReadMatrix3f ();
    Vector3f ReadVector3f ();
    Vector2f ReadVector2f ();
    float ReadFloat ();
    bool ReadBool ();
    ColorRGBA ReadColorFloat ();
    ColorRGBA ReadColorInt ();

private:
    char* m_acBuffer;
    int m_iBufSize;
    int m_iPos;
};

#endif
