/***************************************************************
* GeometryApplication.h                                        *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of geometry.  For this file to        *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

//Move the structure definition to the header so that the
//member functions can be defined.
struct GEOMETRY_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
};

class CGeometryApplication : public CHostApplication  
{
public:
	void SetupDevice();
	void RenderMesh();
	BOOL LoadMesh();
	//These functions initialize all of our vertices
	BOOL CreateGeometry();
	void DestroyGeometry();

	void InitializeIndexed();
	void InitializeList(GEOMETRY_VERTEX *pVertices);
	void InitializeStrip(GEOMETRY_VERTEX *pVertices);
	void InitializeFan(GEOMETRY_VERTEX *pVertices);
	
	//This function handles lighting.  Lighting will
	//be explained more in later chapters.
	void InitializeLights();

	CGeometryApplication();
	virtual ~CGeometryApplication();

	BOOL FillVertexBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	//The vertex buffer will hold the vertices for all of our 
	//triangles.  The index buffer is used for the indexed 
	//primitives.  The mesh object is our loaded .X file.
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer;
	LPD3DXMESH m_pMesh;

	//This is the basic material we will apply to all of our 
	//shapes.
	D3DMATERIAL8 m_ShapeMaterial;

	//This is the array of materials used by the mesh object
	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;


	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
