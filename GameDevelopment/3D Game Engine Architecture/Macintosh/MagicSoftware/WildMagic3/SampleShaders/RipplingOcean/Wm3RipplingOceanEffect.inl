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
inline void RipplingOceanEffect::SetConstants (const Vector4f& rkConstants)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetConstants(rkConstants);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetConstants () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetConstants();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetLightDir (const Vector3f& rkLightDir)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetLightDir(rkLightDir);
}
//----------------------------------------------------------------------------
inline Vector3f RipplingOceanEffect::GetLightDir () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetLightDir();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetWaveDirX (const Vector4f& rkWaveDirX)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetWaveDirX(rkWaveDirX);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetWaveDirX () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetWaveDirX();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetWaveDirY (const Vector4f& rkWaveDirY)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetWaveDirY(rkWaveDirY);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetWaveDirY () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetWaveDirY();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetWaveSpeed (const Vector4f& rkWaveSpeed)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetWaveSpeed(rkWaveSpeed);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetWaveSpeed () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetWaveSpeed();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetWaveOffset (const Vector4f& rkWaveOffset)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetWaveOffset(rkWaveOffset);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetWaveOffset () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetWaveOffset();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetWaveHeight (const Vector4f& rkWaveHeight)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetWaveHeight(rkWaveHeight);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetWaveHeight () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetWaveHeight();
}
//----------------------------------------------------------------------------
inline void RipplingOceanEffect::SetBumpSpeed (const Vector4f& rkBumpSpeed)
{
    StaticCast<RipplingOceanVShader>(VShader)->SetBumpSpeed(rkBumpSpeed);
}
//----------------------------------------------------------------------------
inline Vector4f RipplingOceanEffect::GetBumpSpeed () const
{
    return StaticCast<RipplingOceanVShader>(VShader)->GetBumpSpeed();
}
//----------------------------------------------------------------------------
