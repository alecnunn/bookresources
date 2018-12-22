/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
		 
    
  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVWORLDTREE_H
#define __NVWORLDTREE_H

#include "nvinc.h"
#include "nvdebug.h"
#include "nvworldframe.h"

namespace nv_objects
{

// X file helpers
class NVXFileRotateKey
{
public:
	DWORD			dwTime;
	DWORD			dwFloats;	
	float			w;
	float			x;
	float			y;
	float			z;
};

class NVXFileScaleKey
{
public:
	DWORD	dwTime;
	DWORD	dwFloats;	
	D3DXVECTOR3	vScale;	
};


class NVXFilePositionKey
{
public:
	DWORD	dwTime;
	DWORD	dwFloats;	
	D3DXVECTOR3	vPos;	
};

class NVXFileMatrixKey
{
public:
	DWORD	dwTime;
	DWORD	dwFloats;	
	D3DXMATRIX	mat;	
};

class NVWorldFrame;
typedef std::list<NVWorldFrame*> tWorldFrames;

class NVWorldTree
{
public:
	NVWorldTree();
	virtual ~NVWorldTree();

	// Searches
    NVWorldFrame* FindFrame(const char *szName);
	HRESULT FindBones(NVWorldFrame* pRootFrame);
	
	// Animation
	void SetTime(float fElapsedTime);
	void UpdateFrames(const D3DXMATRIX* pmatCur, NVWorldFrame* pframeCur = NULL);
	void ComputeBounds(NVAABounds* pBounds);

	// Methods for X file support
	HRESULT LoadXFile(LPDIRECT3DDEVICE8 pD3DDevice, NVWorldFrame* pframeParent, const char* fileName);
    
	// Accessors
	tWorldFrames& GetMeshFrames() { return m_MeshFrames; }
	tWorldFrames& GetAnimationFrames() { return m_AnimationFrames; }
	NVWorldFrame* GetRootFrame() { return m_pframeRoot; }

#ifdef _DEBUG
	void Dump(NVWorldFrame* pframeCur);
#endif

private:
	// Helper functions
	HRESULT LoadXFileMesh(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent);
	HRESULT LoadXFileFrames(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent);
	HRESULT LoadXFileAnimation(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent);
	HRESULT LoadXFileAnimationSet(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent);

    D3DXVECTOR3 m_vCenter;
    float m_fRadius;

    // Frame hierarchy
	NVWorldFrame* m_pframeRoot;

	// Quick lists of animation and mesh frames
	tWorldFrames m_AnimationFrames;
	tWorldFrames m_MeshFrames;

	float m_fCurTime;
    float m_fMaxTime;

};
#ifdef _DEBUG
#define DEBUG_DUMP_NVWORLDTREE(a, b) (a)->Dump(b);
#else
#define DEBUG_DUMP_NVWORLDTREE(a, b)
#endif


}; // namespace nv_objects

#endif __NVWORLDTREE_H
