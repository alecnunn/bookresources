 // Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#include "MeshData.h"
//----------------------------------------------------------------------------
MeshData::MeshData()
{
    m_iNumOrigVert = 0;
    m_spkVertMap = NULL;
}
//----------------------------------------------------------------------------
MeshData::MeshData(int iNumOrigVert, IntArrayPtr spkVertMap)
{
    m_iNumOrigVert = iNumOrigVert;
    m_spkVertMap = spkVertMap;
}
//----------------------------------------------------------------------------
MeshData::~MeshData()
{
    m_spkVertMap = NULL;
}
//----------------------------------------------------------------------------
MeshData& MeshData::operator= (const MeshData& rkData)
{
    m_iNumOrigVert = rkData.m_iNumOrigVert;
    m_spkVertMap = rkData.m_spkVertMap;
    return *this;
}
//----------------------------------------------------------------------------
int MeshData::GetNumOrigVerts()
{
    return m_iNumOrigVert;
}
//----------------------------------------------------------------------------
int MeshData::GetMapping(int iIndex)
{
    return ((*m_spkVertMap)[iIndex]);
}
//----------------------------------------------------------------------------