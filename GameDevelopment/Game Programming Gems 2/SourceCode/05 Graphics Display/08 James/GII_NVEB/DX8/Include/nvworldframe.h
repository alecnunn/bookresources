/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
		 
    
  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVWORLDFRAME_H
#define __NVWORLDFRAME_H

#include "nvinc.h"
#include "nvdebug.h"
#include "nvcomplexmesh.h"
#include "nvworldtree.h"

namespace nv_objects
{

class NVComplexMesh;
class NVWorldTree;

// Animation keys
class NVRotateKey
{
public:
	DWORD			dwTime;
	D3DXQUATERNION	quatRotate;	
};
typedef std::vector<NVRotateKey> tRotateKeys;

struct NVPositionKey
{
public:
	DWORD	dwTime;
	D3DXVECTOR3	vPos;	
};
typedef std::vector<NVPositionKey> tPositionKeys;

struct NVScaleKey
{
public:
	DWORD	dwTime;
	D3DXVECTOR3	vScale;	
};
typedef std::vector<NVScaleKey> tScaleKeys;

struct NVMatrixKey
{
public:
	DWORD	dwTime;
	D3DXMATRIX	mat;	
};
typedef std::vector<NVMatrixKey> tMatrixKeys;
typedef std::vector<NVComplexMesh*> tMeshes;

class NVWorldFrame;
typedef std::list<NVWorldFrame*> tWorldFrames;

class NVWorldFrame
{
public:
	NVWorldFrame(NVWorldTree* pWorldTree, const char* pszName);
	virtual ~NVWorldFrame();

    void AddChildFrame(NVWorldFrame* pframe);
    void AddMesh(NVComplexMesh* pMesh);
	NVWorldFrame* FindFrame(const char *szFrame);

    void ResetMatrix();
	void UpdateFrames(const D3DXMATRIX* matCur);

	void SetTime(float fTime);
	void SetRotation(const D3DXMATRIX* pRotation);
	void SetTranslation(const D3DXMATRIX* pTranslation);

	tWorldFrames& GetChildren() { return m_Children; }
	const tMeshes& GetMeshes() { return m_Meshes; }
	const D3DXMATRIX* GetCurrentTransform() const { return &m_matCombined; }
	const D3DXMATRIX* GetFrameTransform() const { return &m_matRot; }
	const std::string& GetName() const { return m_strName; }
	bool IsAnimationFrame() const { return m_bAnimationFrame; }


#ifdef _DEBUG
	static DWORD m_dwIndent;
	void Dump();
#endif

private:
	friend NVWorldTree;

	NVWorldTree* m_pWorldTree;

	// Meshes for this frame, if there are any
	tMeshes m_Meshes;

    D3DXMATRIX m_matRot;
    D3DXMATRIX m_matTrans;
    D3DXMATRIX m_matRotOrig;
    D3DXMATRIX m_matCombined;

    // animation information
    tPositionKeys m_PositionKeys;
    tRotateKeys m_RotateKeys;
	tScaleKeys m_ScaleKeys;
	tMatrixKeys m_MatrixKeys;

    bool m_bAnimationFrame;

	NVWorldFrame* m_pframeToAnimate;
	tWorldFrames m_Children;

	std::string m_strName;

	

};
#ifdef _DEBUG
#define DEBUG_DUMP_NVWORLDFRAME(a) (a)->Dump();
#else
#define DEBUG_DUMP_NVWORLDFRAME(a)
#endif

}; // nv_objects

#endif //_NVWORLDFRAME_H

