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
inline void IridescenceEffect::SetInterpolateFactor (float fInterpolateFactor)
{
    StaticCast<IridescenceVShader>(VShader)->SetInterpolateFactor(fInterpolateFactor);
}
//----------------------------------------------------------------------------
inline float IridescenceEffect::GetInterpolateFactor () const
{
    return StaticCast<IridescenceVShader>(VShader)->GetInterpolateFactor();
}
//----------------------------------------------------------------------------
