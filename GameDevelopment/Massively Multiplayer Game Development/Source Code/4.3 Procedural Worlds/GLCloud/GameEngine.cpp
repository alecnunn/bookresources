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
#include "GameEngine.h"
#include "GLUtil.h"

CGameEngine::CGameEngine()
{
	m_nTime = 0;
	m_nSeed = 367343;
	m_fHalfSize = 1.5f;
	m_fFont.Init(GetGameApp()->GetHDC());
	m_bRotatingLight = false;
	m_bRotatingClouds = false;
	m_bUseImpostor = true;

	glPushMatrix();
	m_3DCamera.SetPosition(CDoubleVector(0, 0, 65));
	m_vLight = CVector(0, -1, 0);
	glPopMatrix();

	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	CTexture::InitStaticTextures();

	CMatrix m;
	m.ModelMatrix(CVector(0, 0, 0), CVector(0, 1, 0), CVector(0, 0, 1));
	m_cBlock.CQuaternion::operator=(m);
	m_cBlock.SetPosition(CDoubleVector(0.0));
	m_cBlock.Init(96, 96, 16);
	m_cBlock.NoiseFill(m_nSeed++);
	m_cBlock.Light(m_vLight);

	//if(glh_init_extension("WGL_ARB_pixel_format") && glh_init_extension("WGL_ARB_pbuffer"))
	//	m_PBuffer.Init(256, 256);
}

CGameEngine::~CGameEngine()
{
}

void CGameEngine::RenderFrame(int nMilliseconds)
{
	// Determine the FPS
	static char szFrameCount[20] = {0};
	static int nTime = 0;
	static int nFrames = 0;
	nTime += nMilliseconds;
	if(nTime >= 1000)
	{
		m_fFPS = (float)(nFrames * 1000) / (float)nTime;
		sprintf(szFrameCount, "%2.2f FPS", m_fFPS);
		nTime = nFrames = 0;
	}
	nFrames++;

	// Move the camera
	HandleInput(nMilliseconds * 0.001f);
	if(m_bRotatingClouds)
		m_cBlock.Rotate(CVector(1, 0, 0), 0.2f * nMilliseconds * 0.001f);
	if(m_bRotatingLight)
		m_vLight = CQuaternion(CVector(0, 0, 1), 0.2f * nMilliseconds * 0.001f).RotateVector(m_vLight);

	// Draw the frame
	glPushMatrix();
	glLoadMatrixf(m_3DCamera.GetViewMatrix());
	static int nTest = 0;
	if(m_bUseImpostor)
	{
		// Instead of calculating error, just update the impostor every 16th frame
		// If the shading needs to be updated, update it every 16th frame,
		// but not on the same frame as the impostor update or the movement will be too jerky
		if(!((nTest+8) & 0xF))
			if(m_bRotatingLight || m_bRotatingClouds)
				m_cBlock.Light(m_vLight);
		if(!(nTest & 0xF))
		{
			//glClearColor(0.253f, 0.47f, 0.683f, 0);
			m_PBuffer.HandleModeSwitch();
			m_PBuffer.MakeCurrent();
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_cBlock.Draw(&m_3DCamera, m_fHalfSize, true);
			GetGameApp()->MakeCurrent();
		}
		nTest++;

		// Clear the backgrund to a sky blue and draw the cloud block
		glClearColor(0.253f, 0.47f, 0.683f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_cBlock.CImpostor::Draw(&m_3DCamera);
	}
	else
	{
		// If not using impostors, update the shading if necessary and draw every frame
		nTest = 0;
		if(m_bRotatingLight || m_bRotatingClouds)
			m_cBlock.Light(m_vLight);
		glClearColor(0.253f, 0.47f, 0.683f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_cBlock.Draw(&m_3DCamera, m_fHalfSize, false);
	}
	glPopMatrix();

	// Draw info in the top-left corner
	char szBuffer[256];
	m_fFont.Begin();
	glColor3d(1.0, 1.0, 1.0);
	m_fFont.SetPosition(0, 0);
	m_fFont.Print(szFrameCount);
	m_fFont.SetPosition(0, 15);
	sprintf(szBuffer, "Cell Size: %.2f", m_fHalfSize*2);
	m_fFont.Print(szBuffer);
	m_fFont.End();
	glDisable(GL_LIGHTING);	// CFont::End() enables lighting
	glFlush();
}

void CGameEngine::HandleInput(float fSeconds)
{
	static bool bMDown = false;
	if(GetKeyState('M') & 0x8000)
	{
		if(!bMDown)
			m_bRotatingLight = !m_bRotatingLight;
		bMDown = true;
	}
	else
		bMDown = false;

	static bool bRDown = false;
	if(GetKeyState('R') & 0x8000)
	{
		if(!bRDown)
			m_bRotatingClouds = !m_bRotatingClouds;
		bRDown = true;
	}
	else
		bRDown = false;

	static bool bIDown = false;
	if(GetKeyState('I') & 0x8000)
	{
		if(!bIDown)
			m_bUseImpostor = !m_bUseImpostor;
		bIDown = true;
	}
	else
		bIDown = false;

	if(GetKeyState('U') & 0x8000)
	{
		m_cBlock.NoiseFill(m_nSeed++);
		m_cBlock.Light(m_vLight);
	}
	if(GetKeyState(VK_ADD) & 0x8000)
		m_fHalfSize += 0.05f;
	if(GetKeyState(VK_SUBTRACT) & 0x8000)
		m_fHalfSize = Max(0.5f, m_fHalfSize-0.05f);

	// Turn left/right means rotate around the up axis
	if(GetKeyState(VK_NUMPAD6) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * -0.5f);
	if(GetKeyState(VK_NUMPAD4) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * 0.5f);

	// Turn up/down means rotate around the right axis
	if(GetKeyState(VK_NUMPAD8) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * -0.5f);
	if(GetKeyState(VK_NUMPAD2) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * 0.5f);

	// Roll means rotate around the view axis
	if(GetKeyState(VK_NUMPAD7) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * -0.5f);
	if(GetKeyState(VK_NUMPAD9) & 0x8000)
		m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * 0.5f);

#define THRUST		1.0f	// Acceleration rate due to thrusters (units/s*s)
#define RESISTANCE	0.1f	// Damping effect on velocity

	// Handle acceleration keys
	CVector vAccel(0.0f);
	if(GetKeyState(VK_SPACE) & 0x8000)
		m_3DCamera.SetVelocity(CVector(0.0f));	// Full stop
	else
	{
		// Add camera's acceleration due to thrusters
		float fThrust = THRUST;
		if(GetKeyState(VK_CONTROL) & 0x8000)
			fThrust *= 10.0f;

		// Thrust forward/reverse affects velocity along the view axis
		if(GetKeyState('W') & 0x8000)
			vAccel += m_3DCamera.GetViewAxis() * fThrust;
		if(GetKeyState('S') & 0x8000)
			vAccel += m_3DCamera.GetViewAxis() * -fThrust;

		// Thrust left/right affects velocity along the right axis
		if(GetKeyState('D') & 0x8000)
			vAccel += m_3DCamera.GetRightAxis() * fThrust;
		if(GetKeyState('A') & 0x8000)
			vAccel += m_3DCamera.GetRightAxis() * -fThrust;

		m_3DCamera.Accelerate(vAccel, fSeconds, RESISTANCE);
	}
}
