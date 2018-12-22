#ifndef GPGTEXTURE_H_
#define GPGTEXTURE_H_
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
//
//
//	GPGTexture.h
//
//	DESCRIPTION
//	   Interface definition for class GPGTexture.  This module 
//	manages state for texture maps, including data, resolution, wrap mode, 
//  texture rendering function (i.e. decal, modulate, blend, etc.), filter
//  modes.  
//
//	NOTES
//  This is currently a very vanilla texture implementation.  To be complete,
//	it needs some work.  For example, it handles only RGB, no alpha, and only
//  three channel images with unsigned char data format.  It should be extended 
//  to handle single, dual, and quad channel (8,16,32 bit) and RGBA and ARGB images 
//  (or arbitrary byte ordering).  It also assumes byte alignment.
//
//	EXAMPLE USAGE: (texture data assumed to be in unsigned char array pTexture)
//	
//	GPGTexture *Texture = new GPGTexture(pTexture, resolutionU, resolutionV);
//	// Set all necessary parameters before loading the texture
//	Texture->SetTextureFunction(GPGTexture::DECAL);
//	Texture->SetTextureFilter(GPGTexture::MIN, GPGTexture::NEAREST);
//	Texture->SetTextureFilter(GPGTexture::MAG, GPGTexture::NEAREST);
//  
//	// To load the texture into tex memory, setting all the associated API parameters,
//	// use the rendermethod (GPGRenderOGL, in this case)
//	if (rendermethod->LoadTexture() == GPG_ERROR)
//	// print or return error code
//	else
//	// everythings groovy, proceed to render with the texture...
//
//
//	...
//
//	Note that the above is not necessary.  The rendermethod's draw method checks for
//	textures in each material and automatically binds it as needed, loading it if it
//	isn't already in texture RAM.
//
//	TODO: Add support for Mipmaps
//
#include "GPGDataTypes.h"
#include "GPGResult.h"

#define MAX_TEXTURE_PATH_LENGTH 255

class GPGTexture
{
public:

	GPGTexture();
	GPGTexture(	U8 *pTextureData,
				U32 resolutionU,
				U32 resolutionV,
				U32 numChannels = 3);
	~GPGTexture();

	void SetTextureData(	U8 *pTextureData,
							U32 resolutionU,
							U32 resolutionV,
							U32 numChannels = 3);
	GPGRESULT GetTextureData(U8 **pTextureData,
							U32 *pResolutionU,
							U32 *pResolutionV,
							U32 *pNumChannels = NULL) const ;

	void SetResolution(U32 resolutionU, U32 resolutionV) 
	{ m_resolutionU = resolutionU; m_resolutionV = resolutionV; }
	
	void GetResolution(U32 *pResolutionU, U32 *pResolutionV) const
	{ *pResolutionU = m_resolutionU; *pResolutionV = m_resolutionV; }

	// returns whether this is a 2D or 1D texture 
	// important to check before enabling a specific type 
	U8 GetNumberOfDimensions() { return (m_resolutionV == 1) ? 1 : 2; }

	// Graphics API dependent functions

	enum GPGTextureDimension	{ TEXTURE_S, TEXTURE_T };
	enum GPGTextureWrapMode		{ CLAMP, REPEAT };
	enum GPGTextureFunction		{ MODULATE, DECAL, BLEND, REPLACE };
	enum GPGTextureFilterMINMAG { MIN, MAG };
	enum GPGTextureFilterMode	{ NEAREST, LINEAR, LINEAR_MIPMAP_LINEAR };


	// Set the clamp mode to one of CLAMP or REPEAT
	GPGRESULT SetTextureWrapMode(GPGTextureDimension dim, GPGTextureWrapMode mode);
	GPGRESULT GetTextureWrapMode(GPGTextureDimension dim, U32 *mode);

	// Set texturing function (i.e. blend, modulate, decal, replace)
	GPGRESULT SetTextureFunction(GPGTextureFunction function) 
	{ m_textureFunction = function; return GPG_OK; }
	GPGRESULT GetTextureFunction(U32 *function)
	{ *function = (U32) m_textureFunction; return GPG_OK; }

	// Set texture min or mag filter.  First arg is one of MIN or MAG, second is
	// one of NEAREST or LINEAR
	GPGRESULT SetTextureFilter(	GPGTextureFilterMINMAG minOrMag, 
								GPGTextureFilterMode mode);
	GPGRESULT GetTextureFilter( GPGTextureFilterMINMAG minOrMag, U32 *mode);

	void SetTextureID(U32 textureID) { m_textureID = textureID; };
	U32  GetTextureID() const { return m_textureID; }

	GPGRESULT SetPath(U8 *path);
	U8* GetPath() { return m_path; }
private:
	
	U8 *m_pTextureData;
	U8 *m_path;

	// U and V resolution
	U32 m_resolutionU;
	U32 m_resolutionV;
	U32 m_numChannels;

	// ID if this texture is resident in texture RAM
	U32 m_textureID;

	GPGTextureFunction		m_textureFunction;
	GPGTextureFilterMode	m_magFilterMode;
	GPGTextureFilterMode	m_minFilterMode;
	GPGTextureWrapMode		m_wrapModeS;
	GPGTextureWrapMode		m_wrapModeT;
};

#endif
