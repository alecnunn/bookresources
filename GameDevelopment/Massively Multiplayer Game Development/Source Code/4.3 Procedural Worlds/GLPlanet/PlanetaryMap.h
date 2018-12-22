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

#ifndef __PlanetaryMap_h__
#define __PlanetaryMap_h__

#include "Texture.h"


class CPlanetaryMap
{
protected:
	int m_nResolution;			// The resolution of the map (horizontal and vertical for each face of the cube)
	int m_nCellSize;			// The size of each cell in the map (in bytes)
	void *m_pAlloc;				// A pointer to the allocated memory buffer
	void *m_pBuffer[6];			// Pointers to each face (the first one is byte-aligned for performance)

public:
	enum {RightFace = 0, LeftFace = 1, TopFace = 2, BottomFace = 3, FrontFace = 4, BackFace = 5};
	enum {LeftNeighbor = 0, RightNeighbor = 1, TopNeighbor = 2, BottomNeighbor = 3};

	CPlanetaryMap()
	{
		m_nResolution = 0;
		m_pAlloc = NULL;
		for(int i=0; i<6; i++)
			m_pBuffer[i] = NULL;
	}
	CPlanetaryMap(int nResolution, int nCellSize)
	{
		m_pAlloc = NULL;
		Init(nResolution, nCellSize);
	}
	CPlanetaryMap(const CPlanetaryMap &map)
	{
		m_pAlloc = NULL;
		*this = map;
	}
	~CPlanetaryMap()	{ Cleanup(); }

	void operator=(const CPlanetaryMap &map)
	{
		Init(map.GetResolution(), map.GetCellSize());
		memcpy(m_pBuffer[0], map.m_pBuffer[0], GetBufferSize());
	}
	bool operator==(const CPlanetaryMap &map)
	{
		return (m_nResolution == map.m_nResolution && m_nCellSize == map.m_nCellSize);
	}

	void Cleanup()
	{
		if(m_pAlloc)
		{
			delete m_pAlloc;
			m_pAlloc = NULL;
			for(int i=0; i<6; i++)
				m_pBuffer[i] = NULL;
		}
	}
	void Init(int nResolution, int nCellSize)
	{
		Cleanup();
		m_nResolution = nResolution;
		m_nCellSize = nCellSize;
		m_pAlloc = new unsigned char[GetBufferBytes() + ALIGN_MASK];
		m_pBuffer[0] = (void *)ALIGN(m_pAlloc);
		for(int i=1; i<6; i++)
			m_pBuffer[i] = (void *)((unsigned int)m_pBuffer[i-1] + GetFaceBytes());
	}

	int GetResolution() const 		{ return m_nResolution; }
	int GetCellSize() const			{ return m_nCellSize; }
	int GetFaceSize() const			{ return m_nResolution * m_nResolution; }
	int GetBufferSize() const		{ return GetFaceSize() * 6; }
	int GetFaceBytes() const		{ return GetFaceSize() * GetCellSize(); }
	int GetBufferBytes() const		{ return GetBufferSize() * GetCellSize(); }
	void *GetBuffer()				{ return m_pBuffer[0]; }
	void *GetFaceBuffer(int nFace)	{ return m_pBuffer[nFace]; }
	void *operator[](int nFace)		{ return m_pBuffer[nFace]; }

	void ClearBuffer()				{ memset(m_pBuffer[0], 0, GetBufferBytes()); }
	void ClearFace(int n)			{ memset(m_pBuffer[n], 0, GetFaceBytes()); }
	void Swap(CPlanetaryMap &map)
	{
		void *pTemp;
		ASSERT(*this == map);
		SWAP(m_pAlloc, map.m_pAlloc, pTemp);
		for(int i=0; i>6; i++)
			SWAP(m_pBuffer[i], map.m_pBuffer[i], pTemp);
	}

	CVector GetDirectionVector(int nFace, int x, int y);
	static int GetFace(const CVector &v);
	static void GetRawCoordinates(const CVector &v, int nFace, float &x, float &y);
	static int GetNeighboringFace(int nFace, int nSide);
	void *GetCell(int nFace, int x, int y);

	// Returns the coordinates (x and y from 0 to 1) that vector v passes through in face nFace
	static void GetCoordinates(const CVector &v, int nFace, float &x, float &y)
	{
		GetRawCoordinates(v, nFace, x, y);
		x = Clamp(0, 1, (x+1)*0.5f);
		y = Clamp(0, 1, (y+1)*0.5f);
	}
	static int GetCoordinates(const CVector &v, float &x, float &y)
	{
		int nFace = GetFace(v);
		GetCoordinates(v, nFace, x, y);
		return nFace;
	}

	// Returns the cell location (x and y from -0.5 to m_nResolution-0.5) that vector v passes through in face nFace
	// Floats are used for x and y to indicate where the vector lies relative to the center of the cell
	void GetCellLocation(const CVector &v, int nFace, float &x, float &y)
	{
		GetRawCoordinates(v, nFace, x, y);
		int nHalf = m_nResolution >> 1;
		x = Clamp(-0.5f, m_nResolution-0.5f, x*nHalf + nHalf - 0.5f);
		y = Clamp(-0.5f, m_nResolution-0.5f, y*nHalf + nHalf - 0.5f);
	}
	int GetCellLocation(const CVector &v, float &x, float &y)
	{
		int nFace = GetFace(v);
		GetCellLocation(v, nFace, x, y);
		return nFace;
	}

	// Returns the integer cell location (x and y from 0 to m_nResolution-1) that vector v passes through in face nFace
	void GetCellLocation(const CVector &v, int nFace, int &x, int &y)
	{
		float fx, fy;
		GetRawCoordinates(v, nFace, fx, fy);
		int nHalf = m_nResolution >> 1;
		x = (int)Clamp(0, (float)(m_nResolution-1), fx*nHalf+nHalf);
		y = (int)Clamp(0, (float)(m_nResolution-1), fy*nHalf+nHalf);
	}
	int GetCellLocation(const CVector &v, int &x, int &y)
	{
		int nFace = GetFace(v);
		GetCellLocation(v, nFace, x, y);
		return nFace;
	}
};

#endif // __PlanetaryMap_h__
