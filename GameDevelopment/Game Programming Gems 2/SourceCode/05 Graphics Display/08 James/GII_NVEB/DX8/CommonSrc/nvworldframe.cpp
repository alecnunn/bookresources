/*********************************************************************NVMH2****
Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "nvworldframe.h"

using namespace std;

namespace nv_objects
{

#ifdef _DEBUG
DWORD NVWorldFrame::m_dwIndent = 0;
#endif

NVWorldFrame::NVWorldFrame(NVWorldTree* pWorldTree, const char* pszName)
	: m_pframeToAnimate(NULL),
	m_bAnimationFrame(false),
	m_strName(pszName),
	m_pWorldTree(pWorldTree)
{
	// Set the matrices to start of day state
    D3DXMatrixIdentity(&m_matRot);
    D3DXMatrixIdentity(&m_matRotOrig);
    D3DXMatrixIdentity(&m_matTrans);
}

NVWorldFrame::~NVWorldFrame()
{
	// Delete the meshes
	tMeshes::const_iterator itrMeshes = m_Meshes.begin();
	while (itrMeshes != m_Meshes.end())
	{
		NVComplexMesh* pMesh = *itrMeshes;
		SAFE_DELETE(pMesh);
		itrMeshes++;
	}

	// Delete the child frames
	tWorldFrames::const_iterator itrChildren = m_Children.begin();
	while (itrChildren != m_Children.end())
	{
		NVWorldFrame* pChild = *itrChildren;
		SAFE_DELETE(pChild);
		itrChildren++;
	}
}

// Update the global time.  This is passed to all the animation frames
void NVWorldFrame::SetTime(float fGlobalTime)
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

    if (!m_MatrixKeys.empty())
    {
        fTime = (float)fmod(fGlobalTime, m_MatrixKeys[m_MatrixKeys.size() - 1].dwTime);

        for (iKey = 0 ;iKey < m_MatrixKeys.size() ; iKey++)
        {
            if ((float)m_MatrixKeys[iKey].dwTime > fTime)
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
        fTime1 = (float)m_MatrixKeys[dwp2].dwTime;
        fTime2 = (float)m_MatrixKeys[dwp3].dwTime;

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

        m_pframeToAnimate->m_matRot = m_MatrixKeys[iKey].mat;
    }
    else
    {
        D3DXMatrixIdentity(&matResult);

        if (!m_ScaleKeys.empty())
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_ScaleKeys[m_ScaleKeys.size() - 1].dwTime);

            for (iKey = 0 ;iKey < m_ScaleKeys.size() ; iKey++)
            {
                if ((float)m_ScaleKeys[iKey].dwTime > fTime)
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
            fTime1 = (float)m_ScaleKeys[dwp2].dwTime;
            fTime2 = (float)m_ScaleKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            D3DXVec3Lerp(&vScale,
                    &m_ScaleKeys[dwp2].vScale,
                    &m_ScaleKeys[dwp3].vScale,
                    fLerpValue);


            D3DXMatrixScaling(&matTemp, vScale.x, vScale.y, vScale.z);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);

            bAnimate = true;
        }

        //check rot keys
        if (!m_RotateKeys.empty())
        {
            dwp2 = dwp3 = 0;

            fTime = (float)fmod(fGlobalTime, m_RotateKeys[m_RotateKeys.size() - 1].dwTime);

            for (iKey = 0 ;iKey < m_RotateKeys.size() ; iKey++)
            {
                if ((float)m_RotateKeys[iKey].dwTime > fTime)
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
            fTime1 = (float)m_RotateKeys[dwp2].dwTime;
            fTime2 = (float)m_RotateKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);

            //s=0;
            D3DXQUATERNION q1,q2;
            q1.x =-m_RotateKeys[dwp2].quatRotate.x;
            q1.y =-m_RotateKeys[dwp2].quatRotate.y;
            q1.z =-m_RotateKeys[dwp2].quatRotate.z;
            q1.w =m_RotateKeys[dwp2].quatRotate.w;

            q2.x =-m_RotateKeys[dwp3].quatRotate.x;
            q2.y =-m_RotateKeys[dwp3].quatRotate.y;
            q2.z =-m_RotateKeys[dwp3].quatRotate.z;
            q2.w =m_RotateKeys[dwp3].quatRotate.w;

            D3DXQuaternionSlerp(&quat,
                                &q1,
                                &q2,
                                fLerpValue);

            D3DXMatrixRotationQuaternion(&matTemp, &quat);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
            bAnimate = true;
        }

        if (!m_PositionKeys.empty())
        {
            dwp2=dwp3=0;

            fTime = (float)fmod(fGlobalTime, m_PositionKeys[m_PositionKeys.size() - 1].dwTime);

            for (iKey = 0 ;iKey < m_PositionKeys.size() ; iKey++)
            {
                if ((float)m_PositionKeys[iKey].dwTime > fTime)
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
            fTime1 = (float)m_PositionKeys[dwp2].dwTime;
            fTime2 = (float)m_PositionKeys[dwp3].dwTime;

            if ((fTime2 - fTime1) ==0)
                fLerpValue = 0;
            else
                fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);


            D3DXVec3Lerp((D3DXVECTOR3*)&vPos,
                    &m_PositionKeys[dwp2].vPos,
                    &m_PositionKeys[dwp3].vPos,
                    fLerpValue);

            D3DXMatrixTranslation(&matTemp, vPos.x, vPos.y, vPos.z);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
            bAnimate = true;
        }
        else
        {
            D3DXMatrixTranslation(&matTemp, m_pframeToAnimate->m_matRotOrig._41, m_pframeToAnimate->m_matRotOrig._42, m_pframeToAnimate->m_matRotOrig._43);

            D3DXMatrixMultiply(&matResult, &matResult, &matTemp);
        }

        if (bAnimate)
        {
            m_pframeToAnimate->m_matRot = matResult;
        }
    }
}

// Finds a frame given its name
NVWorldFrame* NVWorldFrame::FindFrame(const char *szFrame)
{
    if (m_strName.compare(szFrame) == 0)
        return this;

	// Walk our children
	tWorldFrames::iterator itrChildren = m_Children.begin();
	while (itrChildren != m_Children.end())
	{
		NVWorldFrame* pChild = *itrChildren;
		NVWorldFrame* pFound = pChild->FindFrame(szFrame);
		if (pFound)
			return pFound;

		itrChildren++;
	}
    
    return NULL;
}

// Resets all the matrices
void NVWorldFrame::ResetMatrix()
{
    m_matRot = m_matRotOrig;
    D3DXMatrixIdentity(&m_matTrans);        

	// Walk our children
	tWorldFrames::iterator itrChildren = m_Children.begin();
	while (itrChildren != m_Children.end())
	{
		NVWorldFrame* pChild = *itrChildren;
		pChild->ResetMatrix();
		itrChildren++;
	}
}

// Add a child frame
void NVWorldFrame::AddChildFrame(NVWorldFrame* pframe)
{
	m_Children.push_back(pframe);
}

void NVWorldFrame::SetRotation(const D3DXMATRIX* pRotation) 
{
	m_matRot = *pRotation; 
}

void NVWorldFrame::SetTranslation(const D3DXMATRIX* pTranslation) 
{
	m_matTrans = *pTranslation; 
}

// Updates the frame transform hierarchy
void NVWorldFrame::UpdateFrames(const D3DXMATRIX* pmatCur)
{
	// Update our local transform
    D3DXMatrixMultiply(&m_matCombined, &m_matRot, pmatCur);
    D3DXMatrixMultiply(&m_matCombined, &m_matCombined, &m_matTrans );
    
	// Walk our children
	tWorldFrames::iterator itrChildren = m_Children.begin();
	while (itrChildren != m_Children.end())
	{
		NVWorldFrame* pChild = *itrChildren;
		pChild->UpdateFrames(&m_matCombined);
		itrChildren++;
	}
}

// Add a mesh to this frame
void NVWorldFrame::AddMesh(NVComplexMesh *pMesh)
{
    m_Meshes.push_back(pMesh);
	m_pWorldTree->GetMeshFrames().push_back(this);
}

// Dumps the hierarchy from here on down
#ifdef _DEBUG
void NVWorldFrame::Dump()
{
	m_dwIndent += 4;

	std::string strHasMesh = (!m_Meshes.empty() ? "Has Mesh, " : "");
	std::string strHasAnimation = (m_bAnimationFrame ? "Has Animation, " : "");
	std::string strAnimationLink;
	std::string strName;
	std::string strIndent;
	
	strIndent.resize(m_dwIndent);
	for (DWORD i = 0; i < m_dwIndent; i++)
	{
		strIndent[i] = ' ';
	}

	if (m_pframeToAnimate)
	{
		strAnimationLink = "Is frame animator (" + m_pframeToAnimate->GetName() + "), ";
	}
	else
	{
		strAnimationLink = "";
	}
	
	if (m_strName.empty())
	{
		strName = "Unnamed, ";
	}
	else
	{
		strName = m_strName + ", ";
	}

	DISPDBG(3, strIndent << "Frame: " << strName << strHasMesh << strHasAnimation << strAnimationLink);

	// Walk our children
	tWorldFrames::iterator itrChildren = m_Children.begin();
	while (itrChildren != m_Children.end())
	{
		NVWorldFrame* pChild = *itrChildren;
		pChild->Dump();
		itrChildren++;
	}

	m_dwIndent -= 4;
}
#endif


}; // namespace nv_objects

