#ifndef DUALMULTIPLANEMETHOD_H
#define DUALMULTIPLANEMETHOD_H
/****************************************************************************************\

	DualMultiPlaneMethod.h

	Rendering interface for the elevation map method

	Part of the Gems III sample code library.
	Created 12/27/2001 by Greg Snook

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <D3D8.h>
#include <D3Dx8.h>
#include "dxutil.h"

// this class defines an interface for the rendering methods we will use.

class DualMultiPlaneMethod
{
	public:

	DualMultiPlaneMethod();
	~DualMultiPlaneMethod();

    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
	HRESULT Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe);

	private:
    LPDIRECT3DDEVICE8       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DVERTEXBUFFER8 m_pVBufferA;
    LPDIRECT3DVERTEXBUFFER8 m_pVBufferB;
    LPDIRECT3DINDEXBUFFER8	m_pIBuffer;
	LPDIRECT3DTEXTURE8		m_pTextureA;
	LPDIRECT3DTEXTURE8		m_pTextureB;
    DWORD                   m_hPixelShader;
    DWORD					m_hVertexShader;
};

inline DualMultiPlaneMethod::DualMultiPlaneMethod()
:m_pd3dDevice(0)
,m_pVBufferA(0)
,m_pVBufferB(0)
,m_pIBuffer(0)
,m_pTextureA(0)
,m_pTextureB(0)
,m_hPixelShader(0)
,m_hVertexShader(0)
{
}

inline DualMultiPlaneMethod::~DualMultiPlaneMethod()
{
}



#endif