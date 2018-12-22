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

#ifndef __ROAMSphere_h__
#define __ROAMSphere_h__

#include "SurfaceMap.h"
#include "ROAMDiamond.h"

/****************************************************************************
* Class: CROAMSphere
*****************************************************************************
* This class implements the spherical ROAM algorithm. It keeps lists of the
* vertices, triangles, and diamonds. Because it is currently used to implement
* a fractal-based planet, it also has a member variable of the CFractal class
* and a number of others for defining the planet's basic properties. Its main
* methods are Init(), Update(), and Draw(). Init() initializes the planet
* with a random seed and certain parameters, like radius and roughness.
* Update() updates the priority lists and performs any necessary splits or
* merges in the mesh. Draw() draws the mesh using either smooth or flat shading.
****************************************************************************/
class CROAMSphere
{
protected:
	static unsigned short m_nIndex[128*1024*3];
	static int m_nStart[6];
	static int m_nEnd[6];

	CROAMTriangleList m_llTriangle[6];
	CROAMDiamondList m_llDiamond;
	CHeightMap *m_pHeightMap;
	CSurfaceMap *m_pSurfaceMap;
	int m_nTriangles;
	float m_fMaxError;

	void RebuildEdgeList();
	void BuildCube();
	void UpdateTriangleOffset(CROAMTriangle *pTriangle);
	bool Split(CROAMTriangle *pTriangle);
	void Merge(CROAMDiamond *pDiamond);

public:
	CROAMSphere()
	{
		m_fMaxError = 0.001f;
		m_nTriangles = 0;
	}
	~CROAMSphere()
	{
	}

	int GetTriangleCount()	{ return m_nTriangles; }

	void Init(CHeightMap *pHeightMap, CSurfaceMap *pSurfaceMap=NULL);
	void Update(CVector vPosition, CVector vHeading, float fHorizon, float fMaxError);

	void BuildIndexList()
	{
		m_nTriangles = 0;
		CROAMTriangle *pTriangle;
		int nFace, nIndex = 0;
		for(nFace=0; nFace<6; nFace++)
		{
			m_nStart[nFace] = nIndex;
			for(pTriangle=m_llTriangle[nFace].GetHead(); pTriangle->IsInList(); pTriangle = pTriangle->GetNext())
			{
				pTriangle->Draw(m_nIndex, nIndex);
				m_nTriangles++;
			}
			m_nEnd[nFace] = nIndex;
		}
	}

	void Draw()
	{
		int nFace;
		BuildIndexList();
		for(nFace=0; nFace<6; nFace++)
			DrawFace(nFace);
	}
	void DrawFace(int nFace)
	{
		int nStart = m_nStart[nFace];
		int nSize = m_nEnd[nFace] - nStart;
		while(nSize > 60000)
		{
			glDrawElements(GL_TRIANGLES, 60000, GL_UNSIGNED_SHORT, &m_nIndex[nStart]);
			nSize -= 60000;
			nStart += 60000;
		}
		glDrawElements(GL_TRIANGLES, nSize, GL_UNSIGNED_SHORT, &m_nIndex[nStart]);
	}
};

#endif // __ROAMSphere_h__
