// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2003.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "Wm3Command.h"
#include "CgToWm3.h"
#include <iostream>

using namespace Wm3;

/*
    Possible fixes:  Right now the -forceVS2 option is a horrible ugly
    kludge.  The CG run-time will gladly compile programs of any length, even
    if DirectX will not compile them due to length limitations (128 for
    vs_1_1)  However, cgc.exe will issue a warning (but not an error??) for
    it, but there is no way to get this warning.  Even -strict does not force
    the error to become a warning.  Thus, there's no real way to tell,
    (aside from grepping the output for the "// %i instructions" line and
    comparing with some preset values in the TryVersion function) to say
    that vs1.1 will not work for a given program.  That would be just as
    ugly a hack, if not uglier.  This is slightly more elegant.  Right now
    it just doesn't try vs1.1 if you have the -forceVS2 option.  Hopefully
    you're running cgc before cgconverter in your build file anyway and you
    might catch the warning as it flies by.  You'll know when the file won't
    compile on DirectX, too.  :P

    If there is a way to do it automagically and elegantly, I think that 
    would be a much better solution.
*/

// The CgToWm3 class reads in the options, compiles the programs,
// and then enumerates the constants.  As it creates the constants,
// it adds them to its EffectData member variable.  Once all the information
// about the program has been added, the EffectData class takes over
// and writes all that information back out to files.

// Constant names to be used inside the CG program.
const char* CgToWm3::ms_aacConstName[ShaderConstant::SC_QUANTITY] =
{
        "TransformM",
        "TransformP",
        "TransformMV",
        "TransformMVP",
        "CameraPosition",
        "CameraDirection",
        "CameraUp",
        "CameraRight",
        "FogColor",
        "FogParams",
        "MaterialEmissive",
        "MaterialAmbient",
        "MaterialDiffuse",
        "MaterialSpecular",
        "MaterialShininess",
        "LightPosition",
        "LightDirection",
        "LightAmbient",
        "LightDiffuse",
        "LightSpecular",
        "LightSpotCutoff",
        "LightAttenParams",
        ""
};

//----------------------------------------------------------------------------
void print_usage()
{
    printf("Usage:\nCgToWm3 <inputfile> [-forceVS2] [-nostrip] [-name <name>]"
        "[-ventry <entry>] [-pentry <entry>]\n\n"
        "inputfile: Cg filename to be processed (not optional)\n"
        "-name: Change the name of the effect (default is from inputfile)\n"
        "-forceVS2: Force DX vertex shaders to be VS2.0\n"
        "-nostrip: Don't strip Hungarian prefixes [a,f,k] from variables\n"
        "-ventry <entry>: Vertex shader main is <entry> instead of 'vmain'\n"
        "-pentry <entry>: Pixel shader main is <entry> instead of 'pmain'\n");
}
//----------------------------------------------------------------------------
int main(int iArgc, char** aacArgv)
{
    // get the args
    Command kCommand(iArgc, aacArgv);

    char* acFilename = NULL;

    if ( kCommand.Boolean("help") )
    {
        print_usage();
        return -1;
    }

    if ( !kCommand.Filename(acFilename) )
    {
        printf("Error: No inputfile\n");
        print_usage();
        return -1;
    }

    char* acVEntry = NULL;
    kCommand.String("ventry",acVEntry);
    if (!acVEntry)
    {
        acVEntry = new char[6];
        strcpy(acVEntry,"vmain");
    }

    char* acPEntry = NULL;
    kCommand.String("pentry",acPEntry);
    if (!acPEntry)
    {
        acPEntry = new char[6];
        strcpy(acPEntry,"pmain");
    }

    char* acName = NULL;
    kCommand.String("name",acName);
    if (!acName)
    {
        // NOTE: If the filename is something.cg, then
        // make "something" the filename.  There
        // is absolutely no validation here that this
        // is an acceptable file/class name, so
        // use the default at your own peril.

        // find the extension in the filename
        int iLen = (int)strlen(acFilename);
        for (int iC = iLen; iC >= 0; iC--)
        {
            if (acFilename[iC] == '.')
                break;
        }

        // manually copy since we only want a substring
        acName = new char[iC+1];
        for (int i = 0; i < iC; i++)
        {
            acName[i] = acFilename[i];
        }
        acName[iC] = 0;

        // Capitalize first letter, just in case.
        acName[0] = toupper(acName[0]);
    }

    bool bForceVS2 = (kCommand.Boolean("forceVS2") ? true : false);
    bool bStrip = (kCommand.Boolean("nostrip") ? false : true);

    // do all the work
    CgToWm3 kConv(bStrip,bForceVS2);

    if (kConv.SetData (acFilename, acVEntry, acPEntry, acName))
        kConv.WriteToFiles();
    else
        printf ("Warning: No shaders created--not writing to files.");

    // clean up
    if (acVEntry)
        delete[] acVEntry;
    if (acPEntry)
        delete[] acPEntry;
    if (acFilename)
        delete[] acFilename;
    if (acName)
        delete[] acName;
 
    return 0;
}
//----------------------------------------------------------------------------
CgToWm3::CgToWm3 (bool bStrip, bool bForceVS2)
{
    // Initialize data
    m_iVertexRegHint = CgToWm3::NONE;
    m_iNormalRegHint = CgToWm3::NONE;
    m_iColorRegHint = CgToWm3::NONE;
    m_bForceVS2 = bForceVS2;
    m_bStrip = bStrip;

    for (int i = 0; i < 8; i++)
    {
        m_aiTexCoordRegHint[i] = CgToWm3::NONE;
    }
}
//----------------------------------------------------------------------------
bool CgToWm3::SetData (const char* acFilename, const char* acVEntry,
    const char* acPEntry, const char* acName)
{
    // Set up the EffectData member variable by compiling the program
    // into the most restrictive shader version and then enumerating all
    // of the constants into the EffectData class.

    // Return if this was successful.  If at least one shader gets
    // created, consider that a success.
    bool bSuccess = false;

    // We're going to pass this as a compiler option to all DX compiles
    // This is because DX9 requires this option.
    const char * aacArgs[] = { "-profileopts dcls", "-strict", 0 };

    // create context
    m_kContext = cgCreateContext();
    if (!cgIsContext( m_kContext) )
    {
        FatalError( "Could not create CG context." );
    }

    m_kData.SetName(acName);

    // VERTEX SHADER

    // try to compile program with most lenient profile
    // using VS_2_X will not limit registers/instructions, 
    // will not allow GL state in the code
    m_kProgram = cgCreateProgramFromFile(m_kContext, CG_SOURCE, 
        acFilename, CG_PROFILE_VS_2_X, acVEntry, aacArgs );

    // make sure it compiled
    if (!cgIsProgram(m_kProgram))
    {
        DbgWarn( "Could not compile under profile "
            "VS_2_X (trying ARBVP1). %s..\n", 
            cgGetErrorString(cgGetError()));

        cgDestroyProgram(m_kProgram);

        m_kProgram = cgCreateProgramFromFile(m_kContext, CG_SOURCE, 
            acFilename, CG_PROFILE_ARBVP1, acVEntry, NULL );

        if (!cgIsProgram( m_kProgram ) )
        {
            DbgWarn( "Could not compile under profile "
                "ARBVP1 either. %s..\n", 
                cgGetErrorString(cgGetError()));

            DbgWarn("Not creating vertex shader\n");
        }
    }

    if (cgIsProgram(m_kProgram))
    {
        bSuccess = true;

        DbgInfo("Parameter listing (Vertex): \n");

        // build constants
        VertexShader* pkShader = new VertexShader();
        m_kData.SetVShader(pkShader);
        CreateShaderConstants(pkShader);

        DbgInfo("Compiled Program Listing:\n");

        // make sure that if texture coordinate i is given, that
        // texture coordinates 0-(i-1) are also given.
        for (int iMaxTexCoord = 7; iMaxTexCoord >=0; iMaxTexCoord--)
        {
            if (m_aiTexCoordRegHint[iMaxTexCoord] != CgToWm3::NONE)
                break;
        }

        for (int k = 0; k < iMaxTexCoord; k++)
        {
            // due to the way shaders/wild magic works the texture coordinates
            // must all fall consecutively in a range 0-n, where 0 <= n <= 7.
            if (m_aiTexCoordRegHint[k] == CgToWm3::NONE)
            {
                FatalError("Must use sequential tex coords.");
            }
        }
        cgDestroyProgram(m_kProgram);

        // Now, compile the program and find the minimal version that it
        // will run on under each API.

        // OpenGL has only one version
        if (TryVersion(CG_PROFILE_ARBVP1, acFilename, acVEntry, NULL))
        {
            m_kData.SetProgram(Shader::ST_VERTEX_SHADER,Renderer::RT_OPENGL,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("OPENGL-ARBVP1 profile created\n");
        }
        else
        {
            DbgWarn("Program will not work under OPENGL.\n");
        }
        cgDestroyProgram(m_kProgram);

        // DirectX has a few possible versions
        if (!m_bForceVS2 && TryVersion(CG_PROFILE_VS_1_1, acFilename, 
            acVEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_VERTEX_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-VS 1.1 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_VS_2_0, acFilename, acVEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_VERTEX_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-VS 2.0 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_VS_2_X, acFilename, acVEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_VERTEX_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgWarn("VS 2.X profiles may not work with every graphics "
                "card.\n");
            DbgInfo("DX-VS 2.X profile created\n");
        }
        else
        {
            DbgWarn("Program will not work under DX.\n");
        }

        cgDestroyProgram(m_kProgram);

    }

    DbgInfo("\n");

    // PIXEL SHADER

    // try to compile program with most lenient profile
    // using PS_2_X will not limit registers/instructions, 
    // will not allow GL state in the code
    m_kProgram = cgCreateProgramFromFile(m_kContext, CG_SOURCE, 
        acFilename, CG_PROFILE_PS_2_X, acPEntry, aacArgs );

    // make sure it compiled
    if (!cgIsProgram(m_kProgram))
    {
        DbgWarn("Could not compile under profile "
            "PS_2_X (trying ARBFP1). %s..\n", 
            cgGetErrorString(cgGetError()));

        cgDestroyProgram(m_kProgram);

        m_kProgram = cgCreateProgramFromFile(m_kContext, CG_SOURCE, 
            acFilename, CG_PROFILE_ARBFP1, acPEntry, NULL );

        if (!cgIsProgram(m_kProgram))
        {
            DbgWarn("Could not compile under profile "
                "ARBFP1 either. %s..\n", 
                cgGetErrorString(cgGetError()));
            DbgWarn("Cannot compile pixel shader under any profile." );
        }
    }

    if (cgIsProgram(m_kProgram))
    {
        bSuccess = true;

        // build constants
        PixelShader* pkShader = new PixelShader();
        m_kData.SetPShader(pkShader);
        CreateShaderConstants(pkShader);

        DbgInfo("Compiled Program Listing:\n");

        // Now, compile the program and find the minimal version that it
        // will run on under each API.

        // OpenGL has only one version
        if (TryVersion(CG_PROFILE_ARBFP1, acFilename, acPEntry, NULL))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_OPENGL,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("OPENGL-ARBFP1 profile created\n");
        }
        else
        {
            DbgWarn("Program will not work under OPENGL.\n" );
        }

        cgDestroyProgram( m_kProgram );

        // DirectX has a few possible versions
        
        if (TryVersion(CG_PROFILE_PS_1_1, acFilename, acPEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-PS 1.1 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_PS_1_2, acFilename, acPEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-PS 1.2 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_PS_1_3, acFilename, acPEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-PS 1.3 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_PS_2_0, acFilename, acPEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgInfo("DX-PS 2.0 profile created\n");
        }
        else if (TryVersion(CG_PROFILE_PS_2_X, acFilename, acPEntry, aacArgs))
        {
            m_kData.SetProgram(Shader::ST_PIXEL_SHADER,Renderer::RT_DIRECT3D,
                cgGetProgramString(m_kProgram, CG_COMPILED_PROGRAM));
            DbgWarn("PS 2.X profiles may not work with "\
                "every graphics card.\n" );
            DbgWarn("DX-PS 2.X profile created\n");
        }
        else
        {
            DbgWarn("Warning!!!Program will not work under DX.\n" );
        }

        cgDestroyProgram( m_kProgram );
    }

    DbgInfo( "\n");
    
    return bSuccess;
}
//----------------------------------------------------------------------------
CgToWm3::~CgToWm3 ()
{
    if (cgIsContext(m_kContext))
    {
        cgDestroyContext(m_kContext);
    }

    if (cgIsProgram(m_kProgram))
    {
        cgDestroyProgram(m_kProgram);
    }

}
//----------------------------------------------------------------------------
void CgToWm3::CreateShaderConstants (Shader* pkShader)
{
    RecurseProgramParameters(pkShader,
        cgGetFirstParameter(m_kProgram, CG_PROGRAM));

    DbgInfo( "Total User/State Constants: %i\n", 
        pkShader->GetConstantQuantity() );
}
//----------------------------------------------------------------------------
void CgToWm3::RecurseProgramParameters(Shader* pkShader,
    CGparameter kParam)
{
    if (kParam == 0)
        return;

    int i;
    CGparameter kParamR;

    do
    {
        switch(cgGetParameterType(kParam))
        {
        case CG_STRUCT:
            RecurseProgramParameters(pkShader, 
                cgGetFirstStructParameter(kParam));
            break;
        case CG_ARRAY:
            kParamR = kParam;

            int iAck;
            // Recurse until we hit a non-array element
            for (int d = 0; d < cgGetArrayDimension(kParam); d++)
            {
                kParamR = cgGetArrayParameter(kParamR, 0);
                iAck = cgGetParameterType(kParamR);
            }

            // If this is a uniform parameter and the array is made
            // up of just simple elements (and not structs) then
            // the array as a whole will be the constant.
            if (cgGetParameterVariability(kParam) == CG_UNIFORM &&
                cgGetParameterType(kParamR) != CG_ARRAY &&
                cgGetParameterType(kParamR) != CG_STRUCT)
            {
                AddParam(pkShader->GetShaderType(),kParam);
            }
            else
            {
                // Otherwise, break the array up into separate constants.
                for (int d = 0; d < cgGetArrayDimension(kParam); d++)
                {
                    for (i = 0; i < cgGetArraySize(kParam, d); ++i)
                    {
                        RecurseProgramParameters(pkShader,
                            cgGetArrayParameter(kParam, i));
                    }
                }
            }
            break;
        default:
            // handle the single parameter

            // sanity check
            if (!cgIsParameter(kParam))
            {
                FatalError("Not a parameter.");
            }
            // make sure it is actually used in the program
            if (cgIsParameterReferenced(kParam))
            {
                AddParam(pkShader->GetShaderType(),kParam);
            }
            break;
        }
    } while((kParam = cgGetNextParameter(kParam))!= 0);
}

//----------------------------------------------------------------------------
ShaderConstant* CgToWm3::AddConstant (int iShaderType, const char* acName,
    int iRows, int iCols, int iRegister, int iConstType, int iTypeOption)
{
    // This function will edit the name to be a unique name without
    // brackets and Hungarian prefixes.  It will also add this
    // variable into the EffectData member object.

    // Create the constant
    ShaderConstant* pkConst = new ShaderConstant(iConstType,
        iTypeOption,iRegister,iRows);

    // Create a unique, valid name for this variable.
    // First:
    // 1) Count how many bracket pairs exist
    // 2) Count what the first "non-prefix" char is (where prefix is defined
    // by the regular expression: "^a*[fk]").
    int iBrackets = 0;
    int iNonPrefix = 0;
    for (int i = 0; i < (int)strlen(acName); i++)
    {
        if (acName[i] == ']')
            iBrackets++;

        // If still considering the prefix...
        if (m_bStrip && i == iNonPrefix)
        {
            // If not the first character, then the last character must
            // be an 'a'.  Also, the current character must be an a,f,
            // or k to be part of the prefix.
            if ((i == 0 || acName[i-1] == 'a') && 
                acName[i] == 'a' || acName[i] == 'f' || acName[i] == 'k')
            {
                // If part of the prefix
                iNonPrefix++;
            }
        }
    }

    // Remove brackets and prefix
    // Turn "aakName[0][2]" into "Name_0_2"
    int iOrigLen = (int)strlen(acName)+1;
    int iNewLen = iOrigLen - iBrackets;
    char* acEdit = new char[iNewLen];

    // character count for new string
    int iC = 0;

    // Walk through original string, starting
    // after prefix characters, replacing "[" and "]"
    // with "_" and "" respectively.
    for (int i = iNonPrefix; i < iOrigLen; i++)
    {
        if (acName[i] == '[')
            acEdit[iC++] = '_';
        else if (acName[i] != ']')
            acEdit[iC++] = acName[i];
    }

    // Make first letter uppercase, regardless.
    acEdit[0] = toupper(acEdit[0]);
    // Create new string, clean up memory
    String kName = String(acEdit);
    delete[] acEdit;

    // Names may already exist due to prefix or bracket
    // stripping.
    if (m_akNames[iShaderType].Exists(kName))
    {
        // If "name" exists, try "name_2" through "name_999"...
        int iCount = 2;
        char acCount[5];
        do
        {
            sprintf(acCount,"_%i",iCount);
            String kNewName = kName + String(acCount);

            if (!m_akNames[iShaderType].Exists(kNewName))
            {
                m_akNames[iShaderType].InsertNoCheck(kNewName);
                pkConst->SetName(kNewName);
                break;
            }
        } while (++iCount < 1000);

        if (iCount >= 1000)
        {
            // If too many variables...(which should be impossible).
            FatalError ("Too many variables of the same name.");
        }
    }
    else
    {
        // Doesn't exist, so insert and set name
        m_akNames[iShaderType].InsertNoCheck(kName);
        pkConst->SetName(kName);
    }

    // Finally, store this constant.
    if (iShaderType == Shader::ST_PIXEL_SHADER)
    {
        m_kData.AddPConstant(pkConst,iCols);
    }
    else if (iShaderType == Shader::ST_VERTEX_SHADER)
    {
        m_kData.AddVConstant(pkConst,iCols);
    }
    else
    {
        FatalError("Unknown shader type!");
    }

    return pkConst;
}
//----------------------------------------------------------------------------
void CgToWm3::AddParam (int iShaderType, CGparameter kParam)
{
    CGresource kResource;
    const char* acName = cgGetParameterName(kParam);

    int iArraySize = 1;
    if (cgGetParameterType(kParam) == CG_ARRAY)
    {
        int d;

        // Find the size of this array
        for (d = 0; d < cgGetArrayDimension(kParam); d++)
        {
            iArraySize *= cgGetArraySize(kParam, d);
        }

        // Walk down until we hit a non-array element
        while(cgGetArrayDimension(kParam) > 0)
            kParam = cgGetArrayParameter(kParam, 0);

        // This shouldn't be a struct or an array (checked above
        // in RecurseParameter(...).
        assert (cgGetParameterType(kParam) != CG_ARRAY &&
            cgGetParameterType(kParam) != CG_STRUCT);
    }

    int iTemp, iTemp2, iRegister, iRows, iCols, iTypeOption;
    int iConstType = ShaderConstant::SC_USER_DEFINED;
    ShaderConstant* pkTemp;

    // We don't need to handle out parameters (at this point)
    // DX and GL handle both of these automatically
    if (cgGetParameterDirection(kParam) == CG_OUT)
    {
        DbgInfo("Output (%s), ignored\n", acName);
        return;
    }

    switch (cgGetParameterVariability(kParam))
    {
        case CG_VARYING:
            DbgInfo("Varying (%s), ", acName);

            kResource = cgGetParameterBaseResource(kParam);

            if (kResource == CG_TEXCOORD0)
            {
                iTemp = cgGetParameterResourceIndex(kParam);

                iRegister = GetRegister(kParam);
                m_aiTexCoordRegHint[iTemp] = iRegister;
                DbgInfo("TEXCOORD%i (v%i)\n", iTemp, iRegister);
            }
            else if (kResource == CG_POSITION0)
            {
                iTemp = cgGetParameterResourceIndex(kParam);
                // only position 0 is accepted in WML
                if (iTemp)
                {
                    FatalError("May only use Position0 or Position.");
                }

                iRegister = GetRegister(kParam);
                m_iVertexRegHint = iRegister;
                DbgInfo("POSITION (v%i)\n", iRegister);
            }
            else if (kResource == CG_NORMAL0)
            {
                iTemp = cgGetParameterResourceIndex(kParam);
                // only normal 0 is accepted in WML
                if (iTemp)
                {
                    FatalError("May only use Normal0 or Normal.");
                }
                iRegister = GetRegister(kParam);
                m_iNormalRegHint = iRegister;
                DbgInfo("NORMAL (v%i)\n", iRegister);
            }
            else if (kResource == CG_COLOR0)
            {
                iTemp = cgGetParameterResourceIndex(kParam);
                // only color 0 is accepted in WML
                if (iTemp)
                {
                    FatalError("May only use Color0 or Color.");
                }
                iRegister = GetRegister(kParam);
                m_iColorRegHint = iRegister;
                DbgInfo("COLOR (v%i)\n", iRegister );
            }
            else
            {
                DbgInfo( "ignored\n" );
            }
            break;
        case CG_UNIFORM:
            DbgInfo("Uniform (%s), ", acName);
            iRegister = cgGetParameterResourceIndex(kParam);

            // check for texture first
            if (cgGetParameterType(kParam) == CG_SAMPLER2D)
            {
                // Capitalize the first letter of the texture
                // name.
                char* acNewName = new char[strlen(acName)+1];
                strcpy(acNewName,acName);
                acNewName[0] = toupper(acNewName[0]);
                String kTexName(acNewName);
                delete[] acNewName;

                m_kData.AddTexture(kTexName,iRegister);
                DbgInfo("texture2D\n");
                break;
            }

            // If not texture, check size of data
            GetSize(kParam, iRows, iCols);

            // invalid type?
            if (!iRows || !iCols)
            {
                DbgInfo("ignored\n");
                return;
            }

            if (iArraySize > 1)
            {
                iCols = 4;
                iRows *= iArraySize;
            }

            // Prefix must be Wm3 if a state constant
            if (iArraySize == 1 && !strncmp(acName, "Wm3", 3))
            {
                // State constant
                // Strip the Wm3 prefix and match the rest of the name
                MatchConstantName(&(acName[3]), iConstType, iTypeOption);
                AddConstant(iShaderType, acName, iRows, iCols,
                    iRegister, iConstType, iTypeOption);
                DbgInfo("state constant (c%i)\n", iRegister);
            }
            else
            {
                // User defined constant
                iConstType = ShaderConstant::SC_USER_DEFINED;
                iTypeOption = ShaderConstant::SCO_NONE;
                AddConstant(iShaderType, acName, iRows, iCols, iRegister,
                    iConstType, iTypeOption);
                DbgInfo("user constant (c%i)\n", iRegister);
            }
            break;
        case CG_CONSTANT:
            DbgInfo("Uniform (%s), ", acName);

            iRegister = cgGetParameterResourceIndex(kParam);
            GetSize(kParam,iRows,iCols);
            if (!iRows || !iCols)
                return;

            iConstType = ShaderConstant::SC_NUMERICAL_CONSTANT;
            iTypeOption = ShaderConstant::SCO_NONE;
            pkTemp = AddConstant(iShaderType, acName, iRows, iCols, iRegister,
                iConstType, iTypeOption);

            for (iTemp = 0; iTemp < iRows; iTemp++)
            {
                const double* adVal = cgGetParameterValues(kParam,CG_CONSTANT, 
                    &iTemp2);
                pkTemp->SetRegisterData(iTemp,(float)adVal[0],(float)adVal[1],
                    (float)adVal[2],(float)adVal[3]);
            }
  
            DbgInfo("numerical constant (c%i) [", iRegister);
            for( iTemp = 0; iTemp < 4*iRows-1; iTemp++ )
            {
                DbgInfo("%.2f, ", (float)(cgGetParameterValues(kParam,
                    CG_CONSTANT, &iTemp2)[iTemp]));
            }
            DbgInfo("%.2f", (float)(cgGetParameterValues(kParam,
                CG_CONSTANT, &iTemp2)[iTemp]));

            DbgInfo("]\n");
            break;

        default:
            DbgWarn("Unknown parameter type!\n");
            break;
    }
}
//----------------------------------------------------------------------------
void CgToWm3::MatchConstantName (const char* acName, 
    int& riType, int& riTypeOption)
{
    // For a given constant name, implicitly return its type and option
    // after checking to see if it shares the name of a state constant
    // (with optional option suffix).

    int iTemp;

    riTypeOption = ShaderConstant::SCO_NONE;

    for (riType = 0; riType < ShaderConstant::SC_QUANTITY; riType++)
    {
        if (!strncmp(acName, ms_aacConstName[riType], 
            strlen(ms_aacConstName[riType])))
        {
            switch(riType)
            {
            case ShaderConstant::SC_TRANSFORM_MV:
            case ShaderConstant::SC_TRANSFORM_P:
            case ShaderConstant::SC_TRANSFORM_MVP:
            case ShaderConstant::SC_TRANSFORM_M:
                iTemp = (int)strlen(ms_aacConstName[riType]);
                if (strlen(acName) == iTemp)
                {
                    riTypeOption = ShaderConstant::SCO_MATRIX;
                    return;
                }
                else if (!strcmp(&acName[iTemp], "InvTrans"))
                {
                    riTypeOption = ShaderConstant::SCO_INVERSE_TRANSPOSE;
                    return;
                }
                else if (!strcmp(&acName[iTemp], "Inv"))
                {
                    riTypeOption = ShaderConstant::SCO_INVERSE;
                    return;
                }
                else if (!strcmp(&acName[iTemp], "Trans"))
                {
                    riTypeOption = ShaderConstant::SCO_TRANSPOSE;
                    return;
                }
                break;
            case ShaderConstant::SC_LIGHT_DIRECTION:
            case ShaderConstant::SC_LIGHT_POSITION:
            case ShaderConstant::SC_LIGHT_AMBIENT:
            case ShaderConstant::SC_LIGHT_DIFFUSE:
            case ShaderConstant::SC_LIGHT_SPECULAR:
            case ShaderConstant::SC_LIGHT_SPOTCUTOFF:
            case ShaderConstant::SC_LIGHT_ATTENPARAMS:
                if (strlen(acName) == strlen(ms_aacConstName[riType])+1)
                {
                    // lights have a single digit after them
                    // e.g. LIGHT_POSITION0 or LIGHT_AMBIENT7
                    riTypeOption = (int)acName[strlen(acName)-1]-(int)'0';
                    if ((riTypeOption < 0) || (riTypeOption >=8))
                    {
                        FatalError("Illegal light num.");
                    }
                }
                else
                {
                    FatalError("Lights must have one digit"\
                        " in their name.\n");
                    }
                return;
            default:
                if (strlen(acName) == strlen(ms_aacConstName[riType]))
                {
                    riTypeOption = ShaderConstant::SCO_NONE;
                    return;
                }
                else
                {
                    FatalError("Invalid suffix on constant.");
                }
                break;
            }
        }
    }

    FatalError("Invalid state constant name.");
}
//----------------------------------------------------------------------------
int CgToWm3::GetRegister (CGparameter kParam)
{
    return cgD3D9ResourceToDeclUsage(cgGetParameterResource(kParam));
}
//----------------------------------------------------------------------------
bool CgToWm3::TryVersion (CGprofile kProfile, const char* acFilename,
    const char* acEntry, const char** aacArgs)
{
    if (cgIsProgram(m_kProgram))
        cgDestroyProgram(m_kProgram);

    m_kProgram = cgCreateProgramFromFile(m_kContext, CG_SOURCE, 
        acFilename, kProfile, acEntry, aacArgs );

    if (cgIsProgram(m_kProgram))
    {
        // Probably need to look for "// %i instructions" line
        return true;
    }
    else
        return false;
}
//----------------------------------------------------------------------------
void CgToWm3::WriteToFiles ()
{
    if (!m_kData.WriteToFile())
        FatalError("Writing failed.");
}
//----------------------------------------------------------------------------
void CgToWm3::FatalError (const char* acMessage)
{
    printf("ERROR: ");
    printf(acMessage);
    printf("\n");
    exit(-1);
}
//----------------------------------------------------------------------------
void CgToWm3::DbgInfo (const char* acFormat, ...)
{
    va_list kArgs;
    va_start(kArgs, acFormat);
    vprintf(acFormat,kArgs);
    va_end(kArgs);
}
//----------------------------------------------------------------------------
void CgToWm3::DbgWarn (const char* acFormat, ...)
{
    va_list kArgs;
    va_start(kArgs, acFormat);
    printf("WARNING: ");
    vprintf(acFormat,kArgs);
    va_end(kArgs);
}
//----------------------------------------------------------------------------
void CgToWm3::GetSize (CGparameter kParam, int& riRows, int& riCols)
{
    CGtype kType = cgGetParameterType(kParam);
    switch( kType )
    {
        case CG_FLOAT:
            riRows = 1;
            riCols = 1;
            return;
        case CG_FLOAT2:
            riRows = 1;
            riCols = 2;
            return;
        case CG_FLOAT3:
            riRows = 1;
            riCols = 3;
            return;
        case CG_FLOAT4:
            riRows = 1;
            riCols = 4;
            return;
        case CG_FLOAT3x3:
            riRows = 3;
            riCols = 3;
            return;
        case CG_FLOAT4x4:
            riRows = 4;
            riCols = 4;
            return;
        case CG_SAMPLER1D:
        case CG_SAMPLER2D:
        case CG_SAMPLER3D:
        case CG_SAMPLERRECT:
        case CG_SAMPLERCUBE:
            riRows = 0;
            riCols = 0;
            return;
        default:
            FatalError( "Type not handled by WML (sorry)!\n" );
            riRows = 0;
            riCols = 0;
            return;
    }
}
//----------------------------------------------------------------------------