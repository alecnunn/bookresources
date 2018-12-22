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
//	GPGFace
//
//	DESCRIPTION
//    Interface for accessing the vertex indices of a face in a mesh.
//    By default, vertex indices are stored using the 32-bit index 
//    data type (INDEX) for OpenGL rendering.  If you plan on using D3d
//    or want to use less memory for models, define GPG_U16_INDEX on 
//    the compiler command line to change INDEX to a 16-bit index.
//    

#ifndef GPGFace_h
#define GPGFace_h

#include "GPGDataTypes.h"


// Define GPG_U16_INDEX on compiler cmd line to 
// change INDEX to a 16-bit index (useful for d3d)
#ifdef GPG_U16_INDEX
typedef U16	INDEX;	// index into vertex array.
#else
typedef U32	INDEX;	// index into vertex array.
#endif


class GPGFace
{
public:
	// Set vertex indices for corners a, b, c
	void Set(INDEX a, INDEX b, INDEX c);

	// Get or set vertex a
	INDEX& VertexA();

	// Get or set vertex b
	INDEX& VertexB();

	// Get or set vertex c
	INDEX& VertexC();

	// Get or set values using index in the range 0..2
	INDEX& Vertex(INDEX index);

private:
	INDEX m_value[3];	// array of vertex indices
};


//
// Inlines
//

inline void GPGFace::Set(INDEX a, INDEX b, INDEX c) { m_value[0] = a; m_value[1] = b; m_value[2] = c; }
inline INDEX& GPGFace::VertexA() { return m_value[0]; }
inline INDEX& GPGFace::VertexB() { return m_value[1]; }
inline INDEX& GPGFace::VertexC() { return m_value[2]; }
inline INDEX& GPGFace::Vertex(INDEX index) { GPGASSERT(index < 3); return m_value[index]; }

#endif
