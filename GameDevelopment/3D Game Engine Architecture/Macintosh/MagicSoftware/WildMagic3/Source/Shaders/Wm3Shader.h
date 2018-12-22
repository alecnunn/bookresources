// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SHADER_H
#define WM3SHADER_H

#include "Wm3ShaderConstant.h"
#include "Wm3BindInfo.h"
#include "Wm3Renderer.h"

namespace Wm3
{

class WM3_ITEM Shader : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Shader ();

    enum  // ShaderType
    {
        ST_VERTEX_SHADER,
        ST_PIXEL_SHADER,
        ST_QUANTITY
    };
    int GetShaderType () const;

    void SetProgram (const char* acProgram);
    const char* GetProgram () const;
    int GetConstantQuantity () const;
    const ShaderConstantPtr GetConstant (int i) const;
    void AddConstant (ShaderConstant* pkConstant);

protected:
    // streaming
    Shader (int eShaderType = ST_QUANTITY);

    int m_eShaderType;
    const char* m_acProgram;
    TArray<ShaderConstantPtr> m_kConstant;

// internal use
public:
    // store renderer-specific information for binding/unbinding shaders
    BindInfoArray BIArray;
};

WM3_REGISTER_STREAM(Shader);
typedef Pointer<Shader> ShaderPtr;
#include "Wm3Shader.inl"

}

#endif
