/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <iostream>
#include <vector>
#ifndef _WIN32
#include <string>
#endif

// Code from "Half of the Solution" Section
//

// function specification
struct Function
{
    // simple variable spec
    enum eVarType
    {
        VAR_VOID, VAR_BOOL, VAR_INT, VAR_FLOAT, VAR_STRING,
    };

    // possible calling conventions
    enum eCallType
    {
        CALL_CDECL, CALL_FASTCALL, CALL_STDCALL, CALL_THISCALL,
    };

    typedef std::vector <eVarType> ParamVec;

    std::string  m_Name;
    void*        m_Proc;
    unsigned int m_SerialID;
    eVarType     m_ReturnType;
    ParamVec     m_ParamTypes;
    eCallType    m_CallType;
};

typedef std::vector <Function> FunctionVec;

// the global set of specifications for exported functions
FunctionVec g_Functions;


