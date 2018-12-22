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

#ifndef EDGE_H
#define EDGE_H

#define EMPTY_FACE_SLOT  9999999
#define BOUNDARY_EDGE			0x00000001
#define CREASE_ANGLE_EDGE 0x00000010

typedef unsigned int	U32;
typedef unsigned short  U16;
typedef unsigned char   U8;

class Edge
{
private:
	// Vertex indices
	U32 m_uVertexIndex[2];	
	U32 m_uFaceIndex[2];
	U32 m_uFlag;

public:	
	void SetVertexIndex(int corner, U32 uIndex);
	void SetVertexIndices(U32 a, U32 b);

	void SetFaceIndex(int corner, U32 uIndex);
	void SetFaceIndices(U32 a, U32 b);

	void SetEdgeFlag(U32 uFlag);

	U32 GetVertexIndex(int corner);
	U32 GetFaceIndex(int corner);
	U32 GetEdgeFlag();

	// Used for Hash Table
	bool bOpen;
	U32 uNumFaceEntries;

	Edge *next;
};
inline void Edge::SetEdgeFlag(U32 uFlag)
{
	m_uFlag = uFlag;
}

inline void Edge::SetVertexIndex(int corner, U32 uIndex)
{
	m_uVertexIndex[corner] = uIndex;
}

inline void Edge::SetFaceIndex(int corner, U32 uIndex)
{
	m_uFaceIndex[corner] = uIndex;
}

inline void Edge::SetVertexIndices(U32 a, U32 b)
{
	m_uVertexIndex[0] = a;
	m_uVertexIndex[1] = b;
}

inline void Edge::SetFaceIndices(U32 a, U32 b)
{
	m_uFaceIndex[0] = a;
	m_uFaceIndex[1] = b;
}

inline U32 Edge::GetEdgeFlag()
{
	return m_uFlag;
}

inline U32 Edge::GetVertexIndex(int corner)
{
	return m_uVertexIndex[corner];
}

inline U32 Edge::GetFaceIndex(int corner)
{
	return m_uFaceIndex[corner];
}

#endif  // EDGE_H