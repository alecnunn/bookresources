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
#ifndef GPGVERTEXMAP_DOT_H
#define GPGVERTEXMAP_DOT_H

#include "GPGResult.h"
#include "GPGDebug.h"
#include "GPGDataTypes.h"

// The conversion of a model to an GPGMeshGroup of GPGMeshes may duplicate 
// vertices to handle the multiple normals or texture coordinates that may 
// occur at a particular original vertex.  The VertexMap maps an original 
// vertex index to the GPGMesh index for GPGMeshGroup and the GPGMesh vertex index.

class GPGVertexMap
{
	public:
		GPGVertexMap();
		~GPGVertexMap();

		// Returns number of original vertices
		U32 GetNumMapEntries(void);

		// Returns how many copies of a particular original vertex there are in the GPG mesh.
		U32 GetNumVertexCopies(U32 originalVertexIndex);

		// Specify the original vertex index and the copy number.  Returns the
		// corresponding GPGMesh index for GPGMeshGroup and the GPGMesh vertex index.
		GPGRESULT GetVertexCopy(U32 originalVertexIndex,
								U32 copyIndex,
								U32 *indexGPGMesh,
								U32 *indexGPGVertex);
		
		// These are used to build the map.
		GPGRESULT AllocateMap(U32 numOrigVertices);
		GPGRESULT AllocateCopies(U32 origVertexIndex, U32 numCopies);
		void AddVertex(U32 origVertexIndex, U32 indexGPGMesh, U32 indexGPGVertex);

	private:
		U32 mapSize;		// number of elements in pCopies number of pointers MeshVert lists

		struct MeshVert {
			U32 mesh;		// GPGMesh index for GPGMeshGroup
			U32 vert;		// GPGMesh vertex index
		};

		U32 *pCounts;
		MeshVert **ppCopies;
};


//
// inlines
//

// Returns how many copies of a particular imesh vertex there are in the GPG mesh.
inline U32 GPGVertexMap::GetNumVertexCopies(U32 IMeshVertexIndex)
{ 
	return pCounts[IMeshVertexIndex];
}

// Gives you the index into the GPGMeshGroup and position in the GPGMesh of a 
// particular copy of an imesh vertex.
inline GPGRESULT GPGVertexMap::GetVertexCopy(U32 imeshVertexIndex,
												U32 copyIndex,
												U32 *indexGPGMesh,
												U32 *indexGPGVertex)
{
	MeshVert& mapEntry = ppCopies[imeshVertexIndex][copyIndex];
	*indexGPGMesh   = mapEntry.mesh;
	*indexGPGVertex = mapEntry.vert;
	return GPG_OK;
}

#endif
