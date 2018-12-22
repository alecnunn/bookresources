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
//	GPGMaterial.h
//
//	DESCRIPTION
//	   Interface definition for class GPGMaterial.  This module 
//	manages state for surface materials, including standard material properties
//	such as ambient, diffuse, specular, as well as layered textures.
//	
//	NOTES
//	For textures, GPGMaterial manages only an array of pointers and a count of 
//	active layers -- it doesn't worry about the mode of application of the layers.
//	Each texture must be initialized properly to blend or modulate the layers
//	below it.  Most common usage for textures should be through the AddTexture() /
//	RemoveTexture() / GetTexture() interface.
//
//	Note that the textures in the array are owned elsewhere, they are merely 
//  assigned as pointers to become "layers".  Therefore, this class performs no 
//	allocation / deallocation of texture objects.

#ifndef GPGMATERIAL_H
#define GPGMATERIAL_H

#include <memory.h>
#include <string.h>
#include "GPGResult.h"
#include "GPGDataTypes.h"
#include "GPGVector4.h"
//#include "CGPGStaticStrFunc.h"

#define MAX_TEXTURE_LAYERS 8

class GPGTexture;

class GPGMaterial
{
	
public:
	GPGMaterial();
	~GPGMaterial();

	void    SetAmbient(GPGVector4 *color) { memcpy(&ambient, color, sizeof(GPGVector4)); }
	GPGVector4* GetAmbient() { return &ambient; }

	void    SetDiffuse(GPGVector4* color) { memcpy(&diffuse, color, sizeof(GPGVector4)); }
	GPGVector4* GetDiffuse() { return &diffuse; }

	void    SetSpecular(GPGVector4* color) { memcpy(&specular, color, sizeof(GPGVector4)); }
	GPGVector4* GetSpecular() { return &specular; }


	void    SetShininess(F32 shiny) { shininess = shiny; }
	F32     GetShininess() { return shininess;}

	void	SetNumTextureLayers(U8 numLayers) { numTextureLayers = numLayers; }
	U8		GetNumTextureLayers() { return numTextureLayers; }
	
	U32		SetTexture(GPGTexture *pTexture, U32 layer = 0);
	GPGTexture*	GetTexture(U32 layer = 0);

	U32		AddTexture(GPGTexture *pTexture);

	GPGRESULT RemoveTexture(U32 layer = 0);

	GPGRESULT SetName(char *name);
	char* GetName() { return m_name; }

	GPGRESULT SetTextureLayerActive(int layer);
	GPGRESULT SetTextureLayerInactive(int layer);
	bool GetTextureLayerStatus(int layer) {return texLayerEnabled[layer];}
	GPGRESULT SetAllTextureLayersActive();
	GPGRESULT SetAllTextureLayersInactive();

private:
	GPGVector4 ambient;
	GPGVector4 specular;
	GPGVector4 diffuse;
	F32 shininess;
	GPGTexture* textures[MAX_TEXTURE_LAYERS];
	bool texLayerEnabled[MAX_TEXTURE_LAYERS];

	U8 numTextureLayers;
	char *m_name;
};


#endif
