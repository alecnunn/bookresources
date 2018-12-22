//-----------------------------------------------------------------------------
// File: mload.cpp
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <objbase.h>
#include <malloc.h> // _alloca
#include <stdio.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "rmxfguid.h"
#include "rmxftmpl.h"
#include "eb_effect.h"
#include "shader_PaletteSkin.h"

HRESULT CalculateSum(SFrame *pframe, D3DXMATRIX *pmatCur, D3DXVECTOR3 *pvCenter, 
                     UINT *pcVertices)
{
    HRESULT hr = S_OK;
    PBYTE pbPoints = NULL;
    UINT cVerticesLocal = 0;
    PBYTE pbCur;
    D3DXVECTOR3 *pvCur;
    D3DXVECTOR3 vTransformedCur;
    UINT iPoint;
    SMeshContainer *pmcCur;
    SFrame *pframeCur;
    UINT cVertices;
    D3DXMATRIX matLocal;
    
    D3DXMatrixMultiply(&matLocal, &pframe->matRot, pmatCur);
    
    pmcCur = pframe->pmcMesh;
    while (pmcCur != NULL)
    {
		DWORD fvfsize = D3DXGetFVFVertexSize(pmcCur->pMesh->GetFVF());
		
        cVertices = pmcCur->pMesh->GetNumVertices();
        
        hr = pmcCur->pMesh->LockVertexBuffer(0, &pbPoints);
        if (FAILED(hr))
            goto e_Exit;
        
        for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize)
        {
            pvCur = (D3DXVECTOR3*)pbCur;
            
            if ((pvCur->x != 0.0) || (pvCur->y != 0.0) || (pvCur->z != 0.0))
            {
                cVerticesLocal++;
                
                D3DXVec3TransformCoord(&vTransformedCur, pvCur, &matLocal);
                
                pvCenter->x += vTransformedCur.x;
                pvCenter->y += vTransformedCur.y;
                pvCenter->z += vTransformedCur.z;
            }
        }
        
        
        pmcCur->pMesh->UnlockVertexBuffer();
        pbPoints = NULL;
        
        pmcCur = pmcCur->pmcNext;
    }
    
    *pcVertices += cVerticesLocal;
    
    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = CalculateSum(pframeCur, &matLocal, pvCenter, pcVertices);
        if (FAILED(hr))
            goto e_Exit;
        
        pframeCur = pframeCur->pframeSibling;
    }
    
e_Exit:
    if (pbPoints != NULL)
    {
        pmcCur->pMesh->UnlockVertexBuffer();
    }
    
    return hr;
}




HRESULT CalculateRadius(SFrame *pframe, D3DXMATRIX *pmatCur, D3DXVECTOR3 *pvCenter, 
                        float *pfRadiusSq)
{
    HRESULT hr = S_OK;
    PBYTE pbPoints = NULL;
    PBYTE pbCur;
    D3DXVECTOR3 *pvCur;
    D3DXVECTOR3 vDist;;
    UINT iPoint;
    UINT cVertices;
    SMeshContainer *pmcCur;
    SFrame *pframeCur;
    float fRadiusLocalSq;
    float fDistSq;
    D3DXMATRIX matLocal;
    
    D3DXMatrixMultiply(&matLocal, &pframe->matRot, pmatCur);
    
    pmcCur = pframe->pmcMesh;
    fRadiusLocalSq = *pfRadiusSq;
    while (pmcCur != NULL)
    {
        DWORD fvfsize = D3DXGetFVFVertexSize(pmcCur->pMesh->GetFVF());
		
        cVertices = pmcCur->pMesh->GetNumVertices();
        
        hr = pmcCur->pMesh->LockVertexBuffer(0, &pbPoints);
        if (FAILED(hr))
            goto e_Exit;
        
        for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
        {
            pvCur = (D3DXVECTOR3*)pbCur;
            
            if ((pvCur->x == 0.0) && (pvCur->y == 0.0) && (pvCur->z == 0.0))
                continue;
            
            D3DXVec3TransformCoord(&vDist, pvCur, &matLocal);
            
            vDist -= *pvCenter;
            
            fDistSq = D3DXVec3LengthSq(&vDist);
            
            if( fDistSq > fRadiusLocalSq )
                fRadiusLocalSq = fDistSq;
        }
        
        
        pmcCur->pMesh->UnlockVertexBuffer();
        pbPoints = NULL;
        
        pmcCur = pmcCur->pmcNext;
    }
    
    *pfRadiusSq = fRadiusLocalSq;
    
    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = CalculateRadius(pframeCur, &matLocal, pvCenter, pfRadiusSq);
        if (FAILED(hr))
            goto e_Exit;
        
        pframeCur = pframeCur->pframeSibling;
    }
    
e_Exit:
    if (pbPoints != NULL)
    {
        pmcCur->pMesh->UnlockVertexBuffer();
    }
    
    return hr;
}




HRESULT CalculateBoundingSphere(SDrawElement *pdeCur)
{
    HRESULT hr = S_OK;
    D3DXVECTOR3 vCenter(0,0,0);
    UINT cVertices = 0;
    float fRadiusSq = 0;
    D3DXMATRIX matCur;
    
    D3DXMatrixIdentity(&matCur);
    hr = CalculateSum(pdeCur->pframeRoot, &matCur, &vCenter, &cVertices);
    if (FAILED(hr))
        goto e_Exit;
    
    if (cVertices > 0)
    {
        vCenter /= (float)cVertices;
        
        D3DXMatrixIdentity(&matCur);
        hr = CalculateRadius(pdeCur->pframeRoot, &matCur, &vCenter, &fRadiusSq);
        if (FAILED(hr))
            goto e_Exit;
    }
    
    pdeCur->fRadius = (float)sqrt((double)fRadiusSq);;
    pdeCur->vCenter = vCenter;
e_Exit:
    return hr;
}




HRESULT CShaderPaletteSkin::FindBones(SFrame *pframeCur, SDrawElement *pde)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;
    
    pmcMesh = pframeCur->pmcMesh;
    while (pmcMesh != NULL)
    {
        if (pmcMesh->m_pSkinMesh)
        {
            char** pBoneName = static_cast<char**>(pmcMesh->m_pBoneNamesBuf->GetBufferPointer());
            for (DWORD i = 0; i < pmcMesh->m_pSkinMesh->GetNumBones(); ++i)
            {
                SFrame* pFrame = pde->FindFrame(pBoneName[i]);
                pmcMesh->m_pBoneMatrix[i] = &(pFrame->matCombined);
            }
        }
        pmcMesh = pmcMesh->pmcNext;
    }
    
    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = FindBones(pframeChild, pde);
        if (FAILED(hr))
            return hr;
        
        pframeChild = pframeChild->pframeSibling;
    }
    
    return S_OK;
}




HRESULT CShaderPaletteSkin::LoadMeshHierarchy(const char* filePath)
{
    TCHAR* pszFile = const_cast<TCHAR*>(filePath);
    SDrawElement *pdeMesh = NULL;
    HRESULT hr = S_OK;
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
    LPDIRECTXFILEDATA pxofobjCur = NULL;
    DWORD dwOptions;
    int cchFileName;

	if (pszFile == NULL)
        return E_INVALIDARG;
    
    pdeMesh = new SDrawElement();
    
    delete pdeMesh->pframeRoot;
    pdeMesh->pframeAnimHead = NULL;
    
    pdeMesh->pframeRoot = new SFrame();
    if (pdeMesh->pframeRoot == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    
    dwOptions = 0;
    
    cchFileName = strlen(pszFile);
    if (cchFileName < 2)
    {
        hr = E_FAIL;
        goto e_Exit;
    }
    
    hr = DirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;
    
    // Register templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
        D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;
    
    // Create enum object.
    hr = pxofapi->CreateEnumObject((LPVOID)pszFile,
        DXFILELOAD_FROMFILE,
        &pxofenum);
    if (FAILED(hr))
        goto e_Exit;
    
    
    // Enumerate top level objects.
    // Top level objects are always data object.
    while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
    {
        hr = LoadFrames(pxofobjCur, pdeMesh, dwOptions, pdeMesh->pframeRoot);
        GXRELEASE(pxofobjCur);
        
        if (FAILED(hr))
            goto e_Exit;
    }
    
    hr = FindBones(pdeMesh->pframeRoot, pdeMesh);
    if (FAILED(hr))
        goto e_Exit;
    
    
    delete []pdeMesh->szName;
    pdeMesh->szName = new char[cchFileName+1];
    if (pdeMesh->szName == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memcpy(pdeMesh->szName, pszFile, cchFileName+1);

    // delete the current mesh, now that the load has succeeded
    DeleteSelectedMesh();

    // link into the draw list
    pdeMesh->pdeNext = m_pdeHead;
    m_pdeHead = pdeMesh;
    
    m_pdeSelected = pdeMesh;
    m_pmcSelectedMesh = pdeMesh->pframeRoot->pmcMesh;
    
    
    m_pframeSelected = pdeMesh->pframeRoot;
    
    hr = CalculateBoundingSphere(pdeMesh);
    if (FAILED(hr))
        goto e_Exit;
    
    m_pdeSelected->fCurTime = 0.0f;
    m_pdeSelected->fMaxTime = 200.0f;
    
    D3DXMatrixTranslation(&m_pdeSelected->pframeRoot->matRot,
        -pdeMesh->vCenter.x, -pdeMesh->vCenter.y, -pdeMesh->vCenter.z);
    m_pdeSelected->pframeRoot->matRotOrig = m_pdeSelected->pframeRoot->matRot;
    
e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    
    if (FAILED(hr))
    {
        delete pdeMesh;
    }
    
    return hr;
}




HRESULT CShaderPaletteSkin::LoadAnimation(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
                                         DWORD options, SFrame *pframeParent)
{
    HRESULT hr = S_OK;
    SRotateKeyXFile *pFileRotateKey;
    SScaleKeyXFile *pFileScaleKey;
    SPositionKeyXFile *pFilePosKey;
    SMatrixKeyXFile *pFileMatrixKey;
    SFrame *pframeCur;
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
    char *szFrameName;
    
    pframeCur = new SFrame();
    if (pframeCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pframeCur->bAnimationFrame = true;
    
    pframeParent->AddFrame(pframeCur);
    pde->AddAnimationFrame(pframeCur);
    
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
                    if (pframeCur->pframeToAnimate != NULL)
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                    
                    hr = pxofobjChild->GetName(NULL, &cchName);
                    if (FAILED(hr))
                        goto e_Exit;
                    
                    if (cchName == 0)
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                        
                    }
                    
                    szFrameName = (char*)_alloca(cchName);
                    if (szFrameName == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                    }
                    
                    hr = pxofobjChild->GetName(szFrameName, &cchName);
                    if (FAILED(hr))
                        goto e_Exit;
                    
                    pframeCur->pframeToAnimate = pde->FindFrame(szFrameName);
                    if (pframeCur->pframeToAnimate == NULL)
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                GXRELEASE(pxofobjChild);
            }
            
            GXRELEASE(pxofobjChildRef);
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
                    hr = LoadFrames(pxofobjChild, pde, options, pframeCur);
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
                        if (pframeCur->m_pRotateKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_pRotateKeys = new SRotateKey[cKeys];
                        if (pframeCur->m_pRotateKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_cRotateKeys = cKeys;
                        //NOTE x files are w x y z and QUATERNIONS are x y z w
                        
                        pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
                            pframeCur->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;
                            pframeCur->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
                            pframeCur->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
                            pframeCur->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
                            pframeCur->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
                            
                            pFileRotateKey += 1;
                        }
                    }
                    else if (dwKeyType == 1)
                    {
                        if (pframeCur->m_pScaleKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_pScaleKeys = new SScaleKey[cKeys];
                        if (pframeCur->m_pScaleKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_cScaleKeys = cKeys;
                        
                        pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
                            pframeCur->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
                            pframeCur->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;
                            
                            pFileScaleKey += 1;
                        }
                    }
                    else if (dwKeyType == 2)
                    {
                        if (pframeCur->m_pPositionKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_pPositionKeys = new SPositionKey[cKeys];
                        if (pframeCur->m_pPositionKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_cPositionKeys = cKeys;
                        
                        pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
                            pframeCur->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
                            pframeCur->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;
                            
                            pFilePosKey += 1;
                        }
                    }
                    else if (dwKeyType == 4)
                    {
                        if (pframeCur->m_pMatrixKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_pMatrixKeys = new SMatrixKey[cKeys];
                        if (pframeCur->m_pMatrixKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
                        pframeCur->m_cMatrixKeys = cKeys;
                        
                        pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
                            pframeCur->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
                            pframeCur->m_pMatrixKeys[iKey].mat = pFileMatrixKey->mat;
                            
                            pFileMatrixKey += 1;
                        }
                    }
                    else
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
                }
                
                GXRELEASE(pxofobjChild);
            }
        }
        
        GXRELEASE(pxofChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    GXRELEASE(pxofobjChildRef);
    return hr;
}




HRESULT CShaderPaletteSkin::LoadAnimationSet(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
                                            DWORD options, SFrame *pframeParent)
{
    SFrame *pframeCur;
    const GUID *type;
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    DWORD cchName;
    
    pframeCur = new SFrame();
    if (pframeCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pframeCur->bAnimationFrame = true;
    
    pframeParent->AddFrame(pframeCur);
    
    hr = pxofobjCur->GetName(NULL, &cchName);
    if (FAILED(hr))
        goto e_Exit;
    
    if (cchName > 0)
    {
        pframeCur->szName = new char[cchName];
        if (pframeCur->szName == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName(pframeCur->szName, &cchName);
        if (FAILED(hr))
            goto e_Exit;
    }
    
    
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
                hr = LoadAnimation(pxofobjChild, pde, options, pframeCur);
                if (FAILED(hr))
                    goto e_Exit;
            }
            
            GXRELEASE(pxofobjChild);
        }
        
        GXRELEASE(pxofChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    return hr;
}




HRESULT SplitMesh
        (
            LPD3DXMESH  pMesh,              // ASSUMPTION:  *pMesh is attribute sorted & has a valid attribute table
            DWORD       iAttrSplit,         // **ppMeshB gets the mesh comprising of this attribute range onward
            DWORD*      rgiAdjacency, 
            DWORD       optionsA, 
            DWORD       optionsB, 
            LPD3DXMESH* ppMeshA, 
            LPD3DXMESH* ppMeshB
        )
{
    *ppMeshA = NULL;

    *ppMeshB = NULL;

    
    HRESULT hr  = S_OK;

    PBYTE   pbVerticesIn    = NULL;
    PBYTE   pbIndicesIn     = NULL;
    DWORD*  piAttribsIn     = NULL;

    LPD3DXMESH pMeshA   = NULL;
    LPD3DXMESH pMeshB   = NULL;
    
    LPD3DXBUFFER pVertexRemapA  = NULL;
    LPD3DXBUFFER pVertexRemapB  = NULL;
        
    DWORD*  rgiAdjacencyA   = NULL;
    DWORD*  rgiAdjacencyB   = NULL;




    LPDIRECT3DDEVICE8   pDevice     = NULL;


    D3DXATTRIBUTERANGE* rgAttrTable = NULL;

    DWORD               cAttrTable  = 0;



    DWORD   cVerticesA;
    DWORD   cVerticesB;

    DWORD   cFacesA;
    DWORD   cFacesB;

    DWORD   cbVertexSize;

    DWORD   dw32bit;


    dw32bit         = pMesh->GetOptions() & D3DXMESH_32BIT;

    //cbVertexSize    = D3DXGetFVFVertexSize(pMesh->GetFVF());
	cbVertexSize = sizeof(PSVertex);

    hr  = pMesh->GetDevice(&pDevice);

    if (FAILED(hr))
        goto e_Exit;


    hr  = pMesh->GetAttributeTable(NULL, &cAttrTable);

    if (FAILED(hr))
        goto e_Exit;

    
    rgAttrTable = new D3DXATTRIBUTERANGE[cAttrTable];

    if (rgAttrTable == NULL)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit;
    }


    hr  = pMesh->GetAttributeTable(rgAttrTable, NULL);

    if (FAILED(hr))
        goto e_Exit;

    if (iAttrSplit == 0)
    {
        cVerticesA  = 0;

        cFacesA     = 0;
    }
    else if (iAttrSplit >= cAttrTable)
    {
        cVerticesA  = pMesh->GetNumVertices();

        cFacesA     = pMesh->GetNumFaces();
    }
    else
    {
        cVerticesA  = rgAttrTable[iAttrSplit].VertexStart;

        cFacesA     = rgAttrTable[iAttrSplit].FaceStart;
    }

    cVerticesB  = pMesh->GetNumVertices() - cVerticesA;

    cFacesB     = pMesh->GetNumFaces() - cFacesA;


    hr  = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pbVerticesIn);

    if (FAILED(hr))
        goto e_Exit;


    hr  = pMesh->LockIndexBuffer(D3DLOCK_READONLY, &pbIndicesIn);

    if (FAILED(hr))
        goto e_Exit;


    hr  = pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &piAttribsIn);

    if (FAILED(hr))
        goto e_Exit;


    if (cFacesA && cVerticesA)
    {
        PBYTE   pbVerticesOut   = NULL;
        PBYTE   pbIndicesOut    = NULL;
        DWORD*  piAttribsOut    = NULL;
        DWORD i;

        hr  = D3DXCreateMeshFVF(cFacesA, cVerticesA, optionsA | dw32bit, pMesh->GetFVF(), pDevice, &pMeshA);

        if (FAILED(hr))
            goto e_ExitA;


        hr  = pMeshA->LockVertexBuffer(0, &pbVerticesOut);

        if (FAILED(hr))
            goto e_ExitA;


        hr  = pMeshA->LockIndexBuffer(0, (LPBYTE*)&pbIndicesOut);

        if (FAILED(hr))
            goto e_ExitA;


        hr  = pMeshA->LockAttributeBuffer(0, &piAttribsOut);

        if (FAILED(hr))
            goto e_ExitA;


        memcpy(pbVerticesOut, pbVerticesIn, cVerticesA * cbVertexSize * sizeof(BYTE));

        if (dw32bit)
        {
            memcpy(pbIndicesOut, pbIndicesIn, cFacesA * 3 * sizeof(DWORD));
        }
        else
        {
            memcpy(pbIndicesOut, pbIndicesIn, cFacesA * 3 * sizeof(WORD));
        }

      
        memcpy(piAttribsOut, piAttribsIn, cFacesA * sizeof(DWORD));


        rgiAdjacencyA   = new DWORD[cFacesA * 3];

        if (rgiAdjacencyA == NULL)
        {
            hr  = E_OUTOFMEMORY;

            goto e_ExitA;
        }


        for (i = 0; i <  cFacesA * 3; i++)
        {
            rgiAdjacencyA[i]    = (rgiAdjacency[i] < cFacesA) ? rgiAdjacency[i] : 0xFFFFFFFF;
        }


e_ExitA:

        if (pbVerticesOut != NULL)
        {
            pMeshA->UnlockVertexBuffer();
        }


        if (pbIndicesOut != NULL)
        {
            pMeshA->UnlockIndexBuffer();
        }


        if (piAttribsOut != NULL)
        {
            pMeshA->UnlockAttributeBuffer();
        }


        if (FAILED(hr))
            goto e_Exit;
    }


    // calculate Mesh A's attribute table

    if (pMeshA != NULL)
    {
        hr  = pMeshA->OptimizeInplace
                      (
                          D3DXMESHOPT_VERTEXCACHE,
                          rgiAdjacencyA,
                          NULL,
                          NULL,
                          &pVertexRemapA
                      );

        if (FAILED(hr))
            goto e_Exit;
    }


    if (cFacesB && cVerticesB)
    {
        PBYTE   pbVerticesOut   = NULL;
        PBYTE   pbIndicesOut    = NULL;
        DWORD*  piAttribsOut    = NULL;
        DWORD i;

        hr  = D3DXCreateMeshFVF(cFacesB, cVerticesB, optionsB | dw32bit, pMesh->GetFVF(), pDevice, &pMeshB);

        if (FAILED(hr))
            goto e_ExitB;


        hr  = pMeshB->LockVertexBuffer(0, &pbVerticesOut);

        if (FAILED(hr))
            goto e_ExitB;


        hr  = pMeshB->LockIndexBuffer(0, &pbIndicesOut);

        if (FAILED(hr))
            goto e_ExitB;


        hr  = pMeshB->LockAttributeBuffer(0, &piAttribsOut);

        if (FAILED(hr))
            goto e_ExitB;


        memcpy(pbVerticesOut, pbVerticesIn + (cVerticesA * cbVertexSize), cVerticesB * cbVertexSize * sizeof(BYTE));


        // copy & renumber indices

        if (dw32bit)
        {
            for (DWORD i = 0; i < cFacesB * 3; i++)
            {
                ((DWORD*)pbIndicesOut)[i]    = ((DWORD*)pbIndicesIn)[(cFacesA * 3) + i]  - (DWORD)cVerticesA;
            }
        }
        else
        {
            for (DWORD i = 0; i < cFacesB * 3; i++)
            {
                ((WORD*)pbIndicesOut)[i]    = ((WORD*)pbIndicesIn)[(cFacesA * 3) + i]  - (WORD)cVerticesA;
            }
        }

        memcpy(piAttribsOut, piAttribsIn + cFacesA, cFacesB * sizeof(DWORD));


        rgiAdjacencyB   = new DWORD[cFacesB * 3];

        if (rgiAdjacencyB == NULL)
        {
            hr  = E_OUTOFMEMORY;

            goto e_ExitB;
        }


        // copy & renumber adjacency

        for (i = 0; i < cFacesB * 3; i++)
        {
            rgiAdjacencyB[i]    = (rgiAdjacency[(cFacesA * 3) + i] >= cFacesA && rgiAdjacency[(cFacesA * 3) + i] != 0xFFFFFFFF) ? rgiAdjacency[(cFacesA * 3) + i] - cFacesA : 0xFFFFFFFF;
        }


e_ExitB:

        if (pbVerticesOut != NULL)
        {
            pMeshB->UnlockVertexBuffer();
        }


        if (pbIndicesOut != NULL)
        {
            pMeshB->UnlockIndexBuffer();
        }


        if (piAttribsOut != NULL)
        {
            pMeshB->UnlockAttributeBuffer();
        }


        if (FAILED(hr))
            goto e_Exit;
    }


    // calculate Mesh B's attribute table

    if (pMeshB != NULL)
    {
        hr  = pMeshB->OptimizeInplace
                      (
                          D3DXMESHOPT_ATTRSORT,
                          rgiAdjacencyB,
                          NULL,
                          NULL,
                          &pVertexRemapB
                      );

        if (FAILED(hr))
            goto e_Exit;
    }


e_Exit:
    
    if (rgAttrTable != NULL)
    {
        delete[] rgAttrTable;
    }

    if (pbVerticesIn != NULL)
    {
        pMesh->UnlockVertexBuffer();
    }

    if (pbIndicesIn != NULL)
    {
        pMesh->UnlockIndexBuffer();
    }

    if (piAttribsIn != NULL)
    {
        pMesh->UnlockAttributeBuffer();
    }
    
    if (rgiAdjacencyA != NULL)
    {
        delete[] rgiAdjacencyA;
    }

    if (rgiAdjacencyB != NULL)
    {
        delete[] rgiAdjacencyB;
    }

    GXRELEASE(pDevice);

    GXRELEASE(pVertexRemapA);

    GXRELEASE(pVertexRemapB);

    if (FAILED(hr))
    {
        GXRELEASE(pMeshA);

        GXRELEASE(pMeshB);

        pMeshA  = NULL;

        pMeshB  = NULL;
    }

    *ppMeshA    = pMeshA;

    *ppMeshB    = pMeshB;

    return hr;
}

HRESULT CShaderPaletteSkin::CreateBasisVectors(LPDIRECT3DVERTEXBUFFER8 pVertexBuffer, LPDIRECT3DINDEXBUFFER8 pIndexBuffer)
{
	DWORD i;
	PSVertex* pVertices;
	WORD* pIndices;
	HRESULT hr;
	
	assert(pVertexBuffer);
	assert(pIndexBuffer);
	
	// Calculate number of vertices and indices
	D3DVERTEXBUFFER_DESC VBDesc;
	D3DINDEXBUFFER_DESC IBDesc;
	
	pVertexBuffer->GetDesc(&VBDesc);
	pIndexBuffer->GetDesc(&IBDesc);
	
	DWORD dwNumIndices;
	DWORD dwNumVertices;
	switch(IBDesc.Format)
	{
	case D3DFMT_INDEX16:
		dwNumIndices = IBDesc.Size / 2;
		break;
	case D3DFMT_INDEX32:
		dwNumIndices = IBDesc.Size / 4;
		break;
	default:
		assert(0);
		return E_FAIL;
	}
	
	dwNumVertices = VBDesc.Size / sizeof(PSVertex);
	
	// Get a pointer to the indices and the vertices
	hr = pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	
	hr = pIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);
	if (FAILED(hr))
		return hr;
	
	// Clear the basis vectors
	for (i = 0; i < dwNumVertices; i++)
	{
		pVertices[i].S = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVertices[i].T = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
	// Walk through the triangle list and calculate gradiants for each triangle.
	// Sum the results into the S and T components.
    for( i = 0; i < dwNumIndices; i += 3 )
    {       
		DWORD TriIndex[3];
		D3DXVECTOR3 du, dv;
		D3DXVECTOR3 edge01;
		D3DXVECTOR3 edge02;
		D3DXVECTOR3 cp;
		
		TriIndex[0] = pIndices[i];
		TriIndex[1] = pIndices[i+1];
		TriIndex[2] = pIndices[i+2];
		
		assert((TriIndex[0] < dwNumVertices) && (TriIndex[1] < dwNumVertices) && (TriIndex[2] < dwNumVertices));
		
		PSVertex& v0 = pVertices[TriIndex[0]];
		PSVertex& v1 = pVertices[TriIndex[1]];
		PSVertex& v2 = pVertices[TriIndex[2]];
		
		// x, s, t
		edge01 = D3DXVECTOR3( v1.Position.x - v0.Position.x, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.x - v0.Position.x, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );
		
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > 1e-12 )
		{
			v0.S.x += -cp.y / cp.x;
			v0.T.x += -cp.z / cp.x;
			
			v1.S.x += -cp.y / cp.x;
			v1.T.x += -cp.z / cp.x;
			
			v2.S.x += -cp.y / cp.x;
			v2.T.x += -cp.z / cp.x;
		}
		
		// y, s, t
		edge01 = D3DXVECTOR3( v1.Position.y - v0.Position.y, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.y - v0.Position.y, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );
		
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > 1e-12 )
		{
			v0.S.y += -cp.y / cp.x;
			v0.T.y += -cp.z / cp.x;
			
			v1.S.y += -cp.y / cp.x;
			v1.T.y += -cp.z / cp.x;
			
			v2.S.y += -cp.y / cp.x;
			v2.T.y += -cp.z / cp.x;
		}
		
		// z, s, t
		edge01 = D3DXVECTOR3( v1.Position.z - v0.Position.z, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.z - v0.Position.z, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );
		
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > 1e-12 )
		{
			v0.S.z += -cp.y / cp.x;
			v0.T.z += -cp.z / cp.x;
			
			v1.S.z += -cp.y / cp.x;
			v1.T.z += -cp.z / cp.x;
			
			v2.S.z += -cp.y / cp.x;
			v2.T.z += -cp.z / cp.x;
		}
    }
	
    // Calculate the SxT vector
	for(i = 0; i < dwNumVertices; i++)
	{		
		// Normalize the S, T vectors
		D3DXVec3Normalize(&pVertices[i].S, &pVertices[i].S);
		D3DXVec3Normalize(&pVertices[i].T, &pVertices[i].T);
		
		// Get the cross of the S and T vectors
		D3DXVec3Cross(&pVertices[i].SxT, &pVertices[i].S, &pVertices[i].T);
		
		// Need a normalized normal
		D3DXVec3Normalize(&pVertices[i].Normal, &pVertices[i].Normal);
		
		// v coordinates go in opposite direction from the texture v increase in xyz
		pVertices[i].T = -pVertices[i].T;
		
		// Get the direction of the SxT vector
		if (D3DXVec3Dot(&pVertices[i].SxT, &pVertices[i].Normal) < 0.0f)
		{
			pVertices[i].SxT = -pVertices[i].SxT;
		}
	}
	
	pVertexBuffer->Unlock();
	pIndexBuffer->Unlock();
	
	return S_OK;

}



HRESULT CShaderPaletteSkin::GenerateMesh(SMeshContainer *pmcMesh)
{
    // ASSUMPTION:  pmcMesh->m_rgiAdjacency contains the current adjacency
    HRESULT hr  = S_OK;

    DWORD*  pAdjacencyIn    = NULL;

    DWORD   cFaces  = pmcMesh->m_pSkinMesh->GetNumFaces();

    pAdjacencyIn    = new DWORD[pmcMesh->m_pSkinMesh->GetNumFaces() * 3];

    if (pAdjacencyIn == NULL)
    {
        hr = E_OUTOFMEMORY;

        return hr;
    }
    
    memcpy(pAdjacencyIn, pmcMesh->m_rgiAdjacency, cFaces * 3 * sizeof(DWORD));

    GXRELEASE(pmcMesh->pMesh);
    GXRELEASE(pmcMesh->pMeshHW);
    GXRELEASE(pmcMesh->pMeshSW);

    pmcMesh->pMesh      = NULL;
    pmcMesh->pMeshHW    = NULL;
    pmcMesh->pMeshSW    = NULL;
    
	hr = pmcMesh->m_pSkinMesh->ConvertToIndexedBlendedMesh(D3DXMESH_SYSTEMMEM, pAdjacencyIn, PALETTE_SIZE/*255*/, NULL,
		&pmcMesh->cpattr, &pmcMesh->m_pBoneCombinationBuf, &pmcMesh->pMesh);
	if (FAILED(hr))
		return hr;
	
	//get attribute table
	pmcMesh->m_pAttrTable  = new D3DXATTRIBUTERANGE[pmcMesh->cpattr];
	
	hr = pmcMesh->pMesh->GetAttributeTable(pmcMesh->m_pAttrTable, NULL);
	
	if (FAILED(hr))
		return hr;
	
	//create new VB for rendering purposes
	MeshVertex* meshPtr;
	PSVertex* psPtr;
	
	m_pD3DDev->CreateVertexBuffer(sizeof(PSVertex)*(pmcMesh->pMesh->GetNumVertices()), D3DUSAGE_WRITEONLY, 
		0, D3DPOOL_DEFAULT, &m_pRenderVB);
	
	//lock and fill
	m_pRenderVB->Lock(0, 0, (BYTE**)&psPtr, 0);
	pmcMesh->pMesh->LockVertexBuffer(0, (BYTE**)&meshPtr);
	
	for(int i = 0; i < pmcMesh->pMesh->GetNumVertices(); i++)
	{
		memset(&psPtr[i], 0, sizeof(PSVertex));
		
		psPtr[i].Position = meshPtr[i].Position;
		psPtr[i].weight0  = meshPtr[i].weight;
		psPtr[i].weight1  = 1.0f - meshPtr[i].weight;
		psPtr[i].index1	  = (float)((int)(meshPtr[i].indices & 0x000000FF)) * 3.f;
		psPtr[i].index2   = (float)((int)((meshPtr[i].indices & 0x0000FF00) >> 8)) * 3.f;
		psPtr[i].Normal   = meshPtr[i].Normal;
		psPtr[i].Texture  = meshPtr[i].Texture;
	}
	pmcMesh->pMesh->UnlockVertexBuffer();
	m_pRenderVB->Unlock();
	
	//go through and fill in S, T, SxT
	LPDIRECT3DINDEXBUFFER8 tempIB;
	pmcMesh->pMesh->GetIndexBuffer(&tempIB);
	CreateBasisVectors(m_pRenderVB, tempIB);
	tempIB->Release();  //decrement reference count

    delete[] pAdjacencyIn;

    return hr;
}




HRESULT CShaderPaletteSkin::LoadMesh(LPDIRECTXFILEDATA pxofobjCur, DWORD options, SFrame *pframeParent)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh = NULL;
    LPD3DXBUFFER pbufMaterials = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    DWORD cchName;
    UINT cFaces;
    UINT iMaterial;
    
    pmcMesh = new SMeshContainer();
    if (pmcMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    hr = pxofobjCur->GetName(NULL, &cchName);
    if (FAILED(hr))
        goto e_Exit;
    
    if (cchName > 0)
    {
        pmcMesh->szName = new char[cchName];
        if (pmcMesh->szName == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName(pmcMesh->szName, &cchName);
        if (FAILED(hr))
            goto e_Exit;
    }
    
    hr = D3DXLoadSkinMeshFromXof(pxofobjCur, options, m_pD3DDev, &pbufAdjacency, &pbufMaterials, &pmcMesh->cMaterials, 
        &pmcMesh->m_pBoneNamesBuf, &pmcMesh->m_pBoneOffsetBuf, &pmcMesh->m_pSkinMesh);
    if (FAILED(hr))
        goto e_Exit;
    
    cFaces = pmcMesh->m_pSkinMesh->GetNumFaces();

    // Process skinning data
    if (pmcMesh->m_pSkinMesh->GetNumBones())
    {
        pmcMesh->m_pBoneMatrix = new D3DXMATRIX*[pmcMesh->m_pSkinMesh->GetNumBones()];
        if (pmcMesh->m_pBoneMatrix == NULL)
            goto e_Exit;
        pmcMesh->m_pBoneOffsetMat = reinterpret_cast<D3DXMATRIX*>(pmcMesh->m_pBoneOffsetBuf->GetBufferPointer());
        LPDWORD pAdjacencyIn = static_cast<LPDWORD>(pbufAdjacency->GetBufferPointer());

        pmcMesh->m_rgiAdjacency = new DWORD[cFaces * 3];

        if (pmcMesh->m_rgiAdjacency == NULL)
        {
            hr = E_OUTOFMEMORY;

            goto e_Exit;
        }

        memcpy(pmcMesh->m_rgiAdjacency, pAdjacencyIn, cFaces * 3 * sizeof(DWORD));
        
        hr = GenerateMesh(pmcMesh);

        if (FAILED(hr))
            goto e_Exit;
    }
    else
    {
        pmcMesh->m_pSkinMesh->GetOriginalMesh(&(pmcMesh->pMesh));
        pmcMesh->m_pSkinMesh->Release();
        pmcMesh->m_pSkinMesh = NULL;
        pmcMesh->cpattr = pmcMesh->cMaterials;
    }
    
    if ((pbufMaterials == NULL) || (pmcMesh->cMaterials == 0))
    {
        pmcMesh->rgMaterials = new D3DMATERIAL8[1];
        pmcMesh->pTextures = new LPDIRECT3DTEXTURE8[1];
        if (pmcMesh->rgMaterials == NULL || pmcMesh->pTextures == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        memset(pmcMesh->rgMaterials, 0, sizeof(D3DXMATERIAL));
        pmcMesh->rgMaterials[0].Diffuse.r = 0.5f;
        pmcMesh->rgMaterials[0].Diffuse.g = 0.5f;
        pmcMesh->rgMaterials[0].Diffuse.b = 0.5f;
        pmcMesh->rgMaterials[0].Specular = pmcMesh->rgMaterials[0].Diffuse;
        pmcMesh->pTextures[0] = NULL;
    }
    else
    {
        pmcMesh->rgMaterials = new D3DMATERIAL8[pmcMesh->cMaterials];
        pmcMesh->pTextures = new LPDIRECT3DTEXTURE8[pmcMesh->cMaterials];
        if (pmcMesh->rgMaterials == NULL || pmcMesh->pTextures == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();
        
        for (iMaterial = 0; iMaterial < pmcMesh->cMaterials; iMaterial++)
        {
            
            pmcMesh->rgMaterials[iMaterial] = pMaterials[iMaterial].MatD3D;
            
            pmcMesh->pTextures[iMaterial] = NULL;
            if (pMaterials[iMaterial].pTextureFilename != NULL)
            {
                TCHAR szPath[MAX_PATH] = "tiny_skin.bmp";
                //DXUtil_FindMediaFile(szPath, pMaterials[iMaterial].pTextureFilename);
				//strcpy(szPath, GetFilePath(pMaterials[iMaterial].pTextureFilename).c_str());

                hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("tiny_skin.bmp").c_str(), &(pmcMesh->pTextures[iMaterial]));
                if (FAILED(hr))
                    pmcMesh->pTextures[iMaterial] = NULL;
            }
        }
    }
    
    // add the mesh to the parent frame
    pframeParent->AddMesh(pmcMesh);
    pmcMesh = NULL;
    
e_Exit:
    delete pmcMesh;
    
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pbufMaterials);

    return hr;
}




HRESULT CShaderPaletteSkin::LoadFrames(LPDIRECTXFILEDATA pxofobjCur, SDrawElement *pde,
                                      DWORD options, SFrame *pframeParent)
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    const GUID *type;
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    SFrame *pframeCur;
    DWORD cchName;
    
    // Get the type of the object
    hr = pxofobjCur->GetType(&type);
    if (FAILED(hr))
        goto e_Exit;
    
    
    if (*type == TID_D3DRMMesh)
    {
        hr = LoadMesh(pxofobjCur, options, pframeParent);
        if (FAILED(hr))
            goto e_Exit;
    }
    else if (*type == TID_D3DRMFrameTransformMatrix)
    {
        hr = pxofobjCur->GetData(NULL, &cbSize, (PVOID*)&pmatNew);
        if (FAILED(hr))
            goto e_Exit;
        
        // update the parents matrix with the new one
        pframeParent->matRot = *pmatNew;
        pframeParent->matRotOrig = *pmatNew;
    }
    else if (*type == TID_D3DRMAnimationSet)
    {
        LoadAnimationSet(pxofobjCur, pde, options, pframeParent);
    }
    else if (*type == TID_D3DRMAnimation)
    {
        LoadAnimation(pxofobjCur, pde, options, pframeParent);
    }
    else if (*type == TID_D3DRMFrame)
    {
        pframeCur = new SFrame();
        if (pframeCur == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        hr = pxofobjCur->GetName(NULL, &cchName);
        if (FAILED(hr))
            goto e_Exit;
        
        if (cchName > 0)
        {
            pframeCur->szName = new char[cchName];
            if (pframeCur->szName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjCur->GetName(pframeCur->szName, &cchName);
            if (FAILED(hr))
                goto e_Exit;
        }
        
        pframeParent->AddFrame(pframeCur);
        
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
                hr = LoadFrames(pxofobjChild, pde, options, pframeCur);
                if (FAILED(hr))
                    goto e_Exit;
                
                GXRELEASE(pxofobjChild);
            }
            
            GXRELEASE(pxofChild);
        }
        
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    return hr;
}


         
                                      
HRESULT  CShaderPaletteSkin::DeleteSelectedMesh()
{
    if (m_pdeSelected != NULL)
    {
        SDrawElement *pdeCur = m_pdeHead;
        SDrawElement *pdePrev = NULL;
        while ((pdeCur != NULL) && (pdeCur != m_pdeSelected))
        {
            pdePrev = pdeCur;
            pdeCur = pdeCur->pdeNext;
        }

        if (pdePrev == NULL)
        {
            m_pdeHead = m_pdeHead->pdeNext;
        }
        else
        {
            pdePrev->pdeNext = pdeCur->pdeNext;
        }

        m_pdeSelected->pdeNext = NULL;
        if (m_pdeHead == m_pdeSelected)
            m_pdeHead = NULL;
        delete m_pdeSelected;
        m_pdeSelected = NULL;
    }

    return S_OK;
}
