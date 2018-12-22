//-----------------------------------------------------------------------------
// File: CSourceEffectZoomFX.cpp
//
// Desc: 
//-----------------------------------------------------------------------------

#include <assert.h>

#include "CSourceEffectZoomFX.h"

#define PSET_SETGET	(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::CSourceEffectZoomFX()
// Desc: Constructs the class.
//-----------------------------------------------------------------------------
CSourceEffectZoomFX::CSourceEffectZoomFX()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::~CSourceEffectZoomFX()
// Desc: Destroys the class.
//-----------------------------------------------------------------------------
CSourceEffectZoomFX::~CSourceEffectZoomFX()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::IsSupported()
// Desc: Returns true if sound card supports ZoomFX properties
//-----------------------------------------------------------------------------
BOOL CSourceEffectZoomFX::IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	ZoomFXProperties props;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return FALSE;

	if ( FAILED (hr = p3DBuffer->QueryInterface( IID_IKsPropertySet, (void**)&m_pPropertySet)) )
	{
		SAFE_RELEASE( m_pPropertySet );
		return FALSE;
	}

	if ( FAILED(hr = m_pPropertySet->QuerySupport( DSPROPSETID_ZOOMFX_BufferProperties,
														DSPROPERTY_ZOOMFXBUFFER_ALL,
														&ulSupport )) || 
		 ulSupport != PSET_SETGET )
	{
		SAFE_RELEASE( m_pPropertySet );
		return FALSE;
	}

	// Initialize properties to defaults
	if ( FAILED(hr = m_pPropertySet->Set( DSPROPSETID_ZOOMFX_BufferProperties,
												DSPROPERTY_ZOOMFXBUFFER_ALL,
												0, 0, (void*)&props,
												sizeof( ZoomFXProperties ) )) )
	{
		SAFE_RELEASE( m_pPropertySet );
		return FALSE;
	}
		
	SAFE_RELEASE( m_pPropertySet );

	// Card fully supports ZoomFX properties
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::Initialize()
// Desc: Initializesthe class.
//-----------------------------------------------------------------------------
HRESULT CSourceEffectZoomFX::Initialize( LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	ZoomFXProperties props;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return S_FALSE;

	m_p3DBuffer = p3DBuffer;
	
	if ( FAILED (hr = m_p3DBuffer->QueryInterface( IID_IKsPropertySet, (void**)&m_pPropertySet)) )
	{
		SAFE_RELEASE( m_pPropertySet );
		return DXTRACE_ERR( TEXT("CSourceEffectZoomFX::Initialize p3DBuffer->QueryInterface"), hr );
	}

	if ( FAILED(hr = m_pPropertySet->QuerySupport( DSPROPSETID_ZOOMFX_BufferProperties,
														DSPROPERTY_ZOOMFXBUFFER_ALL,
														&ulSupport )) || 
			ulSupport != PSET_SETGET )
	{
		SAFE_RELEASE( m_pPropertySet );
		return DXTRACE_ERR( TEXT("CSourceEffectZoomFX::Initialize m_PropertySet->QuerySupport"), hr );
	}

	// Initialize properties to defaults
	if ( FAILED(hr = m_pPropertySet->Set( DSPROPSETID_ZOOMFX_BufferProperties,
												DSPROPERTY_ZOOMFXBUFFER_ALL,
												0, 0, (void*)&props,
												sizeof( ZoomFXProperties ) )) )
	{
		SAFE_RELEASE( m_pPropertySet );
		return DXTRACE_ERR( TEXT("CSourceEffectZoomFX::Initialize m_pPropertySet->Set"), hr );
	}
			
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::SetProperties()
// Desc: Set dimensions and orientation of the ZoomFX box
//-----------------------------------------------------------------------------
HRESULT CSourceEffectZoomFX::SetProperties( const ZoomFXProperties& props )
{
	HRESULT hr;

	m_Props = props;

	if ( m_pPropertySet == NULL )
		return S_FALSE;

	// Set buffer properties
	if ( SUCCEEDED(hr = m_pPropertySet->Set( DSPROPSETID_ZOOMFX_BufferProperties,
											 DSPROPERTY_ZOOMFXBUFFER_ALL,
											 0, DS3D_IMMEDIATE, (void*)&props,
											 sizeof( ZoomFXProperties ) )) )
	{
		return S_OK;
	}
	else
	{
		return DXTRACE_ERR( TEXT("CSourceEffectZoomFX::SetProperties m_pPropertySet->Set"), hr );
	}
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::Reset()
// Desc: Reset ZoomFX box which disables ZoomFX
//-----------------------------------------------------------------------------
void CSourceEffectZoomFX::Reset( void )
{
	m_p3DBuffer = NULL;
	m_pPropertySet = NULL;

	m_Props.Reset();
	SetProperties( m_Props );
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::Destroy(()
// Desc: Clean up pointers.
//-----------------------------------------------------------------------------
void CSourceEffectZoomFX::Destroy( void )
{
	SAFE_RELEASE( m_pPropertySet );
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectZoomFX::GetProperties(()
// Desc: Return dimensions and orientation of the ZoomFX box
//-----------------------------------------------------------------------------
void CSourceEffectZoomFX::GetProperties( ZoomFXProperties& props )
{
	props = m_Props;
}