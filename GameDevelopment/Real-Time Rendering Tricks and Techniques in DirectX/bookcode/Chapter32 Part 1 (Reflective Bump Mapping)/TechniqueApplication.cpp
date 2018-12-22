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

DWORD Declaration[] =
{
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_NORMAL,    D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT3),
	D3DVSD_END()
};


struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float Ux, Uy, Uz;
	float Vx, Vy, Vz;
	float UVx, UVy, UVz;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pNormalMap               = NULL;
	m_pCubeTexture             = NULL;
	m_BumpShader               = 0;
	m_BumpPixelShader          = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	D3DCAPS8 Caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
	if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1) &&
		Caps.PixelShaderVersion == D3DPS_VERSION(1,1))
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
						  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
			return FALSE;
	}
	else
	{
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_REF, 
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING)))
			return FALSE;
	}

	//Do the basic camera positioning, etc.
	SetupDevice();
	
	//Load the mesh object
	LoadMesh();

	//Create the buffers we're actually going to use
	ExtractBuffers();

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\Bumpnormal.dds", 
										 &m_pNormalMap)))
		return FALSE;

	if (FAILED(D3DXCreateCubeTextureFromFile(m_pD3DDevice, 
		                                     "..\\media\\cubemap.dds", 
										     &m_pCubeTexture)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the eye position
	D3DXVECTOR4 EyePos(-60.0, 30.0f, 0.0f, 0.0f);

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_BumpShader);

	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Set the light direction
	float Time = (float)GetTickCount() / 1000.0f;
	D3DXVECTOR4 LightDir = D3DXVECTOR4(sin(Time), -fabs(cos(Time)), 0.0f, 0.0f);
	D3DXMATRIX InverseWorld;
	D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
	D3DXVec3Transform(&LightDir, &(D3DXVECTOR3)LightDir, &InverseWorld);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);	

	//Send the world matrix to the shader (Presently, the world matrix
	//is an identity matrix)
	D3DXMatrixTranspose(&m_WorldMatrix, &m_WorldMatrix);
	m_pD3DDevice->SetVertexShaderConstant(10, &m_WorldMatrix, 4);

	//Pass the eye position in
	m_pD3DDevice->SetVertexShaderConstant(5, &EyePos, 1);

	//Some helpers
	D3DXVECTOR4 Helpers (0.5f, 0.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(6, &Helpers, 1);

	m_pD3DDevice->SetTexture(0, m_pNormalMap);
	m_pD3DDevice->SetTexture(1, NULL);
	m_pD3DDevice->SetTexture(2, NULL);
	m_pD3DDevice->SetTexture(3, m_pCubeTexture);

	m_pD3DDevice->SetPixelShader(m_BumpPixelShader);

	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	m_pD3DDevice->SetPixelShader(0);
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	DWORD        NumMaterials;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\bumpsphere.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&NumMaterials, &pOriginalMesh)))
        return FALSE;

	//The format ain't gonna match...
	pOriginalMesh->CloneMesh(D3DXMESH_MANAGED,
							 Declaration,
                             m_pD3DDevice, &m_pMesh);
	
	pOriginalMesh->Release();
	pOriginalMesh = NULL;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BumpShader);
	m_pD3DDevice->DeletePixelShader(m_BumpPixelShader);

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
	m_pD3DDevice->DeleteVertexShader(m_BumpShader);
	m_pD3DDevice->DeletePixelShader(m_BumpPixelShader);
	
	//Clean up
	if (m_pMeshVertexBuffer)
	{
		m_pMeshVertexBuffer->Release();
		m_pMeshVertexBuffer = NULL;
	}

	if (m_pMeshIndexBuffer)
	{
		m_pMeshIndexBuffer->Release();
		m_pMeshIndexBuffer  = NULL;
	}

	if (m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	if (m_pNormalMap)
	{
		m_pNormalMap->Release();
		m_pNormalMap = NULL;
	}

	if (m_pCubeTexture)
	{
		m_pCubeTexture->Release();
		m_pCubeTexture = NULL;
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
	//Let D3DX compute the basis vectors
	D3DXComputeTangent(m_pMesh, 0, m_pMesh, 1, 2, TRUE, NULL);
	
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

	MESH_VERTEX *pMeshVertices;

	//Lock the vertex buffer, but allow writing.
	m_pMeshVertexBuffer->Lock(0, 
		                      m_pMesh->GetNumVertices() * sizeof(MESH_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);

	//Go through each vertex and set UxV
	for (long i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		MESH_VERTEX *pVertex = &pMeshVertices[i];

		D3DXVECTOR3 U(pVertex->Ux, pVertex->Uy, pVertex->Uz);
		D3DXVECTOR3 V(pVertex->Vx, pVertex->Vy, pVertex->Vz);

		D3DXVECTOR3 UxV;
		D3DXVec3Cross(&UxV, &U, &V);

		//Make sure UxV is in the same direction as the normal
		if (D3DXVec3Dot(&UxV, &D3DXVECTOR3(pVertex->nx, pVertex->ny, pVertex->nz)) < 0.0f)
			UxV = -UxV;

		//Now, set each vertex component to the vectors
		pVertex->UVx = UxV.x;
		pVertex->UVy = UxV.y;
		pVertex->UVz = UxV.z;
	}

	//Give back all of our buffers.
	m_pMeshVertexBuffer->Unlock();

	return S_OK;
}

HRESULT CTechniqueApplication::CreateShaders()
{	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\ReflectiveBumpMap.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BumpShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Assemble and create the pixel shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\ReflectiveBumpMapping.psh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreatePixelShader(
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BumpPixelShader)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();



	return S_OK;
}
