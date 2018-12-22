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
//	GPGCornerIter.h
//
//	DESCRIPTION
//		A corner-based iterator for walking the surface
//		of a model.  The iterator stores the current mesh index,
//		face index, and corner index.  It provides methods
//		for going to the clockwise or counterclockwise corner
//		on a face and for jumping across to the adjacent face.
//
//	NOTES
//		Designed to be very fast.  Use GPGFatCornerIter if you
//		need to access the mesh attributes.

#ifndef GPGCornerIter_h
#define GPGCornerIter_h

#include "GPGDataTypes.h"
#include "GPGNeighborFace.h"

class GPGNeighborMesh;
class GPGFatCornerIter;


// (nonlinear, corner-based, iterator)
class GPGCornerIter
{
public:
    GPGCornerIter();
//    ~GPGCornerIter();

    U32 GetMeshIndex();		// Get the current mesh index
    U32 GetFaceIndex();		// Get the current face index
    U32 GetCornerIndex();	// Get the current corner index

	// Get pointer to the current GPGNeighborFace structure.
	// Use this to preview a jump by calling GetNeighbor()
	// or get and set corner and face flags.
	GPGNeighborFace* GetCurrentFace();

	// Get pointer to current corner flags
	GPGCornerFlags* GetCornerFlags();

	// Get pointer to current face flags
	GPGFaceFlags* GetFaceFlags();

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

	// Returns true if iterators have the same mesh and face index.
	BOOL IsEqualFace(/*in*/ GPGCornerIter* pIter);

	// Returns true if iterators have a different mesh or face index.
	BOOL IsNotEqualFace(/*in*/ GPGCornerIter* pIter);

	// Returns true if iterators have the same mesh, face, and corner index.
	BOOL IsEqualCorner(/*in*/ GPGCornerIter* pIter);

	// Returns true if iterators have a different mesh, face, or corner index.
	BOOL IsNotEqualCorner(/*in*/ GPGCornerIter* pIter);

    // Make this iterator like srcIter
    void Clone(/*in*/ GPGCornerIter* srcIter);

	// Make current neighbor link point to the neighbor
	// specified by targetIter.
	void MoveLink(/*in*/ GPGCornerIter* dstIter);

private:
	//
	// Private Initialization
	//
    friend class GPGNeighborMesh;	
    friend class GPGFatCornerIter;	

	// Can only be initialized by GPGNeighborMesh or
	// GPGFatCornerIter.
    void Initialize(/*in*/ GPGNeighborMesh* pNeighborMesh,
					/*in*/ U32 mesh, /*in*/ U32 face, /*in*/ U32 corner);

	//
	// Private methods
	//

	// Called when we cross a mesh boundry.  Loads in the
	// array of neighbor faces for the current mesh index.
	void LoadMesh();

	//
	// Private member variables
	//
	U32 m_mesh;			// current mesh index
	U32 m_face;			// current face index
	U32 m_corner;		// current corner index

	// pointer to our parent neighbor mesh object
	GPGNeighborMesh* m_pNeighborMesh;

	// current array of neighbor faces for the current mesh
	GPGNeighborFace* m_pNeighborFaces;
};


//
//  inlines
//

inline GPGCornerIter::GPGCornerIter() :
	m_mesh(0), m_face(0), m_corner(0),
	m_pNeighborMesh(NULL), m_pNeighborFaces(NULL)
{}


//inline GPGCornerIter::~GPGCornerIter()
//{}


inline U32 GPGCornerIter::GetMeshIndex()
{
	return m_mesh;
}


inline U32 GPGCornerIter::GetFaceIndex()
{
	return m_face;
}


inline U32 GPGCornerIter::GetCornerIndex()
{
	return m_corner;
}


inline GPGNeighborFace* GPGCornerIter::GetCurrentFace()
{
	return &m_pNeighborFaces[m_face];
}


inline GPGCornerFlags* GPGCornerIter::GetCornerFlags()
{
	return m_pNeighborFaces[m_face].GetCornerFlags(m_corner);
}


inline GPGFaceFlags* GPGCornerIter::GetFaceFlags()
{
	return m_pNeighborFaces[m_face].GetFaceFlags();
}


inline void GPGCornerIter::MoveClockwise()
{
	m_corner = Clockwise(m_corner);
}

	
inline void GPGCornerIter::MoveCounterClockwise()
{
	m_corner = CounterClockwise(m_corner);
}


inline BOOL GPGCornerIter::JumpAcross()
{
	U32 newMesh;
	GPGCornerFlags* flags;
	m_pNeighborFaces[m_face].GetNeighbor(m_corner, &newMesh, &m_face, &flags);
	m_corner = flags->cornerIndex;
	if (newMesh != m_mesh)
	{
		m_mesh = newMesh;
		LoadMesh();
		return TRUE;
	}
	return FALSE;
}


inline BOOL GPGCornerIter::JumpAcross(GPGCornerFlags** flags)
{
	U32 newMesh;
	m_pNeighborFaces[m_face].GetNeighbor(m_corner, &newMesh, &m_face, flags);
	m_corner = (*flags)->cornerIndex;
	if (newMesh != m_mesh)
	{
		m_mesh = newMesh;
		LoadMesh();
		return TRUE;
	}
	return FALSE;
}


inline BOOL GPGCornerIter::IsEqualFace(/*in*/ GPGCornerIter* pIter)
{
	return (m_mesh == pIter->GetMeshIndex()) && (m_face == pIter->GetFaceIndex());
}


inline BOOL GPGCornerIter::IsNotEqualFace(/*in*/ GPGCornerIter* pIter)
{
	return (m_mesh != pIter->GetMeshIndex()) || (m_face != pIter->GetFaceIndex());
}


inline BOOL GPGCornerIter::IsEqualCorner(/*in*/ GPGCornerIter* pIter)
{
	return (m_mesh == pIter->GetMeshIndex()) && 
		(m_face == pIter->GetFaceIndex()) &&
		(m_corner == pIter->GetCornerIndex());
}


inline BOOL GPGCornerIter::IsNotEqualCorner(/*in*/ GPGCornerIter* pIter)
{
	return (m_mesh != pIter->GetMeshIndex()) || 
		(m_face != pIter->GetFaceIndex()) ||
		(m_corner != pIter->GetCornerIndex());
}


inline void GPGCornerIter::Clone(/*in*/ GPGCornerIter* pSrcIter)
{
	*this = *pSrcIter;
}


inline void GPGCornerIter::MoveLink(/*in*/ GPGCornerIter* pDstIter)
{
	GPGNeighborFace* pSrcFace = &m_pNeighborFaces[m_face];

	pSrcFace->SetNeighbor(m_corner,
		pDstIter->GetMeshIndex(),
		pDstIter->GetFaceIndex(),
		pDstIter->GetCornerIndex());
}



#endif
