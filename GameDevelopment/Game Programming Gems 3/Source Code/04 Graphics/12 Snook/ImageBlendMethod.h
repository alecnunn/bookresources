#ifndef IMAGEBLENDMETHOD_H
#define IMAGEBLENDMETHOD_H
/****************************************************************************************\

	ImageBlendMethod.h

	Rendering interface for the vertically interlaced texture method

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

class ImageBlendMethod
{
	public:

	ImageBlendMethod();
	~ImageBlendMethod();

    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
	HRESULT Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe);

	private:
    LPDIRECT3DDEVICE8       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DVERTEXBUFFER8 m_pVBuffer;
	LPDIRECT3DTEXTURE8		m_pTexture;
	LPDIRECT3DTEXTURE8		m_pOffsetTexture;

    DWORD                   m_hPixelShader;
    DWORD					m_hVertexShader;

	float					m_pixelStep;

};

inline ImageBlendMethod::ImageBlendMethod()
:m_pd3dDevice(0)
,m_pVBuffer(0)
,m_pTexture(0)
,m_pOffsetTexture(0)
,m_hPixelShader(0)
,m_hVertexShader(0)
{
}

inline ImageBlendMethod::~ImageBlendMethod()
{
}



#endif