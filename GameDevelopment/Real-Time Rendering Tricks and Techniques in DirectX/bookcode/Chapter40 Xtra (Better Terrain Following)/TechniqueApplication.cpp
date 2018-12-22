/***************************************************************
* TechniqueApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* TechniqueApplication class.    	        				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "TechniqueApplication.h"

#define LOOK_AHEAD 0.5f

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pTerrainVertexBuffer     = NULL;
	m_pTerrainIndexBuffer      = NULL;
	m_pTerrainMesh             = NULL;
	m_pCubeVertexBuffer        = NULL;
	m_pCubeIndexBuffer         = NULL;
	m_pCubeMesh                = NULL;
	m_BasicShader              = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	D3DCAPS8 Caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
	if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1))
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
			return FALSE;
	}
	else
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING)))
			return FALSE;
	}

	//Do the basic camera positioning, etc.
	SetupDevice();
	
	//Load the mesh object
	LoadMeshes();

	//Create the buffers we're actually going to use
	ExtractBuffers();

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the eye position
	D3DXVECTOR4 EyePos(10.0, 2.5f, 00.0f, 0.0f);

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 2.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));


	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_BasicShader);

	//Set some lighting constants
	D3DXVECTOR4 Ambient    (0.1,  0.1f, 0.1f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

	//Set the light direction
	D3DXVECTOR4 LightDir = D3DXVECTOR4(0.0f, -1.0f, 0.0f, 0.0f);
	D3DXMATRIX InverseWorld;
	D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);


	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Render the terrain
	m_pD3DDevice->SetStreamSource(0, m_pTerrainVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pTerrainIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pTerrainMesh->GetNumVertices(), 0,
									   m_pTerrainMesh->GetNumFaces());

	D3DXVECTOR3 Position(0.0f, 0.0f, 2.0f * sin((float)GetTickCount() / 2000.0f));
	D3DXVECTOR3 Position2(0.0f, 0.0f, 2.0f * sin(1.0f + (float)GetTickCount() / 2000.0f));
	D3DXVECTOR3 Direction = Position2 - Position;
	D3DXVec3Normalize(&Direction, &Direction);
	D3DXMATRIX CubeMatrix;
	GetTransformation(&Position, &Direction, &CubeMatrix);

	ShaderMatrix = CubeMatrix * m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);

	//render the cube
	m_pD3DDevice->SetStreamSource(0, m_pCubeVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pCubeIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pCubeMesh->GetNumVertices(), 0,
									   m_pCubeMesh->GetNumFaces());
}

HRESULT CTechniqueApplication::LoadMeshes()
{
	LPD3DXMESH    pOriginalMesh;

	if(FAILED(D3DXLoadMeshFromX("..\\media\\TerrainRamp.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, NULL,
								NULL, &pOriginalMesh)))
        return FALSE;

	if (pOriginalMesh->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pTerrainMesh);
		
		pOriginalMesh->Release();
		pOriginalMesh = NULL;
	}
	else
		m_pTerrainMesh = pOriginalMesh;

	LPD3DXMESH    pOriginalMesh2;

	if(FAILED(D3DXLoadMeshFromX("..\\media\\smallcube.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, NULL,
								NULL, &pOriginalMesh2)))
        return FALSE;

	if (pOriginalMesh2->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh2->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pCubeMesh);
		
		pOriginalMesh2->Release();
		pOriginalMesh2 = NULL;
	}
	else
		m_pCubeMesh = pOriginalMesh2;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);

	return TRUE;
}

BOOL CTechniqueApplication::PostReset()
{
	SetupDevice();

	//Recreate the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}


BOOL CTechniqueApplication::PreTerminate()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);
	
	//Clean up
	if (m_pTerrainVertexBuffer)
	{
		m_pTerrainVertexBuffer->Release();
		m_pTerrainVertexBuffer = NULL;
	}

	if (m_pTerrainIndexBuffer)
	{
		m_pTerrainIndexBuffer->Release();
		m_pTerrainIndexBuffer  = NULL;
	}

	if (m_pTerrainMesh)
	{
		m_pTerrainMesh->Release();
		m_pTerrainMesh = NULL;
	}

	if (m_pCubeVertexBuffer)
	{
		m_pCubeVertexBuffer->Release();
		m_pCubeVertexBuffer = NULL;
	}

	if (m_pCubeIndexBuffer)
	{
		m_pCubeIndexBuffer->Release();
		m_pCubeIndexBuffer  = NULL;
	}

	if (m_pCubeMesh)
	{
		m_pCubeMesh->Release();
		m_pCubeMesh = NULL;
	}

	return TRUE;
}


void CTechniqueApplication::SetupDevice()
{
	//Do all the basic setup
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 1000.0f);

	D3DXMatrixIdentity(&m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pTerrainMesh->GetVertexBuffer(&m_pTerrainVertexBuffer);
	m_pTerrainMesh->GetIndexBuffer(&m_pTerrainIndexBuffer);
	m_pCubeMesh->GetVertexBuffer(&m_pCubeVertexBuffer);
	m_pCubeMesh->GetIndexBuffer(&m_pCubeIndexBuffer);

	//Color the terrain
	MESH_VERTEX *pMeshVertices;

	//Lock the vertex buffer, but allow writing.
	m_pTerrainVertexBuffer->Lock(0, m_pTerrainMesh->GetNumVertices() * sizeof(MESH_VERTEX),
		                        (BYTE **)&pMeshVertices, 0);

	//Set the general terrain color
	D3DXCOLOR Diffuse = 0xff00ff00;

	//Set all the terrain vertices
	for (long Vertex = 0; Vertex < m_pTerrainMesh->GetNumVertices(); Vertex++)
	{
		pMeshVertices[Vertex].color = Diffuse;
	}

	//Give back all of our buffers.
	m_pTerrainVertexBuffer->Unlock();

	//Lock the vertex buffer, but allow writing.
	m_pCubeVertexBuffer->Lock(0, m_pCubeMesh->GetNumVertices() * sizeof(MESH_VERTEX),
		                        (BYTE **)&pMeshVertices, 0);

	//Set the general Cube color
	Diffuse = 0xffffffff;

	//Set all the Cube vertices
	for (Vertex = 0; Vertex < m_pCubeMesh->GetNumVertices(); Vertex++)
	{
		pMeshVertices[Vertex].color = Diffuse;
	}

	//Give back all of our buffers.
	m_pCubeVertexBuffer->Unlock();

	return S_OK;
}

HRESULT CTechniqueApplication::CreateShaders()
{
	//Set up the declaration to match the FVF and to
	//read from stream zero.
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION,D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_NORMAL,  D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
		D3DVSD_END()
	};

	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\Basic.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BasicShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::PreRender()
{
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}



float CTechniqueApplication::GetHeightAt(float X, float Z, 
										 D3DXQUATERNION *pRotation)
{
	//Does the ray intersect? (For this sample the answer should always be yes)
	BOOL Intersection;

	//Pick a starting point far above the terrain
	D3DXVECTOR3 RayStart(X, 10000.0f, Z);

	//The ray points straight down
	D3DXVECTOR3 RayDir(0.0f, -1.0f, 0.0f);

	//Which face?
	DWORD FaceIndex;

	//hit coordinates
	float U, V, Distance;
	ID3DXBuffer*     pHitList;
    DWORD            HitCount;

	D3DXIntersect(m_pTerrainMesh, &RayStart, &RayDir, &Intersection,
		          &FaceIndex, &U, &V, &Distance, &pHitList, &HitCount);

	if (Intersection)
	{
		MESH_VERTEX *pMeshVertices;
		short       *pIndices;
		
		//lock the buffers
		m_pTerrainVertexBuffer->Lock(0, 
								  m_pTerrainMesh->GetNumVertices() * sizeof(MESH_VERTEX),
								  (BYTE **)&pMeshVertices, D3DLOCK_READONLY);

		m_pTerrainIndexBuffer->Lock(0, 3 * m_pTerrainMesh->GetNumFaces() * sizeof(short),
								 (BYTE **)&pIndices, D3DLOCK_READONLY);

		//Get the three vertices
		MESH_VERTEX Vertex0 = pMeshVertices[pIndices[FaceIndex * 3 + 0]];
		MESH_VERTEX Vertex1 = pMeshVertices[pIndices[FaceIndex * 3 + 1]];
		MESH_VERTEX Vertex2 = pMeshVertices[pIndices[FaceIndex * 3 + 2]];

		//Give everything back
		m_pTerrainVertexBuffer->Unlock();
		m_pTerrainIndexBuffer->Unlock();

		//Convert the vertices to D3DX vectors for easier math
		D3DXVECTOR3 V0(Vertex0.x, Vertex0.y, Vertex0.z);
		D3DXVECTOR3 V1(Vertex1.x, Vertex1.y, Vertex1.z);
		D3DXVECTOR3 V2(Vertex2.x, Vertex2.y, Vertex2.z);
		
		//Compute the actual intersection point
		D3DXVECTOR3 IntersectionPoint = V0 + U * (V1 - V0) + V * (V2 - V0);

		//Get the face normal from the plane equation
		D3DXPLANE Plane;
		D3DXPlaneFromPoints(&Plane, &V0, &V1, &V2);
		D3DXVECTOR3 PlaneNormal(Plane.a, Plane.b, Plane.c);
		D3DXVECTOR3 CubeNormal(0.0f, 1.0f, 0.0f);

		D3DXVECTOR3 Axis;
		D3DXVec3Cross(&Axis, &CubeNormal, &PlaneNormal);
		float Angle = acos(D3DXVec3Dot(&CubeNormal, &PlaneNormal));

		//Set the matrix
		D3DXQuaternionRotationAxis(pRotation, &Axis, Angle);

		//Return the height
		return IntersectionPoint.y;
	}
	else
		return 0.0f;
}


void CTechniqueApplication::GetTransformation(D3DXVECTOR3 *pCurrentPosition,
											  D3DXVECTOR3 *pDirection,
											  D3DXMATRIX *pTransform)
{
	//Get all the information about the current state
	D3DXQUATERNION CurrentRotation;
	pCurrentPosition->y = GetHeightAt(pCurrentPosition->x, pCurrentPosition->z,
		                              &CurrentRotation);

	//Get all the information about the future state
	D3DXVECTOR3 FuturePosition = *pCurrentPosition + (LOOK_AHEAD * *pDirection);
	D3DXQUATERNION FutureRotation;
	FuturePosition.y = GetHeightAt(FuturePosition.x, FuturePosition.z,
		                           &FutureRotation);

	//working variable for quaternion transformations
	D3DXMATRIX TempTransform;

	//Rotate the direction vector by the future rotation to get
	//the actual plane aligned direction
	D3DXMatrixRotationQuaternion(&TempTransform, &FutureRotation);
	D3DXVECTOR3 FutureAligned;
	D3DXVec3TransformNormal(&FutureAligned, pDirection, &TempTransform);
	D3DXVec3Normalize(&FutureAligned, &FutureAligned);

	//Get the vector between the current and future position
	D3DXVECTOR3 StraightShot = (FuturePosition - *pCurrentPosition);
	D3DXVec3Normalize(&StraightShot, &StraightShot);

	//Get the dot product of the final direction and the straightshot
	float Interpolator = D3DXVec3Dot(&FutureAligned, &StraightShot);

	//translate according to the height interpolation
	D3DXMatrixTranslation(pTransform, pCurrentPosition->x, 
		                  pCurrentPosition->y, pCurrentPosition->z);

	//interpolate the rotation
	D3DXQUATERNION Slerp;
	D3DXQuaternionSlerp(&Slerp, &CurrentRotation, &FutureRotation, Interpolator);
	D3DXMatrixRotationQuaternion(&TempTransform, &Slerp);

	//concatenate
	*pTransform = TempTransform * *pTransform;
}
