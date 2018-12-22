// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "EffectData.h"
#include <fstream>
#include <iostream>
using namespace std;

//----------------------------------------------------------------------------
EffectData::EffectData ()
{
}
//----------------------------------------------------------------------------
EffectData::~EffectData ()
{
}
//----------------------------------------------------------------------------
void EffectData::SetVShader (Shader* pkVShader)
{
    m_spkVShader = pkVShader;
}
//----------------------------------------------------------------------------
void EffectData::SetPShader (Shader* pkPShader)
{
    m_spkPShader = pkPShader;
}
//----------------------------------------------------------------------------
void EffectData::SetName (const String& rkName)
{
    m_kName = rkName;
}
//----------------------------------------------------------------------------
void EffectData::SetProgram (int iShaderType, int iRend, const String& rkProg)
{
    m_aakProgram[iShaderType][iRend] = rkProg;
}
//----------------------------------------------------------------------------
void EffectData::AddVConstant (ShaderConstant* pkConst, int iSize)
{
    // Shaders do not store sizes of each constant (float vs. float2, etc..)
    // so additionally store the size in an array.

    m_spkVShader->AddConstant(pkConst);
    m_kVConstSize.Append(iSize);
}
//----------------------------------------------------------------------------
void EffectData::AddPConstant (ShaderConstant* pkConst, int iSize)
{
    // Shaders do not store sizes of each constant (float vs. float2, etc..)
    // so additionally store the size in an array.

    m_spkPShader->AddConstant(pkConst);
    m_kPConstSize.Append(iSize);
}
//----------------------------------------------------------------------------
void EffectData::AddTexture (String kTexName, int iTexUnit)
{
    m_kTexName.Append(kTexName);
    m_kTexUnit.Append(iTexUnit);
}
//----------------------------------------------------------------------------
static bool HasUserConstants (const Shader* pkShader)
{
    // Return true if and only if pkShader has USER_DEFINED constants.

    for (int i = 0; i < pkShader->GetConstantQuantity(); i++)
    {
        if (pkShader->GetConstant(i)->GetSCType() == 
            ShaderConstant::SC_USER_DEFINED)
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
bool EffectData::WriteToFile ()
{
    // The main writing function.  Returns true if all the writing
    // succeeded.

    // Try to write as much as possible, even if other errors
    bool bError = false;
    ofstream kFile;

    String kEfFile = String(ms_acPref) + m_kName + String("Effect");

    // Create Effect Header
    kFile.open(kEfFile + String(".h"));
    if (kFile.is_open())
        WriteEffectHeader(kFile);
    else
        bError = true;
    kFile.close();

    // Create Effect Inline (if necessary)
    if ((m_spkVShader && HasUserConstants(m_spkVShader)) ||
        ((m_spkPShader && HasUserConstants(m_spkPShader))))
    {
        kFile.open(kEfFile + String(".inl"));
        if (kFile.is_open())
            WriteEffectInline(kFile);
        else
            bError = true;
        kFile.close();
    }

    // Create Effect Body
    kFile.open(kEfFile + String(".cpp"));
    if (kFile.is_open())
        WriteEffectBody(kFile);
    else
        bError = true;
    kFile.close();

    if (m_spkVShader)
    {
        String kVName = String(ms_acPref) + GetClassName(m_spkVShader);

        // Create VShader header
        kFile.open(kVName + String(".h"));
        if (kFile.is_open())
            WriteShaderHeader(kFile, m_spkVShader,m_kVConstSize);
        else
            bError = true;
        kFile.close();

        if (HasUserConstants(m_spkVShader))
        {
            // Create VShader inline file
            kFile.open(kVName + String(".inl"));
            if (kFile.is_open())
                WriteShaderInline(kFile, m_spkVShader,m_kVConstSize);
            else
                bError = true;
            kFile.close();
        }

        // Create VShader code file
        kFile.open(kVName + String(".cpp"));
        if (kFile.is_open())
            WriteShaderBody(kFile, m_spkVShader);
        else
            bError = true;
        kFile.close();
    }

    if (m_spkPShader)
    {
        String kPName = String(ms_acPref) + GetClassName(m_spkPShader);

        // Create PShader header
        kFile.open(kPName + String(".h"));
        if (kFile.is_open())
            WriteShaderHeader(kFile, m_spkPShader,m_kPConstSize);
        else
            bError = true;
        kFile.close();

        if (HasUserConstants(m_spkPShader))
        {
            // Create PShader inline file
            kFile.open(kPName + String(".inl"));
            if (kFile.is_open())
                WriteShaderInline(kFile, m_spkPShader,m_kPConstSize);
            else
                bError = true;
            kFile.close();
        }

        // Create PShader code file
        kFile.open(kPName + String(".cpp"));
        if (kFile.is_open())
            WriteShaderBody(kFile, m_spkPShader);
        else
            bError = true;
        kFile.close();
    }

    return !bError;
}
//----------------------------------------------------------------------------
void EffectData::WriteBoilerPlate (ofstream& rkFile)
{
    rkFile <<
        "// Magic Software, Inc.\n"
        "// http://www.magic-software.com\n"
        "// http://www.wild-magic.com\n"
        "// Copyright (c) 2004.  All Rights Reserved\n"
        "//\n"
        "// The Wild Magic Library (WM3) source code"
        " is supplied under the terms of\n"
        "// the license agreement "
        "http://www.wild-magic.com/License/WildMagic3.pdf and\n"
        "// may not be copied or disclosed except "
        "in accordance with the terms of that\n"
        "// agreement.\n"
        "\n";
}
//----------------------------------------------------------------------------
static bool IsArray (int iRows, int iCols)
{
    // Should this piece of data be represented as an array?
    // If there are more than four rows, then it has to be.
    // If there are 4 columns, but fewer than four rows, then it should
    // be an array as well, because a Matrix2 or a Matrix3 can't
    // represent it entirely.
    return (iRows > 4 || (iRows >=2 && iRows <= 3 && iCols == 4));
}
//----------------------------------------------------------------------------
static String GetType (int iRows, int iCols)
{
    // Return the WildMagic object type for a constant of a given
    // number of rows and columns.

    if (!IsArray(iRows,iCols) && iRows > 1)
    {
        assert( iCols == iRows );
        switch (iRows)
        {
        case 2:
            return "Matrix2f";
        case 3:
            return "Matrix3f";
        case 4:
            return "Matrix4f";
        default:
            return "void";
        }
    }
    else
    {
        switch (iCols)
        {
        case 1:
            return "float";
        case 2:
            return "Vector2f";
        case 3:
            return "Vector3f";
        case 4:
            return "Vector4f";
        default:
            return "void";
        }
    }
}
//----------------------------------------------------------------------------
static String GetPrefix (int iRows, int iCols)
{
    // Return the parameter prefix for a constant of a particular
    // number of rows and columns.

    if (iRows > 1 || iCols > 1)
        return "rk";
    else
        return "f";
}
//----------------------------------------------------------------------------
static String GetParam (int iRows, int iCols)
{
    // Return the formal parameter type for a shader constant of a particular
    // number of rows and columns.

    if (iRows > 1 || iCols > 1)
        return String("const ") + GetType(iRows,iCols) + String("&");
    else
        return "float";
}
//----------------------------------------------------------------------------
void EffectData::WriteGetSetFuncHeads (ofstream& rkFile, 
    const Shader* pkShader, TArray<int>& rkSizes)
{
    // Both the Effect class and the Shader classes have the same
    // function declarations to get and set user defined constants.
    // This functionality is grouped together here.
    // This function prints out all the getter and setter functions for
    // user constants for a given shader (and associated array of sizes).

    for (int i = 0; i < pkShader->GetConstantQuantity(); i++)
    {
        const ShaderConstant* pkConst = pkShader->GetConstant(i);

        if (pkConst->GetSCType() != ShaderConstant::SC_USER_DEFINED)
            continue;

        String kConstName = pkConst->GetName();

        int iRows = pkConst->GetRegisterQuantity();
        int iCols = rkSizes[i];

        if (IsArray(iRows,iCols))
        {
            rkFile << "    void Set" << (const char*)kConstName <<
                " (int i, const Vector4f& rkData);\n";
            rkFile << "    Vector4f Get" << (const char*)kConstName <<
                " (int i) const;\n";
        }
        else
        {
            String kType = GetType(iRows,iCols);
            String kPrefix = GetPrefix(iRows,iCols);
            String kParam = GetParam(iRows,iCols);

            rkFile << "    void Set" << (const char*)kConstName <<
                " (" << (const char*)kParam << " " << (const char*)kPrefix << 
                (const char*)kConstName << ");\n";

            rkFile << "    " << (const char*)kType << " Get" << 
                (const char*)kConstName << " () const;\n";
        }
    }
}
//----------------------------------------------------------------------------
String EffectData::GetClassName (const Shader* pkShader)
{
    // Returns the class name for a particular shader type.

    switch (pkShader->GetShaderType())
    {
    case Shader::ST_VERTEX_SHADER:
        return m_kName + String("VShader");
        break;
    case Shader::ST_PIXEL_SHADER:
        return m_kName + String("PShader");
        break;
    default:
        return m_kName + String("UnknownShader");
        break;
    }
}
//----------------------------------------------------------------------------
String EffectData::GetParentClass (const Shader* pkShader)
{
    // Returns the name of the class that this shader is derived from.

    switch (pkShader->GetShaderType())
    {
    case Shader::ST_VERTEX_SHADER:
        return ms_acVShader;
        break;
    case Shader::ST_PIXEL_SHADER:
        return ms_acPShader;
        break;
    default:
        return "Unknown";
        break;
    }
}
//----------------------------------------------------------------------------
void EffectData::WriteShaderHeader (ofstream& rkFile, const Shader* pkShader,
    TArray<int>& rkSizes)
{
    // Write the Shader.h file (both VShader and PShader).

    WriteBoilerPlate(rkFile);

    String kClassName = GetClassName(pkShader);
    String kDefineName = String ("WM3") + kClassName.ToUpper() + String("_H");
    String kParent = GetParentClass(pkShader);

    rkFile << "#ifndef " << (const char*)kDefineName << "\n" <<
        "#define " << (const char*)kDefineName << "\n\n" <<
        "#include \"" << ms_acPref << (const char*)kParent << ".h\"\n\n" <<
        "namespace " << ms_acPref << "\n" <<
        "{\n\n";

    rkFile << 
        "class WM3_ITEM " << (const char*)kClassName << " : public " << 
        (const char*)kParent << "\n" <<
        "{\n" <<
        "    WM3_DECLARE_RTTI;\n" <<
        "    WM3_DECLARE_NAME_ID;\n" <<
        "    WM3_DECLARE_STREAM;\n\n" <<
        "public:\n" <<
        "    " << (const char*)kClassName << " ();\n" <<
        "    virtual ~" << (const char*)kClassName << " ();\n\n";

    WriteGetSetFuncHeads (rkFile,pkShader,rkSizes);

    rkFile << "\nprivate:\n" <<
        "    static const char* ms_aacProgram[Renderer::RT_QUANTITY];\n" <<
        "};\n\n" <<
        "WM3_REGISTER_STREAM(" << (const char*)kClassName << ");\n" <<
        "typedef Pointer<" << (const char*)kClassName << "> " << 
        (const char*)kClassName << "Ptr;\n\n";

    // Only an inline file if there are user constants to get/set
    if (HasUserConstants(pkShader))
    {
        rkFile << "#include \"" << ms_acPref << (const char*)kClassName << 
        ".inl\"\n\n";
    }

    rkFile << "}\n\n#endif\n";
}
//----------------------------------------------------------------------------
void EffectData::WriteShaderInline (ofstream& rkFile, const Shader* pkShader,
    TArray<int>& rkSizes)
{
    // Write the Shader.inl file.  Works for both VShader and PShader.

    WriteBoilerPlate(rkFile);

    rkFile << ms_acSep;

    String kClassName = GetClassName(pkShader);

    for (int i = 0; i < pkShader->GetConstantQuantity(); i++)
    {
        const ShaderConstant* pkConst = pkShader->GetConstant(i);
        if (pkConst->GetSCType() != ShaderConstant::SC_USER_DEFINED)
            continue;

        String kConstName = pkConst->GetName();
        
        int iLoc = i;
        int iRows = pkConst->GetRegisterQuantity();
        int iCols = rkSizes[i];
        String kType = GetType(iRows,iCols);
        String kPrefix = GetPrefix(iRows,iCols);
        String kParam = GetParam(iRows,iCols);

        bool bArray = IsArray(iRows, iCols);

        // SETTER FUNCTION
        rkFile << "inline void " << (const char*)kClassName << "::Set" <<
                (const char*)kConstName << " (";
        if (bArray)
            rkFile << "int i, ";
        rkFile << (const char*)kParam << " " << (const char*)kPrefix << 
            (const char*)kConstName;

        rkFile << ")\n" << "{\n";

        int iAssignRows = (bArray ? 1 : iRows);
        for (int iR = 0; iR < iAssignRows; iR++)
        {
            for (int iC = 0; iC < iCols; iC++)
            {
                rkFile << "    m_kConstant[" << iLoc+iR << "]->Data()[" <<
                    iC;

                if (bArray)
                    rkFile << "+4*i";
                
                rkFile << "] = " << (const char*)kPrefix << 
                    (const char*)kConstName;

                if (iAssignRows > 1)
                    rkFile << "[" << iR << "]";

                if (iCols > 1)
                {
                    rkFile << "[" << iC << "]";
                }

                rkFile << ";\n";
            }
        }

        rkFile << "}\n" << ms_acSep;

        // GETTER FUNCTION
        // NOTE: This assumes that Matrixnf and Vectornf have
        // constructors that take a list of floats.
        rkFile << "inline " << (const char*)kType << " " << 
            (const char*)kClassName <<  "::Get" << (const char*)kConstName << 
            " (";

        if (bArray)
            rkFile << "int i";
        
        rkFile << ") const\n" << 
            "{\n" <<
            "    return " << (const char*)kType << "(";

        for (int iR = 0; iR < iAssignRows; iR++)
        {
            for (int iC = 0; iC < iCols; iC++)
            {
                rkFile << "m_kConstant[" << iR+iLoc << "]->Data()[";
                if (bArray)
                    rkFile << "4*i+";
                rkFile << iC << "]";

                // If not the last one...
                if ((iR != iAssignRows - 1) || (iC != iCols - 1))
                    rkFile << ",\n        ";
                else // If last...
                    rkFile << ");\n";
            }
        }
        rkFile << "}\n" << ms_acSep;
    }
}
//----------------------------------------------------------------------------
static bool IsLight (int iType)
{
    // Returns true if and only if this shader constant refers to a light
    // parameter.

    return (iType >= ShaderConstant::SC_LIGHT_POSITION && 
        iType <= ShaderConstant::SC_LIGHT_ATTENPARAMS);
}
//----------------------------------------------------------------------------
static String GetOptionComment (int iOption)
{
    // Returns a string representing the type of matrix that this
    // shader constant option stands for.

    // Turn option constants into readable comment strings
    switch(iOption)
    {
    case ShaderConstant::SCO_MATRIX:
        return "";
    case ShaderConstant::SCO_TRANSPOSE:
        return "transpose ";
    case ShaderConstant::SCO_INVERSE:
        return "inverse ";
    case ShaderConstant::SCO_INVERSE_TRANSPOSE:
        return "inverse transpose ";
    default:
        // shouldn't get here
        assert( false );
        return "[ERROR: Invalid Matrix Suffix in .cg program] ";
    }
}
//----------------------------------------------------------------------------
static String GetOption (int iType, int iOption, int iQuantity)
{
    // Return the string representation of a given option constant/type pair.

    if (IsLight(iType))
    {
        switch (iOption)
        {
        case ShaderConstant::SCO_LIGHT0:
            return "SCO_LIGHT0";
        case ShaderConstant::SCO_LIGHT1:
            return "SCO_LIGHT1";
        case ShaderConstant::SCO_LIGHT2:
            return "SCO_LIGHT2";
        case ShaderConstant::SCO_LIGHT3:
            return "SCO_LIGHT3";
        case ShaderConstant::SCO_LIGHT4:
            return "SCO_LIGHT4";
        case ShaderConstant::SCO_LIGHT5:
            return "SCO_LIGHT5";
        case ShaderConstant::SCO_LIGHT6:
            return "SCO_LIGHT6";
        case ShaderConstant::SCO_LIGHT7:
            return "SCO_LIGHT7";
        default:
            // shouldn't get here
            assert( false );
            return "[ERROR: Invalid light option in .cg program]";
        }
    }
    else if (iQuantity > 1)
    {
        switch(iOption)
        {
        case ShaderConstant::SCO_MATRIX:
            return "SCO_MATRIX";
        case ShaderConstant::SCO_TRANSPOSE:
            return "SCO_TRANSPOSE";
        case ShaderConstant::SCO_INVERSE:
            return "SCO_INVERSE";
        case ShaderConstant::SCO_INVERSE_TRANSPOSE:
            return "SCO_INVERSE_TRANSPOSE";
        default:
            // shouldn't get here
            assert( false );
            return "[ERROR: Invalid matrix suffix in .cg program]";
        }
    }
    else
    {
        assert (iQuantity == 1);
        assert (iOption == ShaderConstant::SCO_NONE);
        return "SCO_NONE";
    } 
}
//----------------------------------------------------------------------------
void WriteProgram (ofstream& rkFile, const String& rkProg)
{
    // Write out a shader program represented by rkProg.
    // It puts a quote at the beginning of each line and a \n" at the end.

    rkFile << "\"";
    for (int i = 0; i < rkProg.GetLength(); i++)
    {
        if (rkProg[i] == '\n')
        {
            // output \n" and then a linebreak
            rkFile << "\\n\"\n";

            // Catch CR/LF pair
            if ((int)rkProg[i+1] == 10)
                rkFile << rkProg[++i];

            // Don't start new quote if at the end of the program
            if (rkProg.GetLength() != i+1)
                rkFile << "\"";
        }
        else
        {
            rkFile << rkProg[i];
        }
    }
}
//----------------------------------------------------------------------------
void EffectData::WriteShaderBody (ofstream& rkFile, const Shader* pkShader)
{
    // Write the Shader.cpp.  Works for both VShader and PShader.

    // BOILERPLATE/HEADER
    WriteBoilerPlate(rkFile);

    String kName = GetClassName(pkShader);
    String kParent = GetParentClass(pkShader);

    rkFile << "#include \"" << ms_acPref << (const char*)kName << ".h\"\n" <<
        "using namespace " << ms_acPref << ";\n\n" <<
        "WM3_IMPLEMENT_RTTI(" << ms_acPref << "," << (const char*)kName << 
        "," << (const char*)kParent << ");\n" << "WM3_IMPLEMENT_STREAM(" << 
        (const char*)kName << ");\n" << "WM3_IMPLEMENT_DEFAULT_STREAM(" << 
        (const char*)kName << "," << (const char*)kParent << ");\n" <<
        "WM3_IMPLEMENT_DEFAULT_NAME_ID(" << (const char*)kName << "," << 
        (const char*)kParent << ");\n\n" << ms_acSep;

    // CONSTRUCTOR
    rkFile << (const char*)kName << "::" << (const char*)kName << " ()\n"
        "{\n"
        "    // shader program (load the type of the current renderer)\n"
        "    m_acProgram = ms_aacProgram[Renderer::GetType()];\n";

    // Only include this code if there are actually constants to use it
    if (pkShader->GetConstantQuantity() > 0)
    {
        // variable setup
        rkFile << "\n    // shader constants\n" <<
            "    int iType, iOption, iReg, iRegQuantity;\n" <<
            "    ShaderConstant* pkConst;\n\n";

        // Set up each variable
        for (int i = 0; i < pkShader->GetConstantQuantity(); i++)
        {
            const ShaderConstant* pkConst = pkShader->GetConstant(i);
            int iReg = pkConst->GetRegister();
            int iQuantity = pkConst->GetRegisterQuantity();
            int iType = pkConst->GetSCType();
            int iOption = pkConst->GetSCOption();

            // Comment
            rkFile << "    // " << ms_aacComment1[iType] << " ";

            // ...if light, add light number
            if (IsLight(iType))
                rkFile << iOption << " ";

            bool bArray = IsArray(iQuantity,
                pkShader->GetShaderType() == Shader::ST_VERTEX_SHADER ?
                m_kVConstSize[i] : m_kPConstSize[i]);

            // ...if is matrix, add qualifier
            if (iQuantity > 1 && !bArray)
                rkFile << (const char*)GetOptionComment(iOption);

            // ...add type (vector/matrix/array)
            if (bArray)
                rkFile << "array ";
            else if (iQuantity > 1)
                rkFile << "matrix ";
            else
                rkFile << "vector ";

            // ...if constant/user defined add register number
            if (iType == ShaderConstant::SC_NUMERICAL_CONSTANT ||
                iType == ShaderConstant::SC_USER_DEFINED)
            {
                rkFile << "(register " << iReg << ")\n";
            }
            else // ...or add final static comment
            {
                rkFile << ms_aacComment2[iType] << "\n";
            }

            // If user defined, write out the constant name too
            if (iType == ShaderConstant::SC_USER_DEFINED)
            {
                rkFile << "    // " << (const char*)pkConst->GetName() << 
                    "\n";
            }

            // Type
            rkFile << "    iType = ShaderConstant::" << 
                ms_aacConstant[iType] << ";\n";

            // Option
            rkFile << "    iOption = ShaderConstant::";
            if (bArray)
                rkFile << "SCO_NONE";
            else
                rkFile << (const char*)GetOption(iType,iOption,iQuantity);
            rkFile << ";\n";

            // Register
            rkFile << "    iReg = " << iReg << ";\n";

            // Register quantity
            rkFile << "    iRegQuantity = " << iQuantity << ";\n";

            // Create
            rkFile << "    pkConst = new ShaderConstant(iType,iOption,iReg,"
                "iRegQuantity);\n";

            // If a numerical constant, set the default value
            rkFile << showpoint;
            if (iType == ShaderConstant::SC_NUMERICAL_CONSTANT)
            {
                for (int i = 0; i < iQuantity; i++)
                {
                    int iLoc = 4*i;
                    // Cast everything as floats to avoid compiler
                    // warnings about truncating doubles.
                    //rkFile << "    pkConst->SetRegisterData(" << i << 
                    //    ",(float)" << pkConst->Data(iLoc) << ",(float)" <<
                    //    pkConst->Data(iLoc+1) << ",(float)" <<
                    //    pkConst->Data(iLoc+2) << ",(float)" <<
                    //    pkConst->Data(iLoc+3) << ");\n";
                    rkFile << "    pkConst->SetRegisterData(" << i << ","
                        << pkConst->Data(iLoc  ) << "f,"
                        << pkConst->Data(iLoc+1) << "f,"
                        << pkConst->Data(iLoc+2) << "f,"
                        << pkConst->Data(iLoc+3) << "f);\n";
                }
            }
            rkFile << noshowpoint;

            // Append
            rkFile << "    m_kConstant.Append(pkConst);\n\n";
        }
    }
    rkFile << "}\n" << ms_acSep;

    // DESTRUCTOR
    rkFile << (const char*)kName << "::~" << (const char*)kName << " ()\n" <<
        "{\n" <<
        "}\n" <<
        ms_acSep;

    // STATIC DATA
    rkFile << "const char* " << (const char*)kName << 
        "::ms_aacProgram[Renderer::RT_QUANTITY] =\n" <<
        "{\n";

    // PROGRAM(s)
    for (int i = 0; i < Renderer::RT_QUANTITY; i++)
    {
        WriteProgram(rkFile,m_aakProgram[pkShader->GetShaderType()][i]);
        if (i != Renderer::RT_QUANTITY - 1)
            rkFile << ",\n";
    }

    rkFile << "};\n";
}
//----------------------------------------------------------------------------
String EffectData::GetImageParams ()
{
    // Returns a string representing the list of Image* parameters
    // for the ShaderEffect constructor.

    String kParams;

    int iNumTex = m_kTexName.GetQuantity();
    for (int i = 0; i < iNumTex; i++)
    {
        kParams += "Image* pk";
        kParams += (const char*)m_kTexName[i];

        if (i != iNumTex - 1)
            kParams += ", ";
    }

    return kParams;
}
//----------------------------------------------------------------------------
void EffectData::WriteEffectHeader (ofstream& rkFile)
{
    // Writes the ShaderEffect.h file.

    WriteBoilerPlate(rkFile);

    String kName = m_kName + String("Effect");
    String kDefine = (String(ms_acPref) + kName + String("_H")).ToUpper();

    rkFile << "#ifndef " << (const char*)kDefine << "\n"
        "#define " << (const char*)kDefine << "\n\n"
        "#include \"" << ms_acPref << ms_acShader << ".h\"\n";

    bool bHasVConstants = m_spkVShader && HasUserConstants(m_spkVShader);
    bool bHasPConstants = m_spkPShader && HasUserConstants(m_spkPShader);

    // Need to include V and P shader for the Effect.inl file if it
    // exists.
    if (bHasVConstants)
    {
        rkFile << "#include \"" << ms_acPref << 
            (const char*)GetClassName(m_spkVShader) << ".h\"\n";
    }
    if (bHasPConstants)
    {
        rkFile << "#include \"" << ms_acPref << 
            (const char*)GetClassName(m_spkPShader) << ".h\"\n";
    }
        
    rkFile << "\nnamespace " << ms_acPref << "\n"
        "{\n\n"
        "class WM3_ITEM " << (const char*)kName << " : public " << 
        ms_acShader << "\n"
        "{\n"
        "    WM3_DECLARE_RTTI;\n"
        "    WM3_DECLARE_NAME_ID;\n"
        "    WM3_DECLARE_STREAM;\n\n"
        "public:\n"
        "    " << (const char*)kName << " (" << 
        (const char*)GetImageParams() << ");\n"
        "    virtual ~" << (const char*)kName << " ();\n\n"
        "    // create a clone of the effect\n"
        "    virtual Effect* Clone ();\n\n";

    if (m_spkVShader)
        WriteGetSetFuncHeads(rkFile,m_spkVShader,m_kVConstSize);
    if (m_spkPShader)
        WriteGetSetFuncHeads(rkFile,m_spkPShader,m_kPConstSize);

    rkFile << "\nprotected:\n"
        "    // streaming\n";

    // If there exist textures, then we need a separate constructor here.
    // If there aren't, then this is redundant and shouldn't be written out.
    if (m_kTexName.GetQuantity() > 1)
        rkFile << "    " << (const char*)kName << " ();\n";

    rkFile << "    virtual void DoCloning (Effect* pkClone);\n"
        "};\n\n"
        "WM3_REGISTER_STREAM(" << (const char*)kName << ");\n"
        "typedef Pointer<" << (const char*)kName << "> " << 
        (const char*)kName << "Ptr;\n\n";

    if ((m_spkVShader && bHasVConstants) || (m_spkPShader && bHasPConstants))
    {
        rkFile << "#include \"" << ms_acPref << (const char*)kName << 
            ".inl\"\n\n";
    }

    rkFile << "}\n\n#endif\n";
}
//----------------------------------------------------------------------------
void EffectData::WriteEffectInlineFuncs (ofstream& rkFile, 
    const Shader* pkShader, TArray<int>& rkSizes)
{
    // Writes all of the get/setter functions for a given shader's user
    // constants into the ShaderEffect.inl file.

    String kShadName;

    if (pkShader->GetShaderType() == Shader::ST_PIXEL_SHADER)
        kShadName = "PShader";
    else if (pkShader->GetShaderType() == Shader::ST_VERTEX_SHADER)
        kShadName = "VShader";
    else
        kShadName = "Error";

    // Cast shader as the correct type
    kShadName = String("StaticCast<") + GetClassName(pkShader) +
        String(">(") + kShadName + String(")");

    String kClassName = m_kName + String("Effect");

    for (int i = 0; i < pkShader->GetConstantQuantity(); i++)
    {
        const ShaderConstant* pkConst = pkShader->GetConstant(i);
        if (pkConst->GetSCType() != ShaderConstant::SC_USER_DEFINED)
            continue;
        String kConstName = pkConst->GetName();
        
        int iLoc = pkConst->GetRegister();
        int iRows = pkConst->GetRegisterQuantity();
        int iCols = rkSizes[i];
        String kType = GetType(iRows,iCols);
        String kPrefix = GetPrefix(iRows,iCols);
        String kParam = GetParam(iRows,iCols);

        bool bArray = IsArray(iRows, iCols);

        // SETTER FUNCTION
        rkFile << "inline void " << (const char*)kClassName << "::Set" << (
            const char*)kConstName << " (";
        if (bArray)
            rkFile << "int i, ";
        rkFile << (const char*)kParam << " " << 
            (const char*)kPrefix << (const char*)kConstName << ")\n" <<
            "{\n";
        
        rkFile << "    " << (const char*)kShadName << "->Set" << 
            (const char*)kConstName << "(";
        if (bArray)
            rkFile << "i,";
        rkFile << (const char*)kPrefix << (const char*)kConstName << ");\n";

        rkFile << "}\n" << ms_acSep;

        // GETTER FUNCTION
        rkFile << "inline " << (const char*)kType << " " << 
            (const char*)kClassName << "::Get" << (const char*)kConstName << 
            " (";
        if (bArray)
            rkFile << "int i";
        rkFile << ") const\n" << "{\n";
        
        rkFile << "    return " << (const char*)kShadName << "->Get" << 
            (const char*)kConstName << "(";
        if (bArray)
            rkFile << "i";
        rkFile << ");\n";

        rkFile << "}\n" << ms_acSep;
    }
}
//----------------------------------------------------------------------------
void EffectData::WriteEffectInline (ofstream& rkFile)
{
    // Writes the ShaderEffect.inl file.

    WriteBoilerPlate(rkFile);

    rkFile << ms_acSep;

    if (m_spkVShader)
        WriteEffectInlineFuncs(rkFile,m_spkVShader,m_kVConstSize);

    if (m_spkPShader)
        WriteEffectInlineFuncs(rkFile,m_spkPShader,m_kPConstSize);
}
//----------------------------------------------------------------------------
void EffectData::WriteEffectBody (ofstream& rkFile)
{
    // Writes the ShaderEffect.cpp file.

    WriteBoilerPlate(rkFile);

    String kName = m_kName + String("Effect");
    String kVShad = m_spkVShader ? GetClassName(m_spkVShader) : "";
    String kPShad = m_spkPShader ? GetClassName(m_spkPShader) : "";

    // Includes
    rkFile << "#include \"" << ms_acPref << (const char*)kName << ".h\"\n";

    if (m_spkVShader)
    {
        rkFile << "#include \"" << ms_acPref << (const char*)kVShad <<
           ".h\"\n";
    }
    if (m_spkPShader)
    {
        rkFile << "#include \"" << ms_acPref << (const char*)kPShad << 
            ".h\"\n";
    }
    rkFile << "using namespace " << ms_acPref << ";\n\n";

    // Streaming/RTTI
    rkFile << "WM3_IMPLEMENT_RTTI(" << ms_acPref << "," << 
        (const char*)kName << "," << ms_acShader << ");\n";
    rkFile << "WM3_IMPLEMENT_STREAM(" << (const char*)kName << ");\n";
    rkFile << "WM3_IMPLEMENT_DEFAULT_STREAM(" << (const char*)kName << "," << 
        ms_acShader << ");\n";
    rkFile << "WM3_IMPLEMENT_DEFAULT_NAME_ID(" << (const char*)kName << "," <<
        ms_acShader << ");\n\n";

    rkFile << ms_acSep;

    // Constructor
    rkFile << (const char*)kName << "::" << (const char*)kName << " (" << 
        (const char*)GetImageParams() << ")\n"
        "{\n";

    if (m_spkVShader)
        rkFile << "    VShader = new " << (const char*)kVShad << ";\n";
    if (m_spkPShader)
        rkFile << "    PShader = new " << (const char*)kPShad << ";\n";
    
    int iNumTex = m_kTexName.GetQuantity();
    for (int i = 0; i < iNumTex; i++)
    {
        String kTex = String("pk") + m_kTexName[i] + String("Tex");
        String kImg = String("pk") + m_kTexName[i];
        rkFile << "\n    Texture* " << (const char*)kTex << " = new Texture;\n"
            "    " << (const char*)kTex << "->SetImage(" << 
            (const char*)kImg << ");\n";

        rkFile << "    " << (const char*)kTex << 
            "->Mipmap = Texture::MM_NEAREST;\n    " <<
            (const char*)kTex << "->Apply = Texture::AM_DECAL;\n"
            "    Textures.Append(" << (const char*)kTex << ");\n";
    }

    rkFile << "}\n" << ms_acSep;

    // Cloning constructor only needed if there are textures, otherwise
    // the normal constructor takes no parameters and can be used.
    if (m_kTexName.GetQuantity() > 1)
    {
        // Cloning Constructor
        rkFile << (const char*)kName << "::" << (const char*)kName << " ()\n"
            "{\n"
            "}\n" << 
            ms_acSep;
    };

    // Destructor
    rkFile << (const char*)kName << "::~" << (const char*)kName << " ()\n"
        "{\n"
        "}\n" <<
        ms_acSep;

    // Clone ()
    rkFile << "Effect* " << (const char*)kName << "::Clone ()\n"
        "{\n"
        "    " << (const char*)kName << "* pkClone = new " << 
        (const char*)kName << ";\n"
        "    DoCloning(pkClone);\n"
        "    return pkClone;\n"
        "}\n" <<
        ms_acSep;

    // DoCloning ()
    rkFile << "void " << (const char*)kName << 
        "::DoCloning (Effect* pkEffect)\n"
        "{\n"
        "    " << ms_acShader << "::DoCloning(pkEffect);\n"
        "}\n" <<
        ms_acSep;
}
//----------------------------------------------------------------------------

const char* EffectData::ms_acVShader = "VertexShader";
const char* EffectData::ms_acPShader = "PixelShader";
const char* EffectData::ms_acShader = "ShaderEffect";
const char* EffectData::ms_acPref = "Wm3";
const char* EffectData::ms_acSep = "//---------------------------------------"
    "-------------------------------------\n";

// Shader constant comment prefixes
const char* EffectData::ms_aacComment1[ShaderConstant::SC_USER_DEFINED+1] =
{
    "model-to-world",
    "projection",
    "model-to-view",
    "model-to-view * projection",
    "camera position",
    "camera direction",
    "camera up",
    "camera right",
    "fog color",
    "fog params",
    "material emissive",
    "material ambient",
    "material diffuse",
    "material specular",
    "material shininess",
    "light position",
    "light direction",
    "light ambient",
    "light diffuse",
    "light specular",
    "light spotcutoff",
    "light attenuation params",
    "numerical constant",
    "ERROR!",
    "user defined"
};

// Shader constant comment suffixes
const char* EffectData::ms_aacComment2[ShaderConstant::SC_USER_DEFINED+1] =
{
    "(4x4)",
    "(4x4)",
    "(4x4)",
    "(4x4)",
    "(x,y,z,1)",
    "(x,y,z,0)",
    "(x,y,z,0)",
    "(x,y,z,0)",
    "(r,g,b,a)",
    "(start, end, density, enabled)",
    "(r,g,b,a)",
    "(r,g,b,a)",
    "(r,g,b,a)",
    "(r,g,b,a)",
    "(shiny, undefined, undefined, undefined)",
    "(x,y,z,0)",
    "(x,y,z,0)",
    "(r,g,b,a)",
    "(r,g,b,a)",
    "(r,g,b,a)",
    "(angle, cos, sin, exponent)",
    "(constant, linear, quadratic, intensity)",
    "",
    " ERROR!",
    ""
};

// Mapping of shader constant types to Strings of the constant names
const char* EffectData::ms_aacConstant[ShaderConstant::SC_USER_DEFINED+1] =
{
        "SC_TRANSFORM_M",
        "SC_TRANSFORM_P",
        "SC_TRANSFORM_MV",
        "SC_TRANSFORM_MVP",
        "SC_CAMERA_POSITION",
        "SC_CAMERA_DIRECTION",
        "SC_CAMERA_UP",
        "SC_CAMERA_RIGHT",
        "SC_FOG_COLOR",
        "SC_FOG_PARAMS",
        "SC_MATERIAL_EMISSIVE",
        "SC_MATERIAL_AMBIENT",
        "SC_MATERIAL_DIFFUSE",
        "SC_MATERIAL_SPECULAR",
        "SC_MATERIAL_SHININESS",
        "SC_LIGHT_POSITION",
        "SC_LIGHT_DIRECTION",
        "SC_LIGHT_AMBIENT",
        "SC_LIGHT_DIFFUSE",
        "SC_LIGHT_SPECULAR",
        "SC_LIGHT_SPOTCUTOFF",
        "SC_LIGHT_ATTENPARAMS",
        "SC_NUMERICAL_CONSTANT",
        "SC_QUANTITY",
        "SC_USER_DEFINED"
};