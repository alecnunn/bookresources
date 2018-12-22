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

#ifndef __HeightMap_h__
#define __HeightMap_h__

#include "PlanetaryMap.h"

class CCrater
{
public:
	float x, y, m_fRadius;

	CCrater()
	{
	}
	void Init()
	{
		x = ((float)rand()/(float)RAND_MAX)*0.5f + 0.25f;
		y = ((float)rand()/(float)RAND_MAX)*0.5f + 0.25f;
		m_fRadius = ((float)rand()/(float)RAND_MAX)*0.125f + 0.125f;
	}
};

class CCraterMap
{
protected:
	int m_nSeed;
	int m_nCraters;
	CCrater m_pCrater[20];

public:
	enum {TopLeft=9, TopRight=99, BottomLeft=999, BottomRight=9999};
	CCraterMap()							{}
	~CCraterMap()							{}

	void Init(int nSeed, int nCraters, bool bCreate=true)
	{
		m_nSeed = nSeed;
		srand(m_nSeed);
		m_nCraters = nCraters;
		for(int i=0; i<nCraters; i++)
		{
			if((float)rand()/(float)RAND_MAX < 0.5f)
				m_nCraters--;
		}
		for(i=0; i<m_nCraters; i++)
			m_pCrater[i].Init();
	}

	static int GetChildSeed(int nSeed, float &x, float &y)
	{
		nSeed = nSeed * 100;
		if(x < 0.5f)
		{
			if(y < 0.5f)
				nSeed += TopLeft;
			else
			{
				y -= 0.5f;
				nSeed += BottomLeft;
			}
		}
		else
		{
			if(y < 0.5f)
			{
				x -= 0.5f;
				nSeed += TopRight;
			}
			else
			{
				x -= 0.5f;
				y -= 0.5f;
				nSeed += BottomRight;
			}
		}
		x *= 2.0f;
		y *= 2.0f;
		return nSeed;
	}
	
	float GetDistance(int i, float x, float y)
	{
		float dx = x-m_pCrater[i].x;
		float dy = y-m_pCrater[i].y;
		dx *= dx;
		dy *= dy;
		return dx + dy;
	}
	float GetOffset(float x, float y, float fScale)
	{
		float fOffset = 0;
		for(int i=0; i<m_nCraters; i++)
		{
			float fDist = GetDistance(i, x, y);
			float fRadius = m_pCrater[i].m_fRadius * m_pCrater[i].m_fRadius;
			if(fDist < fRadius)
			{
				fDist /= fRadius;
				float fDist2 = fDist*fDist;
				//fOffset += (2.1f*fDist - 1.1f*fDist2*fDist2 - 1) * m_pCrater[i].m_fRadius;
				fOffset += ((fDist2 - 0.25f) * 4.0f * m_pCrater[i].m_fRadius) * (1-fDist);
			}
		}
		return fOffset * fScale;
	}
};

class CHeightMap : public CPlanetaryMap
{
protected:
	// Members for fractal routine
	CFractal m_frHeight;
	int m_nFractalSeed;
	float m_fOctaves;
	float m_fRoughness;

	// Members for crater routine
	int m_nCraterSeed;
	int m_nCraterTopLevel;
	int m_nCraterBottomLevel;
	int m_nCratersPerLevel;
	float m_fCraterScale[50];

	// Members for planet parameters
	float m_fRadius;
	float m_fMaxHeight;
	float m_fWaterLevel;

public:
	CHeightMap() : CPlanetaryMap()
	{
		m_nFractalSeed = 0;
		m_nCraterSeed = 0;
	}

	float GetRadius()		{ return m_fRadius; }
	float GetMaxHeight()	{ return m_fMaxHeight; }
	float GetWaterLevel()	{ return m_fWaterLevel; }

	void InitCraters(int nSeed, int nCraterTopLevel, int nCraterBottomLevel, int nCratersPerLevel)
	{
		m_nCraterSeed = nSeed;
		m_nCraterTopLevel = nCraterTopLevel;
		m_nCraterBottomLevel = nCraterBottomLevel;
		m_nCratersPerLevel = nCratersPerLevel;
	}
	void InitFractal(int nSeed, float fOctaves, float fRoughness)
	{
		m_nFractalSeed = nSeed;
		m_fOctaves = fOctaves;
		m_fRoughness = fRoughness;
		m_frHeight.Init(3, m_nFractalSeed, 1.0f - m_fRoughness, 2.0f);
	}
	void Init(int nResolution, float fRadius, float fMaxHeight=0, float fWaterLevel=-1.0E10f);

	float GetHeightNormalized(const CVector &vNormalized);
	float GetHeight(const CVector &vPosition, float fHeight)		{ return GetHeightNormalized(vPosition / fHeight); }
	float GetHeightOffset(const CVector &vPosition, float fHeight)	{ return GetHeightNormalized(vPosition / fHeight) - fHeight; }
	float GetHeight(const CVector &vPosition)						{ return GetHeight(vPosition, vPosition.Magnitude()); }
	float GetHeightOffset(const CVector &vPosition)					{ return GetHeightOffset(vPosition, vPosition.Magnitude()); }
};

#endif // __HeightMap_h__
