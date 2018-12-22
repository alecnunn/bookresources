/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Cartoon Rendering: Real-time Silhouette Edge Detection and Rendering sample code

Copyright (c) 2001, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

/***************************************************************************************
File:  
			Mesh.h

 Purpose:
		   This file contains the data structures for creating, storing, and
		   rendering a mesh in opengl.
/***************************************************************************************/
#ifndef MESH_H
#define MESH_H

#include <stdlib.h>
#include "mtxlib.h"
#include "Face.h"

typedef float						F32;
typedef unsigned int		U32;
typedef unsigned short	U16;
typedef unsigned char	U8;

class Mesh
{
private:
	U32 m_uNumVertices;
	U32 m_uNumFaces;
	U32 m_uNumTextures;
	U32	m_stride;

	Face *m_pFaces;
	U8		*m_pVertices;
    U8*		 m_ptr;
public:
	Mesh();
	~Mesh();
	
	void AllocateMesh( bool bHasTexture);

	// Get Functions
	U32 GetNumFaces();
	U32 GetNumVertices();
	U32 GetNumTextures();

	vector3* GetVertexPosition(U32 uIndex);
	vector3* GetVertexNormal(U32 uIndex);
	vector2* GetVertexTexCoord(U32 uIndex);

	Face*GetFacePtr(U32 uIndex);
	Face*GetFacePtr();
	U8*		GetVertexPtr();

	// Set Functions
	void SetNumFaces(U32 uNumFaces);
	void SetNumVertices(U32 uNumVertices);
	void SetNumTextures(U32 uNumTextures);
};


// INLINE Functions

// Get Functions
inline U8* Mesh::GetVertexPtr()
{
	return m_pVertices;
}

inline U32 Mesh::GetNumFaces()
{
	return 	m_uNumFaces;
}

inline U32 Mesh::GetNumVertices()
{
	return m_uNumVertices;
}

inline U32 Mesh::GetNumTextures()
{
	return m_uNumTextures;
}

inline vector3* Mesh::GetVertexPosition(U32 uIndex)
{
	return (vector3*)(m_ptr + uIndex * m_stride);
}

inline vector3* Mesh::GetVertexNormal(U32 uIndex)
{
	return (vector3*)(m_ptr + uIndex * m_stride + sizeof(vector3));
}

inline vector2* Mesh::GetVertexTexCoord(U32 uIndex)
{
	return (vector2*)(m_ptr + uIndex * m_stride + sizeof(vector3)*2);
}

inline Face* Mesh::GetFacePtr(U32 uIndex)
{
	return &m_pFaces[uIndex];
}

inline Face* Mesh::GetFacePtr()
{
	return m_pFaces;
}

// Set Functions
inline void Mesh::SetNumFaces(U32 uNumFaces)
{
	m_uNumFaces = uNumFaces;
}

inline void Mesh::SetNumVertices(U32 uNumVertices)
{
	m_uNumVertices = uNumVertices;
}

inline void Mesh::SetNumTextures(U32 uNumTextures)
{
	m_uNumTextures  = uNumTextures;
}

#endif  // MESH_H