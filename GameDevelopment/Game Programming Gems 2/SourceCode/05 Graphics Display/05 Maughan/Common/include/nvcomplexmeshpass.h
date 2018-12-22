/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVCOMPLEXMESHPASS_H
#define __NVCOMPLEXMESHPASS_H

#include "nvmaterialresource.h"
namespace nv_objects
{

class NVComplexMesh;

// Data for sub-sections of a mesh render
// Constains details of what should be drawn for this part of the mesh
class NVComplexMeshSection
{
public:
	typedef enum
	{
		IndexedTriangleList = 0,
		TriangleList = 1,
		IndexedTriangleStrip = 2
	} SectionTriType;

	NVComplexMeshSection()
		: m_TriType(IndexedTriangleList)
	{}

	NVResourceID GetMaterialID() const { return m_MaterialID; }
	void SetMaterialID(NVResourceID MaterialID) { m_MaterialID = MaterialID; }

	// This set of faces
    DWORD m_dwAttribId;
    DWORD m_dwIndexStart;
    DWORD m_dwIndexCount;
    DWORD m_dwVertexStart;
    DWORD m_dwVertexCount;
	SectionTriType m_TriType;
	NVResourceID m_MaterialID;
};

typedef std::list<NVComplexMeshSection> tSectionList;

// The data required for a rendering pass of the mesh
// A mesh can have any number of rendering passes to get the job done.
class NVComplexMeshPass
{
public:
	NVComplexMeshPass(NVComplexMesh* pParent);
	NVComplexMeshPass(const NVComplexMeshPass& rhs);
	virtual ~NVComplexMeshPass();

	// Rendering
	virtual bool Render(LPDIRECT3DDEVICE8 pDevice);
	virtual bool RenderSection(LPDIRECT3DDEVICE8 pDevice, tSectionList::const_iterator itrSections);
	virtual void Release();

	// Accessors
	virtual tSectionList& GetSections() { return m_Sections; }

#ifdef _DEBUG
	friend std::ostream& operator << (std::ostream& stream, const NVComplexMeshPass& section);
#endif

	// list of mesh sections
	tSectionList m_Sections;

	NVComplexMesh* m_pMesh;
};


typedef std::vector<NVComplexMeshPass*> tMeshPass;


}; // namespace nv_objects

#endif