// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
inline int Shader::GetShaderType () const
{
    return m_eShaderType;
}
//----------------------------------------------------------------------------
inline void Shader::SetProgram (const char* acProgram)
{
    m_acProgram = acProgram;
}
//----------------------------------------------------------------------------
inline const char* Shader::GetProgram () const
{
    return m_acProgram;
}
//----------------------------------------------------------------------------
inline int Shader::GetConstantQuantity () const
{
    return m_kConstant.GetQuantity();
}
//----------------------------------------------------------------------------
inline const ShaderConstantPtr Shader::GetConstant (int i) const
{
    assert( 0 <= i && i < m_kConstant.GetQuantity() );
    return m_kConstant[i];
}
//----------------------------------------------------------------------------
