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
//	GPGFatCornerIter.h
//
//	DESCRIPTION
//		Includes all the functionality of the GPGCornerIter plus 
//		the ability to access the mesh attributes (face indices,
//		vertex positions, normals, etc.).
//
//	NOTES
//		Has a little more overhead than the GPGCornerIter because
//		is stores and advances mesh attribute iterators.

#ifndef GPGFatCornerIter_h
#define GPGFatCornerIter_h

#include "GPGCornerIter.h"
#include "GPGIterators.h"


class GPGNeighborMesh;
class GPGMeshGroup;


// (nonlinear, corner-based, iterator)
class GPGFatCornerIter : public GPGCornerIter
{
public:
    GPGFatCornerIter();
//    ~GPGFatCornerIter();

	// Get the face structure for the current face
    GPGFace* GetFace();		

	// Get the vertex index of the current corner
	INDEX GetVertexIndex();	

	// Get the vertex position of the current corner
    GPGVector3* GetPosition();	

	// Get the normal vector of the current corner.
	// Returns NULL if current mesh does not have normals.
    GPGVector3* GetNormal();	

	// Get the texture coord of the current corner.
	// Returns NULL if current mesh does not have the texture layer.
	// Note: The texture layer was specified when 
	//       GPGNeighborMesh::GetFatCornerIter() was called.
    GPGVector2* GetTexCoord();

	// Move from the current corner to the 
	// clockwise corner on the same face.
    void MoveClockwise();

	// Move from the current corner to the 
	// counterclockwise corner on the same face.
    void MoveCounterClockwise();

	// Move to the next face in the linked-list of faces
	// around the edge opposite the current corner.
	// The current corner will be opposite the edge that
	// was opposite the old corner.
	// 
	// Returns true if mesh boundry crossed
    BOOL JumpAcross();

	// Same as JumpAcross() except the "flags" argument
	// will contain all the flags from the jump.
	//
	// Returns true if mesh boundry crossed
	BOOL JumpAcross(GPGCornerFlags** flags);

    // Make this iterator like srcIter
    void Clone(/*in*/ GPGFatCornerIter* srcIter);

private:
	//
	// Private Initialization
	//
    friend class GPGNeighborMesh;	

	// Can only be initialized by GPGNeighborMesh
    void Initialize(/*in*/ GPGNeighborMesh* pNeighborMesh,
					/*in*/ U32 mesh, /*in*/ U32 face, /*in*/ U32 corner,
					/*in*/ GPGMeshGroup* pMeshGroup,
					/*in*/ U32 textureLayer);

	//
	// Private methods
	//
	void LoadMesh();

	//
	// Private member variables
	//
	GPGMeshGroup* m_pMeshGroup;	// pointer to attached mesh group
	U32 m_textureLayer;			// texture layer returned by GetTexCoord()
	GPGFace* m_pCurrFace;		// pointer to current face structure
	INDEX m_currVertexIndex;	// vertex index of the current corner

	// iterators initialized for the current mesh index
	GPGFaceIter m_faceIter;
	GPGVector3Iter m_positionIter;

	BOOL m_hasNormal;
	GPGVector3Iter m_normalIter;

	BOOL m_hasTexCoord;
	GPGVector2Iter m_texCoordIter;
};



inline GPGFatCornerIter::GPGFatCornerIter() :
	m_pMeshGroup(NULL),	m_textureLayer(0),
	m_pCurrFace(NULL), m_currVertexIndex(0),
	m_hasNormal(FALSE), m_hasTexCoord(FALSE)
{}


//inline GPGFatCornerIter::~GPGFatCornerIter()
//{}


inline void GPGFatCornerIter::MoveClockwise()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	GPGCornerIter::MoveClockwise();

	// update current vertex index
	m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());
}


inline void GPGFatCornerIter::MoveCounterClockwise()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	GPGCornerIter::MoveCounterClockwise();

	// update current vertex index
	m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());
}


inline BOOL GPGFatCornerIter::JumpAcross()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	BOOL isNewMesh = GPGCornerIter::JumpAcross();

	if (isNewMesh)
	{
		LoadMesh();	// load iterators for new mesh
	}

	// update pointer to current face
	m_pCurrFace = m_faceIter.Index(GetFaceIndex());

	// update current vertex index
	m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());

	return isNewMesh;
}


inline BOOL GPGFatCornerIter::JumpAcross(GPGCornerFlags** flags)
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	BOOL isNewMesh = GPGCornerIter::JumpAcross(flags);

	if (isNewMesh)
	{
		LoadMesh();	// load iterators for new mesh
	}

	// update pointer to current face
	m_pCurrFace = m_faceIter.Index(GetFaceIndex());

	// update current vertex index
	m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());

	return isNewMesh;
}


inline GPGFace* GPGFatCornerIter::GetFace()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	return m_pCurrFace;
}


inline INDEX GPGFatCornerIter::GetVertexIndex()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	return m_currVertexIndex;
}


inline GPGVector3* GPGFatCornerIter::GetPosition()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	return m_positionIter.Index(m_currVertexIndex);
}


inline GPGVector3* GPGFatCornerIter::GetNormal()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	if (m_hasNormal)
		return m_normalIter.Index(m_currVertexIndex);
	else
		return NULL;

}


inline GPGVector2* GPGFatCornerIter::GetTexCoord()
{
	GPGASSERT(m_pMeshGroup);	// make sure iterator was initialized
	if (m_hasTexCoord)
		return m_texCoordIter.Index(m_currVertexIndex);
	else
		return NULL;
}


inline void GPGFatCornerIter::Clone(/*in*/ GPGFatCornerIter* srcIter)
{
	*this = *srcIter;
}


#endif
