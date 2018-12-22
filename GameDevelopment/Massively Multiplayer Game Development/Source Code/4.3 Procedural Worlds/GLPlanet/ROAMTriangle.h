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

#ifndef __ROAMTriangle_h__
#define __ROAMTriangle_h__

#include "GLUtil.h"


typedef enum
{
	PriorityWaitMask = 0x03,
	TriangleFaceMask = 0x1C,
	TriangleDrawMask = 0x20
} ROAMFlags;


class CROAMPriority
{
public:
	CVector m_vMidpoint;		// The midpoint of the third edge (the one to split)
	float m_fOffset;			// The offset in height the midpoint will require when split
	float m_fLength;			// The length of the third edge (the one to split)

public:
	void CopyPriorityInfo(CROAMPriority *p)
	{
		m_vMidpoint = p->m_vMidpoint;
		m_fOffset = p->m_fOffset;
		m_fLength = p->m_fLength;
	}
	float GetPriority(const CVector &vPosition, const CVector &vHeading, const float fHorizon)
	{
		float fPriority = 0.0f;
		float fDistance = vPosition.DistanceSquared(m_vMidpoint);
		// Only calculate priority for vertices closer than the horizon
		// (Make an exception if the triangle is larger than the camera's distance to it)
		if(fDistance < m_fLength || fDistance < fHorizon)
		{
			// Only calculate priority for vertices inside the angle of view
			// (Make an exception if the triangle is larger than the camera's distance to it)
			if(fDistance < m_fLength || (vHeading | DirectionVector(vPosition, m_vMidpoint)) > 0.75f)
			{
				float fError = m_fOffset;
				if(m_fOffset > 1.0f)
					fError *= m_fOffset;
				fPriority = (fDistance > 25.0f) ? fError / sqrtf(fDistance) : fError * 0.2f;
			}
		}
		return fPriority;
	}
};

/****************************************************************************
* Class: CROAMTriangle
*****************************************************************************
* This class implements the ROAM triangle. It is quite large because it
* relies heavily on caching information so it doesn't have to calculate things
* every frame. Some deal with calculating split priority while others deal
* with handling the actual split. Like CVertex, it is derived from CListNode
* and uses CMemoryBlock if USE_ALLOCATOR is defined.
*
* Note that the parent pointer points to the parent of this triangle, but this
* is still not a tree-like structure! I do not keep a binary tree of all
* triangles used to reach the current model, I only keep the triangles that
* are part of the current model. When I split 2 triangles into 4, I modify
* the 2 existing triangles and add 2 new ones, setting the parent pointers of
* the new triangles to point to the existing ones. Later when I merge 4 into
* 2, the two child triangles get deleted and the parents get changed back.
*
* One more important thing to note is vertex and edge order. ROAM will only
* split two right triangles that make up a square, so I think of every
* triangle as a right triangle (even though it's not really a right triangle
* because it's on a sphere). My vertices always run counter-clockwise with
* the middle vertex being in the middle. Edges are handled in a similar
* fashion. Edge 0 indicates vertices 0 - 1, edge 1 indicates vertices 1 - 2,
* and edge 2 indicates vertices 2 - 0.
*
* It looks something like this:
* (square)
* 1----0 /2   Note: m_pEdge[2] is always the edge that is split, and a
* |   /	/ |         triangle can be split only if m_pEdge[2]->m_pEdge[2]
* |  / /  |         points back to itself. If it doesn't, its neighbor must
* | / /   |         be split first (recursively).
* 2/ 0----1
****************************************************************************/
class CROAMDiamond;		// Forward reference
#define CROAMTriangleList CLinkedList<CROAMTriangle>
class CROAMTriangle : public CListNode<CROAMTriangle>, public CROAMPriority
{
// Attributes
public:
	// Triangle members
	unsigned short m_nVertex[3];	// Indices into a CVertexArray
	CROAMTriangle *m_pEdge[3];		// Pointers to the triangles sharing edges with this one
	CROAMTriangle *m_pParent;		// Pointer to the parent triangle that created this one
	CROAMDiamond *m_pDiamond;		// Set if this triangle is part of a diamond
	CVector m_vNormal;

	// Split priority members
	unsigned char m_nFlags;

// Operations
public:
	CROAMTriangle()	{ memset((char *)this, 0, sizeof(CROAMTriangle)); }
	CROAMTriangle(unsigned short n1, unsigned short n2, unsigned short n3)
	{
		memset((char *)this, 0, sizeof(CROAMTriangle));
		SetVertices(n1, n2, n3);
	}

	int GetPriorityWait()		{ return (m_nFlags & PriorityWaitMask); }
	void SetPriorityWait(int n)	{ m_nFlags = n | (m_nFlags & ~PriorityWaitMask); }
	int GetTriangleFace()		{ return (m_nFlags & TriangleFaceMask) >> 2; }
	void SetTriangleFace(int n)	{ m_nFlags = (n<<2) | (m_nFlags & ~TriangleFaceMask); }
	
	CVertex *Vertex(unsigned short n)	{ return CVertex::Array[m_nVertex[n]]; }

	void SetVertices(unsigned short n1, unsigned short n2, unsigned short n3)
	{
		m_nVertex[0] = n1; m_nVertex[1] = n2; m_nVertex[2] = n3;
		m_vNormal = NormalVector(Vertex(0)->m_vPosition, Vertex(1)->m_vPosition, Vertex(2)->m_vPosition);
	}
	void SetEdges(CROAMTriangle *p1, CROAMTriangle *p2, CROAMTriangle *p3)
	{
		m_pEdge[0] = p1; m_pEdge[1] = p2; m_pEdge[2] = p3;
	}
	void ReplaceEdge(CROAMTriangle *pOld, CROAMTriangle *pNew)
	{
		if(m_pEdge[0] == pOld)
			m_pEdge[0] = pNew;
		else if(m_pEdge[1] == pOld)
			m_pEdge[1] = pNew;
		else if(m_pEdge[2] == pOld)
			m_pEdge[2] = pNew;
	}

	const CVector &GetNormal()			{ return m_vNormal; }
	bool HasVertex(CVertex *pVertex){ return (pVertex->m_vPosition == Vertex(0)->m_vPosition || pVertex->m_vPosition == Vertex(1)->m_vPosition || pVertex->m_vPosition == Vertex(2)->m_vPosition); }
	bool HasVertex(unsigned short n)
	{
		if(m_nVertex[0] == n || m_nVertex[1] == n || m_nVertex[2] == n)
			return true;
		return HasVertex(CVertex::Array[n]);
	}
	void UpdateVertexNormal(unsigned short nVertex)
	{
		// Go counter-clockwise through neighbors to add up normals of all triangles using this vertex
		CVertex *pVertex = Vertex(nVertex);
		pVertex->m_vNormal = GetNormal();
		CROAMTriangle *pPrev = this;
		CROAMTriangle *pNext = m_pEdge[nVertex];
		while(pNext != this)
		{
			pVertex->m_vNormal += pNext->GetNormal();
			int nNext = pNext->m_pEdge[0] == pPrev ? 1 : pNext->m_pEdge[1] == pPrev ? 2 : 0;
			pPrev = pNext;
			pNext = pNext->m_pEdge[nNext];
		}
		CVertex::Array.UpdateElement(m_nVertex[nVertex]);
	}
	void UpdateVertexNormals()
	{
		for(int i=0; i<3; i++)
			UpdateVertexNormal(i);
	}

	unsigned short GetOpposite(unsigned short n1, unsigned short n2)
	{
		if(m_nVertex[0] != n1 && m_nVertex[0] != n2)
			return m_nVertex[0];
		if(m_nVertex[1] != n1 && m_nVertex[1] != n2)
			return m_nVertex[1];
		return m_nVertex[2];
	}
	void Draw(unsigned short *nArray, int &nIndex)
	{
		//if(m_nFlags & TriangleDrawMask)
		{
			nArray[nIndex++] = m_nVertex[0];
			nArray[nIndex++] = m_nVertex[1];
			nArray[nIndex++] = m_nVertex[2];
		}
	}
};

#endif // __ROAMTriangle_h__
