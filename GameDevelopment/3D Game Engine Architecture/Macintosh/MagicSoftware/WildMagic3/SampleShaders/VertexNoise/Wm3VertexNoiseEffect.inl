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
inline void VertexNoiseEffect::SetNoiseTranslate (const Vector4f& rkNoiseTranslate)
{
    StaticCast<VertexNoiseVShader>(VShader)->SetNoiseTranslate(rkNoiseTranslate);
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseEffect::GetNoiseTranslate () const
{
    return StaticCast<VertexNoiseVShader>(VShader)->GetNoiseTranslate();
}
//----------------------------------------------------------------------------
inline void VertexNoiseEffect::SetNoiseScale (float fNoiseScale)
{
    StaticCast<VertexNoiseVShader>(VShader)->SetNoiseScale(fNoiseScale);
}
//----------------------------------------------------------------------------
inline float VertexNoiseEffect::GetNoiseScale () const
{
    return StaticCast<VertexNoiseVShader>(VShader)->GetNoiseScale();
}
//----------------------------------------------------------------------------
inline void VertexNoiseEffect::SetDisplacement (float fDisplacement)
{
    StaticCast<VertexNoiseVShader>(VShader)->SetDisplacement(fDisplacement);
}
//----------------------------------------------------------------------------
inline float VertexNoiseEffect::GetDisplacement () const
{
    return StaticCast<VertexNoiseVShader>(VShader)->GetDisplacement();
}
//----------------------------------------------------------------------------
inline void VertexNoiseEffect::SetPg (int i, const Vector4f& rkPg)
{
    StaticCast<VertexNoiseVShader>(VShader)->SetPg(i,rkPg);
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseEffect::GetPg (int i) const
{
    return StaticCast<VertexNoiseVShader>(VShader)->GetPg(i);
}
//----------------------------------------------------------------------------
inline void VertexNoiseEffect::SetBaseColor (const Vector4f& rkBaseColor)
{
    StaticCast<VertexNoiseVShader>(VShader)->SetBaseColor(rkBaseColor);
}
//----------------------------------------------------------------------------
inline Vector4f VertexNoiseEffect::GetBaseColor () const
{
    return StaticCast<VertexNoiseVShader>(VShader)->GetBaseColor();
}
//----------------------------------------------------------------------------
