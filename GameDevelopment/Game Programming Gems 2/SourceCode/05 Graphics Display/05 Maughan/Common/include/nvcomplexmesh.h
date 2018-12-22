/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVCOMPLEXMESH_H
#define __NVCOMPLEXMESH_H

#pragma warning(disable: 4786)
#include "nvinc.h"
#include "nvcomplexmeshpass.h"

namespace nv_objects
{

class NVComplexMeshSection;
class NVComplexMeshPass;
class NVComplexMesh;

static const float SMALL_FLOAT = 1e-12f;
static const DWORD NVCOMPLEXMESH_NO_VERTEX = (DWORD)-1;

// CM TODO.  Add more vertex stream types.
typedef std::vector<D3DXVECTOR2> tVec2Array;
typedef std::vector<D3DXVECTOR3> tVec3Array;
typedef std::vector<D3DXVECTOR4> tVec4Array;
typedef enum tageNVComplexMeshVertexDataType
{
	NVCOMPLEXMESH_FLOAT2 = D3DVSDT_FLOAT2,
	NVCOMPLEXMESH_FLOAT3 = D3DVSDT_FLOAT3,
	NVCOMPLEXMESH_FLOAT4 = D3DVSDT_FLOAT4
} eNVComplexMeshVertexDataType;

typedef std::vector<D3DXVECTOR3> tVec3Array;
typedef std::vector<D3DXVECTOR4> tVec4Array;
typedef std::vector<WORD> tIndexArray;
typedef std::vector<LPDIRECT3DINDEXBUFFER8> tIndexBufferArray;

// Holds a vertex stream type
class NVComplexMeshVertexData
{
public:
	NVComplexMeshVertexData(eNVComplexMeshVertexDataType DataType, DWORD dwLocation = NVCOMPLEXMESH_NO_VERTEX)
		: m_DataType(DataType),
		m_dwLocation(dwLocation)
	{ }

	virtual ~NVComplexMeshVertexData()
	{
		Release();
	}

	void Release()
	{
		m_DataVec2.clear();
		m_DataVec3.clear();
		m_DataVec4.clear();
	}

	void UnIndex(const tIndexArray& Indices)
	{
		DISPDBG(3, "NVComplexMesh::UnIndex");
		tIndexArray::const_iterator itrIndices = Indices.begin();
		DWORD dwCount = 0;
		switch(m_DataType)
		{
			case NVCOMPLEXMESH_FLOAT2:
			{
				tVec2Array NewArray;
				NewArray.resize(Indices.size());
				DISPDBG(3, "NVComplexMesh: There are " << Indices.size() << " Indices");
				while (itrIndices != Indices.end())
				{
					assert(*itrIndices < m_DataVec2.size());
					NewArray[dwCount++] = m_DataVec2[*itrIndices];
					itrIndices++;
				};
				m_DataVec2 = NewArray;
			}
			break;

			case NVCOMPLEXMESH_FLOAT3:
			{
				tVec3Array NewArray;
				NewArray.resize(Indices.size());
				DISPDBG(2, "NVComplexMesh: There are " << Indices.size() << " Indices");
				while (itrIndices != Indices.end())
				{
					assert(*itrIndices < m_DataVec3.size());
					NewArray[dwCount++] = m_DataVec3[*itrIndices];
					itrIndices++;
				};
				m_DataVec3 = NewArray;
			}
			break;

			case NVCOMPLEXMESH_FLOAT4:
			{
				tVec4Array NewArray;
				NewArray.resize(Indices.size());
				DISPDBG(2, "NVComplexMesh: There are " << Indices.size() << " Indices");
				while (itrIndices != Indices.end())
				{
					assert(*itrIndices < m_DataVec4.size());
					NewArray[dwCount++] = m_DataVec4[*itrIndices];
					itrIndices++;
				};
				m_DataVec4 = NewArray;
			}
			break;
		}
	}

	void SetLocation(DWORD dwLocation) { m_dwLocation = dwLocation; }

	// Acessors
	eNVComplexMeshVertexDataType GetDataType() const { return m_DataType; }
	DWORD GetLocation() const { return m_dwLocation; }
	
	tVec2Array& GetVec2Array() { assert(m_DataType == NVCOMPLEXMESH_FLOAT2); return m_DataVec2; }
	tVec3Array& GetVec3Array() { assert(m_DataType == NVCOMPLEXMESH_FLOAT3); return m_DataVec3; }
	tVec4Array& GetVec4Array() { assert(m_DataType == NVCOMPLEXMESH_FLOAT4); return m_DataVec4; }
	
	eNVComplexMeshVertexDataType m_DataType;

	tVec2Array m_DataVec2;
	tVec3Array m_DataVec3;
	tVec4Array m_DataVec4;

	DWORD m_dwLocation;
};
typedef std::map<const std::string, NVComplexMeshVertexData> tVertexDataMap;
	
class NVComplexMeshBounds
{
public:
	virtual void Transform(const D3DXMATRIX* pMatrix)
	{
		D3DXVec3TransformCoord(&m_vecMinExtents, &m_vecMinExtents, pMatrix);
		D3DXVec3TransformCoord(&m_vecMaxExtents, &m_vecMaxExtents, pMatrix);

		D3DXVECTOR3 vecMin, vecMax;
		vecMin.x = min(m_vecMinExtents.x, m_vecMaxExtents.x);
		vecMin.y = min(m_vecMinExtents.y, m_vecMaxExtents.y);
		vecMin.z = min(m_vecMinExtents.z, m_vecMaxExtents.z);

		vecMax.x = max(m_vecMinExtents.x, m_vecMaxExtents.x);
		vecMax.y = max(m_vecMinExtents.y, m_vecMaxExtents.y);
		vecMax.z = max(m_vecMinExtents.z, m_vecMaxExtents.z);

		m_vecCenter = (vecMax + vecMin) / 2.0f;
		m_vecMaxExtents = vecMax;
		m_vecMinExtents = vecMin;

		// The bounding radius will either be from the center to the min or the center to the max
		// The max & min distance should be about the same, but I have seen differences, so we will select
		//  the max of the two anyway to ensure we get it right
		D3DXVECTOR3 maxDirection(m_vecMaxExtents - m_vecCenter);
		D3DXVECTOR3 minDirection(m_vecMinExtents - m_vecCenter);

		float fRadiusSquared1 = D3DXVec3Dot(&maxDirection, &maxDirection);
		float fRadiusSquared2 = D3DXVec3Dot(&minDirection, &minDirection);
		m_fRadius = (fRadiusSquared1 > fRadiusSquared2) ? sqrtf(fRadiusSquared1) : sqrtf(fRadiusSquared2);
	}

public:
	D3DXVECTOR3 m_vecCenter;
	D3DXVECTOR3 m_vecMinExtents;
	D3DXVECTOR3 m_vecMaxExtents;
	float m_fRadius;
};

// Complex mesh class.  Used for drawing objects with textures/materials/shaders
class NVComplexMesh
{
public:
	NVComplexMesh();
	virtual ~NVComplexMesh();
	virtual void Release();

	// Rendering
	virtual bool Translate(const D3DXVECTOR3& Tranlation);
	virtual bool Scale(float fScale);
	virtual bool Transform(const D3DXMATRIX& Transform);

	virtual bool Render(LPDIRECT3DDEVICE8 pDevice);
	virtual bool SetupFVFStream();
	virtual bool PrepareDeviceObjects(LPDIRECT3DDEVICE8 pDevice);

	// File loaders
	virtual bool CreateFromXFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName);
	virtual bool CreateFromMFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName);
	virtual bool CreateFromSphere(float fRadius, WORD wNumRings, WORD wNumSections, const D3DXVECTOR3& PositionScaling, const D3DXVECTOR2& TextureScaling);


	// Helpers
	virtual bool ComputeBounds(NVComplexMeshBounds& Bounds);
	virtual bool GenerateNormals();
	virtual bool FlipNormals();
	virtual bool UnIndex();
	
	virtual tIndexArray& GetIndexArray() { return m_Indices; }
	virtual bool GenerateBasis(DWORD dwTexture);
	virtual DWORD GetTrianglesDrawn() { return m_dwNumTrianglesDrawn; }
	virtual DWORD GetVerticesDrawn() const { return m_dwNumVerticesDrawn; }
	
	virtual void SetNumVertices(DWORD dwNum) { m_dwNumVertices = dwNum; }
	virtual DWORD GetNumVertices() const { return m_dwNumVertices; }

	virtual void SetNumIndices(DWORD dwNum) { m_dwNumIndices = dwNum;}
	virtual DWORD GetNumIndices() const { return m_dwNumIndices; }
	
	// Add/find vertex data
	virtual NVComplexMeshVertexData* AddVertexData(const std::string& strName, eNVComplexMeshVertexDataType DataType, DWORD dwLocation = NVCOMPLEXMESH_NO_VERTEX);
	virtual NVComplexMeshVertexData* FindVertexData(const std::string& strName);

	virtual NVComplexMeshPass* DuplicatePass(DWORD dwSource, DWORD dwDest);
	virtual NVComplexMeshPass* AddPass(DWORD dwPass)
	{
		NVComplexMeshPass* pPass = new NVComplexMeshPass(this);
		if(m_Passes.size() <= dwPass)
		{
			m_Passes.resize(dwPass + 1);
		}
		m_Passes[dwPass] = pPass;
		return m_Passes[dwPass];
	}

	virtual void RemovePass(DWORD dwPass)
	{
		if (m_Passes.size() <= dwPass)
			return;

		SAFE_DELETE(m_Passes[dwPass]);
	}

	virtual NVComplexMeshPass* GetPass(DWORD dwPass) 
	{
		if(m_Passes.size() <= dwPass)
			return NULL;
		else 
			return m_Passes[dwPass]; 
	}

	virtual LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() { m_pVB->AddRef(); return m_pVB; }
	virtual const tIndexBufferArray& GetIndexBuffers() { return m_IndexBuffers; }

	virtual void AddTrianglesDrawn(DWORD dwNum) { m_dwNumTrianglesDrawn += dwNum; }
	virtual void AddVerticesDrawn(DWORD dwNum) { m_dwNumVerticesDrawn += dwNum; }
	virtual void ResetStats() { m_dwNumTrianglesDrawn = m_dwNumVerticesDrawn = 0; }

protected:

	friend NVComplexMeshPass;
	tMeshPass m_Passes;

	// Vertex/index data
	tVertexDataMap m_VertexData;
	DWORD m_dwStride;
	DWORD m_dwNumVertices;
	DWORD m_dwNumIndices;
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	tIndexBufferArray m_IndexBuffers;
	tIndexArray m_Indices;

	DWORD m_dwNumTrianglesDrawn;
	DWORD m_dwNumVerticesDrawn;


};




} // namespace nv_objects

#endif // __NVCOMPLEXMESH_H