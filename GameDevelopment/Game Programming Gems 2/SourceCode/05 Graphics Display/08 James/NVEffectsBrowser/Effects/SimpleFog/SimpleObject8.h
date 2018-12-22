/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\SimpleFog
File:  SimpleObject8.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#ifndef __SimpleObject8_H__
#define __SimpleObject8_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <d3d8.h>
#include <d3dx8.h>

#include "SimpleObjectVertex.h"

//////////////////////////////////////////////////////////


#define MAX_VERT_IND		65535

//////////////////////////////////////////////////////////


class SimpleObject8  
{
public:

	SimpleObjVertex *	m_pVertices;
	WORD				m_wNumVerts;
	WORD			*	m_pIndices;
	WORD				m_nNumInd;

	D3DPRIMITIVETYPE m_PrimType;

	bool		m_bIsValid;


	void UnAllocate();	// Free all allocated memory
	void Allocate( WORD nvert, int nind );

	void InitSphere( float radius, int nLat, int nLong );  // Sphere with n_ verts each direction
	void InitSpiral( float width, float height, int n_sub_width,
					int n_sub_height, float n_twists );

	void AddPositionNoise( D3DXVECTOR3 vNoiseDir, float noise_magnitude );

	void AddPositionNoiseGrid( D3DXVECTOR3 vNoiseDir, float noise_magnitude,
								int grid_nx, int nsmooth );

	void TranslateXMinToZero();

	void GenerateNormals();			// from vertex pos and triangle data

	void Draw( LPDIRECT3DDEVICE8  pDev );

	SimpleObject8();
	virtual ~SimpleObject8();
};


#endif   // ndef __SimpleObject8_H__

