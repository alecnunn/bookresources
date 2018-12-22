// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#ifndef CONVERTTOWMOF_H
#define CONVERTTOWMOF_H

#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MArgList.h>
#include <maya/MFileObject.h>
#include <maya/MFStream.h>

class ConvertToWmof : public MPxFileTranslator
{
public:
    ConvertToWmof();
    ~ConvertToWmof();

    virtual MStatus writer(const MFileObject& file,
        const MString& optionsString, FileAccessMode mode);

    virtual bool haveReadMethod() const;
    virtual bool haveWriteMethod() const;
    virtual MString defaultExtension() const;
    virtual MString filter () const;
    virtual MFileKind identifyFile(const MFileObject& fileName,
        const char* buffer,short size) const;

    static void* creator();
    MStatus doIt(const MArgList& rkArgs);
};

#endif