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
//	GPGNeighborMesh.h
//
//	DESCRIPTION
//		Container class for neighbor mesh.  A neighbor mesh provides
//		information about the adjacency of faces, shared vertices, 
//		and shared edges.
//
//	NOTES
//		The default layout is Compact which uses 16 bytes per face.
// 
//		To compile Neighbor Mesh with the Huge layout (28 bytes per
//		face), define the preprocessor symbol GPG_NEIGHBOR_HUGE.
//		See GPGNeighborFace.h for details.
//
//		Use GPGNeighborMeshUtil class to build a neighbor mesh from
//		an GPGMeshGroup.

#ifndef GPGNeighborMesh_h
#define GPGNeighborMesh_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"
#include "GPGResult.h"
#include "GPGNeighborFace.h"
#include "GPGCornerIter.h"
#include "GPGFatCornerIter.h"


class GPGMeshGroup;


class GPGNeighborMesh
{
public:
	GPGNeighborMesh();
	virtual ~GPGNeighborMesh();

	// Allocate memory for neighbor mesh in a parallel array
	// fashion, according to the sizes in GPGMeshGroup.
	//
	// Will return GPG_E_OUT_OF_MEMORY if a memory allocation fails.
    GPGRESULT Allocate(GPGMeshGroup* pMeshGroup);

	// Frees memory for neighbor mesh.
	void Deallocate();

	// Returns TRUE if Allocate() has been successfully called.
	BOOL IsAllocated();

	// Returns TRUE if neighbor mesh connectivity is current.
	// If FALSE, then the neighbor mesh connectivity does not 
	// match the connectivity in the GPGMeshGroup.
	// (also known as the dirty flag)
	BOOL IsValid();

	// TRUE indicates that the neighbor mesh is current.
	// Set to FALSE if a mesh was added/deleted from GPGMeshGroup,
	// or if an associated GPGMesh has had a change in connectivity
	// (if the faces have been changed/added/deleted).
	void SetValid(/*in*/ BOOL state);

	// Returns number of meshes (partitions) in attached GPGMeshGroup.
	// Returns 0 if Allocate() has not been called.
	U32 GetNumMeshes();

	// Returns a pointer to the array of neighbor faces.
	// This is used to perform an operation that requires iterating 
	// through all the neighbor faces.  An example of this is
	// initializing the neighbor links.
	//
    // Allocate must have been called (and successful) before
	// this method is used.
	//
	// Call GetNumMeshes() to determine maximum meshIndex.
	// Call GetNumFaces() from the associated GPGMesh to determine
	// the maximum index into the GPGNeighborFace array.
    GPGNeighborFace* GetNeighborFaceArray(/*in*/ U32 meshIndex);

	// Get the lightweight corner iterator for a corner of
	// a face on a mesh.  The corner iterator is used for moving
	// from one face to its neighbor faces.
	//
	// Call GetNumMeshes() to determine maximum meshIndex.
	void GetCornerIter(/*in*/ U32 meshIndex,
						/*in*/ U32 faceIndex,
						/*in*/ U32 cornerIndex,
						/*in-out*/ GPGCornerIter* pCornerIter);

	// Get the general purpose corner iterator for a corner of
	// a face on a mesh.  This iterator provides methods that access
	// face and vertex attribute information on the mesh.  For 
	// performance reasons, you must specify which texture layer 
	// you want access to.
	//
	// Call GetNumMeshes() to determine maximum meshIndex.
	void GetFatCornerIter(/*in*/ U32 textureLayer,
							/*in*/ U32 meshIndex,
							/*in*/ U32 faceIndex,
							/*in*/ U32 cornerIndex,
							/*in-out*/ GPGFatCornerIter* pCornerIter);

private:
	BOOL m_validState;
	U32 m_numMeshes;
	GPGNeighborFace** m_ppNeighborFaces;
	GPGMeshGroup* m_pMeshGroup;
};


//
// inlines
//

inline BOOL GPGNeighborMesh::IsAllocated()
{
	return BOOL(m_pMeshGroup);
}


inline BOOL GPGNeighborMesh::IsValid()
{
	return m_validState;
}

inline void GPGNeighborMesh::SetValid(BOOL state)
{
	m_validState = state;
}

inline U32 GPGNeighborMesh::GetNumMeshes()
{
	return m_numMeshes;
}

inline GPGNeighborFace* GPGNeighborMesh::GetNeighborFaceArray(/*in*/ U32 meshIndex)
{
	GPGASSERT(m_ppNeighborFaces);
	GPGASSERT(meshIndex < m_numMeshes);
	return m_ppNeighborFaces[meshIndex];
}

inline void GPGNeighborMesh::GetCornerIter(/*in*/ U32 meshIndex,
											/*in*/ U32 faceIndex,
											/*in*/ U32 cornerIndex,
											/*in-out*/ GPGCornerIter* pCornerIter)
{
	GPGASSERT(meshIndex < m_numMeshes);
	GPGASSERT(cornerIndex < 3);
	pCornerIter->Initialize(this, meshIndex, faceIndex, cornerIndex);

}

inline void GPGNeighborMesh::GetFatCornerIter(/*in*/ U32 textureLayer,
												/*in*/ U32 meshIndex,
												/*in*/ U32 faceIndex,
												/*in*/ U32 cornerIndex,
												/*in-out*/ GPGFatCornerIter* pCornerIter)
{
	GPGASSERT(meshIndex < m_numMeshes);
	GPGASSERT(cornerIndex < 3);
	pCornerIter->Initialize(this, meshIndex, faceIndex, cornerIndex, m_pMeshGroup, textureLayer);
}

#endif
