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
inline int SkinController::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline int SkinController::GetBoneQuantity () const
{
    return m_iBoneQuantity;
}
//----------------------------------------------------------------------------
inline Node* SkinController::GetBone (int iBone) const
{
    assert( 0 <= iBone && iBone < m_iBoneQuantity );
    return m_apkBones[iBone];
}
//----------------------------------------------------------------------------
inline float& SkinController::Weight (int iVertex, int iBone)
{
    assert( 0 <= iVertex && iVertex < m_iVertexQuantity );
    assert( 0 <= iBone && iBone < m_iBoneQuantity );
    return m_aafWeight[iVertex][iBone];
}
//----------------------------------------------------------------------------
inline Vector3f& SkinController::Offset (int iVertex, int iBone)
{
    assert( 0 <= iVertex && iVertex < m_iVertexQuantity );
    assert( 0 <= iBone && iBone < m_iBoneQuantity );
    return m_aakOffset[iVertex][iBone];
}
//----------------------------------------------------------------------------
