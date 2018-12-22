//-----------------------------------------------------------------------------
// File: mdraw.cpp
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8.h>
#include "eb_effect.h"
#include "shader_PaletteSkin.h"
#include "Constants.h"

HRESULT CShaderPaletteSkin::DrawMeshContainer(SMeshContainer *pmcMesh)
{
    UINT ipattr;
    HRESULT hr = S_OK;
    LPD3DXBONECOMBINATION pBoneComb;
    LPDIRECT3DDEVICE8 m_pDevice = m_pD3DDev;

    if (pmcMesh->m_pSkinMesh)
    {
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
		for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
		{
			for (DWORD i = 0; i < PALETTE_SIZE/*255*/; ++i)
			{
				DWORD matid = pBoneComb[ipattr].BoneId[i];
				if (matid != UINT_MAX)
				{
					D3DXMATRIX tempMat;
					D3DXMatrixMultiply(&tempMat, &pmcMesh->m_pBoneOffsetMat[matid], pmcMesh->m_pBoneMatrix[matid]);
					
					D3DXMatrixTranspose(&tempMat, &tempMat);
					m_pDevice->SetVertexShaderConstant(CV_BONESTART + (i * 3), &tempMat, 3);
				}
			}
			
			m_pDevice->SetMaterial(&(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId]));
			//texture stage 1 contains decal
			hr = m_pDevice->SetTexture(1, pmcMesh->pTextures[pBoneComb[ipattr].AttribId]);
			if(FAILED(hr))
				return hr;
			
        	LPDIRECT3DINDEXBUFFER8 tempIBuffer;
			
			pmcMesh->pMesh->GetIndexBuffer(&tempIBuffer);
			m_pDevice->SetStreamSource(0, m_pRenderVB, sizeof(PSVertex));
			m_pDevice->SetIndices(tempIBuffer, 0);
			hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, pmcMesh->m_pAttrTable[ipattr].VertexStart, 
				pmcMesh->m_pAttrTable[ipattr].VertexCount, pmcMesh->m_pAttrTable[ipattr].FaceStart * 3, 
				pmcMesh->m_pAttrTable[ipattr].FaceCount);
			tempIBuffer->Release();
			if(FAILED(hr))
				return hr;
		}
		
    }
    else
    {
        for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
        {
            m_pDevice->SetMaterial(&(pmcMesh->rgMaterials[ipattr]));
            m_pDevice->SetTexture(0, pmcMesh->pTextures[ipattr]);
		    hr = pmcMesh->pMesh->DrawSubset( ipattr );
            if(FAILED(hr))
                return hr;
        }
    }

    return S_OK;


}




HRESULT CShaderPaletteSkin::UpdateFrames(SFrame *pframeCur, D3DXMATRIX &matCur)
{
    HRESULT hr = S_OK;
    pframeCur->matCombined = matCur;
    D3DXMatrixMultiply(&pframeCur->matCombined, &pframeCur->matRot, &matCur);
    D3DXMatrixMultiply(&pframeCur->matCombined, &pframeCur->matCombined, &pframeCur->matTrans );
    SFrame *pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = UpdateFrames(pframeChild, pframeCur->matCombined);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }
    return S_OK;
}




HRESULT CShaderPaletteSkin::DrawFrames(SFrame *pframeCur, UINT &cTriangles)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;
    LPDIRECT3DDEVICE8 m_pDevice = m_pD3DDev;

    if (pframeCur->pmcMesh != NULL)
    {
        hr = m_pDevice->SetTransform(D3DTS_WORLD, &pframeCur->matCombined);
        if(FAILED(hr))
            return hr;
    }

    pmcMesh = pframeCur->pmcMesh;
    while (pmcMesh != NULL)
    {
        hr = DrawMeshContainer(pmcMesh);
        if (FAILED(hr))
            return hr;

        cTriangles += pmcMesh->m_pSkinMesh->GetNumFaces();

        pmcMesh = pmcMesh->pmcNext;
    }

    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = DrawFrames(pframeChild, cTriangles);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}




void SFrame::SetTime(float fGlobalTime)
{
    UINT iKey;
    UINT dwp2;
    UINT dwp3;
    D3DXMATRIX matResult;
    D3DXMATRIX matTemp;
    float fTime1;
    float fTime2;
    float fLerpValue;
    D3DXVECTOR3 vScale;
    D3DXVECTOR3 vPos;
    D3DXQUATERNION quat;
    BOOL bAnimate = false;
    float fTime;

    if (m_pMatrixKeys )
    {
        fTime = (float)fmod(fGlobalTime, m_pMatrixKeys[m_cMatrixKeys-1].dwTime);

        for (iKey = 0 ;iKey < m_cMatrixKeys ; iKey++)
        {
            if ((float)m_pMatrixKeys[iKey].dwTime > fTime)
            {
                dwp3 = iKey;

                if (iKey > 0)
                {
                    dwp2= iKey - 1;
                }
                else  // when iKey == 0, then dwp2 == 0
                {
                    dwp2 = iKey;
                }

                break;
            }
        }
        fTime1 = (float)m_pMatrixKeys[dwp2].dwTime;
        fTime2 = (float)m_pMatrixKeys[dwp3].dwTime;

        if ((fTime2 - fTime1) ==0)
            fLerpValue = 0;
        else
            fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

        if (fLerpValue > 0.5)
        {
            iKey = dwp3;
        }
        else
        {
            iKey = dwp2;
        }

        pframeToAnimate->matRot = m_pMatrixKeys[iKey].mat;
    }
    else
    {
        D3DXMatrixIdentity(&matResult);

        if (m_pScaleKeys)
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_pScaleKeys[m_cScaleKeys-1].dwTime);

            for (iKey = 0 ;iKey < m_cScaleKeys ; iKey++)
            {
                if ((float)m_pScaleKeys[iKey].dwTime > fTime)
                {
                    dwp3 = iKey;

                    if (iKey > 0)
                    {
                        dwp2= iKey - 1;
                    }
                    else  // when iKey == 0, then dwp2 == 0
                    {
                        dwp2 = iKey;
                    }

                    break;
                }
            }
            fTime1 = (float)m_pScaleKeys[dwp2].dwTime;
            fTime2 = (float)m_pScaleKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            D3DXVec3Lerp(&vScale,
                    &m_pScaleKeys[dwp2].vScale,
                    &m_pScaleKeys[dwp3].vScale,
                    fLerpValue);


            D3DXMatrixScaling(&matTemp, vScale.x, vScale.y, vScale.z);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);

            bAnimate = true;
        }

        //check rot keys
        if (m_pRotateKeys )
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_pRotateKeys[m_cRotateKeys-1].dwTime);

            for (iKey = 0 ;iKey < m_cRotateKeys ; iKey++)
            {
                if ((float)m_pRotateKeys[iKey].dwTime > fTime)
                {
                    dwp3 = iKey;

                    if (iKey > 0)
                    {
                        dwp2= iKey - 1;
                    }
                    else  // when iKey == 0, then dwp2 == 0
                    {
                        dwp2 = iKey;
                    }

                    break;
                }
            }
            fTime1 = (float)m_pRotateKeys[dwp2].dwTime;
            fTime2 = (float)m_pRotateKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            //s=0;
            D3DXQUATERNION q1,q2;
            q1.x =-m_pRotateKeys[dwp2].quatRotate.x;
            q1.y =-m_pRotateKeys[dwp2].quatRotate.y;
            q1.z =-m_pRotateKeys[dwp2].quatRotate.z;
            q1.w =m_pRotateKeys[dwp2].quatRotate.w;

            q2.x =-m_pRotateKeys[dwp3].quatRotate.x;
            q2.y =-m_pRotateKeys[dwp3].quatRotate.y;
            q2.z =-m_pRotateKeys[dwp3].quatRotate.z;
            q2.w =m_pRotateKeys[dwp3].quatRotate.w;

            D3DXQuaternionSlerp(&quat,
                                &q1,
                                &q2,
                                fLerpValue);

            D3DXMatrixRotationQuaternion(&matTemp, &quat);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
            bAnimate = true;
        }

        if (m_pPositionKeys)
        {
            dwp2=dwp3=0;

            fTime = (float)fmod(fGlobalTime, m_pPositionKeys[m_cRotateKeys-1].dwTime);

            for (iKey = 0 ;iKey < m_cPositionKeys ; iKey++)
            {
                if ((float)m_pPositionKeys[iKey].dwTime > fTime)
                {
                    dwp3 = iKey;

                    if (iKey > 0)
                    {
                        dwp2= iKey - 1;
                    }
                    else  // when iKey == 0, then dwp2 == 0
                    {
                        dwp2 = iKey;
                    }

                    break;
                }
            }
            fTime1 = (float)m_pPositionKeys[dwp2].dwTime;
            fTime2 = (float)m_pPositionKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);


            D3DXVec3Lerp((D3DXVECTOR3*)&vPos,
                    &m_pPositionKeys[dwp2].vPos,
                    &m_pPositionKeys[dwp3].vPos,
                    fLerpValue);

            D3DXMatrixTranslation(&matTemp, vPos.x, vPos.y, vPos.z);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
            bAnimate = true;
        }
        else
        {
            D3DXMatrixTranslation(&matTemp, pframeToAnimate->matRotOrig._41, pframeToAnimate->matRotOrig._42, pframeToAnimate->matRotOrig._43);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
        }

        if (bAnimate)
        {
            pframeToAnimate->matRot = matResult;
        }
    }
}




