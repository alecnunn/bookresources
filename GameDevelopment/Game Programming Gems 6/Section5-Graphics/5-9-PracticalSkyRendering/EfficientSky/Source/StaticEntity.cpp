// StaticEntity.cpp: A Static Entity.
// Date: 08/03/05
// Created by: Aurelio Reis

#include "dxstdafx.h"


#include "../Include/Entity.h"
#include "../Include/SceneManager.h"


// Constructor.
CStaticEntity::CStaticEntity() {}

// Destructor.
CStaticEntity::~CStaticEntity() {}

// Prepare any special values for rendering this entity.
void CStaticEntity::PrepareRender()
{
	if ( !m_bVisible || !m_pModel )
	{
		return;
	}

	HRESULT hr;

	D3DXMATRIXA16 mWorldViewProjection;
	D3DXVECTOR3 vLightDir;
	D3DXCOLOR   vLightDiffuse;
	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;

	// Get the projection & view matrix from the camera class
	mWorld = m_matTransform;
	mProj = CSceneManager::m_pInstance->m_pCurrentView->m_matProj;
	mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;

	mWorldViewProjection = mWorld * mView * mProj;

	V( g_pSkyEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
	g_pSkyEffect->SetTechnique( g_hRenderTerrain );

	D3DXMATRIXA16 mWorldIT;
	D3DXMatrixInverse( &mWorldIT, NULL, &mWorld );
	D3DXMatrixTranspose( &mWorldIT, &mWorldIT );
	g_pSkyEffect->SetMatrix( "g_mWorldIT", &mWorldIT );

	g_pSkyEffect->SetTexture( "g_DiffuseCubeMap", g_pDiffuseIrradianceSkyCubeTexture );

	g_pSkyEffect->SetTexture( "g_DiffuseMap", g_pTerrainDiffuseMap );
	g_pSkyEffect->SetTexture( "g_NormalMap", g_pTerrainNormalMap );

	V( g_pSkyEffect->SetValue( g_hSunLightDir, &g_pSkyEntity->GetSunDirection(), sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hSunLightColor, &g_pSkyEntity->GetSunColor(), sizeof( D3DXVECTOR4 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightDir, &g_pSkyEntity->GetMoonDirection(), sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightColor, &g_pSkyEntity->GetMoonColor(), sizeof( D3DXVECTOR4 ) ) );
}