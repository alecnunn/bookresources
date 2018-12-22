//-----------------------------------------------------------------------------
// File: CSourceEffectEAX.cpp
//
// Desc: 
//-----------------------------------------------------------------------------

#include <assert.h>

#include "CSourceEffectEAX.h"

#define PSET_SETGET	(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)

//-----------------------------------------------------------------------------
// Name: CSourceEffectEAX::CSourceEffectEAX()
// Desc: Constructs the class.
//-----------------------------------------------------------------------------
CSourceEffectEAX::CSourceEffectEAX()
{
	m_pEAXSource = NULL;
	m_p3DBuffer = NULL;
	m_bInitializedProperties = FALSE;
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectEAX::~CSourceEffectEAX()
// Desc: Destroys the class.
//-----------------------------------------------------------------------------
CSourceEffectEAX::~CSourceEffectEAX()
{
	// Release EAX source interface
	SAFE_RELEASE( m_pEAXSource );
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectEAX::IsSupported()
// Desc: Returns true if sound card supports EAX source properties
//-----------------------------------------------------------------------------
BOOL CSourceEffectEAX::IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return FALSE;

	// Create EAX listener interface
	if ( FAILED( hr = p3DBuffer->QueryInterface( IID_IKsPropertySet, (void **)&m_pEAXSource )) )
	{
		SAFE_RELEASE( m_pEAXSource );
		return FALSE;
	}

	// Check for EAX listener support
	if ( FAILED( hr = m_pEAXSource->QuerySupport( DSPROPSETID_EAX20_BufferProperties, DSPROPERTY_EAXBUFFER_ALLPARAMETERS, &ulSupport) ) || 
		 (ulSupport & PSET_SETGET) != PSET_SETGET )
	{
		SAFE_RELEASE( m_pEAXSource );
		return FALSE;
	}

	SAFE_RELEASE( m_pEAXSource );

	// Card fully supports EAX source properties
	return TRUE;	
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectEAX::Initialize()
// Desc: Initializesthe class.
//-----------------------------------------------------------------------------
HRESULT CSourceEffectEAX::Initialize( LPDIRECTSOUND3DBUFFER p3DBuffer )
{
	HRESULT hr;
	unsigned long ulSupport = 0;
	
	assert( p3DBuffer != NULL );

	// Check to make sure we have a buffer we can use
	if ( p3DBuffer == NULL )
		return S_FALSE;

	m_p3DBuffer = p3DBuffer;

	// Create EAX source interface
	if ( FAILED( hr = m_p3DBuffer->QueryInterface( IID_IKsPropertySet, (void **)&m_pEAXSource )) )
	{
		return DXTRACE_ERR( TEXT("CSourceEffectEAX::Initialize pDS->QueryInterface"), hr );
	}

	// Check for EAX source support
	if ( FAILED( hr = m_pEAXSource->QuerySupport( DSPROPSETID_EAX20_BufferProperties, DSPROPERTY_EAXBUFFER_ALLPARAMETERS, &ulSupport) ) || 
		 (ulSupport & PSET_SETGET) != PSET_SETGET )
	{
		return DXTRACE_ERR( TEXT("CSourceEffectEAX::Initialize pDS->QuerySupport"), hr );
	}
	
	// Card fully supports EAX listener properties
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSourceEffectEAX::Update()
// Desc: When the listener moves/rotates, ask EAXManager for our updated source properties
//-----------------------------------------------------------------------------
HRESULT CSourceEffectEAX::Update( LPEAXMANAGER pEAXManager, D3DXVECTOR3 vPos, char* szPresetName )
{
	BOOL bCommit = FALSE;
	HRESULT hr;
	if ( pEAXManager != NULL && m_p3DBuffer != NULL )
	{
		// If you add your sounds in EAGLE, you can reference their preset name here and get their initial 
		// EAX properties.  I chose not to add the sounds in EAGLE, but here is the code if you decide to 
		// yourself.
		/*if ( m_bInitializedProperties == FALSE && szPresetName )
		{
			// Set up the source properties once by requesting them from EAX Manager
			long lPresetID;
			hr = pEAXManager->GetSourceID(szPresetName, &lPresetID);
			if ( hr == EM_OK )
			{
				// Next get the actual preset data.
				SOURCEATTRIBUTES sa;
				if ( pEAXManager->GetSourceAttributes(lPresetID, &sa) == EM_OK )
				{
					m_pEAXSource->Set(DSPROPSETID_EAX_BufferProperties, 
										DSPROPERTY_EAXBUFFER_ALLPARAMETERS, 
										NULL,
										0, 
										&sa.eaxAttributes, 
										sizeof(EAXBUFFERPROPERTIES));
					bCommit = TRUE;
				}
			}

			// Even if we weren't successful, only try once
			m_bInitializedProperties = TRUE;
		}*/

		
		// An EMPOINT struct is initialized with the sounds position. If the coordinate system
		// being used differed, here is where you would want to convert the data (e.g. swap Y and
		// Z if needed).
		EMPOINT ep, epv;
		ep.fX = vPos.x; 
		ep.fY = vPos.y; 
		ep.fZ = -vPos.z;	// This is done here because I've rotated the level by -90 degrees on the X axis
								// and EAX Manager has no idea about this.
		 
		long obstruction,occlusion;
		float obstructionLFRatio,occlusionLFRatio,occlusionRoomRatio;
		hr = pEAXManager->GetSourceDynamicAttributes(0,&ep,
								&obstruction,&obstructionLFRatio,
								&occlusion,&occlusionLFRatio,&occlusionRoomRatio,
								&epv,0); 
		if ( hr == EM_OK )
		{
			m_pEAXSource->Set(DSPROPSETID_EAX20_BufferProperties,DSPROPERTY_EAXBUFFER_OBSTRUCTION,
						NULL,DS3D_DEFERRED,&obstruction,sizeof(long));
			m_pEAXSource->Set(DSPROPSETID_EAX20_BufferProperties,DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO,
						NULL,DS3D_DEFERRED,&obstructionLFRatio,sizeof(float));
			m_pEAXSource->Set(DSPROPSETID_EAX20_BufferProperties,DSPROPERTY_EAXBUFFER_OCCLUSION,
						NULL,DS3D_DEFERRED,&occlusion,sizeof(long));
			m_pEAXSource->Set(DSPROPSETID_EAX20_BufferProperties,DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO, 
						NULL,DS3D_DEFERRED,&occlusionLFRatio,sizeof(float));
			m_pEAXSource->Set(DSPROPSETID_EAX20_BufferProperties, DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO,
						NULL,DS3D_DEFERRED,&occlusionRoomRatio,sizeof(float));

			bCommit = TRUE;
		}
	}

	return bCommit;
}