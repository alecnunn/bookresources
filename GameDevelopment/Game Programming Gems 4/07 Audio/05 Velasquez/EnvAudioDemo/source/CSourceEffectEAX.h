//-----------------------------------------------------------------------------
// File: CSourceEffectEAX.h
//
// Desc: 
//-----------------------------------------------------------------------------

#ifndef CSOURCEEFFECTEAX_H
#define CSOURCEEFFECTEAX_H

#include <d3dx9math.h>
#include <dxerr9.h>
#include <dsound.h>
#include <eax.h>

// EAX Manager object
#include "eaxman.h"

#include "DXUtil.h"

//-----------------------------------------------------------------------------
// Name: class CSourceEffectEAX
// Desc: 
//-----------------------------------------------------------------------------
class CSourceEffectEAX
{
protected:
	LPKSPROPERTYSET				m_pEAXSource;		// DirectSound property set interface
	LPDIRECTSOUND3DBUFFER		m_p3DBuffer;		// DS3D buffer that this effect is associated with
	BOOL						m_bInitializedProperties;

public:
	CSourceEffectEAX();
	~CSourceEffectEAX();

	BOOL IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer );
	HRESULT Initialize( LPDIRECTSOUND3DBUFFER p3DBuffer );
	HRESULT Update(	LPEAXMANAGER pEAXManager, D3DXVECTOR3 vPos, char* szPresetName = NULL );
};

#endif	// CSOURCEEFFECTEAX_H