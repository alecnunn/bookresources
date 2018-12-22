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
inline void SkinningVShader::SetSkinningMat (int i,
    const Vector4f& rkSkinningMat)
{
    m_kConstant[1]->Data()[0+4*i] = rkSkinningMat[0];
    m_kConstant[1]->Data()[1+4*i] = rkSkinningMat[1];
    m_kConstant[1]->Data()[2+4*i] = rkSkinningMat[2];
    m_kConstant[1]->Data()[3+4*i] = rkSkinningMat[3];
}
//----------------------------------------------------------------------------
inline Vector4f SkinningVShader::GetSkinningMat (int i) const
{
    return Vector4f(m_kConstant[1]->Data()[4*i+0],
        m_kConstant[1]->Data()[4*i+1],
        m_kConstant[1]->Data()[4*i+2],
        m_kConstant[1]->Data()[4*i+3]);
}
//----------------------------------------------------------------------------
