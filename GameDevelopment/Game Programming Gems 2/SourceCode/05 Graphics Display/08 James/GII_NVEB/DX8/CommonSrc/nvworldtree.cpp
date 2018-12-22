/*********************************************************************NVMH2****
Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "nvworldtree.h"
#include "rmxfguid.h"
#include "rmxftmpl.h"

using namespace std;

namespace nv_objects
{

NVWorldTree::NVWorldTree()
	: m_vCenter(0.0f, 0.0f, 0.0f),
	m_fRadius(1.0f)
{
	m_pframeRoot = new NVWorldFrame(this, "WorldTreeRoot");
}

NVWorldTree::~NVWorldTree()
{
    SAFE_DELETE(m_pframeRoot);

}


NVWorldFrame* NVWorldTree::FindFrame(const char *szName)
{
	return m_pframeRoot->FindFrame(szName);
}

void NVWorldTree::UpdateFrames(const D3DXMATRIX* pmatCur, NVWorldFrame* pframeCur)
{
	if (pframeCur == NULL)
	{
		pframeCur = m_pframeRoot;
	}
	NVASSERT(pframeCur, "No Root Frame!");

    pframeCur->UpdateFrames(pmatCur);
}

// Walks from the designated world frame down and assigns all bones to meshes
// with bone data
HRESULT NVWorldTree::FindBones(NVWorldFrame* pRootFrame)
{
    HRESULT hr = S_OK;
    
	// Walk all the meshes
	tWorldFrames::const_iterator itrFrames = m_MeshFrames.begin();
	while (itrFrames != m_MeshFrames.end())
	{
		const tMeshes& Meshes = (*itrFrames)->GetMeshes();
		tMeshes::const_iterator itrMeshes = Meshes.begin();
		while (itrMeshes != Meshes.end())
		{
			NVComplexMesh* pMesh = *itrMeshes;

			for (DWORD i = 0; i < pMesh->GetNumBones(); ++i)
			{
				const char* pBoneName = pMesh->GetBoneName(i);
				NVASSERT(pBoneName, "Couldn't find bone " << i); 
				if (pBoneName)
				{
					NVWorldFrame* pFrame = pRootFrame->FindFrame(pBoneName);
					NVASSERT(pFrame, "Couldn't find bone " << pBoneName); 
					if (pFrame)
					{
						pMesh->SetBoneMatrixPtr(i, pFrame->GetCurrentTransform());
					}
				}
			}
    
			itrMeshes++;
		}
		itrFrames++;
	}
	
    return hr;
}


#ifdef _DEBUG
void NVWorldTree::Dump(NVWorldFrame* pframeCur)
{
	if (pframeCur == NULL)
	{
		pframeCur = m_pframeRoot;
	}
	pframeCur->Dump();
}
#endif

void NVWorldTree::SetTime(float fElapsedTime)
{
	m_fCurTime += fElapsedTime;
	
	tWorldFrames::iterator itrAnimationFrames = m_AnimationFrames.begin();
	while(itrAnimationFrames != m_AnimationFrames.end())
	{
		(*itrAnimationFrames)->SetTime(m_fCurTime);
		itrAnimationFrames++;
	}
}

void NVWorldTree::ComputeBounds(NVAABounds* pBounds)
{
	pBounds->m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pBounds->m_fRadius = 0.0f;
	pBounds->m_vecMinExtents = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	pBounds->m_vecMaxExtents = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Walk all the meshes
	tWorldFrames::const_iterator itrFrames = m_MeshFrames.begin();
	while (itrFrames != m_MeshFrames.end())
	{
	
		const tMeshes& Meshes = (*itrFrames)->GetMeshes();
		tMeshes::const_iterator itrMeshes = Meshes.begin();
		while (itrMeshes != Meshes.end())
		{
			NVComplexMesh* pMesh = *itrMeshes;

			NVAABounds ThisBounds;

			// Get the mesh extents
			pMesh->ComputeBounds(&ThisBounds);

			ThisBounds.Transform((*itrFrames)->GetCurrentTransform());

			pBounds->Add(&ThisBounds);

			itrMeshes++;
		}
		itrFrames++;
	}
	
	pBounds->CalculateCenterRadius();
}


HRESULT NVWorldTree::LoadXFile(LPDIRECT3DDEVICE8 pD3DDev, NVWorldFrame* pframeParent, const char* fileName)
{
    HRESULT hr = S_OK;
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
    LPDIRECTXFILEDATA pxofobjCur = NULL;
    DWORD dwOptions = 0;
    
    if (fileName == NULL)
        return E_INVALIDARG;

	// If there's a path to this file, jump to this directory to ensure correct loading of textures, etc.
	std::string strDirectory = fileName;
	std::string::size_type Pos = strDirectory.find_last_of("\\", strDirectory.size());
	if (Pos != strDirectory.npos)
	{
		strDirectory = strDirectory.substr(0, Pos);
	}										
	else
	{
		strDirectory = ".";
	}
	// Make sure we are in the right directory for any associated resources
	SetCurrentDirectory(strDirectory.c_str());

	// If we don't have a root frame, create it
	if (!m_pframeRoot)
	{
		m_pframeRoot = new NVWorldFrame(this, fileName);
	}

	// If they didn't specify a parent, make the root frame the parent.
	if (!pframeParent)
	{
		pframeParent = m_pframeRoot;
	}

	// Create a DX file API
    hr = DirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;
    
    // Register templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;
    
    // Create enum object.
    hr = pxofapi->CreateEnumObject((LPVOID)fileName, DXFILELOAD_FROMFILE, &pxofenum);
    if (FAILED(hr))
        goto e_Exit;
        
    // Enumerate top level objects.
    // Top level objects are always data object.
    while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
    {
        hr = LoadXFileFrames(pD3DDev, pxofobjCur, dwOptions, m_pframeRoot);
        SAFE_RELEASE(pxofobjCur);
        
        if (FAILED(hr))
            goto e_Exit;
    }
    
    hr = FindBones(pframeParent);
    if (FAILED(hr))
        goto e_Exit;
 
	m_fCurTime = 0.0f;
    m_fMaxTime = 200.0f;
    
	m_pframeRoot->m_matRotOrig = m_pframeRoot->m_matRot;

    
e_Exit:
    SAFE_RELEASE(pxofobjCur);
    SAFE_RELEASE(pxofenum);
    SAFE_RELEASE(pxofapi);
    
    NVASSERT(SUCCEEDED(hr), "Couldn't load .x file!");
    
    return hr;
}

HRESULT NVWorldTree::LoadXFileMesh(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent)
{
    HRESULT hr = S_OK;
    DWORD cchName;
	NVComplexMesh* pMesh = NULL;
    
    hr = pxofobjCur->GetName(NULL, &cchName);
    if (FAILED(hr))
	{
		NVASSERT(0, "Could not get Mesh name!");
		return hr;
	}

	std::string strMeshName;
	strMeshName.resize(cchName);
    hr = pxofobjCur->GetName(&strMeshName[0], &cchName);
    if (FAILED(hr))
	{
		NVASSERT(0, "Could not get Mesh name!");
		return hr;
	}
	
	pMesh = new NVComplexMesh();
	
	if (pMesh->CreateFromXof(pD3DDevice, pxofobjCur))
	{
	    // add the mesh to the parent frame
		pframeParent->AddMesh(pMesh);
	}
	else
	{
		SAFE_DELETE(pMesh);
	}
   
    return hr;
}


HRESULT NVWorldTree::LoadXFileFrames(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent)
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    const GUID *type;
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    NVWorldFrame *pframeCur;
    DWORD cchName;
    
    // Get the type of the object
    hr = pxofobjCur->GetType(&type);
    if (FAILED(hr))
        goto e_Exit;
    
    
    if (*type == TID_D3DRMMesh)
    {
        hr = LoadXFileMesh(pD3DDevice, pxofobjCur, options, pframeParent);
        if (FAILED(hr))
            goto e_Exit;
    }
    else if (*type == TID_D3DRMFrameTransformMatrix)
    {
        hr = pxofobjCur->GetData(NULL, &cbSize, (PVOID*)&pmatNew);
        if (FAILED(hr))
            goto e_Exit;
        
        // update the parents matrix with the new one
	    pframeParent->m_matRot = *pmatNew;
        pframeParent->m_matRotOrig = *pmatNew;
    }
    else if (*type == TID_D3DRMAnimationSet)
    {
	    LoadXFileAnimationSet(pD3DDevice, pxofobjCur, options, pframeParent);
    }
    else if (*type == TID_D3DRMAnimation)
    {
        LoadXFileAnimation(pD3DDevice, pxofobjCur, options, pframeParent);
    }
    else if (*type == TID_D3DRMFrame)
    {
        hr = pxofobjCur->GetName(NULL, &cchName);
        if (FAILED(hr))
            goto e_Exit;

       	std::string strFrameName;
		strFrameName.resize(cchName);
		hr = pxofobjCur->GetName(&strFrameName[0], &cchName);
		if (FAILED(hr))
			goto e_Exit;

		pframeCur = new NVWorldFrame(this, strFrameName.c_str());
        if (pframeCur == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
   
        pframeParent->AddChildFrame(pframeCur);
        
        // Enumerate child objects.
        // Child object can be data, data reference or binary.
        // Use QueryInterface() to find what type of object a child is.
        while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
        {
            // Query the child for it's FileData
            hr = pxofChild->QueryInterface(IID_IDirectXFileData,
                (LPVOID *)&pxofobjChild);
            if (SUCCEEDED(hr))
            {
                hr = LoadXFileFrames(pD3DDevice, pxofobjChild, options, pframeCur);
                if (FAILED(hr))
                    goto e_Exit;
                
                SAFE_RELEASE(pxofobjChild);
            }
            
            SAFE_RELEASE(pxofChild);
        }
        
    }
    
e_Exit:
    SAFE_RELEASE(pxofobjChild);
    SAFE_RELEASE(pxofChild);
    return hr;
}


HRESULT NVWorldTree::LoadXFileAnimation(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent)
{
    HRESULT hr = S_OK;
    NVXFileRotateKey *pFileRotateKey;
    NVXFileScaleKey *pFileScaleKey;
    NVXFilePositionKey *pFilePosKey;
    NVXFileMatrixKey *pFileMatrixKey;
    NVWorldFrame *pframeCur;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    LPDIRECTXFILEDATAREFERENCE pxofobjChildRef = NULL;
    const GUID *type;
    DWORD dwSize;
    PBYTE pData;
    DWORD dwKeyType;
    DWORD cKeys;
    DWORD iKey;
    DWORD cchName;
   	std::string strAnimationName;

    hr = pxofobjCur->GetName(NULL, &cchName);
	if (FAILED(hr))
	{
		NVASSERT(0,"Couldn't find animation name");
		goto e_Exit;
	}

	strAnimationName.resize(cchName);
	hr = pxofobjCur->GetName(&strAnimationName[0], &cchName);
	if (FAILED(hr))
	{
		NVASSERT(0,"Couldn't find animation name");
		goto e_Exit;
	}

    pframeCur = new NVWorldFrame(this, strAnimationName.c_str());
    if (pframeCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pframeCur->m_bAnimationFrame = true;
    
    pframeParent->AddChildFrame(pframeCur);
    m_AnimationFrames.push_back(pframeCur);
    
    // Enumerate child objects.
    // Child object can be data, data reference or binary.
    // Use QueryInterface() to find what type of object a child is.
    while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
    {
        // Query the child for it's FileDataReference
        hr = pxofChild->QueryInterface(IID_IDirectXFileDataReference,
            (LPVOID *)&pxofobjChildRef);
        if (SUCCEEDED(hr))
        {
            hr = pxofobjChildRef->Resolve(&pxofobjChild);
            if (SUCCEEDED(hr))
            {
                hr = pxofobjChild->GetType(&type);
                if (FAILED(hr))
                    goto e_Exit;
                
                if( TID_D3DRMFrame == *type )
                {
                    if (pframeCur->m_pframeToAnimate != NULL)
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }

					hr = pxofobjChild->GetName(NULL, &cchName);
					if (FAILED(hr))
					{
						NVASSERT(0,"Couldn't find frame name");
						goto e_Exit;
					}

   					std::string strFrameName;
					strFrameName.resize(cchName);
					hr = pxofobjChild->GetName(&strFrameName[0], &cchName);
					if (FAILED(hr))
					{
						NVASSERT(0,"Couldn't find frame name");
						goto e_Exit;
					}

                    pframeCur->m_pframeToAnimate = FindFrame(strFrameName.c_str());
                    if (pframeCur->m_pframeToAnimate == NULL)
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                SAFE_RELEASE(pxofobjChild);
            }
            
            SAFE_RELEASE(pxofobjChildRef);
        }
        else
        {
            
            // Query the child for it's FileData
            hr = pxofChild->QueryInterface(IID_IDirectXFileData,
                (LPVOID *)&pxofobjChild);
            if (SUCCEEDED(hr))
            {
                hr = pxofobjChild->GetType(&type);
                if (FAILED(hr))
                    goto e_Exit;
                
                if ( TID_D3DRMFrame == *type )
                {
                    hr = LoadXFileFrames(pD3DDevice, pxofobjChild, options, pframeCur);
                    if (FAILED(hr))
                        goto e_Exit;
                }
                else if ( TID_D3DRMAnimationOptions == *type )
                {
                    //ParseAnimOptions(pChildData,pParentFrame);
                    //i=2;
                }
                else if ( TID_D3DRMAnimationKey == *type )
                {
                    hr = pxofobjChild->GetData( NULL, &dwSize, (PVOID*)&pData );
                    if (FAILED(hr))
                        goto e_Exit;
                    
                    dwKeyType = ((DWORD*)pData)[0];
                    cKeys = ((DWORD*)pData)[1];
                    
                    if (dwKeyType == 0)
                    {
                        if (!pframeCur->m_RotateKeys.empty())
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        //NOTE x files are w x y z and QUATERNIONS are x y z w
                      
                        pFileRotateKey = (NVXFileRotateKey*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							NVRotateKey RotateKey;
                            RotateKey.dwTime = pFileRotateKey->dwTime;
                            RotateKey.quatRotate.x = pFileRotateKey->x;
                            RotateKey.quatRotate.y = pFileRotateKey->y;
                            RotateKey.quatRotate.z = pFileRotateKey->z;
                            RotateKey.quatRotate.w = pFileRotateKey->w;
                            
							pframeCur->m_RotateKeys.push_back(RotateKey);

                            pFileRotateKey++;
                        }
                    }
                    else if (dwKeyType == 1)
                    {
                        if (!pframeCur->m_ScaleKeys.empty())
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pFileScaleKey =  (NVXFileScaleKey*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							NVScaleKey ScaleKey;
                            ScaleKey.dwTime = pFileScaleKey->dwTime;
                            ScaleKey.vScale = pFileScaleKey->vScale;

							pframeCur->m_ScaleKeys.push_back(ScaleKey);
                            
                            pFileScaleKey++;
                        }
                    }
                    else if (dwKeyType == 2)
                    {
                        if (!pframeCur->m_PositionKeys.empty())
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pFilePosKey =  (NVXFilePositionKey*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							NVPositionKey PositionKey;
                            PositionKey.dwTime = pFilePosKey->dwTime;
                            PositionKey.vPos = pFilePosKey->vPos;

							pframeCur->m_PositionKeys.push_back(PositionKey);
                            
                            pFilePosKey++;
                        }
                    }
                    else if (dwKeyType == 4)
                    {
                        if (!pframeCur->m_MatrixKeys.empty())
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
	                    pFileMatrixKey =  (NVXFileMatrixKey*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							NVMatrixKey MatrixKey;
                            MatrixKey.dwTime = pFileMatrixKey->dwTime;
                            MatrixKey.mat = pFileMatrixKey->mat;
                            
							pframeCur->m_MatrixKeys.push_back(MatrixKey);
                            
							pFileMatrixKey ++;
                        }
                    }
                    else
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                SAFE_RELEASE(pxofobjChild);
            }
        }
        
        SAFE_RELEASE(pxofChild);
    }
    
e_Exit:
    SAFE_RELEASE(pxofobjChild);
    SAFE_RELEASE(pxofChild);
    SAFE_RELEASE(pxofobjChildRef);
    return hr;
}

HRESULT NVWorldTree::LoadXFileAnimationSet(LPDIRECT3DDEVICE8 pD3DDevice, LPDIRECTXFILEDATA pxofobjCur, DWORD options, NVWorldFrame* pframeParent)
{
    NVWorldFrame *pframeCur;
    const GUID *type;
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    DWORD cchName;
   	std::string strAnimationName;

    hr = pxofobjCur->GetName(NULL, &cchName);
	if (FAILED(hr))
	{
		NVASSERT(0,"Couldn't find animation name");
		goto e_Exit;
	}

	strAnimationName.resize(cchName);
	hr = pxofobjCur->GetName(&strAnimationName[0], &cchName);
	if (FAILED(hr))
	{
		NVASSERT(0,"Couldn't find animation name");
		goto e_Exit;
	}

    pframeCur = new NVWorldFrame(this, strAnimationName.c_str());
    if (pframeCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    pframeCur->m_bAnimationFrame = true;
    
    pframeParent->AddChildFrame(pframeCur);
    
   
    // Enumerate child objects.
    // Child object can be data, data reference or binary.
    // Use QueryInterface() to find what type of object a child is.
    while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
    {
        // Query the child for it's FileData
        hr = pxofChild->QueryInterface(IID_IDirectXFileData,
            (LPVOID *)&pxofobjChild);
        if (SUCCEEDED(hr))
        {
            hr = pxofobjChild->GetType(&type);
            if (FAILED(hr))
                goto e_Exit;
            
            if( TID_D3DRMAnimation == *type )
            {
                hr = LoadXFileAnimation(pD3DDevice, pxofobjChild, options, pframeCur);
                if (FAILED(hr))
                    goto e_Exit;
            }
            
            SAFE_RELEASE(pxofobjChild);
        }
        
        SAFE_RELEASE(pxofChild);
    }
    
e_Exit:
    SAFE_RELEASE(pxofobjChild);
    SAFE_RELEASE(pxofChild);
    return hr;
}

}; // namespace nv_objects
