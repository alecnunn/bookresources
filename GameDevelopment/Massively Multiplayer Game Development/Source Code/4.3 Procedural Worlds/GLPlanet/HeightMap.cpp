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
#include "HeightMap.h"

void CHeightMap::Init(int nResolution, float fRadius, float fMaxHeight, float fWaterLevel)
{
	m_fRadius = fRadius;
	m_fMaxHeight = fMaxHeight;
	m_fWaterLevel = fWaterLevel;

	float fScale = m_fRadius;
	if(m_nCraterBottomLevel > 49)
		m_nCraterBottomLevel = 49;
	for(int i=0; i<=m_nCraterBottomLevel; i++)
	{
		m_fCraterScale[i] = fScale / (2.0f * powf(fScale, 0.25f));
		fScale *= 0.5f;
	}

	if(nResolution)
	{
		CPlanetaryMap::Init(nResolution, sizeof(float));
		float *pCell = (float *)GetBuffer();
		for(int nFace=0; nFace<6; nFace++)
		{
			for(int y=0; y<m_nResolution; y++)
			{
				for(int x=0; x<m_nResolution; x++)
				{
					*pCell = GetHeight(GetDirectionVector(nFace, x, y));
					pCell++;
				}
			}
		}
	}
}

float CHeightMap::GetHeightNormalized(const CVector &vNormalized)
{
	// The height always starts out as the radius
	float fHeight = m_fRadius;
	CVector v = vNormalized;

	// If a fractal seed was passed in, adjust the height based on the fractal routine
	if(m_nFractalSeed)
		fHeight += m_frHeight.fBmTest(v, m_fOctaves) * m_fMaxHeight;

	float x, y;
	int nFace = GetCoordinates(v, x, y);
	CPtrList llMods;
	CVector vPos = vNormalized * fHeight;

	// If a crater seed was passed in, adjust the height based on the crater routine
	if(m_nCraterSeed)
	{
		int nSeed = m_nCraterSeed + (nFace<<4);

		// Skip over the top levels of the crater tree, generating the appropriate seeds on the way
		for(int i=0; i<m_nCraterTopLevel; i++)
			nSeed = CCraterMap::GetChildSeed(nSeed, x, y);

		// Now add all the applicable crater offsets down to the bottom of the tree
		CCraterMap node;
		while(i <= m_nCraterBottomLevel)
		{
			node.Init(nSeed, m_nCratersPerLevel);
			fHeight += node.GetOffset(x, y, m_fCraterScale[i]);
			nSeed = CCraterMap::GetChildSeed(nSeed, x, y);
			i++;
		}
	}

	// Return the final adjusted height
	return fHeight;
}
