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
#include "StarSystem.h"

void CStarSystem::Init(CInfo &info)
{
	char szKey[_MAX_PATH];
	CStar::Init(info);
	m_nPlanets = info.GetIntValue("Planets");
	m_pPlanets = new CPlanet[m_nPlanets];
	for(int i=0; i<m_nPlanets; i++)
	{
		sprintf(szKey, "%s.%d", m_szName, i+1);
		m_pPlanets[i].SetParent(this);
		m_pPlanets[i].Init(CInfo(info, szKey));
	}
}

void CStarSystem::Cleanup()
{
	if(m_pPlanets)
	{
		delete[] m_pPlanets;
		m_pPlanets = NULL;
	}
}

void CStarSystem::Update(C3DObject *pCamera, float fSeconds, int nPolygonMode, float fMaxError, bool bUpdate, bool bTexture)
{
	m_llPlanets.RemoveAll();

	// Handle collision detection and distance-based sorting
	CPlanet *pCollision = NULL;
	for(int i=0; i<GetPlanetCount(); i++)
	{
		CPlanet *pPlanet = GetPlanet(i);
		for(int j=0; j<=pPlanet->GetMoonCount(); j++)
		{
			CPlanet *pCurrent = (j < pPlanet->GetMoonCount()) ? pPlanet->GetMoon(j) : pPlanet;
			if(pCurrent->CollisionCheck(pCamera))
				pCollision = pCurrent;
			for(CPtrListNode *pNode=m_llPlanets.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
			{
				if(((CPlanet *)pNode->GetData())->GetHeight() > pCurrent->GetHeight())
					break;
			}
			pNode->InsertBefore(pCurrent);
		}
	}

	if(pCollision)
	{
		// Very cheesy collision response for now
		pCamera->SetVelocity(-pCamera->GetVelocity());
		CDoubleVector v = pCamera->GetPosition() - pCollision->GetPosition();
		double dScale = (DELTA - pCollision->GetHeight()) / v.Magnitude();
		v *= dScale;
		pCamera->SetPosition(pCamera->GetPosition() + v);
	}

	for(CPtrListNode *pNode=m_llPlanets.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
	{
		CPlanet *pPlanet = (CPlanet *)pNode->GetData();
		if(pPlanet->GetImpostorError(pCamera) >= 0.00002f)
		{
			if(pPlanet->GetImpostorScreenSpace(pCamera) > 1.0f)
			{
				pPlanet->ClearFlags(CImpostor::Enabled | CImpostor::NeedsUpdate);
				pPlanet->SetViewpoint(pCamera, 0);
			}
			else
			{
				pPlanet->SetFlags(CImpostor::Enabled | CImpostor::NeedsUpdate);
				CMatrix mView;
				pPlanet->GetImpostorViewMatrix(pCamera, mView);
				mView.Transpose();
				C3DObject objView(mView);
				objView.SetPosition(pCamera->GetPosition());
				pPlanet->SetViewpoint(&objView, 0);
			}
			if(bUpdate)
				pPlanet->Update(fMaxError);
		}
	}
}

void CStarSystem::Draw(C3DObject *pCamera, float fSeconds, int nPolygonMode, float fMaxError, bool bUpdate, bool bTexture)
{
	CPtrListNode *pNode;
	CPlanet *pPlanet;
	GLUtil()->BeginVertexArray();

	// Start by updating any impostors that need to be updated
	GLUtil()->BeginPBuffer();
	glPolygonMode(GL_FRONT, nPolygonMode);
	for(pNode=GetHeadPlanet(); pNode->IsInList(); pNode = pNode->GetNext())
	{
		pPlanet = (CPlanet *)pNode->GetData();
		if(pPlanet->GetFlags(CImpostor::NeedsUpdate) != 0)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			pPlanet->InitImpostorRender(pCamera);
			pPlanet->Draw(pCamera, this, bTexture, true);
			pPlanet->FinishImpostorRender();
		}
	}
	glPolygonMode(GL_FRONT, GL_FILL);
	GLUtil()->EndPBuffer();

	// Draw the billboards in the star system
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	CStar::Draw(pCamera);
	for(pNode = GetTailPlanet(); pNode->IsInList(); pNode = pNode->GetPrevious())
	{
		pPlanet = (CPlanet *)pNode->GetData();
		if(pPlanet->GetFlags(CImpostor::Enabled))
			pPlanet->DrawImpostor(pCamera);
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	// If the nearest planet is being rendered normally, render it here
	pPlanet = (CPlanet *)GetHeadPlanet()->GetData();
	if(pPlanet->GetFlags(CImpostor::Enabled) == 0)
	{
		glPolygonMode(GL_FRONT, nPolygonMode);
		pPlanet->Draw(pCamera, this, bTexture, false);
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	GLUtil()->EndVertexArray();
}
