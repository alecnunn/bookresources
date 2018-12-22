/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GraphicsLayer.h
 *    Desc: Declaration of a layer abstracting Direct3D
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GRAPHICSLAYER_H
#define _GRAPHICSLAYER_H

#include <list>
#include <string>
using std::string;
using std::list;

#include <d3d8.h>

#include "GameTypes.h"
#include "DxHelper.h"

class cApplication; 

class cGraphicsLayer
{

protected:

	HWND					m_hWnd;			// The handle to the window

	LPDIRECT3D8				m_pD3D;			// The IDirect3D8 interface
	LPDIRECT3DDEVICE8		m_pDevice;		// The IDirect3DDevice8 interface

	LPDIRECT3DSURFACE8		m_pBackSurf;	// Pointer to the back buffer

	RECT	m_rcScreenRect;					// The dimensions of the screen

	cGraphicsLayer( HWND hWnd );			// Constructor
	static cGraphicsLayer* m_pGlobalGLayer;	// Pointer to the main global gfx object

public:

	void DestroyAll();
	~cGraphicsLayer();
	
	/**
	 * Initialization calls.
	 */

	void InitD3DFullScreen( GUID* pGuid, int width, int height, int bpp );

	/**
	 * This function uses Direct3DX to write text to the back buffer.
	 * Its much faster than using the GDI
	 */
	void DrawTextString( int x, int y, DWORD color, const char * str );


	//==========--------------------------  Accessor functions

	// Gets a pointer to the IDirect3D8
	LPDIRECT3D8 GetD3D()
	{
		return m_pD3D;
	}

	// Gets a pointer to the device
	LPDIRECT3DDEVICE8 GetDevice()
	{
		return m_pDevice;
	}

	// Gets a pointer to the back buffer
	LPDIRECT3DSURFACE8 GetBackBuffer()
	{
		return m_pBackSurf;
	}

	// Gets the screen width
	int Width() const
	{
		return m_rcScreenRect.right;
	}

	// Gets the screen height
	int Height() const
	{
		return m_rcScreenRect.bottom;
	}

	// Presents the back buffer to the primary surface
	void Flip();

	// Gets a pointer to the main gfx object
	static cGraphicsLayer* GetGraphics()
	{
		return m_pGlobalGLayer;
	}

	// Initializes this object
	static void Create(
		HWND hWnd, // handle to the window
		short width, short height, // width and height
		GUID* pGuid ); // Device guid
};

inline cGraphicsLayer* Graphics()
{
	return cGraphicsLayer::GetGraphics();
}


#endif //_GRAPHICSLAYER_H
