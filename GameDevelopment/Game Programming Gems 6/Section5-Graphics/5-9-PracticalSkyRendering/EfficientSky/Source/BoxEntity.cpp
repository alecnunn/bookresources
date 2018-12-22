// BoxEntity.cpp: A Box Entity.
// Date: 08/03/05
// Created by: Aurelio Reis

#include "dxstdafx.h"


#include "../Include/Entity.h"
#include "../Include/SceneManager.h"


// Constructor.
CBoxEntity::CBoxEntity() {}

// Destructor.
CBoxEntity::~CBoxEntity() {}

// Prepare any special values for rendering this entity.
void CBoxEntity::PrepareRender()
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

	// Reset to view origin.
#ifndef USE_MODEL_CAMERA
	// See if they want to apply the special view hack.
	if ( GetViewHack() )
	{
		mView._41 = mView._42 = mView._43 = 0.0f;
	}
	D3DXVECTOR3 vEyePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
#else
	D3DXVECTOR3 vEyePos = *g_Camera.GetEyePt();
#endif

	mWorldViewProjection = mWorld * mView * mProj;

	D3DXMATRIXA16 mInvWorld;
	D3DXMatrixInverse( &mInvWorld, NULL, &mWorld );

	D3DXVec3TransformNormal( &vEyePos, &vEyePos, &mInvWorld );

	// Update the effect's variables.
	g_pSkyEffect->SetTechnique( g_hCubeMapPassthrough );

#ifdef USE_TEST_CUBEMAP
	g_pSkyEffect->SetTexture( "g_CubeMap", g_pTestCubeMap );
#else
	g_pSkyEffect->SetTexture( "g_CubeMap", g_pSkyCubeTexture );
#endif

	V( g_pSkyEffect->SetValue( g_hLocalEyePos, &vEyePos, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
	V( g_pSkyEffect->SetValue( g_hSunLightDir, &g_pSkyEntity->GetSunDirection(), sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hSunLightColor, &g_pSkyEntity->GetSunColor(), sizeof( D3DXVECTOR4 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightDir, &g_pSkyEntity->GetMoonDirection(), sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightColor, &g_pSkyEntity->GetMoonColor(), sizeof( D3DXVECTOR4 ) ) );
}