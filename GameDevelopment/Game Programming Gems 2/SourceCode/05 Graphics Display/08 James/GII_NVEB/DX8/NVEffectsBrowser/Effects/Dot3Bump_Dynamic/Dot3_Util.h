/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\BlinnBumpDynamicMap
File:  Dot3_Util.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
	Essential utility stuff for the dot3 shader examples
	Dot3Vertex - a 'fat' vertex with the S,T and SxT components in it
	    
      
        
******************************************************************************/

#ifndef __NVEB_DOT3_UTIL_H
#define __NVEB_DOT3_UTIL_H



// A special vertex with basis vectors included
class Dot3Vertex
{
public:
	Dot3Vertex(const D3DXVECTOR3& _Position, const D3DXVECTOR3& _Normal, const D3DXVECTOR2& _Texture)
		: Position(_Position), 
			Normal(_Normal),
			Texture(_Texture)
	{};

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Diffuse;
	D3DXVECTOR2 Texture;
	D3DXVECTOR3 S;
	D3DXVECTOR3 T;
    D3DXVECTOR3 SxT;
};



// Creates basis vectors for index triangle lists.
HRESULT CreateBasisVectors(LPDIRECT3DVERTEXBUFFER8 pVertexBuffer, LPDIRECT3DINDEXBUFFER8 pIndexBuffer);
HRESULT FindAndFixDegenerateVertexBasis(	Dot3Vertex	* pVertices,
											DWORD		  dwNumVertices,
											WORD		* pIndices,
											DWORD		  dwNumIndices,
											bool bUnifyNormals			);

HRESULT CreateQ8W8V8U8FromHeight( LPDIRECT3DDEVICE8 pD3DDev, LPDIRECT3DTEXTURE8 pSource,
								  LPDIRECT3DTEXTURE8* pTextureReturn);
HRESULT CreateNVHSFromHeight( LPDIRECT3DDEVICE8 pD3DDev, LPDIRECT3DTEXTURE8 pSource,
							  LPDIRECT3DTEXTURE8* pTextureReturn);




#endif //__NVEB_DOT3_UTIL_H

