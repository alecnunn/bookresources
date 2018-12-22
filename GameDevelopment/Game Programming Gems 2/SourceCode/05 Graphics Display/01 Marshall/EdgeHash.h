/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Cartoon Rendering: Real-time Silhouette Edge Detection and Rendering sample code

Copyright (c) 2001, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

/***************************************************************************************
File:  
			EdgeHash.h

  Purpose:
	 	   This file includes the necessary functions to create an edge
			list from a polygonal mesh.  It also contains a function to compact the
			final unique edge list from the hash table.
***************************************************************************************/
#ifndef EDGEHASH_H
#define EDGEHASH_H

#include "Edge.h"
#include "Mesh.h"

class EdgeHash
{
private:
	U32 HashFunction(U32 uEdgeIndexA, U32 uEdgeIndexB);
	Edge *m_pEdgeTable;
	U32 m_uHashSize;
	U32 m_uNumEdges;

public:
	EdgeHash();
	~EdgeHash();

	void CreateHashTable(Mesh *pMesh);
	void Insert(U32 uEdgeIndexA, U32 uEdgeIndexB, U32 uFaceIndex);
	bool CheckEdgeEntry(Edge *pEdge, U32 uEdgeIndexA, U32 uEdgeIndexB);
	void CompactEdgeTable(Edge *pEdgeList);
	void DeleteHashTable();

	U32 GetNumEdges();
};

inline void EdgeHash::CompactEdgeTable(Edge *pEdgeList)
{
	Edge *pEdge;
	U32 uNumEntries = 0;

	for(U32 i=0; i < m_uHashSize; i++)
	{
		// Does entry have edge stored
		if(! m_pEdgeTable[i].bOpen)
		{
			pEdge = &m_pEdgeTable[i];
			do{
				pEdgeList[uNumEntries].SetVertexIndices(pEdge->GetVertexIndex(0), pEdge->GetVertexIndex(1));
				pEdgeList[uNumEntries].SetFaceIndices(pEdge->GetFaceIndex(0), pEdge->GetFaceIndex(1));
				pEdgeList[uNumEntries].SetEdgeFlag(0);
				pEdgeList[uNumEntries].next = NULL;

				uNumEntries += 1;
				pEdge = pEdge->next;
			} while(pEdge != NULL);
		}
	} // end for loop
}

inline U32 EdgeHash::GetNumEdges()
{
	return m_uNumEdges;
}

inline U32 EdgeHash::HashFunction(U32 uEdgeIndexA, U32 uEdgeIndexB)
{
	return ((uEdgeIndexA + uEdgeIndexB)% m_uHashSize);
}

inline void EdgeHash::Insert(U32 uEdgeIndexA, U32 uEdgeIndexB, U32 uFaceIndex)
{
	U32 uHashEntry = HashFunction(uEdgeIndexA, uEdgeIndexB);
	U32 uFaceEntrySlot = m_pEdgeTable[uHashEntry].uNumFaceEntries;
	Edge *pEdge, *pPrevEdge;
	bool bFound = false;

	if(m_pEdgeTable[uHashEntry].bOpen)
	{
		m_pEdgeTable[uHashEntry].SetVertexIndices(uEdgeIndexA, uEdgeIndexB);
		m_pEdgeTable[uHashEntry].SetFaceIndex(uFaceEntrySlot, uFaceIndex);
		m_pEdgeTable[uHashEntry].uNumFaceEntries = 1;
		m_pEdgeTable[uHashEntry].bOpen = 0;
		m_uNumEdges += 1;
	}
	else
	{
		// The first slot is occupied.  Check slot to see if same edge.  If not follow the next pointer.
		pEdge = &m_pEdgeTable[uHashEntry];
		do{
			pPrevEdge = pEdge;
			uFaceEntrySlot = pEdge->uNumFaceEntries;

			if(CheckEdgeEntry(pEdge, uEdgeIndexA, uEdgeIndexB))
			{	// edge is the same, store the faceindex
				if( uFaceEntrySlot < 2)
				{
					pEdge->SetFaceIndex(uFaceEntrySlot, uFaceIndex);
					pEdge->uNumFaceEntries +=1;
				}
				bFound = true;
				break;
			}
			else
			{ // continue on to next edge
				pEdge = pEdge->next;
			}
		} while(pEdge != NULL);

		// Edge was not found
		if(!bFound)
		{
			pEdge = new Edge; 
			pEdge->SetVertexIndices(uEdgeIndexA, uEdgeIndexB);
			pEdge->SetFaceIndices(uFaceIndex, EMPTY_FACE_SLOT);
			pEdge->uNumFaceEntries = 1;
			pEdge->SetEdgeFlag(0);
			pEdge->next = NULL;
			m_uNumEdges += 1;
			pPrevEdge->next = pEdge;
		}
	}  // else slot not open
}

inline bool EdgeHash::CheckEdgeEntry(Edge *pEdge, U32 uEdgeIndexA, U32 uEdgeIndexB)
{
	if( ((pEdge->GetVertexIndex(0) == uEdgeIndexA) && (pEdge->GetVertexIndex(1) == uEdgeIndexB)) ||
		 ((pEdge->GetVertexIndex(1) == uEdgeIndexA) && (pEdge->GetVertexIndex(0) == uEdgeIndexB)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline void EdgeHash::CreateHashTable(Mesh *pMesh)
{
	m_uHashSize = pMesh->GetNumFaces() * 2;
	m_uNumEdges = 0;

	m_pEdgeTable = new Edge[m_uHashSize]; 

	for(U32 i=0; i < m_uHashSize; i++)
	{
		m_pEdgeTable[i].bOpen = true;
		m_pEdgeTable[i].uNumFaceEntries = 0;
		m_pEdgeTable[i].SetFaceIndices(EMPTY_FACE_SLOT, EMPTY_FACE_SLOT);
		m_pEdgeTable[i].SetVertexIndices(EMPTY_FACE_SLOT, EMPTY_FACE_SLOT);
		m_pEdgeTable[i].next = NULL;
	}
}

inline void EdgeHash::DeleteHashTable()
{
	Edge *pEdge, *nextEdge;
	U32 i;

	if(m_pEdgeTable)
	{
		for (i=0; i < m_uHashSize; i++)
		{
			pEdge = m_pEdgeTable[i].next;

			while (pEdge != NULL)
			{
				nextEdge = pEdge->next;
				delete pEdge;
				pEdge = nextEdge;
			} // end while
		} // end for

		delete [ ] m_pEdgeTable;
		m_pEdgeTable = NULL;
	}// end if
}

inline EdgeHash::EdgeHash()
{
	m_pEdgeTable = NULL;
}

//  Cleanup the HashTable
inline EdgeHash::~EdgeHash()
{
	if(m_pEdgeTable)
		DeleteHashTable();
}

#endif // EDGEHASH_H