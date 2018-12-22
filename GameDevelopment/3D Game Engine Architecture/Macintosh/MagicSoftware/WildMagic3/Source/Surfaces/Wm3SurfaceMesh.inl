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
inline int SurfaceMesh::GetLevel () const
{
    return m_iLevel;
}
//----------------------------------------------------------------------------
inline bool SurfaceMesh::IsLocked () const
{
    return m_spkOrigVertices == 0;
}
//----------------------------------------------------------------------------
