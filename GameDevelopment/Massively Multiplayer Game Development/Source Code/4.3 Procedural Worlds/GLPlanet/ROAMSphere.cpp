/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Master.h"
#include "ROAMSphere.h"


unsigned short CROAMSphere::m_nIndex[128*1024*3];
int CROAMSphere::m_nStart[6];
int CROAMSphere::m_nEnd[6];


void CROAMSphere::RebuildEdgeList()
{
	// Set up the edge pointers for all triangles
	for(int i=0; i<6; i++)
	{
		for(CROAMTriangle *p1 = m_llTriangle[i].GetHead(); p1->IsInList(); p1 = p1->GetNext())
		{
			for(int j=i; j<6; j++)
			{
				for(CROAMTriangle *p2 = m_llTriangle[j].GetHead(); p2->IsInList(); p2 = p2->GetNext())
				{
					if(p2 == p1)
						continue;
					bool bMatch = true;
					if(p2->HasVertex(p1->m_nVertex[0]) && p2->HasVertex(p1->m_nVertex[1]))
						p1->m_pEdge[0] = p2;
					else if(p2->HasVertex(p1->m_nVertex[1]) && p2->HasVertex(p1->m_nVertex[2]))
						p1->m_pEdge[1] = p2;
					else if(p2->HasVertex(p1->m_nVertex[2]) && p2->HasVertex(p1->m_nVertex[0]))
						p1->m_pEdge[2] = p2;
					else
						bMatch = false;
					if(bMatch)
					{
						if(!p1->HasVertex(p2->m_nVertex[2]))
							p2->m_pEdge[0] = p1;
						else if(!p1->HasVertex(p2->m_nVertex[0]))
							p2->m_pEdge[1] = p1;
						else
							p2->m_pEdge[2] = p1;
					}
				}
			}
		}
	}
}

void CROAMSphere::BuildCube()
{
	unsigned short nVertex[4];
	CROAMTriangle *pTriangle;

	// Create front face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(-1, 1, 1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(-1, -1, 1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(1, -1, 1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(1, 1, 1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::FrontFace);
	m_llTriangle[CPlanetaryMap::FrontFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::FrontFace);
	m_llTriangle[CPlanetaryMap::FrontFace].AddTail(pTriangle);

	// Create back face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(1, 1, -1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(1, -1, -1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(-1, -1, -1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(-1, 1, -1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::BackFace);
	m_llTriangle[CPlanetaryMap::BackFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::BackFace);
	m_llTriangle[CPlanetaryMap::BackFace].AddTail(pTriangle);

	// Create left face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(-1, 1, -1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(-1, -1, -1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(-1, -1, 1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(-1, 1, 1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::LeftFace);
	m_llTriangle[CPlanetaryMap::LeftFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::LeftFace);
	m_llTriangle[CPlanetaryMap::LeftFace].AddTail(pTriangle);

	// Create right face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(1, 1, 1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(1, -1, 1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(1, -1, -1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(1, 1, -1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::RightFace);
	m_llTriangle[CPlanetaryMap::RightFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::RightFace);
	m_llTriangle[CPlanetaryMap::RightFace].AddTail(pTriangle);

	// Create top face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(-1, 1, -1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(-1, 1, 1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(1, 1, 1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(1, 1, -1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::TopFace);
	m_llTriangle[CPlanetaryMap::TopFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::TopFace);
	m_llTriangle[CPlanetaryMap::TopFace].AddTail(pTriangle);

	// Create bottom face vertices and triangles
	nVertex[0] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[0]]->Init(CVector(-1, -1, 1));
	nVertex[1] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[1]]->Init(CVector(-1, -1, -1));
	nVertex[2] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[2]]->Init(CVector(1, -1, -1));
	nVertex[3] = CVertex::Array.LockElement();
	CVertex::Array[nVertex[3]]->Init(CVector(1, -1, 1));
	pTriangle = new CROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	pTriangle->SetTriangleFace(CPlanetaryMap::BottomFace);
	m_llTriangle[CPlanetaryMap::BottomFace].AddTail(pTriangle);
	pTriangle = new CROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	pTriangle->SetTriangleFace(CPlanetaryMap::BottomFace);
	m_llTriangle[CPlanetaryMap::BottomFace].AddTail(pTriangle);

	RebuildEdgeList();
}

void CROAMSphere::UpdateTriangleOffset(CROAMTriangle *pTriangle)
{
	pTriangle->m_vMidpoint = pTriangle->Vertex(0)->m_vPosition.Midpoint(pTriangle->Vertex(2)->m_vPosition);
	pTriangle->m_fOffset = m_pHeightMap->GetHeightOffset(pTriangle->m_vMidpoint);
	pTriangle->m_fLength = pTriangle->Vertex(0)->m_vPosition.DistanceSquared(pTriangle->Vertex(2)->m_vPosition);
}

bool CROAMSphere::Split(CROAMTriangle *pTriangle)
{
	// If the opposite triangle is not ready to split with this one, then it must be split first.
	// After that, it should always be ready to split with this one.
	static CROAMTriangle *pOpposite;
	if(pTriangle->m_pEdge[2]->m_pEdge[2] != pTriangle)
		if(!Split(pTriangle->m_pEdge[2]))
			return false;
	pOpposite = pTriangle->m_pEdge[2];
	ASSERT(pOpposite->m_pEdge[2] == pTriangle);

	// Now split edge 2-0 by getting its midpoint and creating a new vertex
	static unsigned short nTriangleVertex, nOppositeVertex;
	static CVertex *pTriangleVertex, *pOppositeVertex;
	nTriangleVertex = CVertex::Array.LockElement();
	if(nTriangleVertex == 65535)
		return false;
	pTriangleVertex = CVertex::Array[nTriangleVertex];

	pTriangleVertex->m_vPosition = pTriangle->m_vMidpoint;
	float fHeight = pTriangleVertex->m_vPosition.Magnitude();
	pTriangleVertex->m_vPosition *= (fHeight + pTriangle->m_fOffset) / fHeight;
	if(m_pSurfaceMap)
	{
		m_pSurfaceMap->GetRawTextureCoordinates(pTriangleVertex->m_vPosition, fHeight + pTriangle->m_fOffset, pTriangleVertex->m_fTexCoord[0], pTriangleVertex->m_fTexCoord[1]);
		m_pSurfaceMap->GetTextureCoordinates(pTriangleVertex->m_vPosition, pTriangle->GetTriangleFace(), pTriangleVertex->m_fTexCoord[2], pTriangleVertex->m_fTexCoord[3]);
	}

	// If the triangles are on different faces, create distinct vertices for unique texture coordinates
	nOppositeVertex = nTriangleVertex;
	pOppositeVertex = pTriangleVertex;
	if(pTriangle->GetTriangleFace() != pOpposite->GetTriangleFace())
	{
		nOppositeVertex = CVertex::Array.LockElement();
		if(nOppositeVertex == 65535)
			return false;
		pTriangleVertex = CVertex::Array[nTriangleVertex];
		pOppositeVertex = CVertex::Array[nOppositeVertex];
		memcpy(pOppositeVertex, pTriangleVertex, sizeof(CVertex));
		if(m_pSurfaceMap)
			m_pSurfaceMap->GetTextureCoordinates(pOppositeVertex->m_vPosition, pOpposite->GetTriangleFace(), pOppositeVertex->m_fTexCoord[2], pOppositeVertex->m_fTexCoord[3]);
	}

	// Now create two new triangles for the split
	static CROAMTriangle *pNewTriangle[2];
	pNewTriangle[0] = new CROAMTriangle;
	pNewTriangle[1] = new CROAMTriangle;

	// Split the first one from pTriangle
	pTriangle->SetPriorityWait(0);
	pNewTriangle[0]->m_nFlags = pTriangle->m_nFlags;
	pNewTriangle[0]->InsertAfter(pTriangle);
	//m_llTriangle[pTriangle->GetTriangleFace()].AddTail(pNewTriangle[0]);
	pNewTriangle[0]->SetVertices(pTriangle->m_nVertex[2], nTriangleVertex, pTriangle->m_nVertex[1]);
	pTriangle->SetVertices(pTriangle->m_nVertex[1], nTriangleVertex, pTriangle->m_nVertex[0]);
	pNewTriangle[0]->SetEdges(pOpposite, pTriangle, pTriangle->m_pEdge[1]);
	pTriangle->m_pEdge[1]->ReplaceEdge(pTriangle, pNewTriangle[0]);
	pTriangle->SetEdges(pNewTriangle[0], pNewTriangle[1], pTriangle->m_pEdge[0]);
	if(pNewTriangle[0]->m_pEdge[2]->m_pEdge[2] == pNewTriangle[0])
		pNewTriangle[0]->CopyPriorityInfo(pNewTriangle[0]->m_pEdge[2]);
	else
		UpdateTriangleOffset(pNewTriangle[0]);
	if(pTriangle->m_pEdge[2]->m_pEdge[2] == pTriangle)
		pTriangle->CopyPriorityInfo(pTriangle->m_pEdge[2]);
	else
		UpdateTriangleOffset(pTriangle);

	// Split the second one from pOpposite
	pOpposite->SetPriorityWait(0);
	pNewTriangle[1]->m_nFlags = pOpposite->m_nFlags;
	pNewTriangle[1]->InsertAfter(pOpposite);
	//m_llTriangle[pOpposite->GetTriangleFace()].AddTail(pNewTriangle[1]);
	pNewTriangle[1]->SetVertices(pOpposite->m_nVertex[2], nOppositeVertex, pOpposite->m_nVertex[1]);
	pOpposite->SetVertices(pOpposite->m_nVertex[1], nOppositeVertex, pOpposite->m_nVertex[0]);
	pNewTriangle[1]->SetEdges(pTriangle, pOpposite, pOpposite->m_pEdge[1]);
	pOpposite->m_pEdge[1]->ReplaceEdge(pOpposite, pNewTriangle[1]);
	pOpposite->SetEdges(pNewTriangle[1], pNewTriangle[0], pOpposite->m_pEdge[0]);
	if(pNewTriangle[1]->m_pEdge[2]->m_pEdge[2] == pNewTriangle[1])
		pNewTriangle[1]->CopyPriorityInfo(pNewTriangle[1]->m_pEdge[2]);
	else
		UpdateTriangleOffset(pNewTriangle[1]);
	if(pOpposite->m_pEdge[2]->m_pEdge[2] == pOpposite)
		pOpposite->CopyPriorityInfo(pOpposite->m_pEdge[2]);
	else
		UpdateTriangleOffset(pOpposite);

	// Calculate the new vertex's normal vector
#define SPLIT_NORMAL	// Updates the normal of all vertices affected by the split
#ifdef SPLIT_NORMAL
	pTriangle->UpdateVertexNormals();
	pOpposite->UpdateVertexNormals();
#else
	pTriangleVertex->m_vNormal = pTriangle->GetNormal() + pOpposite->GetNormal() + pNewTriangle[0]->GetNormal() + pNewTriangle[1]->GetNormal();
	CVertex::Array.UpdateElement(nTriangleVertex);
	if(nTriangleVertex != nOppositeVertex)
	{
		pOppositeVertex->m_vNormal = pTriangleVertex->m_vNormal;
		CVertex::Array.UpdateElement(nOppositeVertex);
	}
#endif

	// Set up parent pointers
	pNewTriangle[0]->m_pParent = pTriangle;
	pNewTriangle[1]->m_pParent = pOpposite;

	// If either of the original triangles was part of a diamond, it has been destroyed.
	if(pTriangle->m_pDiamond)
		delete pTriangle->m_pDiamond;
	if(pOpposite->m_pDiamond)
		delete pOpposite->m_pDiamond;

	// A new diamond must be created with the 4 new triangles.
	m_llDiamond.AddTail(new CROAMDiamond(pTriangle, pOpposite, pNewTriangle[0], pNewTriangle[1]));
	m_nTriangles += 2;
	return true;
}

void CROAMSphere::Merge(CROAMDiamond *pDiamond)
{
	// First, get the vertex all the triangles share and the parent triangles that the children will be merged into
	unsigned short nVertex = pDiamond->m_pParent[0]->m_nVertex[1];
	unsigned short nOpposite = pDiamond->m_pParent[1]->m_nVertex[1];
	CVertex *pVertex = CVertex::Array[nVertex];

	// Loop twice to merge two child triangles into their parents
	for(int i=0; i<2; i++)
	{
		// Start by updating the parent's vertices and edges, then deleting the child
		CROAMTriangle *pParent = pDiamond->m_pParent[i];
		CROAMTriangle *pChild = pDiamond->m_pChild[i];
		pParent->SetVertices(pParent->m_nVertex[2], pParent->m_nVertex[0], pChild->m_nVertex[0]);
		pParent->SetEdges(pParent->m_pEdge[2], pChild->m_pEdge[2], pDiamond->m_pParent[1-i]);
		pChild->m_pEdge[2]->ReplaceEdge(pChild, pParent);
		delete pChild;

		// Then update the priority information
		if(i == 0)
		{
			CVector v = pParent->Vertex(0)->m_vPosition.Midpoint(pParent->Vertex(2)->m_vPosition);
			pParent->m_vMidpoint = v;
			float fHeight = (float)v.Magnitude();
			pParent->m_fOffset = (float)pVertex->m_vPosition.Magnitude() - fHeight;
		}
		else
			pParent->CopyPriorityInfo(pParent->m_pEdge[2]);

		// Then see if the merged parent forms a new diamond
		CROAMTriangle *p[4];
		pParent->m_pDiamond = NULL;
		p[0] = pParent;
		p[1] = p[0]->m_pEdge[0];
		p[2] = p[1]->m_pEdge[0];
		p[3] = p[2]->m_pEdge[0];
		if(p[3]->m_pEdge[0] == p[0])
		{
			// If it does, make sure parents and children match up
			if(p[1]->m_pParent == p[0] && p[3]->m_pParent == p[2])
				m_llDiamond.AddTail(new CROAMDiamond(p[0], p[2], p[1], p[3]));
			if(p[2]->m_pParent == p[1] && p[0]->m_pParent == p[3])
				m_llDiamond.AddTail(new CROAMDiamond(p[1], p[3], p[2], p[0]));
		}
	}
#define MERGE_NORMAL	// Updates the normal of all vertices affected by the merge
#ifdef MERGE_NORMAL
	pDiamond->m_pParent[0]->UpdateVertexNormals();
	pDiamond->m_pParent[1]->UpdateVertexNormals();
#endif
	pDiamond->m_pParent[0] = pDiamond->m_pParent[1] = NULL;
	pDiamond->m_pChild[0] = pDiamond->m_pChild[1] = NULL;
	delete pDiamond;
	CVertex::Array.UnlockElement(nVertex);
	if(nOpposite != nVertex)
		CVertex::Array.UnlockElement(nOpposite);
	m_nTriangles -= 2;
}

void CROAMSphere::Init(CHeightMap *pHeightMap, CSurfaceMap *pSurfaceMap)
{
	// Build a fresh cube and set up the initial vertices/triangles
	m_pHeightMap = pHeightMap;
	m_pSurfaceMap = pSurfaceMap;
	BuildCube();

	int nFace, i;
	for(nFace=0; nFace<6; nFace++)
	{
		for(CROAMTriangle *pTriangle=m_llTriangle[nFace].GetHead(); pTriangle->IsInList(); pTriangle = pTriangle->GetNext())
		{
			for(i=0; i<3; i++)
			{
				pTriangle->Vertex(i)->m_vPosition.Normalize();
				float fHeight = m_pHeightMap->GetHeightNormalized(pTriangle->Vertex(i)->m_vPosition);
				pTriangle->Vertex(i)->m_vPosition *= fHeight;
				if(m_pSurfaceMap)
				{
					m_pSurfaceMap->GetRawTextureCoordinates(pTriangle->Vertex(i)->m_vPosition, fHeight, pTriangle->Vertex(i)->m_fTexCoord[0], pTriangle->Vertex(i)->m_fTexCoord[1]);
					m_pSurfaceMap->GetTextureCoordinates(pTriangle->Vertex(i)->m_vPosition, nFace, pTriangle->Vertex(i)->m_fTexCoord[2], pTriangle->Vertex(i)->m_fTexCoord[3]);
				}
			}
			UpdateTriangleOffset(pTriangle);
		}
	}
}

void CROAMSphere::Update(CVector vPosition, CVector vHeading, float fHorizon, float fMaxError)
{
	m_fMaxError = fMaxError;
	const float fHalfError = m_fMaxError * 0.5f;
	const float fTwiceError = m_fMaxError * 2.0f;
	float fPriority;

	// Go through the list of diamonds, merging where necessary
	CROAMDiamond *pDiamond, *pOld;
	for(pDiamond=m_llDiamond.GetHead(); pDiamond->IsInList(); pDiamond = pDiamond->GetNext())
	{
		if(pDiamond->GetPriorityWait())
			pDiamond->SetPriorityWait(pDiamond->GetPriorityWait()-1);
		else
		{
			fPriority = pDiamond->GetPriority(vPosition, vHeading, fHorizon);
			if(fPriority < m_fMaxError)
			{
				pOld = pDiamond;
				pDiamond = pDiamond->GetPrevious();
				Merge(pOld);
			}
			else
			{
				pDiamond->SetPriorityWait(fPriority >= fTwiceError ? 2 : 1);
			}
		}
	}

	// Go through the list of triangles, splitting where necessary
	for(int nFace=0; nFace<6; nFace++)
	{
		for(CROAMTriangle *pTriangle=m_llTriangle[nFace].GetHead(); pTriangle->IsInList(); pTriangle = pTriangle->GetNext())
		{
			if(pTriangle->GetPriorityWait())
				pTriangle->SetPriorityWait(pTriangle->GetPriorityWait()-1);
			else
			{
				fPriority = pTriangle->GetPriority(vPosition, vHeading, fHorizon);
				if(fPriority >= m_fMaxError)
 					Split(pTriangle);
				else
				{
					pTriangle->SetPriorityWait(fPriority < DELTA ? 3 : fPriority < fHalfError ? 2 : 1);
				}
			}
		}
	}
}
