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
#include "PlanetInfo.h"

CGameEngine::CGameEngine()
{
	// Initialize the star system and camera objects
	char szPath[_MAX_PATH];
	sprintf(szPath, "%s\\ss.cfg", GetApp()->GetStartupPath());
	ifstream ifTest(szPath, ios::nocreate);
	if(!ifTest)
	{
		MessageBox(NULL, "Unable to find ss.cfg file. Make sure you are running GLPlanet from a directory containing this file.", "Initialization Error", MB_OK);
		exit(0);
	}
	CInfo info(szPath, "StarSystem");
	m_starSystem.Init(info);

	if(!m_engInput.Attach())
		MessageBox(NULL, "Unable to initialize DirectInput. Continuing with setup.", "Initialization Error", MB_OK);
	m_engInput.AddMouseListener((IMouseListener *)this);
	m_engInput.AddKeyboardListener((IKeyboardListener *)this);

	// Initialize tweakable settings
	m_fMaxError = 0.0015f;
	m_nPolygonMode = GL_FILL;
	m_bInfo = true;
	m_bUpdate = true;
	m_bTexture = true;

	// Initialize static textures and the vertex array
	GLUtil()->Init();
	CTexture::InitStaticMembers(76234, 128);
	CPlanet::InitStaticMembers(76234, 128);

	// Load and initialize the font object
	m_fFont.Init(GLUtil()->GetHDC());

	CDoubleVector vPos = m_starSystem.GetPlanet(2)->GetPosition();
	vPos.z += 20000;
	m_3DCamera.SetPosition(vPos);
	//m_3DCamera.SetPosition(CDoubleVector(0.0, 0.0, 100.0f));

	m_starSystem.Update(&m_3DCamera, 0, m_nPolygonMode, m_fMaxError, m_bUpdate, m_bTexture);
	for(CPtrListNode *pNode=m_starSystem.GetHeadPlanet(); pNode->IsInList(); pNode = pNode->GetNext())
		((CPlanet *)pNode->GetData())->SetFlags(CImpostor::NeedsUpdate);
}

CGameEngine::~CGameEngine()
{
	GLUtil()->Cleanup();
	m_engInput.Detach();
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
		float fFPS = (float)(nFrames * 1000) / (float)nTime;
		sprintf(szFrameCount, "%2.2f FPS", fFPS);
		nTime = nFrames = 0;
	}
	nFrames++;

	// Update the star system (movement, collision detection, ROAM and impostor updates)
	float fSeconds = nMilliseconds * 0.001f;
	HandleInput(fSeconds);
	m_starSystem.Update(&m_3DCamera, fSeconds, m_nPolygonMode, m_fMaxError, m_bUpdate, m_bTexture);

	// Clear the display buffer and initialize the MODELVIEW matrix
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadMatrixf(m_3DCamera.GetViewMatrix());
	m_starSystem.Draw(&m_3DCamera, fSeconds, m_nPolygonMode, m_fMaxError, m_bUpdate, m_bTexture);
	glPopMatrix();

	// Draw the FPS in the top-left corner
	GLUtil()->BeginOrtho2D(GameApp()->GetWidth(), GameApp()->GetHeight());
	glColor3d(1.0, 1.0, 1.0);
	int nXPos = 10, nYPos = GameApp()->GetHeight() - 20;
	m_fFont.SetPosition(nXPos, nYPos);
	m_fFont.Print(szFrameCount);
	if(m_bInfo)
	{
		char szBuffer[_MAX_PATH];
		m_fFont.SetPosition(nXPos, nYPos-15);
		sprintf(szBuffer, "V: %d, MaxErr: %f", CVertex::Array.GetLockedElementCount(), m_fMaxError);
		m_fFont.Print(szBuffer);
		m_fFont.SetPosition(nXPos, nYPos-30);
		sprintf(szBuffer, "Speed: %2.2f km/sec", m_3DCamera.GetVelocity().Magnitude());
		m_fFont.Print(szBuffer);

		m_fFont.SetPosition(nXPos, nYPos-45);
		CPlanet *pPlanet = (CPlanet *)m_starSystem.GetHeadPlanet()->GetData();
		sprintf(szBuffer, "%s: %2.2f km, T: %d", pPlanet->GetName(), (float)pPlanet->GetAltitude(), pPlanet->GetTriangleCount());
		m_fFont.Print(szBuffer);
	}
	GLUtil()->EndOrtho2D();

	glFlush();
}

void CGameEngine::OnKeyDown(int nKey)
{
	switch(nKey)
	{
		case DIK_P:
			TogglePolygonMode();
			break;
		case DIK_T:
			ToggleTextures();
			break;
		case DIK_I:
			ToggleInfo();
			break;
		case DIK_U:
			ToggleUpdate();
			break;
	}
}

void CGameEngine::OnKeyUp(int nKey)
{
}

void CGameEngine::OnMouseButtonDown(int nButton)
{
}

void CGameEngine::OnMouseButtonUp(int nButton)
{
}

void CGameEngine::HandleInput(float fSeconds)
{
	m_engInput.Poll();

	for(int i=1; i<=m_starSystem.GetPlanetCount(); i++)
	{
		if(m_engInput.GetKeyState(DIK_1+i-1))
		{
			CPlanet *pPlanet = m_starSystem.GetPlanet(i-1);
			CDoubleVector v = pPlanet->GetPosition();
			v.z += 20000;
			m_3DCamera.SetPosition(v);
			m_3DCamera = CQuaternion(0, 0, 0, 1);
			m_3DCamera.SetVelocity(CVector(0.0f, 0.0f, 0.0f));
		}
	}

	if(m_engInput.GetKeyState(DIK_ADD))
		m_fMaxError *= 0.98f;
	if(m_engInput.GetKeyState(DIK_SUBTRACT))
		m_fMaxError *= 1.02f;

	// Turn left/right means rotate around the up axis
	if(m_engInput.GetKeyState(DIK_NUMPAD6))
		m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * -0.5f);
	if(m_engInput.GetKeyState(DIK_NUMPAD4))
		m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * 0.5f);

	// Turn up/down means rotate around the right axis
	if(m_engInput.GetKeyState(DIK_NUMPAD8))
		m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * -0.5f);
	if(m_engInput.GetKeyState(DIK_NUMPAD2))
		m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * 0.5f);

	// Roll means rotate around the view axis
	if(m_engInput.GetKeyState(DIK_NUMPAD7))
		m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * -0.5f);
	if(m_engInput.GetKeyState(DIK_NUMPAD9))
		m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * 0.5f);

#define THRUST		100.0f	// Acceleration rate due to thrusters (km/s*s)
#define RESISTANCE	0.00f	// Damping effect on velocity

	// Handle acceleration keys
	CVector vAccel;
	if(m_engInput.GetKeyState(DIK_SPACE))
		m_3DCamera.SetVelocity(CVector(0.0f));	// Full stop
	else
	{
		// Calculate camera's acceleration due to gravity
		//vAccel = m_starSystem.GetPlanet(2)->GravityVector(m_3DCamera);
		vAccel = CVector(0.0f);

		// Add camera's acceleration due to thrusters
		float fThrust = THRUST;
		if(m_engInput.GetKeyState(DIK_LCONTROL) || m_engInput.GetKeyState(DIK_RCONTROL))
			fThrust *= 100.0f;

		// Thrust forward/reverse affects velocity along the view axis
		if(m_engInput.GetKeyState(DIK_W))
			vAccel += m_3DCamera.GetViewAxis() * fThrust;
		if(m_engInput.GetKeyState(DIK_S))
			vAccel += m_3DCamera.GetViewAxis() * -fThrust;

		// Thrust left/right affects velocity along the right axis
		if(m_engInput.GetKeyState(DIK_D))
			vAccel += m_3DCamera.GetRightAxis() * fThrust;
		if(m_engInput.GetKeyState(DIK_A))
			vAccel += m_3DCamera.GetRightAxis() * -fThrust;

		m_3DCamera.Accelerate(vAccel, fSeconds, RESISTANCE);
	}
}
