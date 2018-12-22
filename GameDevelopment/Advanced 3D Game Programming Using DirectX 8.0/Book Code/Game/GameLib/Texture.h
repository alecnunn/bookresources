/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Texture.h
 *    Desc: Definition of a class that represents a image
 *          that can be used as a texture map.
 *          This code borrows heavily from the 'Compress' sample
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <ddraw.h>

#include <vector>
#include <string>

class cGraphicsLayer;

class cTexture
{
protected:

	void ReadDDSTexture( LPDIRECT3DTEXTURE8& pTexture );

	static bool m_bSupportsMipmaps;

	void BltToTextureSurface( LPDIRECT3DTEXTURE8 pTempTex );

	LPDIRECT3DTEXTURE8	m_pTexture;

	string	m_name;

	// The stage for this particular texture.
	DWORD	m_stage;	

public:
	cTexture( const char* filename, DWORD stage = 0 );
	virtual ~cTexture();
 
	LPDIRECT3DTEXTURE8 GetTexture();

};

#endif //_TEXTURE_H
