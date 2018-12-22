// scene
#include "stdafx.h"

#include "Scene.h"
#include "HeightPatch.h"
#include "Sky.h"

extern LPDIRECT3DDEVICE8 g_Device;

Scene::Scene()
{
	m_pTerrain=new HeightPatch;
	float size = m_pTerrain->GetSize();
	m_pSky = new Sky(D3DXVECTOR3(0,0,0), size*2.0, 1000);

}

Scene::~Scene()
{
	SAFE_DELETE(m_pTerrain);
	SAFE_DELETE(m_pSky);
}

void Scene::Render()
{
	// Set render states that apply globally to the scene.
	g_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// set up filtering for all texture stages.
	for (int i=0; i < 6; i++)
	{
		g_Device->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		g_Device->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		g_Device->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}


	g_Device->SetRenderState( D3DRS_ZWRITEENABLE, true);
	m_pSky->Render();

	m_pTerrain->Render();

}


