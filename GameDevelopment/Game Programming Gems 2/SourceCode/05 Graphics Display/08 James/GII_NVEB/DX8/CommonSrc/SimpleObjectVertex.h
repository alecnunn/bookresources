/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\CommonSrc
File:  SimpleObjectVertex.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef __SimpleObjectVertex_H__
#define __SimpleObjectVertex_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <d3d8.h>
#include <d3dx8.h>


//////////////////////////////////////////////////////////

typedef struct SimpleObjVertex
{
	D3DXVECTOR3		pos;
	D3DXVECTOR3		nrm;

    D3DCOLOR		diffuse;
    D3DXVECTOR2		t0;
    
} SimpleObjVertex; 



#define SOBJVERT_FVF  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL |	\
						D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)  )	
							




#endif   // ndef __SimpleObjectVertex_H__

