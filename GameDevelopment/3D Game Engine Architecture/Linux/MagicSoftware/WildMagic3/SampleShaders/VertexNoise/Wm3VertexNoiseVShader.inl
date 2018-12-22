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
inline void VertexNoiseVShader::SetNoiseTranslate (const Vector4f& rkNoiseTranslate)
{
    m_kConstant[1]->Data()[0] = rkNoiseTranslate[0];
    m_kConstant[1]->Data()[1] = rkNoiseTranslate[1];
    m_kConstant[1]->Data()[2] = rkNoiseTranslate[2];
    m_kConstant[1]->Data()[3] = rkNoiseTranslate[3];
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseVShader::GetNoiseTranslate () const
{
    return Vector4f(m_kConstant[1]->Data()[0],
        m_kConstant[1]->Data()[1],
        m_kConstant[1]->Data()[2],
        m_kConstant[1]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void VertexNoiseVShader::SetNoiseScale (float fNoiseScale)
{
    m_kConstant[2]->Data()[0] = fNoiseScale;
}
//----------------------------------------------------------------------------
inline float VertexNoiseVShader::GetNoiseScale () const
{
    return float(m_kConstant[2]->Data()[0]);
}
//----------------------------------------------------------------------------
inline void VertexNoiseVShader::SetDisplacement (float fDisplacement)
{
    m_kConstant[3]->Data()[0] = fDisplacement;
}
//----------------------------------------------------------------------------
inline float VertexNoiseVShader::GetDisplacement () const
{
    return float(m_kConstant[3]->Data()[0]);
}
//----------------------------------------------------------------------------
inline void VertexNoiseVShader::SetPg (int i, const Vector4f& rkPg)
{
    m_kConstant[4]->Data()[0+4*i] = rkPg[0];
    m_kConstant[4]->Data()[1+4*i] = rkPg[1];
    m_kConstant[4]->Data()[2+4*i] = rkPg[2];
    m_kConstant[4]->Data()[3+4*i] = rkPg[3];
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseVShader::GetPg (int i) const
{
    return Vector4f(m_kConstant[4]->Data()[4*i+0],
        m_kConstant[4]->Data()[4*i+1],
        m_kConstant[4]->Data()[4*i+2],
        m_kConstant[4]->Data()[4*i+3]);
}
//----------------------------------------------------------------------------
inline void VertexNoiseVShader::SetBaseColor (const Vector4f& rkBaseColor)
{
    m_kConstant[5]->Data()[0] = rkBaseColor[0];
    m_kConstant[5]->Data()[1] = rkBaseColor[1];
    m_kConstant[5]->Data()[2] = rkBaseColor[2];
    m_kConstant[5]->Data()[3] = rkBaseColor[3];
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseVShader::GetBaseColor () const
{
    return Vector4f(m_kConstant[5]->Data()[0],
        m_kConstant[5]->Data()[1],
        m_kConstant[5]->Data()[2],
        m_kConstant[5]->Data()[3]);
}
//----------------------------------------------------------------------------
