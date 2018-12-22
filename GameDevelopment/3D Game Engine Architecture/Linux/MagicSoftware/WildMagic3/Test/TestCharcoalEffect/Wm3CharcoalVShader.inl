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
inline void CharcoalVShader::SetAmbientIntensity (float fAmbientIntensity)
{
    m_kConstant[2]->Data()[0] = fAmbientIntensity;
}
//----------------------------------------------------------------------------
inline float CharcoalVShader::GetAmbientIntensity () const
{
    return float(m_kConstant[2]->Data()[0]);
}
//----------------------------------------------------------------------------
inline void CharcoalVShader::SetContrastExponent (float fContrastExponent)
{
    m_kConstant[3]->Data()[0] = fContrastExponent;
}
//----------------------------------------------------------------------------
inline float CharcoalVShader::GetContrastExponent () const
{
    return float(m_kConstant[3]->Data()[0]);
}
//----------------------------------------------------------------------------
