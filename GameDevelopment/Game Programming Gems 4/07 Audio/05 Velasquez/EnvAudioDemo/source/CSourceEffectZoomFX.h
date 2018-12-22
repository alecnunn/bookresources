//-----------------------------------------------------------------------------
// File: CSourceEffectZoomFX.h
//
// Desc: 
//-----------------------------------------------------------------------------

#ifndef CSOURCEEFFECTZOOMFX_H
#define CSOURCEEFFECTZOOMFX_H

#include <dsound.h>
#include <windows.h>
#include <dxerr9.h>
#include "DXUtil.h"

#include "ZoomFx.h"

struct ZoomFXBox : public ZOOMFX_BOX
{
	ZoomFXBox()
	{
		Reset();
	}
	
	void Reset( void )
	{
		vMin.x = 0.0f;
		vMin.y = 0.0f;
		vMin.z = 0.0f;
	}
};

struct ZoomFXOrientation : public ZOOMFX_ORIENTATION
{
	ZoomFXOrientation()
	{
		Reset();
	}
	void Reset( void )
	{
		vFront.x = 0.0f;
		vFront.y = 0.0f;
		vFront.z = 1.0f;
		vTop.x = 0.0f;
		vTop.y = 1.0f;
		vTop.z = 0.0f;
	}
};

struct ZoomFXProperties : public ZOOMFX_BUFFERPROPERTIES
{
	ZoomFXProperties()
	{
		Reset();
	}

	void Reset( void )
	{
		box.vMin.x = 0.0f;
		box.vMin.y = 0.0f;
		box.vMin.z = 0.0f;
		box.vMax.x = 0.0f;
		box.vMax.y = 0.0f;
		box.vMax.z = 0.0f;

		orientation.vFront.x = 0.0f;
		orientation.vFront.y = 0.0f;
		orientation.vFront.z = 1.0f;
		orientation.vTop.x = 0.0f;
		orientation.vTop.y = 1.0f;
		orientation.vTop.z = 0.0f;
		
		lMacroFx = ZOOMFXBUFFER_MACROFX_DEFAULT;
	}
};

//-----------------------------------------------------------------------------
// Name: class CSourceEffectZoomFX
// Desc: 
//-----------------------------------------------------------------------------
class CSourceEffectZoomFX
{
protected:
	LPDIRECTSOUND3DBUFFER		m_p3DBuffer;
	LPKSPROPERTYSET				m_pPropertySet;
	ZoomFXProperties			m_Props;

public:
	CSourceEffectZoomFX();
	~CSourceEffectZoomFX();

	BOOL IsSupported( LPDIRECTSOUND3DBUFFER p3DBuffer );	// Returns TRUE if ZoomFX is supported
	HRESULT Initialize( LPDIRECTSOUND3DBUFFER p3DBuffer );
	HRESULT SetProperties( const ZoomFXProperties& props );
	void GetProperties( ZoomFXProperties& props );
	void Reset( void );
	void Destroy( void );
};

#endif	// CSOURCEEFFECTZOOMFX_H