// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef EFFECTDATA_H
#define EFFECTDATA_H

#include <fstream>

#include "Wm3String.h"
#include "Wm3Shader.h"
#include "Wm3ShaderConstant.h"
#include "Wm3TArray.h"

using std::ofstream;
using namespace Wm3;

class EffectData
{
public:
    EffectData ();
    virtual ~EffectData();

    // Data setters
    void SetVShader (Shader* pkVShader);
    void SetPShader (Shader* pkPShader);
    void AddVConstant (ShaderConstant* pkConst, int iSize);
    void AddPConstant (ShaderConstant* pkConst, int iSize);
    void AddTexture (String kTexName, int iTexUnit);
    void SetProgram (int iShaderType, int iRend, const String& rkProg);
    void SetName (const String& rkName);

    // Write this effect to a series of files
    bool WriteToFile ();

    static const char* ms_acVShader;
    static const char* ms_acPShader;
    static const char* ms_acShader;
    static const char* ms_acPref;
    static const char* ms_acSep;
    static const char* ms_aacComment1[ShaderConstant::SC_USER_DEFINED+1];
    static const char* ms_aacComment2[ShaderConstant::SC_USER_DEFINED+1];
    static const char* ms_aacConstant[ShaderConstant::SC_USER_DEFINED+1];

protected:
    String m_kName;

    // Shaders (NULL if they don't exist)
    ShaderPtr m_spkVShader;
    ShaderPtr m_spkPShader;

    // Arrays for the size of data (multiples of float(i)f
    TArray<int> m_kVConstSize;
    TArray<int> m_kPConstSize;

    // Parallel arrays for texture info
    TArray<String> m_kTexName;
    TArray<int> m_kTexUnit;

    String m_aakProgram[Shader::ST_QUANTITY][Renderer::RT_QUANTITY];

    // Helper file writing functions
    void WriteBoilerPlate (ofstream& rkFile);
    void WriteGetSetFuncHeads (ofstream& rkFile, const Shader* pkShader, 
        TArray<int>& rkSizes);
    void WriteShaderHeader (ofstream& rkFile, const Shader* pkShader,
        TArray<int>& rkSizes);
    void WriteShaderInline (ofstream& rkFile, const Shader* pkShader,
        TArray<int>& rkSizes);
    void WriteShaderBody (ofstream& rkFile, const Shader* pkShader);
    void WriteEffectHeader (ofstream& rkFile);
    void WriteEffectInline (ofstream& rkFile);
    void WriteEffectBody (ofstream& rkFile);
    void WriteEffectInlineFuncs (ofstream& rkFile, const Shader* pkShader, 
        TArray<int>& rkSizes);

    // Helper String functions
    String GetClassName (const Shader* pkShader);
    String GetParentClass (const Shader* pkShader);
    String GetImageParams ();
};

#endif