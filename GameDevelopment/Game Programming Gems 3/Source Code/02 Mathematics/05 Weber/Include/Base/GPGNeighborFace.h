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
//	GPGNeighborFace.h
//
//	DESCRIPTION
//    Interface definition for neighbor mesh faces and 
//    flag data structures.
//
//	NOTES
//    The corner and face flags are cooperatively shared 
//    across different technologies.  The neighbor mesh
//    resolution controller uses the named flags.
//

#ifndef GPGNeighborFace_h
#define GPGNeighborFace_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"

// Shockwave 3D uses the larger neighbor mesh indices
#define GPG_NEIGHBOR_HUGE


// Maximum index size for each bit layout
#ifdef GPG_NEIGHBOR_HUGE
#define MAX_NBR_MESH_INDEX 0xFFFFFFFF
typedef U32 GPGNBRINDEX;
#else
#define MAX_NBR_MESH_INDEX 0x0000FFFF
typedef U16 GPGNBRINDEX;
#endif

//
// Bit Layout of Compact vs. Huge Versions
// 
// 					
// Corner bits		Compact		Huge
// 	mesh index		16			32
// 	face index		16			32
// 	corner index	2			2
// 	flipped			1			1
//  collapsed		1           1
// 	user flags		4			4
// Sum				40 			72
// 					
// All corners		120			216
// Face flag		8			8
// 					
// Total bits		128			224
// Total bytes		16			28
// 
//
// Define the preprocessor symbol GPG_NEIGHBOR_HUGE
// to compile Neighbor Mesh with the Huge layout.
//

//
//  Corner flags
//
struct GPGCornerFlags
{
	U8 cornerIndex		: 2;
	U8 flippedFlag		: 1;
	U8 collapsedFlag	: 1;

	U8 cornerFlag0		: 1;
	U8 cornerFlag1		: 1;
	U8 cornerFlag2		: 1;
	U8 cornerFlag3		: 1;
};


//
//  Face flags
//
struct GPGFaceFlags
{
	U8 collapseIndex	: 2;
	U8 duplicateFlag	: 1;

	U8 faceFlag0		: 1;
	U8 faceFlag1		: 1;
	U8 faceFlag2		: 1;
	U8 faceFlag3		: 1;
	U8 faceFlag4		: 1;
};



// Compute the clockwise or counterclockwise corner index
U32 Clockwise(/*in*/ U32 corner);
U32 CounterClockwise(/*in*/ U32 corner);


//
// Simple container object
//
// NOTE:  No virtual methods!
//		  Adding a single virtual method costs 4-bytes per face.
//
class GPGNeighborFace
{
public:
//	GPGNeighborFace() {}	// no constructor -- fast initialization by GPGNeighborMesh
//	~GPGNeighborFace() {}	// not virtual for memory reasons


    // Get the face index of the next face in the circular-linked-list
    // of faces around the edge identified by cornerIndex (the edge opposite
	// the corner).
    //
    // In the case where there are no neighbor faces on the identified edge
    // (as in the outermost extent of the mesh), this is best visualized
    // as one face in the circular-linked-list (of faces around the edge).
    // Thus, the current face index is returned.
    //
    void GetNeighbor(/*in*/ U32 corner,				// corner index on current face:
					  /*out*/ U32* pNeighborMesh,	// mesh index of neighbor face
					  /*out*/ U32* pNeighborFace,	// face index of neighbor face
					  /*out*/ GPGCornerFlags** ppFlags); // corner flags
 
    // Set neighbor mesh links for a corner on the current face.
    void SetNeighbor(/*in*/ U32 corner,				// corner index on current face:
													//   must be 0, 1, or 2
                      /*in*/ U32 neighborMesh,		// mesh index of neighbor face
                      /*in*/ U32 neighborFace,		// face index of neighbor face
                      /*in*/ GPGCornerFlags flags);	// corner flags

    // Set neighbor mesh links for a corner on the current face.
    void SetNeighbor(/*in*/ U32 corner,				// corner index on current face
                      /*in*/ U32 neighborMesh,		// mesh index of neighbor face
                      /*in*/ U32 neighborFace,		// face index of neighbor face
                      /*in*/ U32 neighborCorner);	// corner index of neighbor face

	// Get face flags for the current face.
    GPGFaceFlags* GetFaceFlags();

	// Set flags for the current face.
    void SetFaceFlags(/*in*/ GPGFaceFlags flags);

	// Get flags for a corner.
    GPGCornerFlags* GetCornerFlags(/*in*/ U32 corner);

	// Set flags for a corner.
    void SetCornerFlags(/*in*/ U32 corner, /*in*/ GPGCornerFlags flags);

public:

#ifdef GPG_NEIGHBOR_HUGE

	U32 m_neighborMesh[3];				// neighbor index
	U32 m_neighborFace[3];				// neighbor face indices
	GPGCornerFlags m_cornerFlags[3];	// corner flags
	GPGFaceFlags m_faceFlags;			// face flags

#else // Compact

	U16 m_neighborMesh[3];				// neighbor index
	U16 m_neighborFace[3];				// neighbor face indices
	GPGCornerFlags m_cornerFlags[3];	// corner flags
	GPGFaceFlags m_faceFlags;			// face flags

#endif
};


//
//  inlines
//
extern U32 g_ClockWise[];
extern U32 g_CounterClockWise[];


inline U32 Clockwise(/*in*/ U32 corner)
{
	GPGASSERT(corner < 3);
	return g_ClockWise[corner];	// equivalent to (corner + 1) % 3
}


inline U32 CounterClockwise(/*in*/ U32 corner)
{
	GPGASSERT(corner < 3);
	return g_CounterClockWise[corner];	// equivalent to (corner + 2) % 3
}


inline void GPGNeighborFace::GetNeighbor(/*in*/ U32 corner,
										  /*out*/ U32* pNeighborMesh,
										  /*out*/ U32* pNeighborFace,
										  /*out*/ GPGCornerFlags** pFlags)
{
	GPGASSERT(corner < 3);
	*pNeighborMesh = U32(m_neighborMesh[corner]);
	*pNeighborFace = U32(m_neighborFace[corner]);
	*pFlags = &(m_cornerFlags[corner]);

}


inline void GPGNeighborFace::SetNeighbor(/*in*/ U32 corner,
										  /*in*/ U32 neighborMesh,
										  /*in*/ U32 neighborFace,
										  /*in*/ GPGCornerFlags flags)
{
	GPGASSERT(corner < 3);
	m_neighborMesh[corner] = neighborMesh;
	m_neighborFace[corner] = neighborFace;
	m_cornerFlags[corner] = flags;

}


inline void GPGNeighborFace::SetNeighbor(/*in*/ U32 corner,
										  /*in*/ U32 neighborMesh,
										  /*in*/ U32 neighborFace,
										  /*in*/ U32 neighborCorner)
{
	GPGASSERT(corner < 3);
	m_neighborMesh[corner] = neighborMesh;
	m_neighborFace[corner] = neighborFace;
	m_cornerFlags[corner].cornerIndex = neighborCorner;

}


inline GPGFaceFlags* GPGNeighborFace::GetFaceFlags()
{
	return &m_faceFlags;
}


inline void GPGNeighborFace::SetFaceFlags(/*in*/ GPGFaceFlags flags)
{
	m_faceFlags = flags;
}


inline GPGCornerFlags* GPGNeighborFace::GetCornerFlags(/*in*/ U32 corner)
{
	return &(m_cornerFlags[corner]);
}


inline void GPGNeighborFace::SetCornerFlags(/*in*/ U32 corner,
											/*in*/ GPGCornerFlags flags)
{
	m_cornerFlags[corner] = flags;
}

#endif
