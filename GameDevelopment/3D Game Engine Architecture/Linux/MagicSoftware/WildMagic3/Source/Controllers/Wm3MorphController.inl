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
inline int MorphController::GetTargetQuantity () const
{
    return m_iTargetQuantity;
}
//----------------------------------------------------------------------------
inline int MorphController::GetKeyQuantity () const
{
    return m_iKeyQuantity;
}
//----------------------------------------------------------------------------
inline void MorphController::SetVertices (int i, Vector3fArray* pkVertices)
{
    assert( 0 <= i && i < m_iTargetQuantity );
    assert( pkVertices && pkVertices->GetQuantity() == m_iVertexQuantity );
    m_aspkVertices[i] = pkVertices;
}
//----------------------------------------------------------------------------
inline Vector3fArray* MorphController::GetVertices (int i) const
{
    assert( 0 <= i && i < m_iTargetQuantity );
    return m_aspkVertices[i];
}
//----------------------------------------------------------------------------
inline void MorphController::SetTimes (FloatArray* pkTimes)
{
    m_spkTimes = pkTimes;
}
//----------------------------------------------------------------------------
inline FloatArray* MorphController::GetTimes () const
{
    return m_spkTimes;
}
//----------------------------------------------------------------------------
inline void MorphController::SetWeights (int i, FloatArray* pkWeights)
{
    assert( 0 <= i && i < m_iKeyQuantity );
    m_aspkWeights[i] = pkWeights;
}
//----------------------------------------------------------------------------
inline FloatArray* MorphController::GetWeights (int i) const
{
    assert( 0 <= i && i < m_iKeyQuantity );
    return m_aspkWeights[i];
}
//----------------------------------------------------------------------------
