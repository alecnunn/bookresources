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

#ifndef __ROAMDiamond_h__
#define __ROAMDiamond_h__

#include "ROAMTriangle.h"


/****************************************************************************
* Class: CROAMDiamond
*****************************************************************************
* This class implements the ROAM diamond. A diamond represents a set of 4
* triangles that can be merged. ROAM's constraints say that you can't merge
* triangles that have themselves been split until they are merged. So unless
* you plan to build the entire tree every frame, you need to keep a list of
* diamonds that can currently be merged. Every split causes existing diamonds
* to be destroyed and new ones to be created and so does every merge, so
* keeping track of them properly is important. Most of the code for that is
* in CROAMSphere.
****************************************************************************/
#define CROAMDiamondList CLinkedList<CROAMDiamond>
class CROAMDiamond : public CListNode<CROAMDiamond>, public CROAMPriority
{
public:
	// Triangles that make up the diamond
	CROAMTriangle *m_pParent[2];
	CROAMTriangle *m_pChild[2];

	// Merge priority members
	unsigned char m_nFlags;

public:
	CROAMDiamond()	{ memset((char *)this, 0, sizeof(CROAMDiamond)); }
	CROAMDiamond(CROAMTriangle *p1, CROAMTriangle *p2, CROAMTriangle *p3, CROAMTriangle *p4)
	{
		m_pParent[0] = p1;
		m_pParent[0]->m_pDiamond = this;
		m_pParent[1] = p2;
		m_pParent[1]->m_pDiamond = this;
		m_pChild[0] = p3;
		m_pChild[0]->m_pDiamond = this;
		m_pChild[1] = p4;
		m_pChild[1]->m_pDiamond = this;
		m_vMidpoint = m_pParent[0]->Vertex(2)->m_vPosition.Midpoint(m_pChild[0]->Vertex(0)->m_vPosition);
		m_fOffset = m_pParent[0]->Vertex(1)->m_vPosition.Magnitude() - m_vMidpoint.Magnitude();
		m_fLength = m_pParent[0]->Vertex(2)->m_vPosition.DistanceSquared(m_pChild[0]->Vertex(0)->m_vPosition);
		m_nFlags = 0;
	}
	~CROAMDiamond()
	{
		if(m_pParent[0])
			m_pParent[0]->m_pDiamond = NULL;
		if(m_pParent[1])
			m_pParent[1]->m_pDiamond = NULL;
		if(m_pChild[0])
			m_pChild[0]->m_pDiamond = NULL;
		if(m_pChild[1])
			m_pChild[1]->m_pDiamond = NULL;
	}

	int GetPriorityWait()		{ return (m_nFlags & PriorityWaitMask); }
	void SetPriorityWait(int n)	{ m_nFlags = n | (m_nFlags & ~PriorityWaitMask); }
	bool HasTriangle(CROAMTriangle *p)	{ return (m_pParent[0] == p || m_pParent[1] == p || m_pChild[0] == p || m_pChild[1] == p); }
};

#endif // __ROAMDiamond_h__
