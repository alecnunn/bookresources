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
//	GPGMeshGroup.h
//
//	DESCRIPTION
//
//	NOTES
//
//		This class groups meshes together and associates a material
//		with each mesh. Typically a MeshGroup represents a single
//		object that had to be broken into parts because of material 
//		differences.  In addition to material partitioning, the
//		mesh may be additionally partitioned if it was partitioned 
//		in the authoring software.
//
//		This partitioning is done to increase rendering performance by 
//		reducing material state changes.  An entire GPGMesh is usually
//		drawn using one OpenGL or D3D call (after the material state
//		is set).
//
//		The MeshGroup stores only pointers to meshes and materials so
//		a single mesh or material may be shared by many different
//		MeshGroups. 

#ifndef GPGMeshGroup_h
#define GPGMeshGroup_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"
#include "GPGMesh.h"
#include "GPGNeighborMesh.h"
#include "GPGResult.h"
#include "Base/GPGString.h"

class GPGUpdatesGroup;
class GPGMeshGroup
{
public:
	GPGMeshGroup();
	~GPGMeshGroup();

	// Call this method first!  Allocates space for mesh pointers.
    GPGRESULT Allocate(U32 numMeshes);

	// Returns number of meshes
    U32       GetNumMeshes();

	// Returns the array of mesh pointers.
	GPGMesh** GetMeshList();

	// Note: may return null.
    GPGMesh*  GetMesh(U32 index);

	// Stores pointer mesh at specified index.
	// Note: 1) returns mesh that we just passed in.
	//		 2) Old mesh is deleted (if it isn't == to the new mesh)
	GPGMesh*  SetMesh(U32 index, GPGMesh* pMesh);

	// Store pointer mesh at specified index.
	// Note: 1) returns mesh that we just passed in.
	//       2) old mesh is not deleted
	GPGMesh*  SetMeshND(U32 index, GPGMesh* pMesh);
	
	// Get name of mesh at specified index.
	GPGString GetMeshName(U32 index);

	// Set name of mesh at specified index.
	void      SetMeshName(U32 index, GPGString meshName);

	// Get name of mesh at specified index.
	GPGString GetParentMeshName(U32 index);

	// Set name of parent mesh at specified index.
	void SetParentMeshName(U32 index, GPGString parentMeshName);

	// Get pointer of associated neighbor mesh.
	// NOTE: Returns NULL if no neighbor mesh.
	GPGNeighborMesh* GetNeighborMesh();

	// Store pointer to associated neighbor mesh.
	// NOTE: OK to pass in a NULL if neighbor mesh has been deleted.
	void SetNeighborMesh(GPGNeighborMesh* pNeighborMesh);

	// Anchronsims from Toolkit...do not use:
	// Get index into material palatte for mesh at specified index.
	U32      GetMaterial(U32 index);
	// Stores global material index at specified index.
	// Note: returns mat index that we just passed in.
	U32      SetMaterial(U32 index, U32 matIndex);

private:
	U32 m_size;		// number of GPGMesh's in array
	GPGMesh **m_ppMeshes;	// array of meshes
	U32 *m_ppMaterials;		// array of material indices
	GPGString* m_pMeshNames;		// array of mesh names
	GPGString* m_pParentMeshNames;	// array of parent mesh names

	GPGNeighborMesh* m_pNeighborMesh;	// associated neighbor mesh (optional)
};




inline U32 GPGMeshGroup::GetNumMeshes()
{
	return m_size;
}


inline GPGMesh** GPGMeshGroup::GetMeshList()
{
	return m_ppMeshes;
}


inline GPGMesh* GPGMeshGroup::GetMesh(U32 index)
{
	GPGASSERT(index < m_size);
	return m_ppMeshes[index];
}


inline GPGMesh* GPGMeshGroup::SetMesh(U32 index, GPGMesh* pMesh)
{
	GPGASSERT(index < m_size);
//	GPGASSERT(pMesh);
//	GPGASSERT( (unsigned int)((void*) (m_ppMeshes[index])) < 0xC0000000 );
//	GPGASSERT( (unsigned int)((void*)pMesh) < 0xC0000000 );

	if (m_ppMeshes[index] && (pMesh != m_ppMeshes[index]))
    {
        delete m_ppMeshes[ index ];		
		m_ppMeshes[index] = NULL;
	}
	return m_ppMeshes[index] = pMesh; //pMesh;
}

inline GPGMesh* GPGMeshGroup::SetMeshND(U32 index, GPGMesh* pMesh)
{
	GPGASSERT(index < m_size);

	return m_ppMeshes[index] = pMesh; //pMesh;
}


inline U32 GPGMeshGroup::GetMaterial(U32 index)
{
	GPGASSERT(index < m_size);
	return m_ppMaterials[index];
}


inline U32 GPGMeshGroup::SetMaterial(U32 index, U32 matIndex)
{
	GPGASSERT(index < m_size);
	m_ppMaterials[index] = matIndex;
	return matIndex;
}


inline void GPGMeshGroup::SetMeshName(U32 index, GPGString meshName)
{
	GPGASSERT(index < m_size);
	m_pMeshNames[index] = meshName;
}


inline GPGString GPGMeshGroup::GetMeshName(U32 index)
{
	GPGASSERT(index < m_size);
	return m_pMeshNames[index];
}


inline void GPGMeshGroup::SetParentMeshName(U32 index, GPGString parentMeshName)
{
	GPGASSERT(index < m_size);
	m_pParentMeshNames[index] = parentMeshName;
}


inline GPGString GPGMeshGroup::GetParentMeshName(U32 index)
{
	GPGASSERT(index < m_size);
	return m_pParentMeshNames[index];
}


inline GPGNeighborMesh* GPGMeshGroup::GetNeighborMesh()
{
	return m_pNeighborMesh;
}


inline void GPGMeshGroup::SetNeighborMesh(GPGNeighborMesh* pNeighborMesh)
{
	// OK for pNeighborMesh to be NULL
	if( m_pNeighborMesh && ( m_pNeighborMesh != pNeighborMesh )) 
	{
		delete m_pNeighborMesh;
		m_pNeighborMesh = NULL;
	}

	m_pNeighborMesh = pNeighborMesh;
}

#endif
