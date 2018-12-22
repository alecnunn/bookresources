// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

/*
Usage: CgToWm3 <inputfile> [-forceVS2] [-strip] [-name <name>] 
    [-ventry <entry>] [-pentry <entry>]
*/

#ifndef CGTOWM3_H
#define CGTOWM3_H

#include "CgIncludes.h"
#include "EffectData.h"

#include "Wm3Renderer.h"
#include "Wm3VertexShader.h"
#include "Wm3PixelShader.h"
#include "Wm3TSet.h"

class CgToWm3
{

public:
    CgToWm3 (bool bStripPrefix = true, bool bForceVS2 = false);
    virtual ~CgToWm3 ();

    bool SetData (const char* acFilename, const char* acVEntry,
        const char* acPEntry, const char* acName);

    void WriteToFiles ();

protected:

    // helper functions
    void CreateShaderConstants (Shader* pkShader);
    void RecurseProgramParameters(Shader*, CGparameter kParam);
    ShaderConstant* AddConstant (int iShaderType, const char* acName,
        int iRows, int iCols, int iRegister, int iConstType, int iTypeOption);
    void AddParam (int iShaderType, CGparameter kParam);
    int GetRegister (CGparameter kParam);
    void MatchConstantName (const char* acName, int& riType,
        int& riTypeOption);
    bool TryVersion (CGprofile kProfile, const char* acFilename,
        const char* acEntry, const char** aacArgs);
    void GetSize (CGparameter kParam, int& riRows, int& riCols);

    // state variables
    bool m_bForceVS2;
    bool m_bStrip;

    void DbgInfo (const char* acFormat, ...);
    void DbgWarn (const char* acFormat, ...);
    void FatalError (const char* acMessage);

    // what this converter creates
    CGcontext m_kContext;
    CGprogram m_kProgram;
    EffectData m_kData;

    // if a vertex shader, we need:
    int m_iVertexRegHint;
    int m_iNormalRegHint;
    int m_iColorRegHint;
    int m_aiTexCoordRegHint[8];

    // Keep track of variable names already used per shader tape
    TSet<String> m_akNames[Shader::ST_QUANTITY];

    enum { NONE = -1 };

    static const char* ms_aacConstName[ShaderConstant::SC_QUANTITY];
};

#endif

