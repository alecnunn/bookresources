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
#include "SurfaceMap.h"

void CSurfaceMap::Init(int nResolution, CHeightMap *pHeightMap, CColor *pPalette, int nColors)
{
	CPlanetaryMap::Init(nResolution, sizeof(CColor));
	m_pHeightMap = pHeightMap;
	m_pPalette = pPalette;
	m_nColors = nColors;
	m_pbRaw.Init(m_nColors, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, m_pPalette);
	m_tRaw.Init(&m_pbRaw, true, false);

	if(nResolution)
	{
		float *pHeight = (float *)pHeightMap->GetBuffer();
		CColor *pColor = (CColor *)GetBuffer();
		for(int nFace=0; nFace<6; nFace++)
		{
			for(int y=0; y<m_nResolution; y++)
			{
				for(int x=0; x<m_nResolution; x++)
				{
					float fHeight = *pHeight - m_pHeightMap->GetRadius() + m_pHeightMap->GetMaxHeight();
					fHeight = (m_nColors-1) * Clamp(0.001f, 0.999f, fHeight / (2.0f * m_pHeightMap->GetMaxHeight()));
					int nIndex = (int)fHeight;
					float fRatio = fHeight - nIndex;
					*pColor = pPalette[nIndex] * (1-fRatio) + pPalette[nIndex+1] * fRatio;

					pColor++;
					pHeight++;
				}
			}
			m_pbMap[nFace].Init(m_nResolution, m_nResolution, 4, GL_RGBA, GL_UNSIGNED_BYTE, GetFaceBuffer(nFace));
			m_tMap[nFace].Init(&m_pbMap[nFace]);
		}
	}
}
