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
#include "PlanetaryMap.h"

// Returns a direction vector going through the center of the requested cell (x, y) in the face nFace.
CVector CPlanetaryMap::GetDirectionVector(int nFace, int x, int y)
{
	int nHalf = m_nResolution >> 1;
	float fx = (0.5f + (x - nHalf)) / (float)nHalf;
	float fy = (0.5f + (y - nHalf)) / (float)nHalf;
	const float fArray[6][3] = {{1, -fy, -fx}, {-1, -fy, fx}, {fx, 1, fy}, {fx, -1, -fy}, {fx, -fy, 1}, {-fx, -fy, -1}};
	CVector vDirection(fArray[nFace]);
	vDirection.Normalize();
	return vDirection;
}

// Returns the face the vector v is in (there is some uncertainty near the edges).
int CPlanetaryMap::GetFace(const CVector &v)
{
	int nFace;
	float x = ABS(v.x), y = ABS(v.y), z = ABS(v.z);
	if(x > y && x > z)
		nFace = (v.x > 0) ? RightFace : LeftFace;
	else if(y > x && y > z)
		nFace = (v.y > 0) ? TopFace : BottomFace;
	else
		nFace = (v.z > 0) ? FrontFace : BackFace;
	return nFace;
}

// Returns the face neighboring nFace on any side
int CPlanetaryMap::GetNeighboringFace(int nFace, int nSide)
{
	//enum {RightFace = 0, LeftFace = 1, TopFace = 2, BottomFace = 3, FrontFace = 4, BackFace = 5};
	//enum {LeftNeighbor = 0, RightNeighbor = 1, TopNeighbor = 2, BottomNeighbor = 3};
	static const int nNeighborMap[6][4] = 
	{
		{FrontFace, BackFace, TopFace, BottomFace}, // RightFace neighbors
		{BackFace, FrontFace, TopFace, BottomFace}, // LeftFace neighbors
		{LeftFace, RightFace, BackFace, FrontFace}, // TopFace neighbors
		{LeftFace, RightFace, FrontFace, BackFace}, // BottomFace neighbors
		{LeftFace, RightFace, TopFace, BottomFace}, // FrontFace neighbors
		{RightFace, LeftFace, TopFace, BottomFace}  // BackFace neighbors
	};
	return nNeighborMap[nFace][nSide];
}

// Returns the raw coordinates (x and y from -1 to 1) that vector v passes through in face nFace
void CPlanetaryMap::GetRawCoordinates(const CVector &v, int nFace, float &x, float &y)
{
	switch(nFace)
	{
		case RightFace:
			x = -v.z / v.x;
			y = -v.y / v.x;
			break;
		case LeftFace:
			x = v.z / -v.x;
			y = v.y / v.x;
			break;
		case TopFace:
			x = v.x / v.y;
			y = v.z / v.y;
			break;
		case BottomFace:
			x = v.x / -v.y;
			y = v.z / v.y;
			break;
		case FrontFace:
			x = v.x / v.z;
			y = -v.y / v.z;
			break;
		case BackFace:
			x = v.x / v.z;
			y = v.y / v.z;
			break;
	}
}

// Returns a pointer to the specified cell in the specified face.
// If the cell coordinates fall outside the specified face, a translation
// is made to return the proper cell from a neighboring face.
void *CPlanetaryMap::GetCell(int nFace, int x, int y)
{
	if(y >= 0 && y < m_nResolution)
	{
		if(x >= 0 && x < m_nResolution)
		{
			// The requested cell lies within the specified face, no translation needs to be done
		}
		else
		{
			if(x < 0)
			{
				// The requested cell lies in a face to the left (direction is relative to the current face)
				int nMin = -1-x;
				int nMax = m_nResolution+x;
				const int nNeighborFace[6] = {FrontFace, BackFace, LeftFace, LeftFace, LeftFace, RightFace};
				const int nNeighborx[6] = {nMax, nMax, y, -y, nMax, nMax};
				const int nNeighbory[6] = {y, y, nMin, nMax, y, y};
				x = nNeighborx[nFace];
				y = nNeighbory[nFace];
				nFace = nNeighborFace[nFace];
			}
			else
			{
				// The requested cell lies in a face to the right (direction is relative to the current face)
				int nMin = -1-(m_nResolution-1-x);
				int nMax = m_nResolution+(m_nResolution-1-x);
				const int nNeighborFace[6] = {BackFace, FrontFace, RightFace, RightFace, RightFace, LeftFace};
				const int nNeighborx[6] = {nMin, nMin, -y, y, nMin, nMin};
				const int nNeighbory[6] = {y, y, nMin, nMax, y, y};
				x = nNeighborx[nFace];
				y = nNeighbory[nFace];
				nFace = nNeighborFace[nFace];
			}
		}
	}
	else
	{
		if(x >= 0 && x < m_nResolution)
		{
			if(y < 0)
			{
				// The requested cell lies in a face in the up direction (direction is relative to the current face)
				int nMin = -1-y;
				int nMax = m_nResolution+y;
				const int nNeighborFace[6] = {TopFace, TopFace, BackFace, FrontFace, TopFace, TopFace};
				const int nNeighborx[6] = {nMax, nMin, -x, x, x, -x};
				const int nNeighbory[6] = {-x, x, nMin, nMax, nMax, nMin};
				x = nNeighborx[nFace];
				y = nNeighbory[nFace];
				nFace = nNeighborFace[nFace];
			}
			else
			{
				// The requested cell lies in a face in the down direction (direction is relative to the current face)
				int nMin = -1-(m_nResolution-1-y);
				int nMax = m_nResolution+(m_nResolution-1-y);
				const int nNeighborFace[6] = {BottomFace, BottomFace, FrontFace, BackFace, BottomFace, BottomFace};
				const int nNeighborx[6] = {nMax, nMin, x, -x, x, -x};
				const int nNeighbory[6] = {x, -x, nMin, nMax, nMin, nMax};
				x = nNeighborx[nFace];
				y = nNeighbory[nFace];
				nFace = nNeighborFace[nFace];
			}
		}
		else
		{
			// The requested cell lies in a face diagonal to the current face.
			// Because the shape is a cube, there is no diagonal face, so just return NULL.
			return NULL;
		}
	}
	return (void *)((unsigned int)m_pBuffer[nFace] + (y * m_nResolution + x) * GetCellSize());
}
