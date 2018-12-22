/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\CommonSrc
File:  SimpleVBObject8.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#ifndef __Simple_VB_Object8_H__
#define __Simple_VB_Object8_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <d3d8.h>
#include <d3dx8.h>


class SimpleObject8;



class SimpleVBObject8
{
private:
	LPDIRECT3DDEVICE8	m_pD3DDev; 
	bool				m_bIsValid;
	D3DPRIMITIVETYPE	m_PrimType;

public:

	
	IDirect3DVertexBuffer8  *	m_pVertexBuffer;
	WORD						m_wNumVerts;

	IDirect3DIndexBuffer8	*	m_pIndexBuffer;
	WORD						m_wNumInd;




	HRESULT CreateFromSimpleObject( SimpleObject8 * pObj, LPDIRECT3DDEVICE8  pD3DDev );
	HRESULT Free();

	HRESULT Draw();


	SimpleVBObject8();
	~SimpleVBObject8();
};



#endif

