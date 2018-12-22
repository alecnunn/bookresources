/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
		 A complex mesh object for rendering a mesh with different materials/renderstyles
    
  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVCOMPLEXMESH_H
#define __NVCOMPLEXMESH_H

#pragma warning(disable: 4786)
#include "nvinc.h"
#include "nvdebug.h"
#include "nvmaterialresource.h"
#include "nvbounds.h"

namespace nv_objects
{

static const float SMALL_FLOAT = 1e-12f;
static const DWORD NVCOMPLEXMESH_NO_VERTEX = (DWORD)-1;

typedef enum tageD3DDATATYPE
{
	D3DDATATYPE_DWORD = D3DVSDT_UBYTE4,
	D3DDATATYPE_FLOAT1 = D3DVSDT_FLOAT1,
	D3DDATATYPE_FLOAT2 = D3DVSDT_FLOAT2,
	D3DDATATYPE_FLOAT3 = D3DVSDT_FLOAT3,
	D3DDATATYPE_FLOAT4 = D3DVSDT_FLOAT4,
} eD3DDATATYPE;

// Holds a vertex stream type
class NVVertexData
{
public:
	// Construction
	NVVertexData(eD3DDATATYPE DataType, DWORD dwLocation = NVCOMPLEXMESH_NO_VERTEX)
		: m_DataType(DataType),
		m_dwLocation(dwLocation)
	{ }

	// Destruction
	virtual ~NVVertexData()
	{
	}

	// Overriden methods
	virtual void* GetPointer() = 0;
	virtual void resize(DWORD dwSize) = 0;
	virtual size_t size() = 0;
	virtual bool empty() = 0;
	virtual void Release() = 0;

	// Global methodss
	virtual void SetLocation(DWORD dwLocation) { m_dwLocation = dwLocation; }

	// Acessors
	eD3DDATATYPE GetDataType() const { return m_DataType; }
	DWORD GetLocation() const { return m_dwLocation; }
	

private:
	eD3DDATATYPE m_DataType;
	DWORD m_dwLocation;
};

template<class T, eD3DDATATYPE DataType>
static T* GetDataPointer(NVVertexData* pVertexData)
{
	if (pVertexData == NULL)
		return NULL;

	NVASSERT(DataType == pVertexData->GetDataType(), "ERROR: Data type incorrect");
	return static_cast<T*>(pVertexData->GetPointer());
}

template<eD3DDATATYPE TDataType, class TArrayType>
class NVVertexDataType : public NVVertexData
{
public:
	NVVertexDataType()
		: NVVertexData(TDataType)
	{ }

	virtual ~NVVertexDataType()
	{
	}

	virtual void* GetPointer()
	{
		return &m_Data[0];
	}

	virtual void resize(DWORD dwSize)
	{
		m_Data.resize(dwSize);
	}

	virtual size_t size()
	{
		return m_Data.size();
	}

	virtual void Release()
	{
		m_Data.clear();
	}

	virtual bool empty()
	{
		return m_Data.empty();
	}

	// Customized members
	void push_back(const TArrayType& Data)
	{
		m_Data.push_back(Data);
	}

private:
	std::vector<TArrayType> m_Data;
};

typedef class NVVertexDataType<D3DDATATYPE_DWORD, DWORD> tNVData_dword;
typedef class NVVertexDataType<D3DDATATYPE_FLOAT1, float> tNVData_float1;
typedef class NVVertexDataType<D3DDATATYPE_FLOAT2, D3DXVECTOR2> tNVData_float2;
typedef class NVVertexDataType<D3DDATATYPE_FLOAT3, D3DXVECTOR3> tNVData_float3;
typedef class NVVertexDataType<D3DDATATYPE_FLOAT4, D3DXVECTOR4> tNVData_float4;


typedef std::vector<WORD> tIndexArray;
typedef std::vector<LPDIRECT3DINDEXBUFFER8> tIndexBufferArray;

// Names used by vertex data
typedef const DWORD NVVERTNAME;

// Usual vertex entries
static NVVERTNAME NVVERT_POSITION = 0;
static NVVERTNAME NVVERT_NORMAL = 1;
static NVVERTNAME NVVERT_DIFFUSE = 2;
static NVVERTNAME NVVERT_SPECULAR = 3;

// Texture coords
static NVVERTNAME NVVERT_TEXTURE0 = 100;

// Basis vectors
static NVVERTNAME NVVERT_SBASIS0 = 200;
static NVVERTNAME NVVERT_TBASIS0 = 300;
static NVVERTNAME NVVERT_SxTBASIS0 = 400;

// Bone indices and weights
static NVVERTNAME NVVERT_BONEINDICES0 = 500;
static NVVERTNAME NVVERT_BONEWEIGHTS0 = 550;

typedef std::map<NVVERTNAME, NVVertexData*> tVertexDataMap;
	
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

	NVResourceRef GetMaterial() const { return m_Material; }
	void SetMaterial(NVResourceRef Material) { m_Material = Material; }
	
	NVResourceRef GetRenderStyle() const { return m_RenderStyle; }	
	void SetRenderStyle(NVResourceRef RenderStyle) { m_RenderStyle = RenderStyle; }

	// This set of faces
    DWORD m_dwAttribId;
    DWORD m_dwIndexStart;
    DWORD m_dwIndexCount;
    DWORD m_dwVertexStart;
    DWORD m_dwVertexCount;
	SectionTriType m_TriType;
	NVResourceRef m_Material;
	NVResourceRef m_RenderStyle;
};
typedef std::list<NVComplexMeshSection> tSectionList;

// Complex mesh class.  Used for drawing objects with textures/materials/shaders
class NVComplexMesh
{
public:
	typedef enum tagNVComplexMeshOptions
	{
		MeshOption_SmoothBasis = (1 << 0),
		MeshOption_UnifyNormals = (1 << 1),
		MeshOption_FORCEDWORD = 0x7FFFFFFF
	} NVComplexMeshOptions;

	NVComplexMesh();
	virtual ~NVComplexMesh();
	virtual void Release();

	// Rendering
	virtual bool Translate(const D3DXVECTOR3& Tranlation);
	virtual bool Scale(float fScale);
	virtual bool Transform(const D3DXMATRIX& Transform);

	virtual bool Render(LPDIRECT3DDEVICE8 pDevice);
	virtual bool RenderSection(LPDIRECT3DDEVICE8 pDevice, tSectionList::const_iterator itrSections);
	virtual bool SetupFVFStream();
	virtual bool PrepareDeviceObjects(LPDIRECT3DDEVICE8 pDevice);

	// File loaders
	virtual bool CreateFromXFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName);
	virtual bool CreateFromXof(LPDIRECT3DDEVICE8 pDevice, LPDIRECTXFILEDATA pxofobjCur);
	virtual bool CreateFromXFileData(LPDIRECT3DDEVICE8 pDevice, LPDIRECT3DVERTEXBUFFER8 pVB, LPDIRECT3DINDEXBUFFER8 pIB, LPD3DXBUFFER pMaterials, DWORD dwNumMaterials, LPD3DXMESH pMesh);

	virtual bool CreateFromMFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName);
	virtual bool CreateFromSphere(float fRadius, WORD wNumRings, WORD wNumSections, const D3DXVECTOR3& PositionScaling, const D3DXVECTOR2& TextureScaling);


	// Helpers
	virtual bool ComputeBounds(NVAABounds* pBounds);

	virtual bool GenerateNormals();
	virtual bool NormalizeNormals();
	virtual bool FlipNormals();
	virtual void SetRenderStyle(nv_objects::NVResourceRef RenderStyle);	// Sets renderstyle for all sections
	
	virtual tIndexArray& GetIndexArray() { return m_Indices; }
	virtual bool GenerateBasis(DWORD dwTexture, DWORD dwOptions = MeshOption_SmoothBasis);
	virtual DWORD GetTrianglesDrawn() { return m_dwNumTrianglesDrawn; }
	virtual DWORD GetVerticesDrawn() const { return m_dwNumVerticesDrawn; }
	
	virtual void SetNumVertices(DWORD dwNum) { m_dwNumVertices = dwNum; }
	virtual DWORD GetNumVertices() const { return m_dwNumVertices; }

	virtual void SetNumIndices(DWORD dwNum) { m_dwNumIndices = dwNum;}
	virtual DWORD GetNumIndices() const { return m_dwNumIndices; }
	
	// Add/find vertex data
	virtual NVVertexData* AddVertexData(NVVERTNAME Name, eD3DDATATYPE DataType);
	virtual NVVertexData* FindVertexData(NVVERTNAME Name);
	virtual bool ClearVertexLocations();

	virtual DWORD GetFVF() const { return m_dwFVF; }

	virtual LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() { m_pVB->AddRef(); return m_pVB; }
	virtual const tIndexBufferArray& GetIndexBuffers() { return m_IndexBuffers; }

	virtual void AddTrianglesDrawn(DWORD dwNum) { m_dwNumTrianglesDrawn += dwNum; }
	virtual void AddVerticesDrawn(DWORD dwNum) { m_dwNumVerticesDrawn += dwNum; }
	virtual void ResetStats() { m_dwNumTrianglesDrawn = m_dwNumVerticesDrawn = 0; }

	void SetBoneMatrixPtr(DWORD i, const D3DXMATRIX* pMat);


	// Accessors
	virtual tSectionList& GetSections() { return m_Sections; }
	virtual DWORD GetNumBones() const { return m_dwNumBones; }
	virtual DWORD GetNumWeights() const { return m_dwNumWeights; }
	virtual const char* GetBoneName(DWORD i) const;
	virtual DWORD GetBonePaletteSize() const { return m_dwBonePaletteSize; }


protected:

	// Vertex/index data
	tVertexDataMap m_VertexData;
	DWORD m_dwStride;
	DWORD m_dwNumVertices;
	DWORD m_dwNumIndices;
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	tIndexBufferArray m_IndexBuffers;
	tIndexArray m_Indices;

	DWORD m_dwFVF;

	DWORD m_dwNumTrianglesDrawn;
	DWORD m_dwNumVerticesDrawn;

	const D3DXMATRIX** m_pBoneMatrices;
	LPD3DXBUFFER m_pBoneNames;
	LPD3DXBUFFER m_pBoneTransforms;
	LPD3DXBUFFER m_pBoneCombinations;
	DWORD m_dwNumBoneCombinations;
	DWORD m_dwNumBones;
	DWORD m_dwNumWeights;
	DWORD m_dwBonePaletteSize;
	DWORD m_dwDesiredPaletteSize;

	// list of mesh sections
	tSectionList m_Sections;

	// The directory where this mesh lives
	std::string m_strDirectory;

};






} // namespace nv_objects

#endif // __NVCOMPLEXMESH_H