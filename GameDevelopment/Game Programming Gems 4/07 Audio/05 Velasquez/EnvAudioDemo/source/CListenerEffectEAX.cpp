//-----------------------------------------------------------------------------
// File: CListenerEffectEAX.cpp
//
// Desc: 
//-----------------------------------------------------------------------------

#define STRICT

#include <assert.h>
#include <windows.h>
#include <dxerr9.h>
#include <eax-util.h>
#include <stdio.h>

#include "CListenerEffectEAX.h"
#include "math.h"


#define PSET_SETGET	(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::CListenerEffectEAX()
// Desc: Constructs the class.
//-----------------------------------------------------------------------------
CListenerEffectEAX::CListenerEffectEAX(
	void )
{
	m_pListener = NULL;
	m_p3DBuffer = NULL;
	m_lEnvironment = EMFLAG_IDNONE;
}

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::~CListenerEffectEAX()
// Desc: Destroys the class.
//-----------------------------------------------------------------------------
CListenerEffectEAX::~CListenerEffectEAX(
	void )
{
	// Release EAX listener interface
	SAFE_RELEASE( m_pListener );
}

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::IsSupported()
// Desc: Returns true if sound card supports EAX listener properties
//-----------------------------------------------------------------------------
BOOL CListenerEffectEAX::IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return FALSE;

	// Create EAX listener interface
	if ( FAILED( hr = p3DBuffer->QueryInterface( IID_IKsPropertySet, (void **)&m_pListener )) )
	{
		SAFE_RELEASE( m_pListener );
		return FALSE;
	}

	// Check for EAX listener support
	if ( FAILED( hr = m_pListener->QuerySupport( DSPROPSETID_EAX20_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ulSupport) ) || 
		 (ulSupport & PSET_SETGET) != PSET_SETGET )
	{
		SAFE_RELEASE( m_pListener );
		return FALSE;
	}

	SAFE_RELEASE( m_pListener );

	// Card fully supports EAX listener properties
	return TRUE;	
}

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::Initialize()
// Desc: Initializesthe class.
//-----------------------------------------------------------------------------
HRESULT CListenerEffectEAX::Initialize(
	LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return S_FALSE;

	m_p3DBuffer = p3DBuffer;

	// Create EAX listener interface
	if ( FAILED( hr = m_p3DBuffer->QueryInterface( IID_IKsPropertySet, (void **)&m_pListener )) )
	{
		return DXTRACE_ERR( TEXT("CListenerEffec^tEAX::Initialize pDS->QueryInterface"), hr );
	}

	// Check for EAX listener support
	if ( FAILED( hr = m_pListener->QuerySupport( DSPROPSETID_EAX20_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ulSupport) ) || 
		 (ulSupport & PSET_SETGET) != PSET_SETGET )
	{
		return DXTRACE_ERR( TEXT("CListenerEffectEAX::Initialize pDS->QuerySupport"), hr );
	}

	// Card fully supports EAX listener properties
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::SetEnvironment()
// Desc: Sets the listener's environment to pre-defined preset created
//		 in Creative's EAGLE application.
//-----------------------------------------------------------------------------
HRESULT CListenerEffectEAX::SetEnvironment(
	LPEAXMANAGER pEAXManager,
	const char* sEnvironment )
{
	assert( pEAXManager != NULL );
	assert( m_pListener != NULL );

	if ( pEAXManager != NULL && m_pListener != NULL)
	{
	
		// Retrieve the preset ID number.
		if ( pEAXManager->GetEnvironmentID((char*)sEnvironment, &m_lEnvironment) != EM_OK )
			return S_FALSE;
	  
		// Next get the actual preset data.
		EAXLISTENERPROPERTIES eaxlp;
		if ( pEAXManager->GetEnvironmentAttributes(m_lEnvironment, &eaxlp) != EM_OK )
			return S_FALSE;

		m_pListener->Set( DSPROPSETID_EAX20_ListenerProperties, 
							DSPROPERTY_EAXLISTENER_ALLPARAMETERS, 
							NULL,
							0, 
							&eaxlp, 
							sizeof(EAXLISTENERPROPERTIES));
		return S_OK;
	}

	return S_FALSE;
}

#define X_ROTATION_NEGATIVE_90_DEGREES	(-(PI/2))

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::GetEnvironment()
// Desc: Sets the listener's environment to next preset
//		 or wraps around to beginning of preset list.
//-----------------------------------------------------------------------------
BOOL CListenerEffectEAX::Update(
	LPEAXMANAGER pEAXManager,
	D3DXVECTOR3 vPos )
{
	assert( pEAXManager != NULL );
	assert( m_pListener != NULL );
	
	if ( pEAXManager != NULL && m_pListener != NULL )
	{
		// An EMPOINT struct is initialized with the listeners
		// position. If the coordinate system being used differed, 
		// here is where you would want to convert the data 
		// (e.g. swap Y and Z if needed).
		EMPOINT ep;
		ep.fX = vPos.x; 
		ep.fY = vPos.y; 
		ep.fZ = -vPos.z;	// This is done here because I've rotated the level by -90 degrees on the X axis
							// and the EAX Manager has no idea that I've done this.

		long lNewEnvironment;
		HRESULT hr = pEAXManager->GetListenerDynamicAttributes(0, &ep, &lNewEnvironment, EMFLAG_LOCKPOSITION);
		if ( hr != EM_OK )
		{
			// If this fails, then most likely you didn't load the .eal file for your level.
			return FALSE;
		}

		// Check if this is a new environment.
		if ( lNewEnvironment == m_lEnvironment )
			return FALSE;
		else
			m_lEnvironment = lNewEnvironment;

		// Next get the actual preset data.
		EAXLISTENERPROPERTIES eaxlp;

		if ( pEAXManager->GetEnvironmentAttributes(lNewEnvironment, &eaxlp) == EM_OK )
		{
			hr = m_pListener->Set(DSPROPSETID_EAX20_ListenerProperties, 
						  		  DSPROPERTY_EAXLISTENER_ALLPARAMETERS, 
								  NULL,0,&eaxlp,sizeof(EAXLISTENERPROPERTIES));
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: CListenerEffectEAX::GetEnvironmentPresetString()
// Desc: Gets the listener's environment as a string for output
//-----------------------------------------------------------------------------
void CListenerEffectEAX::GetEnvironmentPresetString(
	TCHAR* sEnvironment)
{
	assert ( sEnvironment != NULL );
	TCHAR temp[4];

	ZeroMemory(&temp, sizeof(TCHAR)*4);

	switch ( m_lEnvironment )
	{
		case -1:
			lstrcpy(sEnvironment, "Cave");
			break;	
		case 0:
			lstrcpy(sEnvironment, "Underwater");
			break;
		case 1:
			lstrcpy(sEnvironment, "Hallway");
			break;
		case 2:
			lstrcpy(sEnvironment, "Stone Room");
			break;
		case 3:
			lstrcpy(sEnvironment, "Room");
			break;
		case 4:
			lstrcpy(sEnvironment, "Oustide World");
			break;
		default:
			lstrcpy(sEnvironment, "None");
			break;
	}
	
}




























