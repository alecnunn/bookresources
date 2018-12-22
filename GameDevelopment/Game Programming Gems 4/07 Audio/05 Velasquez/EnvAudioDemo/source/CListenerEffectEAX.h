//-----------------------------------------------------------------------------
// File: CListenerEffectEAX.h
//
// Desc: 
//-----------------------------------------------------------------------------

#ifndef CLISTENEREFFECTEAX_H
#define CLISTENEREFFECTEAX_H

#include <d3dx9math.h>
#include <dsound.h>
#include <windows.h>
#include <dxerr9.h>
#include "DXUtil.h"

#include <eax.h>

// EAX Manager object
#include "eaxman.h"

//-----------------------------------------------------------------------------
// Name: class CListenerEffectEAX
// Desc: 
//-----------------------------------------------------------------------------
class CListenerEffectEAX
{
protected:
	LPKSPROPERTYSET				m_pListener;		// DirectSound property set interface
	LPDIRECTSOUND3DBUFFER		m_p3DBuffer;		// Dummy DS3D buffer 
	long						m_lEnvironment;		// current environment preset as long
	char						m_sEnvironment[64];	// current environemnt preset as string

public:
	CListenerEffectEAX();
	~CListenerEffectEAX();

	BOOL IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer );
	HRESULT Initialize( LPDIRECTSOUND3DBUFFER p3DBuffer );
	HRESULT SetEnvironment( LPEAXMANAGER pEAXManager, const char* sEnvironment );
	BOOL Update( LPEAXMANAGER pEAXManager, D3DXVECTOR3 vPos );
	void GetEnvironmentPresetString( TCHAR* sEnvironment );
};

#endif	// CLISTENEREFFECTEAX_H