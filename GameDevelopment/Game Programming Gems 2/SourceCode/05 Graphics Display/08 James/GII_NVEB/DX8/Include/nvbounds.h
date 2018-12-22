/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
		 Bounding box support
    
  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVBOUNDS_H
#define __NVBOUNDS_H

#include "nvinc.h"
#include "nvdebug.h"

namespace nv_objects
{

class NVAABounds
{
public:
	NVAABounds()
		: m_vecCenter(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
			m_fRadius(0.0f),
			m_vecMinExtents(D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX)),
			m_vecMaxExtents(D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
	{}

	virtual void Add(NVAABounds* pThisBounds)
	{
		// Update the global extents
		if (m_vecMinExtents.x > pThisBounds->m_vecMinExtents.x)
			m_vecMinExtents.x = pThisBounds->m_vecMinExtents.x;

		if (m_vecMinExtents.y > pThisBounds->m_vecMinExtents.y)
			m_vecMinExtents.y = pThisBounds->m_vecMinExtents.y;

		if (m_vecMinExtents.z > pThisBounds->m_vecMinExtents.z)
			m_vecMinExtents.z = pThisBounds->m_vecMinExtents.z;

		if (m_vecMaxExtents.x < pThisBounds->m_vecMaxExtents.x)
			m_vecMaxExtents.x = pThisBounds->m_vecMaxExtents.x;

		if (m_vecMaxExtents.y < pThisBounds->m_vecMaxExtents.y)
			m_vecMaxExtents.y = pThisBounds->m_vecMaxExtents.y;

		if (m_vecMaxExtents.z < pThisBounds->m_vecMaxExtents.z)
			m_vecMaxExtents.z = pThisBounds->m_vecMaxExtents.z;
	}

	virtual void CalculateCenterRadius()
	{
		m_vecCenter = (m_vecMaxExtents + m_vecMinExtents) / 2.0f;

		// The bounding radius will either be from the center to the min or the center to the max
		// The max & min distance should be about the same, but I have seen differences, so we will select
		//  the max of the two anyway to ensure we get it right
		D3DXVECTOR3 maxDirection(m_vecMaxExtents - m_vecCenter);
		D3DXVECTOR3 minDirection(m_vecMinExtents - m_vecCenter);

		float fRadiusSquared1 = D3DXVec3Dot(&maxDirection, &maxDirection);
		float fRadiusSquared2 = D3DXVec3Dot(&minDirection, &minDirection);
		m_fRadius = (fRadiusSquared1 > fRadiusSquared2) ? sqrtf(fRadiusSquared1) : sqrtf(fRadiusSquared2);
	}

	virtual void Transform(const D3DXMATRIX* pMatrix)
	{
		D3DXVec3TransformCoord(&m_vecMinExtents, &m_vecMinExtents, pMatrix);
		D3DXVec3TransformCoord(&m_vecMaxExtents, &m_vecMaxExtents, pMatrix);

		D3DXVECTOR3 vecMin, vecMax;
		vecMin.x = min(m_vecMinExtents.x, m_vecMaxExtents.x);
		vecMin.y = min(m_vecMinExtents.y, m_vecMaxExtents.y);
		vecMin.z = min(m_vecMinExtents.z, m_vecMaxExtents.z);

		vecMax.x = max(m_vecMinExtents.x, m_vecMaxExtents.x);
		vecMax.y = max(m_vecMinExtents.y, m_vecMaxExtents.y);
		vecMax.z = max(m_vecMinExtents.z, m_vecMaxExtents.z);

		m_vecMaxExtents = vecMax;
		m_vecMinExtents = vecMin;
	}

public:
	D3DXVECTOR3 m_vecCenter;
	D3DXVECTOR3 m_vecMinExtents;
	D3DXVECTOR3 m_vecMaxExtents;
	float m_fRadius;
};

}; // nv_objects

#endif // __NVBOUNDS_H