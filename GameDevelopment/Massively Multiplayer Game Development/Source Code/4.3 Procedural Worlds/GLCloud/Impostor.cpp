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
#include "GameApp.h"
#include "Impostor.h"

float CImpostor::CalculateError(C3DObject *pCamera)
{
	CVector vOffset = m_vCamera - pCamera->GetPosition();
	return vOffset.Magnitude();
}

bool CImpostor::IsInView(C3DObject *pCamera)
{
	// Create the 8 corner vertices of the model's 3D bounding box
	CVector vCorner[8];
	vCorner[0] = CVector(m_vDimension.x, m_vDimension.y, m_vDimension.z);
	vCorner[1] = CVector(m_vDimension.x, m_vDimension.y, -m_vDimension.z);
	vCorner[2] = CVector(m_vDimension.x, -m_vDimension.y, -m_vDimension.z);
	vCorner[3] = CVector(m_vDimension.x, -m_vDimension.y, m_vDimension.z);
	vCorner[4] = CVector(-m_vDimension.x, m_vDimension.y, m_vDimension.z);
	vCorner[5] = CVector(-m_vDimension.x, m_vDimension.y, -m_vDimension.z);
	vCorner[6] = CVector(-m_vDimension.x, -m_vDimension.y, -m_vDimension.z);
	vCorner[7] = CVector(-m_vDimension.x, -m_vDimension.y, m_vDimension.z);

	// Calculate the camera model-view matrix for testing the bounding box
	CVector v = m_vPosition - pCamera->GetPosition();
	CMatrix mView = pCamera->GetViewMatrix();
	CMatrix mModel;
	mModel.ModelMatrix(*this, v);
	CMatrix mCamera = mView * mModel;

	CVector vActual;
	for(int i=0; i<8; i++)
	{
		// Transform each corner vertex and divide by -z (equivalent to 90 degree fov projection)
		// This will help us find the 2D bounding box we need to fit around our impostor
		vActual = mCamera.TransformVector(vCorner[i]);
		if(vActual.z < 0)
		{
			vActual.x /= -vActual.z;
			vActual.y /= -vActual.z;
			if(Abs(vActual.x) < 1 && Abs(vActual.y) < 1)
				return true;
		}
	}
	return false;
}

void CImpostor::InitImpostorRender(C3DObject *pCamera)
{
	// Create the 8 corner vertices of the model's 3D bounding box
	CVector vCorner[8];
	vCorner[0] = CVector(m_vDimension.x, m_vDimension.y, m_vDimension.z);
	vCorner[1] = CVector(m_vDimension.x, m_vDimension.y, -m_vDimension.z);
	vCorner[2] = CVector(m_vDimension.x, -m_vDimension.y, -m_vDimension.z);
	vCorner[3] = CVector(m_vDimension.x, -m_vDimension.y, m_vDimension.z);
	vCorner[4] = CVector(-m_vDimension.x, m_vDimension.y, m_vDimension.z);
	vCorner[5] = CVector(-m_vDimension.x, m_vDimension.y, -m_vDimension.z);
	vCorner[6] = CVector(-m_vDimension.x, -m_vDimension.y, -m_vDimension.z);
	vCorner[7] = CVector(-m_vDimension.x, -m_vDimension.y, m_vDimension.z);

	// Calculate the impostor model-view matrix so we can find the 2D bounding box for the billboard texture
	// Currently the up vector is arbitrary. It is chosen in such a way that it can't be parallel to the view vector.
	// (We can optimize impostor resolution by picking a better up vector, but we'd have to be careful that it is always perpendicular to the view vector.)
	CVector v = m_vPosition - pCamera->GetPosition();
	CMatrix mModel, mView;
	mModel.ModelMatrix(*this, v);
	mView.ViewMatrix(CVector(0.0f), v, CVector(v.y, -v.z, v.x));
	m_vUp = CVector(mView.f12, mView.f22, mView.f32);
	CMatrix m = mView * mModel;

	m_fMinx = 1; m_fMiny = 1; m_fMaxx = -1; m_fMaxy = -1;
	for(int i=0; i<8; i++)
	{
		// Transform each corner vertex and divide by -z (equivalent to 90 degree fov projection)
		// This will help us find the 2D bounding box we need to fit around our impostor
		vCorner[i] = m.TransformVector(vCorner[i]);
		vCorner[i] /= -vCorner[i].z;
		m_fMinx = Min(m_fMinx, vCorner[i].x);
		m_fMiny = Min(m_fMiny, vCorner[i].y);
		m_fMaxx = Max(m_fMaxx, vCorner[i].x);
		m_fMaxy = Max(m_fMaxy, vCorner[i].y);
	}

	// Use the size of the bounding box to determine the resolution we need for the impostor
	float fTemp = Max(m_fMaxx-m_fMinx, m_fMaxy-m_fMiny) * 0.5f;

	// If it goes too far past the boundaries of the screen, handle it as a special case
	m_vOffset = CVector(0.0f);
	if(fTemp > 1.0f)
	{
		// When really close to the impostor, these numbers tend to approach infinity. We need to do something to address that
		float fDist = Max(v.Magnitude(), 32);
		m_fMinx = -fDist;
		m_fMaxx = fDist;
		m_fMiny = -fDist;
		m_fMaxy = fDist;
		m_nResolution = 256;
		m_vUp = pCamera->GetUpAxis();

		m_vOffset = GetPosition() - (pCamera->GetPosition() + pCamera->GetViewAxis()*fDist);

		glViewport(0, 0, m_nResolution, m_nResolution);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		v = m_vPosition - pCamera->GetPosition();
		CMatrix mView = pCamera->GetViewMatrix();
		CMatrix mModel;
		mModel.ModelMatrix(*this, v);
		CMatrix mCamera = mView * mModel;
		glLoadMatrixf(mCamera);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0, 1000.0);
		return;
	}

	if(fTemp > 0.75f)
		m_nResolution = 256;
	else if(fTemp > 0.25f)
		m_nResolution = 128;
	else if(fTemp > 0.1f)
		m_nResolution = 64;
	else
		m_nResolution = 32;

	// Set up the necessary viewport, modelview matrix, and projection matrix for rendering to the impostor texture
	glViewport(0, 0, m_nResolution, m_nResolution);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(m);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(m_fMinx, m_fMaxx, m_fMiny, m_fMaxy, 1.0, 1000.0);

	// Scale these up by distance so they can be used to determine billboard vertices
	float fDist = v.Magnitude();
	m_fMinx *= fDist;
	m_fMiny *= fDist;
	m_fMaxx *= fDist;
	m_fMaxy *= fDist;
}

void CImpostor::FinishImpostorRender()
{
	// Copy the currently rendered viewport into the impostor texture
	m_tImpostor.InitCopy(0, 0, m_nResolution, m_nResolution);

	// Clean up the OpenGL matrix stack and viewport
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glViewport(0, 0, GetGameApp()->GetWidth(), GetGameApp()->GetHeight());
}

void CImpostor::Draw(C3DObject *pCamera)
{
	// First determine the actual normal, up, and right vectors for the billboard
	CVector vPos = (m_vPosition - m_vOffset) - pCamera->GetPosition();
	float fDistance = vPos.Magnitude();
	CVector vNormal = vPos / -fDistance;
	CVector vRight = m_vUp ^ vNormal;
	vRight.Normalize();
	CVector vUp = vNormal ^ vRight;
	vUp.Normalize();

	// Then draw the billboarded rectangle
	m_tImpostor.Enable();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3fv(vPos + vUp*m_fMaxy + vRight*m_fMinx);
	glTexCoord2f(0, 0);
	glVertex3fv(vPos + vUp*m_fMiny + vRight*m_fMinx);
	glTexCoord2f(1, 0);
	glVertex3fv(vPos + vUp*m_fMiny + vRight*m_fMaxx);
	glTexCoord2f(1, 1);
	glVertex3fv(vPos + vUp*m_fMaxy + vRight*m_fMaxx);
	glEnd();
	glDisable(GL_BLEND);
	m_tImpostor.Disable();
}
