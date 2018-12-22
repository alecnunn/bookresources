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

#ifndef __Impostor_h__
#define __Impostor_h__

#include "Billboard.h"


class CImpostor : public CBillboard
{
protected:
	CVector m_vOrientation;		// The orientation of the camera relative to the impostor at the time it was rendered
	float m_fDistance;			// The distance to the camera relative to the impostor at the time it was rendered

	short m_nFlags;				// A set of bit flags for keeping track of various states
	short m_nResolution;		// The chosen resolution for the billboard texture map

	// Static members to hold state between calls to InitImpostorRender() and FinishImpostorRender()
	// (The min and max values can't be static because they're used in Draw())
	static int m_nViewport[4];

public:
	enum { NoFlags = 0x0000, Enabled = 0x0001, NeedsUpdate = 0x0002, NewTexture = 0x0004, AllFlags = 0xFFFF };

	CImpostor() : CBillboard()
	{
		m_nResolution = 0;
		m_nFlags = NoFlags;
	}

	void InitImpostorRender(C3DObject *pCamera);
	void FinishImpostorRender();
	void DrawImpostor(C3DObject *pCamera)
	{
		if(GetFlags(Enabled) != 0)
			DrawBillboard(pCamera);
	}

	short GetFlags(short nFlags)	{ return m_nFlags & nFlags; } 
	void SetFlags(short nFlags)		{ m_nFlags |= nFlags; } 
	void ClearFlags(short nFlags)	{ m_nFlags &= ~nFlags; } 

	float GetImpostorError(C3DObject *pCamera)
	{
		float fError = 1.0f;
		if(GetFlags(NeedsUpdate) != 0 || GetFlags(Enabled) == 0)
			return fError;
		CVector vView = m_vPosition - pCamera->GetPosition();
		float fDistance = vView.Magnitude();
		CVector vOrientation = UnitInverse().RotateVector(-vView);
		float fDelta = 0.01f * Abs(m_fDistance-fDistance) / m_fDistance;
		float fAngle = 1 - Abs((m_vOrientation | vOrientation) / fDistance);
		fError = Max(fAngle, fDelta);
		return fError;
	}
	void GetImpostorViewMatrix(CMatrix &mView, CVector &vView)
	{
		CVector vAt(0.0f);
		CVector vUp = CVector(vView.y, -vView.z, vView.x);
		mView.ViewMatrix(vAt, vView, vUp);
	}
	void GetImpostorViewMatrix(C3DObject *pCamera, CMatrix &mView)
	{
		CVector vView = m_vPosition - pCamera->GetPosition();
		GetImpostorViewMatrix(mView, vView);
	}
	float GetImpostorScreenSpace(float fDistance)
	{
		float fAltitude = Max(fDistance - m_fBoundingRadius, DELTA);				// Height above the surface of the bounding radius
		float fHorizon2 = fAltitude*fAltitude + 2.0f*fAltitude*m_fBoundingRadius;	// Distance to horizon squared
		float fHorizon = sqrtf(fHorizon2);											// Distance to horizon
		float fCos = fHorizon / fDistance;											// Cosine of angle between ray to center of sphere and ray to horizon (part of a right triangle)
		float fTemp2 = fDistance / fCos;
		float fBillboardRadius = sqrtf(fTemp2*fTemp2 - fDistance*fDistance);	// Distance from center to sides at the center of the object (size of the billboard)
		return fBillboardRadius / fDistance;
	}
	float GetImpostorScreenSpace(C3DObject *pCamera)
	{
		return GetImpostorScreenSpace(DistanceTo(*pCamera));
	}
	short GetImpostorResolution(float fScreenSpace)
	{
		short nResolution;
		if(fScreenSpace > 1.0f)
			nResolution = 0;
		else if(fScreenSpace > 0.75f)
			nResolution = 512;
		else if(fScreenSpace > 0.25f)
			nResolution = 256;
		else if(fScreenSpace > 0.1f)
			nResolution = 128;
		else if(fScreenSpace > 0.025f)
			nResolution = 64;
		else if(fScreenSpace > 0.01f)
			nResolution = 32;
		else
			nResolution = 16;
		return nResolution;
	}
	short GetImpostorResolution(C3DObject *pCamera)
	{
		return GetImpostorResolution(GetImpostorScreenSpace(pCamera));
	}
};

#endif // __Impostor_h__
