#ifndef IMAGEWARPMETHOD_H
#define IMAGEWARPMETHOD_H
/****************************************************************************************\

	ImageWarpMethod.h

	Rendering interface for the texture warping method

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

class ImageWarpMethod
{
	public:

	ImageWarpMethod();
	~ImageWarpMethod();

    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
	HRESULT Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe);

	private:
    LPDIRECT3DDEVICE8       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DVERTEXBUFFER8 m_pVBuffer;
	LPDIRECT3DTEXTURE8		m_pTexture;
	LPDIRECT3DTEXTURE8		m_pTextureBuffer;
	LPDIRECT3DTEXTURE8		m_pSourceImage;
	LPDIRECT3DTEXTURE8		m_pDepthImage;


	void WarpTexture(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix);

	void WarpRight(D3DCOLOR* pSrc, 
					D3DCOLOR* pDest, 
					float z_scale,
					float front_ratio,
					float front_step,
					float back_ratio,
					float back_step,
					int count);

	void WarpLeft(D3DCOLOR* pSrc, 
					D3DCOLOR* pDest, 
					float z_scale,
					float front_ratio,
					float front_step,
					float back_ratio,
					float back_step,
					int count);
};

inline ImageWarpMethod::ImageWarpMethod()
:m_pd3dDevice(0)
,m_pVBuffer(0)
,m_pTexture(0)
,m_pTextureBuffer(0)
,m_pSourceImage(0)
,m_pDepthImage(0)
{
}

inline ImageWarpMethod::~ImageWarpMethod()
{
}



#endif