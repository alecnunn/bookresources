//--------------------------------------------------------------------------------------
// File: Frustum.h
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

#ifndef __FRUSTUM_H_
#define __FRUSTUM_H_

class Frustum
{
public:

	void SetFromMatrix(const D3DXMATRIX &matWorldViewProj);

	bool CullAABB(float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ) const;

private:
	D3DXPLANE m_Plane[6];
};

#endif // __FRUSTUM_H_
