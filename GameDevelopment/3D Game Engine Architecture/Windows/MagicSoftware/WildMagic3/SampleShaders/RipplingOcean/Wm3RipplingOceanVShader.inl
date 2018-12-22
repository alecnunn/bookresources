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
inline void RipplingOceanVShader::SetConstants (const Vector4f& rkConstants)
{
    m_kConstant[2]->Data()[0] = rkConstants[0];
    m_kConstant[2]->Data()[1] = rkConstants[1];
    m_kConstant[2]->Data()[2] = rkConstants[2];
    m_kConstant[2]->Data()[3] = rkConstants[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetConstants () const
{
    return Vector4f(m_kConstant[2]->Data()[0],
        m_kConstant[2]->Data()[1],
        m_kConstant[2]->Data()[2],
        m_kConstant[2]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetLightDir (const Vector3f& rkLightDir)
{
    m_kConstant[3]->Data()[0] = rkLightDir[0];
    m_kConstant[3]->Data()[1] = rkLightDir[1];
    m_kConstant[3]->Data()[2] = rkLightDir[2];
}
//----------------------------------------------------------------------------
inline Vector3f RipplingOceanVShader::GetLightDir () const
{
    return Vector3f(m_kConstant[3]->Data()[0],
        m_kConstant[3]->Data()[1],
        m_kConstant[3]->Data()[2]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetWaveDirX (const Vector4f& rkWaveDirX)
{
    m_kConstant[4]->Data()[0] = rkWaveDirX[0];
    m_kConstant[4]->Data()[1] = rkWaveDirX[1];
    m_kConstant[4]->Data()[2] = rkWaveDirX[2];
    m_kConstant[4]->Data()[3] = rkWaveDirX[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetWaveDirX () const
{
    return Vector4f(m_kConstant[4]->Data()[0],
        m_kConstant[4]->Data()[1],
        m_kConstant[4]->Data()[2],
        m_kConstant[4]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetWaveDirY (const Vector4f& rkWaveDirY)
{
    m_kConstant[5]->Data()[0] = rkWaveDirY[0];
    m_kConstant[5]->Data()[1] = rkWaveDirY[1];
    m_kConstant[5]->Data()[2] = rkWaveDirY[2];
    m_kConstant[5]->Data()[3] = rkWaveDirY[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetWaveDirY () const
{
    return Vector4f(m_kConstant[5]->Data()[0],
        m_kConstant[5]->Data()[1],
        m_kConstant[5]->Data()[2],
        m_kConstant[5]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetWaveSpeed (const Vector4f& rkWaveSpeed)
{
    m_kConstant[6]->Data()[0] = rkWaveSpeed[0];
    m_kConstant[6]->Data()[1] = rkWaveSpeed[1];
    m_kConstant[6]->Data()[2] = rkWaveSpeed[2];
    m_kConstant[6]->Data()[3] = rkWaveSpeed[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetWaveSpeed () const
{
    return Vector4f(m_kConstant[6]->Data()[0],
        m_kConstant[6]->Data()[1],
        m_kConstant[6]->Data()[2],
        m_kConstant[6]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetWaveOffset (const Vector4f& rkWaveOffset)
{
    m_kConstant[7]->Data()[0] = rkWaveOffset[0];
    m_kConstant[7]->Data()[1] = rkWaveOffset[1];
    m_kConstant[7]->Data()[2] = rkWaveOffset[2];
    m_kConstant[7]->Data()[3] = rkWaveOffset[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetWaveOffset () const
{
    return Vector4f(m_kConstant[7]->Data()[0],
        m_kConstant[7]->Data()[1],
        m_kConstant[7]->Data()[2],
        m_kConstant[7]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetWaveHeight (const Vector4f& rkWaveHeight)
{
    m_kConstant[8]->Data()[0] = rkWaveHeight[0];
    m_kConstant[8]->Data()[1] = rkWaveHeight[1];
    m_kConstant[8]->Data()[2] = rkWaveHeight[2];
    m_kConstant[8]->Data()[3] = rkWaveHeight[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetWaveHeight () const
{
    return Vector4f(m_kConstant[8]->Data()[0],
        m_kConstant[8]->Data()[1],
        m_kConstant[8]->Data()[2],
        m_kConstant[8]->Data()[3]);
}
//----------------------------------------------------------------------------
inline void RipplingOceanVShader::SetBumpSpeed (const Vector4f& rkBumpSpeed)
{
    m_kConstant[9]->Data()[0] = rkBumpSpeed[0];
    m_kConstant[9]->Data()[1] = rkBumpSpeed[1];
    m_kConstant[9]->Data()[2] = rkBumpSpeed[2];
    m_kConstant[9]->Data()[3] = rkBumpSpeed[3];
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanVShader::GetBumpSpeed () const
{
    return Vector4f(m_kConstant[9]->Data()[0],
        m_kConstant[9]->Data()[1],
        m_kConstant[9]->Data()[2],
        m_kConstant[9]->Data()[3]);
}
//----------------------------------------------------------------------------
