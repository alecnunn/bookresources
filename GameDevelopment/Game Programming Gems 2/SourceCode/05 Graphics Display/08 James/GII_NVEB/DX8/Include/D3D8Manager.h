/*********************************************************************NVMH2****
Path:  D:\Dev\devrel\Nv_sdk_4\Dx8\Include
File:  D3D8Manager.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

A simple class to handle Initialization and cleanup of D3D for DX8.
This class is not meant to have any higher scene or rendering functionality.
Please implement that somewhere else!


******************************************************************************/



#if !defined(AFX_D3D8MANAGER_H__D28391E4_9AD0_11D4_A995_00A0D218309C__INCLUDED_)
#define AFX_D3D8MANAGER_H__D28391E4_9AD0_11D4_A995_00A0D218309C__INCLUDED_


#include <d3d8.h>


class D3D8Manager  
{
public:

	LPDIRECT3D8             m_pD3D;			// Used to create D3DDevice
	LPDIRECT3DDEVICE8       m_pd3dDevice;	// The rendering device


	HRESULT InitD3D( HWND hWnd );
	HRESULT InitD3D_DestAlpha( HWND hWnd, BOOL windowed );		// init with 8 bits alpha

	void UnInitD3D();


	LPDIRECT3DDEVICE8 GetDevice()	{ return( m_pd3dDevice ); }


	D3D8Manager();
	virtual ~D3D8Manager();

};

#endif // !defined(AFX_D3D8MANAGER_H__D28391E4_9AD0_11D4_A995_00A0D218309C__INCLUDED_)
