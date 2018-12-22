//--------------------------------------------------------------------------------------
// File: Frustum.cpp
//
// Copyright (C) 2005 - Harald Vistnes (harald@vistnes.org)
// All rights reserved worldwide.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (C) Harald Vistnes, 2005"
//--------------------------------------------------------------------------------------

#include "dxstdafx.h"
#include "Frustum.h"

//----------------------------------------------------------------------------------

void Frustum::SetFromMatrix(const D3DXMATRIX &matWorldViewProj)
{
	// Left clipping plane
	m_Plane[0].a = matWorldViewProj._14 + matWorldViewProj._11;
	m_Plane[0].b = matWorldViewProj._24 + matWorldViewProj._21;
	m_Plane[0].c = matWorldViewProj._34 + matWorldViewProj._31;
	m_Plane[0].d = matWorldViewProj._44 + matWorldViewProj._41;
	// Right clipping plane
	m_Plane[1].a = matWorldViewProj._14 - matWorldViewProj._11;
	m_Plane[1].b = matWorldViewProj._24 - matWorldViewProj._21;
	m_Plane[1].c = matWorldViewProj._34 - matWorldViewProj._31;
	m_Plane[1].d = matWorldViewProj._44 - matWorldViewProj._41;
	// Top clipping plane
	m_Plane[2].a = matWorldViewProj._14 - matWorldViewProj._12;
	m_Plane[2].b = matWorldViewProj._24 - matWorldViewProj._22;
	m_Plane[2].c = matWorldViewProj._34 - matWorldViewProj._32;
	m_Plane[2].d = matWorldViewProj._44 - matWorldViewProj._42;
	// Bottom clipping plane
	m_Plane[3].a = matWorldViewProj._14 + matWorldViewProj._12;
	m_Plane[3].b = matWorldViewProj._24 + matWorldViewProj._22;
	m_Plane[3].c = matWorldViewProj._34 + matWorldViewProj._32;
	m_Plane[3].d = matWorldViewProj._44 + matWorldViewProj._42;
	// Near clipping plane
	m_Plane[4].a = matWorldViewProj._13;
	m_Plane[4].b = matWorldViewProj._23;
	m_Plane[4].c = matWorldViewProj._33;
	m_Plane[4].d = matWorldViewProj._43;
	// Far clipping plane
	m_Plane[5].a = matWorldViewProj._14 - matWorldViewProj._13;
	m_Plane[5].b = matWorldViewProj._24 - matWorldViewProj._23;
	m_Plane[5].c = matWorldViewProj._34 - matWorldViewProj._33;
	m_Plane[5].d = matWorldViewProj._44 - matWorldViewProj._43;
	
	// normalize the planes
	for (int i = 0; i < 6; i++) {
		D3DXPlaneNormalize(&m_Plane[i], &m_Plane[i]);
	}
}	

//----------------------------------------------------------------------------------

bool Frustum::CullAABB(float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ) const
{
	for (int i = 0; i < 6; i++) {
		int in = 8;

		D3DXVECTOR3 p0(fMinX,fMinY,fMinZ);
		D3DXVECTOR3 p1(fMaxX,fMinY,fMinZ);
		D3DXVECTOR3 p2(fMinX,fMaxY,fMinZ);
		D3DXVECTOR3 p3(fMaxX,fMaxY,fMinZ);
		D3DXVECTOR3 p4(fMinX,fMinY,fMaxZ);
		D3DXVECTOR3 p5(fMaxX,fMinY,fMaxZ);
		D3DXVECTOR3 p6(fMinX,fMaxY,fMaxZ);
		D3DXVECTOR3 p7(fMaxX,fMaxY,fMaxZ);

		if (D3DXPlaneDotCoord(&m_Plane[i], &p0) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p1) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p2) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p3) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p4) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p5) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p6) < 0) --in;
		if (D3DXPlaneDotCoord(&m_Plane[i], &p7) < 0) --in;

		if (in == 0) return true;
	}
	return false;
}
