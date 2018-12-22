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
#include "Impostor.h"

int CImpostor::m_nViewport[4];

void CImpostor::InitImpostorRender(C3DObject *pCamera)
{
	// Calculate and store the camera orientation and distance for calculating future errors
	CVector vView = m_vPosition - pCamera->GetPosition();
	m_vOrientation = UnitInverse().RotateVector(-vView);
	m_fDistance = m_vOrientation.Magnitude();
	m_vOrientation /= m_fDistance;

	// Get the suggested resolution for this impostor and see if it's changed since last time
	short nOldResolution = m_nResolution;
	m_nResolution = GetImpostorResolution(GetImpostorScreenSpace(m_fDistance));
	if(nOldResolution != m_nResolution)
		SetFlags(NewTexture);

	// Calculate the model-view matrix for rendering the impostor, and get the up vector for rendering the billboard
	CMatrix mModel, mView;
	mModel.ModelMatrix(*this, vView);
	GetImpostorViewMatrix(mView, vView);
	m_vUp = CVector(mView.f12, mView.f22, mView.f32);

	// Set up the necessary viewport, modelview matrix, and projection matrix for rendering to the impostor texture
	glGetIntegerv(GL_VIEWPORT, m_nViewport);
	glViewport(0, 0, m_nResolution, m_nResolution);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Time for some fancy geometry
	float fAltitude = m_fDistance - m_fBoundingRadius;							// Height above the surface of the bounding radius
	float fHorizon2 = fAltitude*fAltitude + 2.0f*fAltitude*m_fBoundingRadius;	// Distance to horizon squared
	float fHorizon = sqrtf(fHorizon2);											// Distance to horizon
	float fCos = fHorizon / m_fDistance;										// Cosine of angle between ray to center of sphere and ray to horizon (part of a right triangle)
	float fTemp = fAltitude / fCos;												// Horizon distance cut off at the near clipping plane
	float fBound = sqrtf(fTemp*fTemp - fAltitude*fAltitude);					// Distance from center to sides at the near clipping plane (boundaries of the projection frustum) (part of another right triangle)
	float fTemp2 = m_fDistance / fCos;
	m_fBillboardRadius = sqrtf(fTemp2*fTemp2 - m_fDistance*m_fDistance);		// Distance from center to sides at the center of the object (size of the billboard)
	glFrustum(-fBound, fBound, -fBound, fBound, m_fDistance-m_fBoundingRadius, m_fDistance+m_fBoundingRadius);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(mView);
	glPushMatrix();
	glMultMatrixf(mModel);
}

void CImpostor::FinishImpostorRender()
{
	// Copy the currently rendered viewport into the impostor texture
	if(GetFlags(NewTexture) != 0)
		m_tBillboard.InitCopy(0, 0, m_nResolution, m_nResolution);
	else
		m_tBillboard.UpdateCopy(0, 0, m_nResolution, m_nResolution);
	ClearFlags(NeedsUpdate | NewTexture);

	// Clean up the OpenGL matrix stack and viewport
	glViewport(m_nViewport[0], m_nViewport[1], m_nViewport[2], m_nViewport[3]);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopMatrix();
}
