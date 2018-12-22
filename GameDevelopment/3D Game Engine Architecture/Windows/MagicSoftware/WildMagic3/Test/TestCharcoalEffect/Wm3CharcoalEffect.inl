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
inline void CharcoalEffect::SetAmbientIntensity (float fAmbientIntensity)
{
    StaticCast<CharcoalVShader>(VShader)->SetAmbientIntensity(
        fAmbientIntensity);
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetAmbientIntensity () const
{
    return StaticCast<CharcoalVShader>(VShader)->GetAmbientIntensity();
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetContrastExponent () const
{
    return StaticCast<CharcoalVShader>(VShader)->GetContrastExponent();
}
//----------------------------------------------------------------------------
inline void CharcoalEffect::SetSmudgeFactor (float fSmudgeFactor)
{
    StaticCast<CharcoalPShader>(PShader)->SetSmudgeFactor(fSmudgeFactor);
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetSmudgeFactor () const
{
    return StaticCast<CharcoalPShader>(PShader)->GetSmudgeFactor();
}
//----------------------------------------------------------------------------
inline void CharcoalEffect::SetPaperFactor (float fPaperFactor)
{
    StaticCast<CharcoalPShader>(PShader)->SetPaperFactor(fPaperFactor);
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetPaperFactor () const
{
    return StaticCast<CharcoalPShader>(PShader)->GetPaperFactor();
}
//----------------------------------------------------------------------------
