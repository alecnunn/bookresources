// SkyEntity.cpp: A Sky Entity.
// Date: 08/03/05
// Created by: Aurelio Reis

#include "dxstdafx.h"

#include "../Include/Entity.h"
#include "../Include/SceneManager.h"


float g_fStarColorModifier = 0.0f;


// Constructor.
CSkyEntity::CSkyEntity()
{
	m_bUseVertexSky = false;
}

// Destructor.
CSkyEntity::~CSkyEntity() {}

extern CD3DModel				g_DomeModels[ 3 ];

static const float fDEG2RAD		= D3DX_PI / 180.0f;
static const float fRAD2DEG		= 180.0f / D3DX_PI;
static const float fPiDiv2		= D3DX_PI / 2.0f;
static const float f2PI			= D3DX_PI * 2.0f;
static const float f4PI			= D3DX_PI * 4.0f;

#define DEG2RAD( x ) ( ( x ) * fDEG2RAD )
#define RAD2DEG( x ) ( ( x ) * fRAD2DEG )


// Calculate the sun attributes using Preetham's method.
void CSkyEntity::CalculateSunAttrPreetham()
{
	const int iDay = 15, iHour = g_iCurHour;

	//////////////////////////////////////////////////////////////////////////////
	// These equations can be found in "A Practical Analytic Model for Daylight" by Preetham.
	//////////////////////////////////////////////////////////////////////////////

	// Setup just a good looking latitude (in conjunction with the sm below).
	float fViewerLat = 0.0f, fViewerLon = 0.0f;
	float fRadLat = DEG2RAD( fViewerLat );
	float fRadLon = DEG2RAD( fViewerLon );

	// The Standard Meridian.
	float fSM = 0.0f;

	// Calculate the Solar Time.
	float fSolarTime =	iHour + 0.170f * sinf( f4PI * ( iDay - 80 ) / 373 ) -
						0.129f * sinf( f2PI * ( iDay - 8 ) / 355 ) +
						12 * ( fSM - fRadLon ) / D3DX_PI;

	// The Solar Declination (radians).
	float fDec = 0.4093f * sinf( f2PI * ( iDay - 81 ) / 368 );

	// <Optimizations>
	float fPiTimesSTDiv12 = D3DX_PI * fSolarTime / 12.0f;
	float fCosPiTimesSTDiv12 = cosf( fPiTimesSTDiv12 );

	// The solar (altitude) angle from zenith (radians).
	float fTheta = fPiDiv2 - asin( sinf( fRadLat ) * sinf( fDec ) -
								   cosf( fRadLat ) * cosf( fDec ) *
								   fCosPiTimesSTDiv12 );

	// The solar azimuth (radians).
	float fPhi = atanf( ( -cosf( fDec ) * sinf( fPiTimesSTDiv12 ) ) / 
						( -( cosf( fRadLat ) * sinf( fDec ) + sinf( fRadLat ) * cosf( fDec ) * fCosPiTimesSTDiv12 ) ) );

	m_vSunDir = D3DXVECTOR3( sinf( fPhi ) * sinf( fTheta ), cosf( fTheta ), cosf( fPhi ) * sinf( fTheta ) );

	// The amount of aerosols present.
	float Turb = 2.0f;
	float fBeta = 0.04608f * Turb - 0.04586f;

	// The Relative Optical Mass.
	float m = 1.0f / ( cosf( fTheta ) + 0.15f * powf( 93.885f - ( ( fTheta / D3DX_PI ) * 180.0f ), -1.253f ) );

	// If the sun is below the horizon, no color is contributed.
	if( _isnan( m ) )
	{
		return;
	}

	// <Optimizations for multiple frequencies>.
	const float mForR = -m * 0.008735f;
	const float mForA = -m * fBeta;

	// Ratio of small to large particle sizes.
	const float fAlpha = 1.3f;

	float fTau[ 3 ], fTauR, fTauA;

	// Light frequencies (rgb in um)..
	float vLambda[ 3 ] = { 0.65f, 0.570f, 0.475f };

	for ( int i = 0; i < 3; i++ )
	{
		// Rayleigh Scattering (um).
		fTauR = expf( mForR * powf( vLambda[ i ], -4.08f ) );

		// Mie absorption.
		fTauA = expf( mForA * powf( vLambda[ i ], -fAlpha ) );

		// Multiply the terms together.
		fTau[ i ] = fTauR * fTauA;
		
		// NOTE: Ozone absorption, mixed gases absorption, and water vapor absorption not taken into account.
    }

	m_vSunColor = D3DXCOLOR( fTau[ 0 ], fTau[ 1 ], fTau[ 2 ], 1.0f );
}

// Calculate the Sky Attributes.
void CSkyEntity::CalculateSkyAttributes()
{
	CalculateSunAttrPreetham();
//	CalculateSunAttrJensen();
//	CalculateMoonAttrJensen();

	// Hacked up moon direction/color calculation.
	m_vMoonDir = -m_vSunDir;
	m_vMoonColor = D3DXCOLOR( 0.2f, 0.2f, 0.4f, 1.0f );

	static const D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	g_fStarColorModifier = D3DXVec3Dot( &vUp, &m_vMoonDir );

	if ( g_fStarColorModifier < -0.6f )
	{
		g_fStarColorModifier = 0.0f;
	}
	else
	{
		g_fStarColorModifier = max( 0.0f, min( 1.0f, g_fStarColorModifier + 0.6f ) );
	}
}

// Update the Entity.
void CSkyEntity::Update()
{
	// Calculate the Sky Attributes.
	CalculateSkyAttributes();
}

// Prepare any special values for rendering this entity.
void CSkyEntity::PrepareRender()
{
	if ( !m_bVisible || !m_pModel )
	{
		return;
	}

	HRESULT hr;

	D3DXMATRIXA16 mWorldViewProjection;
	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;
	D3DXMATRIXA16 mInvView;

	// Get the projection & view matrix from the camera class
	mWorld = m_matTransform;
	mProj = CSceneManager::m_pInstance->m_pCurrentView->m_matProj;
	mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;

	// Reset to view origin, then move up a few units.
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

	D3DXMatrixInverse( &mInvView, 0, &mView );

	mWorldViewProjection = mWorld * mView * mProj;

	D3DXMATRIXA16 mInvWorld;
	D3DXMatrixInverse( &mInvWorld, NULL, &mWorld );

	D3DXMATRIXA16 mWorldIT;
	D3DXMatrixTranspose( &mWorldIT, &mInvWorld );
	g_pSkyEffect->SetMatrix( "g_mWorldIT", &mWorldIT );

	D3DXVec3TransformNormal( &vEyePos, &vEyePos, &mInvWorld );

	static D3DXMATRIXA16 mScaleBias( 0.5f,		0.0f,		0.0f,		0.0f,
									 0.0f,		0.5f,		0.0f,		0.0f,
									 0.0f,		0.0f,		1.0f,		0.0f,
									 0.5f,		2.0f,		0.5f,		1.0f );

	D3DXMATRIXA16 mTexProj;
    D3DXMatrixPerspectiveFovLH( &mTexProj, D3DXToRadian( 20.0f ), 1.0f, 1.0f, 100.0f );

	D3DXVECTOR3 vTexEye = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
				vTexLookAt = m_vSunDir,
				vTexUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMATRIXA16 mTexView;
	D3DXMatrixLookAtLH( &mTexView, &vTexEye, &vTexLookAt, &vTexUp );

	// Calculate the projective texture matrix for the sun.
	// NOTE: World matrix is purposely ignored.
	D3DXMATRIXA16 mTextureMatrix;
	D3DXMATRIXA16 mViewProj;

	mViewProj = mTexView * mTexProj;
	mTextureMatrix = mViewProj * mScaleBias;
	V( g_pSkyEffect->SetMatrix( "g_mSunTextureMatrix", &mTextureMatrix ) );

	vTexLookAt = m_vMoonDir,
	D3DXMatrixLookAtLH( &mTexView, &vTexEye, &vTexLookAt, &vTexUp );
	mViewProj = mTexView * mTexProj;
	mTextureMatrix = mViewProj * mScaleBias;
	V( g_pSkyEffect->SetMatrix( "g_mMoonTextureMatrix", &mTextureMatrix ) );

extern CD3DModel g_DomeModels[ 3 ];
	if ( &g_DomeModels[ 0 ] == m_pModel )
	{
		V( g_pSkyEffect->SetTexture( "g_SkyNormalMap", g_pSkyNormalMapLo ) );
	}
	else if ( &g_DomeModels[ 1 ] == m_pModel )
	{
		V( g_pSkyEffect->SetTexture( "g_SkyNormalMap", g_pSkyNormalMapMd ) );
	}
	else
	{
		V( g_pSkyEffect->SetTexture( "g_SkyNormalMap", g_pSkyNormalMapHi ) );
	}

	V( g_pSkyEffect->SetTexture( "g_SunTexture", g_pSunTexture ) );
	V( g_pSkyEffect->SetTexture( "g_MoonTexture", g_pMoonTexture ) );

	// Transform the light direction into local model space by multiplying by the inverse world matrix.
	D3DXVec3Normalize( &m_vSunDir, &m_vSunDir );
	D3DXVec3TransformNormal( &m_vSunDir, &m_vSunDir, &mInvWorld );
	D3DXVec3Normalize( &m_vSunDir, &m_vSunDir );

	if ( m_bUseVertexSky )
	{
		g_pSkyEffect->SetTechnique( g_hVertexSky );
	}
	else
	{
		g_pSkyEffect->SetTechnique( g_hPixelSky );
	}

	g_pSkyEffect->SetTexture( "g_StarCubeMap", g_pStarCubeTexture );

	V( g_pSkyEffect->SetValue( g_hLocalEyePos, &vEyePos, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hSunLightDir, &m_vSunDir, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hSunLightColor, &m_vSunColor, sizeof( D3DXVECTOR4 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightDir, &m_vMoonDir, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hMoonLightColor, &m_vMoonColor, sizeof( D3DXVECTOR4 ) ) );
	V( g_pSkyEffect->SetMatrix( g_hWorldViewProjection, &mWorldViewProjection ) );
	V( g_pSkyEffect->SetFloat( g_hTime, g_fTime ) );
	V( g_pSkyEffect->SetFloat( "g_fStarColorModifier", g_fStarColorModifier ) );
}


CSkyEntity *g_pSkyEntity = NULL;