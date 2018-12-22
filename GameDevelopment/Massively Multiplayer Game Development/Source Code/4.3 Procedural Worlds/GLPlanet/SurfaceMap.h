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

#ifndef __SurfaceMap_h__
#define __SurfaceMap_h__

#include "HeightMap.h"

class CSurfaceMap : public CPlanetaryMap
{
protected:
	CHeightMap *m_pHeightMap;
	CColor *m_pPalette;
	int m_nColors;

	CPixelBuffer m_pbRaw;
	CTexture m_tRaw;
	CPixelBuffer m_pbMap[6];
	CTexture m_tMap[6];

public:
	CSurfaceMap() : CPlanetaryMap() {}

	CHeightMap *GetHeightMap()					{ return m_pHeightMap; }
	CTexture &GetTextureMap(int nFace)			{ return m_tMap[nFace]; }
	CTexture &GetRawTextureMap()				{ return m_tRaw; }

	void GetTextureCoordinates(const CVector &v, int nFace, float &x, float &y)
	{
		GetCoordinates(v, nFace, x, y);
	}
	void GetRawTextureCoordinates(const CVector &v, float fHeight, float &x, float &y)
	{
		x = fHeight - m_pHeightMap->GetRadius() + m_pHeightMap->GetMaxHeight();
		x = Clamp(0.001f, 0.999f, x / (2.0f * m_pHeightMap->GetMaxHeight()));
		y = 0.5f;	// Very strange, bug in driver?
	}

	void Init(int nResolution, CHeightMap *pHeightMap, CColor *pPalette, int nColors);
};

#endif // __SurfaceMap_h__
