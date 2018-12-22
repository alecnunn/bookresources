/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#include "nvcomplexmesh.h"
#include "nvstatemanager.h"
#include "nvresourcemanager.h"
#include "nvtextureresource.h"
#include "nvmaterialresource.h"
#include <algorithm>

using namespace std;
namespace nv_objects
{

NVComplexMeshPass::NVComplexMeshPass(NVComplexMesh* pParent)
: m_pMesh(pParent)
{

}

NVComplexMeshPass::NVComplexMeshPass(const NVComplexMeshPass& rhs)
: m_pMesh(rhs.m_pMesh),
m_Sections(rhs.m_Sections)
{

}


NVComplexMeshPass::~NVComplexMeshPass()
{
	Release();
}

void NVComplexMeshPass::Release()
{
	m_Sections.clear();
}



bool NVComplexMeshPass::Render(LPDIRECT3DDEVICE8 pDevice)
{
	tSectionList::const_iterator itrSections = m_Sections.begin();
	while (itrSections != m_Sections.end())
	{
		const NVComplexMeshSection& Attrib = *itrSections;
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource(Attrib.GetMaterialID()));

		// Switch to each section's material.  Note that this is not the recommended way to drive this
		// API, because we are switching material per-section
		pMaterial->SaveMaterial();

		RenderSection(pDevice, itrSections);

		pMaterial->RestoreMaterial();

		itrSections++;
	}
	return true;
}

bool NVComplexMeshPass::RenderSection(LPDIRECT3DDEVICE8 pDevice, tSectionList::const_iterator itrSections)
{

	// Can't render without VB's.
	if (m_pMesh->m_IndexBuffers.empty() || !m_pMesh->m_pVB)
	{
		NVASSERT(0, "IB Empty or no VB");
		return false;
	}

	const NVComplexMeshSection& Attrib = *itrSections;

	// Setup the stream and index buffers for the mesh
	// We don't Save/Restore this data, because it should always be set - there's no 'default' stream
	NVSTATEMANAGER.SetStreamSource(0, m_pMesh->m_pVB, m_pMesh->m_dwStride);

	if (Attrib.m_TriType == NVComplexMeshSection::TriangleList)
	{
		// Draw it.
		NVASSERT(0, "FIXME: Check counts and triangles/vertices drawn");
		NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, Attrib.m_dwIndexStart, Attrib.m_dwIndexCount);
		m_pMesh->AddTrianglesDrawn(Attrib.m_dwIndexCount);
		m_pMesh->AddVerticesDrawn(Attrib.m_dwIndexCount);
	}
	else
	{
		LONG lAttribIndexStart;
		LONG lAttribIndexCount;

		lAttribIndexStart = Attrib.m_dwIndexStart;
		lAttribIndexCount = Attrib.m_dwIndexCount;

		LONG lMaxEntries = (LONG)NV_MAX_INDICES_IN_BUFFER;
		LONG lThisCount;
		tIndexBufferArray::const_iterator itrIndices = m_pMesh->m_IndexBuffers.begin();
		while (itrIndices != m_pMesh->m_IndexBuffers.end())
		{
			// Can render in this section?
			if (lAttribIndexStart < lMaxEntries)
			{
				break;
			}
			lAttribIndexStart -= lMaxEntries;
			itrIndices++;
		}

		NVASSERT(lAttribIndexStart >= 0, "No attributes");

		while((itrIndices != m_pMesh->m_IndexBuffers.end()) && (lAttribIndexCount > 0))
		{
			if ((lMaxEntries - lAttribIndexCount) < 0)
			{
				lThisCount = lMaxEntries;
			}
			else
			{
				lThisCount = lAttribIndexCount;
			}

			NVSTATEMANAGER.SetIndices(*itrIndices, 0);

			if (Attrib.m_TriType == NVComplexMeshSection::IndexedTriangleList)
			{
				// Draw it.
				NVSTATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, Attrib.m_dwVertexStart, Attrib.m_dwVertexCount, (DWORD)lAttribIndexStart, (DWORD)(lThisCount / 3));
				m_pMesh->AddTrianglesDrawn(lThisCount / 3);
				m_pMesh->AddVerticesDrawn(lThisCount);
			}
			else if (Attrib.m_TriType == NVComplexMeshSection::IndexedTriangleStrip)
			{
				// Draw it.
				NVSTATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, Attrib.m_dwVertexStart, Attrib.m_dwVertexCount, (DWORD)lAttribIndexStart, (DWORD)(lThisCount - 2));
				m_pMesh->AddTrianglesDrawn(lThisCount);
				m_pMesh->AddVerticesDrawn(lThisCount);
			}
			else
			{
				NVASSERT(0, "Invalid triangle type!");
			}

			lAttribIndexStart = 0;
			lAttribIndexCount -= lThisCount;

			itrIndices++;
		}
	}

	return true;
}

}; //nv_objects