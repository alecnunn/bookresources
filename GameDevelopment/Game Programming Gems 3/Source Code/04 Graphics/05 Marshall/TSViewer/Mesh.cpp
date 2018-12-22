/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 2001 - 2002, Intel Corporation.
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

//  Mesh.cpp
//		This file contains the member functions for mesh.h

#include "Mesh.h"

// AllocatesMesh:
//			Allocates the memory for the mesh
//			bHasTexture is a flag to allocate whether the mesh has
//			texture coordinates or not.
void Mesh::AllocateMesh(bool bHasTexture)
{
	U32 size = 0;
	//  -----   Vertex and Normal size	----   Does the mesh have texture coordinates?
	size += (sizeof(vector3) * 2) + (sizeof (vector2) * bHasTexture);

   m_pVertices = (U8*) malloc(size * m_uNumVertices);
   m_pFaces = new Face[m_uNumFaces];

   // Initialize the stride
   m_stride = size;
   m_ptr = &m_pVertices[0];
}

// Initialize the mesh
Mesh::Mesh()
{
	m_pVertices = NULL;
	m_pFaces = NULL;
	m_ptr = NULL;
	
	m_uNumVertices = 0;
	m_uNumFaces = 0;
	m_uNumTextures = 0;
	m_uNumTriStrips = 0;
	m_stride = 0;
}

// Clean-up the mesh
Mesh::~Mesh()
{
	if(m_pVertices)
	{
		free(m_pVertices);
		m_pVertices = NULL;
	}

	if(m_pFaces)
	{
		delete [] m_pFaces;
		m_pFaces = NULL;
	}

	if(m_pTriStrips)
	{
		for(unsigned int i = 0; i<m_uNumTriStrips; i++)
		{
			free(m_pTriStrips[i].pIndices);
			m_pTriStrips[i].pIndices = NULL;
		}
		delete [] m_pTriStrips;
	}
}