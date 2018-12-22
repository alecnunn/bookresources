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
inline const TriMesh* BoundingVolumeTree::GetMesh () const
{
    return m_pkMesh;
}
//----------------------------------------------------------------------------
inline const BoundingVolume* BoundingVolumeTree::GetWorldBound () const
{
    return m_spkWorldBound;
}
//----------------------------------------------------------------------------
inline int BoundingVolumeTree::GetTriangleQuantity () const
{
    return m_iTriangleQuantity;
}
//----------------------------------------------------------------------------
inline int BoundingVolumeTree::GetTriangle (int i) const
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
inline const int* BoundingVolumeTree::GetTriangles () const
{
    return m_aiTriangle;
}
//----------------------------------------------------------------------------
inline BoundingVolumeTree* BoundingVolumeTree::GetLChild ()
{
    return m_pkLChild;
}
//----------------------------------------------------------------------------
inline BoundingVolumeTree* BoundingVolumeTree::GetRChild ()
{
    return m_pkRChild;
}
//----------------------------------------------------------------------------
inline bool BoundingVolumeTree::IsInteriorNode () const
{
    return m_pkLChild || m_pkRChild;
}
//----------------------------------------------------------------------------
inline bool BoundingVolumeTree::IsLeafNode () const
{
    return !m_pkLChild && !m_pkRChild;
}
//----------------------------------------------------------------------------
