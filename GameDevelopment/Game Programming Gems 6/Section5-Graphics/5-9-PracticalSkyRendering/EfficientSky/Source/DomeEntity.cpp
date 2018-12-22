// DomeEntity.cpp: A Dome Entity.
// Date: 08/03/05
// Created by: Aurelio Reis

#include "dxstdafx.h"


#include "../Include/Entity.h"
#include "../Include/SceneManager.h"


// Constructor.
CDomeEntity::CDomeEntity()
{
	m_bUseVertexSky = false;
}

// Destructor.
CDomeEntity::~CDomeEntity() {}

extern CD3DModel				g_DomeModels[ 3 ];

D3DXVECTOR3 ConvertToRectangular( float fELat, float fELon, float fGamma )
{
	float fCosLat = cosf( fELat );
	return D3DXVECTOR3(	fGamma * cosf( fELon ) * fCosLat,
						fGamma * sinf( fELon ) * fCosLat,
						fGamma * sinf( fELat ) );
}

// Convert elliptical coordinates to equatorial coordinates (see nightsky paper for info on earth coordinate sysems)
D3DXVECTOR3 ConvertEclipticToEquatorial( float T, D3DXVECTOR3 &vIn ){
	float fEpsilon = 0.409093f - 0.000227f * T;
	
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationX( &matRot, fEpsilon );
	D3DXVec3TransformCoord( &vIn, &vIn, &matRot );

	return vIn;
}

// Convert equatorial coordinates to horizon coordinates (see nightsky paper for info on earth coordinate sysems)
// Note: The result is dependent on the viewers position on Earth.
D3DXVECTOR3 ConvertEquatorialToHorizon( float lat, float lon, float T, D3DXVECTOR3 &vIn )
{
	float LMST = 4.894961f + 230121.675315f * T + lon;
	D3DXMATRIXA16 matRot;

	D3DXMatrixRotationZ( &matRot, -LMST );
	D3DXVec3TransformCoord( &vIn, &vIn, &matRot );

	D3DXMatrixRotationY( &matRot, lat - D3DX_PI / 2.0f );
	D3DXVec3TransformCoord( &vIn, &vIn, &matRot );

	return vIn;
}

D3DXVECTOR3 ConvertRectangularToLocalHorizonCoordinates( float fLat, float fLong, float T, D3DXVECTOR3 &vRectCoords )
{
	float fEpsilon = 0.409093f - 0.000227f * T;
	// Local Mean Sidereal Time (radians).
	float LMST = 4.894961f + 230121.675315f * T + fLong;

	D3DXMATRIXA16 matToHorizon;
	D3DXMATRIXA16 matRotX, matRotY, matRotZ;

	D3DXMatrixRotationX( &matRotX, fEpsilon );
	D3DXMatrixRotationY( &matRotY, fLat - D3DX_PI / 2.0f );
	D3DXMatrixRotationZ( &matRotZ, -LMST );
	matToHorizon = matRotY * matRotZ * matRotX;

	D3DXVec3TransformCoord( &vRectCoords, &vRectCoords, &matToHorizon );
	
	return vRectCoords;
}

static const float fDEG2RAD		= D3DX_PI / 180.0f;
static const float fRAD2DEG		= 180.0f / D3DX_PI;
static const float fPiDiv2		= D3DX_PI / 2.0f;
static const float f2PI			= D3DX_PI * 2.0f;
static const float f4PI			= D3DX_PI * 4.0f;

#define DEG2RAD( x ) ( ( x ) * fDEG2RAD )
#define RAD2DEG( x ) ( ( x ) * fRAD2DEG )


// Calculate the Sky Attributes.
void CDomeEntity::CalculateSkyAttributes( D3DXVECTOR3 &vSunDir, D3DXCOLOR &vLightDiffuse )
{
	const int iDay = 15, iHour = g_iCurHour;

	//////////////////////////////////////////////////////////////////////////////
	// These equations can be found in "A Practical Analytic Model for Daylight" by Preetham.
	//////////////////////////////////////////////////////////////////////////////

	// Setup just a good looking latitude (in conjunction with the sm below).
	float fViewerLat = -10.0f, fViewerLon = 0.0f;
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

	vSunDir = D3DXVECTOR3( sinf( fPhi ) * sinf( fTheta ), cosf( fTheta ), cosf( fPhi ) * sinf( fTheta ) );
	
	// The amount of aerosols present.
	float Turb = 2.0f;
	float fBeta = 0.04608f * Turb - 0.04586f;

	// The Relative Optical Mass.
	float m = 1.0f / ( cosf( fTheta ) + 0.15f * powf( 93.885f - ( ( fTheta / D3DX_PI ) * 180.0f ), -1.253f ) );

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

	vLightDiffuse = D3DXCOLOR( fTau[ 0 ], fTau[ 1 ], fTau[ 2 ], 1 );
}

// Update the Entity.
void CDomeEntity::Update()
{
	// Calculate the Sky Attributes.
	CalculateSkyAttributes( g_vSunDir, g_vSunColor );
}

// Prepare any special values for rendering this entity.
void CDomeEntity::PrepareRender()
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

	D3DXMATRIXA16 mSunProj;
    D3DXMatrixPerspectiveFovLH( &mSunProj, D3DX_PI * 0.75f, 1.0f, 0.1f, 1.0f );

	D3DXVECTOR3 vSunEye = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
				vSunLookAt = g_vSunDir,
				vSunUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMATRIXA16 mSunView;
	D3DXMatrixLookAtLH( &mSunView, &vSunEye, &vSunLookAt, &vSunUp );

	D3DXMATRIXA16 mScaleBias( 0.5f, 0.0f, 0.0f, 0.5f,
							  0.0f, 0.5f, 0.0f, 0.5f,
							  0.0f, 0.0f, 0.5f, 0.5f,
							  0.0f, 0.0f, 0.0f, 1.0f );

	// Calculate the projective texture matrix for the sun.
	// NOTE: World matrix is purposely ignored.
	// FIXME: Projection appears off...?
	D3DXMATRIXA16 mTextureMatrix;
	D3DXMatrixMultiply( &mTextureMatrix, &mSunProj, &mSunView );
	D3DXMatrixMultiply( &mTextureMatrix, &mScaleBias, &mTextureMatrix );

	V( g_pSkyEffect->SetMatrix( "g_mTextureMatrix", &mTextureMatrix ) );

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

	// Transform the light direction into local model space by multiplying by the inverse world matrix.
	D3DXVec3Normalize( &g_vSunDir, &g_vSunDir );
	D3DXVec3TransformNormal( &g_vSunDir, &g_vSunDir, &mInvWorld );
	D3DXVec3Normalize( &g_vSunDir, &g_vSunDir );

	D3DXCOLOR vWhite = D3DXCOLOR( 1, 1, 1, 1 );

	if ( m_bUseVertexSky )
	{
		g_pSkyEffect->SetTechnique( g_hVertexSky );
	}
	else
	{
		g_pSkyEffect->SetTechnique( g_hPixelSky );
	}

	V( g_pSkyEffect->SetValue( g_hLocalEyePos, &vEyePos, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hLightDir, &g_vSunDir, sizeof( D3DXVECTOR3 ) ) );
	V( g_pSkyEffect->SetValue( g_hLightDiffuse, &g_vSunColor, sizeof( D3DXVECTOR4 ) ) );
	V( g_pSkyEffect->SetMatrix( g_hWorldViewProjection, &mWorldViewProjection ) );
	V( g_pSkyEffect->SetFloat( g_hTime, g_fTime ) );      
}