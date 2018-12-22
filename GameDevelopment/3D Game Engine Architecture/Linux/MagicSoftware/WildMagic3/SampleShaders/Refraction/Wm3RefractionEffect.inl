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
inline void RefractionEffect::SetIndexOfRefraction (float fIndexOfRefraction)
{
    StaticCast<RefractionVShader>(VShader)->SetIndexOfRefraction(fIndexOfRefraction);
}
//----------------------------------------------------------------------------
inline float RefractionEffect::GetIndexOfRefraction () const
{
    return StaticCast<RefractionVShader>(VShader)->GetIndexOfRefraction();
}
//----------------------------------------------------------------------------
inline void RefractionEffect::SetFresnelConstants (const Vector4f& rkFresnelConstants)
{
    StaticCast<RefractionVShader>(VShader)->SetFresnelConstants(rkFresnelConstants);
}
//----------------------------------------------------------------------------
inline Vector4f RefractionEffect::GetFresnelConstants () const
{
    return StaticCast<RefractionVShader>(VShader)->GetFresnelConstants();
}
//----------------------------------------------------------------------------
