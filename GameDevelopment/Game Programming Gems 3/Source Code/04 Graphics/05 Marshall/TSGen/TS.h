/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 1998 - 2002, Intel Corporation.
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
/*
*  TS.h  : Triangle Strip Header File Used to Access TS.lib
*
* PURPOSE:
* Defines the data structures for the tri-strips.
*    
*/

#ifndef _TS_GENERATE_HEADER
#define _TS_GENERATE_HEADER

#ifndef BOOL
typedef int BOOL;
#endif

typedef BOOL (* TS_PROGRESSCALLBACK) (int facesRemaining);

typedef enum {NONE, PHASE1, PHASE2} TEXTUREOPTION;

typedef enum{
	TS_UNDEFINED,
	TS_SUCCESS,
	TS_CANCEL,
	TS_NO_FACE_DATA,
	TS_NO_VERTEX_DATA,
	TS_INVALID_FACE_DATA,
	TS_INVALID_VERTEX_DATA,
	TS_INVALID_TEX_COORDS,
	TS_INVALID_SMGROUP,
	TS_INVALID_MATID,
} TSRESULT;


class TSParam
{
public:
	BOOL bConnectFlag;
	TEXTUREOPTION texcoordFlag;
	unsigned long progressFrequency; 
	TS_PROGRESSCALLBACK progressCallback; // this will be called after progressFrequency 
};

typedef enum {VertexA, VertexB, VertexC} TSVertexPosition;

class TSEdge
{
public:
	long int a, b;
	long int numfaces;		
	long int faces[2];	
};

// This class currently handles one set of texture coordinates
class TSUV
{
public:
	float u, v;
};

class TSV3D 
{
public:
	float x, y, z;
};

class TSFace
{
public:
	unsigned int uFaceIndex;
	int iInvalid;
	int iDuplicate;
	long int a, b, c;			// indices into the vertex pool
	long int na, nb, nc;
	long int ta, tb, tc;
	long int iEdges[3];
	unsigned long uMatId;		// Face's Material ID 
	unsigned long uSmoothId;	// Face's Smoothing Group ID
	float fDistance;
	float fArea;
	TSV3D centroid;
};


//  TSIndex provides the references back to the Max 
//  data structures for each vertex added to a tri-strip.
class TSIndex
{
public:
	unsigned long vert;			// Vertex index into Vertex Pool
	unsigned long fIndex;		// Face index into Face Pool
	TSVertexPosition fVert;		// Which vertex within a Face 0,1,2 
};


struct TSTriStrip{		
	TSIndex *indices;			// Contains indices for all vertices of a tri-strip
	unsigned long numIndices;	// Tri-Strip Run: # of Indices
	unsigned long matID;		// Material id associated with a tri-strip
	int invalid;
	struct TSTriStrip *next;		// Linked List of Triangle Strips
};

class TSMesh
{
public:
	char *pMeshName;				// Mesh Name (ie. Dragon)
	unsigned long uNumEdges;			
	unsigned long uNumFaces;		// # of faces in the mesh
	unsigned long uNumVertices;		// # of vertices in the mesh
	TSEdge *pEdge;	
	TSFace *pFace;					// List of faces for a mesh
	TSV3D  *pVertex;				// List of vertices for a mesh
	TSUV   *pTexCoord;				// List of texture coordinates for a mesh
	struct TSTriStrip *tslist, *tshead;   // Triangle Strip List
};


//Prototypes: Interface to TSGen.lib
TSRESULT TSGenerate(TSMesh *iTSMesh, TSParam *itsparameters = NULL);

#endif // _TS_GENERATE_HEADER