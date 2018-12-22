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

#define FIRST_BONE 20.0f

struct PALETTESKIN_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float Bone1, Weight1, Bone2, Weight2;
};

DWORD PaletteSkinDeclaration[] =
{
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_NORMAL,   D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_TEXCOORD0,D3DVSDT_FLOAT4),
	D3DVSD_END()
};

#define D3DFVF_PALETTESKINVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | \
								   D3DFVF_TEX0 | D3DFVF_TEXCOORDSIZE4(0))

CTechniqueApplication::CTechniqueApplication()
{
	m_pMeshVertexBuffer     = NULL;
	m_pBoneVertexBuffer     = NULL;
	m_pMeshIndexBuffer      = NULL;
	m_pMesh                 = NULL;
	m_pMeshMaterials        = NULL;
	m_NumMaterials          = 0;
	m_SkinShader            = 0;
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
	if (FAILED(LoadMesh()))
		return FALSE;

	//Create the buffers we're actually going to use
	ExtractBuffers();

	//Create the grid visuals
	if (FAILED(CreateBones()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	SetBoneConstants();
	
	//D3DXMatrixRotationY(&m_WorldMatrix, (float)GetTickCount() / 1000.0f);
	
	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * 
		                      m_ViewMatrix * 
							  m_ProjectionMatrix;

	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Set some basic parameters
	D3DXVECTOR4 LightDir(0.0f,  -1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 Ambient (0.1f,  0.1f, 0.1f, 1.0f);
	D3DXVECTOR4 One     (1.0f,  0.0f, 0.0f, 0.0f);

	//Convert the light direction to object space and normalize
	D3DXMATRIX WorldInverse;
	D3DXMatrixInverse(&WorldInverse, NULL, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &WorldInverse);
	D3DXVec4Normalize(&LightDir, &LightDir);

	//Set the Constants
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);
	m_pD3DDevice->SetVertexShaderConstant(6, &One, 1);

	m_pD3DDevice->SetVertexShader(m_SkinShader);

	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(PALETTESKIN_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

	//Uncomment this to see the wireframe view
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//make sure the fillmode is reset for other things (like font rendering)
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//Render the control grid. Comment this out if you'd like
	RenderBones();
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

	if(FAILED(D3DXLoadMeshFromX("..\\media\\cylinder.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &pOriginalMesh)))
        return E_FAIL;

	if(FAILED(pOriginalMesh->CloneMesh(D3DXMESH_MANAGED,
							           PaletteSkinDeclaration,
                                       m_pD3DDevice, &m_pMesh)))
        return E_FAIL;
		
	pOriginalMesh->Release();
	pOriginalMesh = NULL;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_SkinShader);

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
	m_pD3DDevice->DeleteVertexShader(m_SkinShader);
	
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

	if (m_pBoneVertexBuffer)
	{
		m_pBoneVertexBuffer->Release();
		m_pBoneVertexBuffer = NULL;
	}

	if (m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	if (m_pMeshMaterials)
	{
		delete m_pMeshMaterials;
		m_pMeshMaterials = NULL;
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

	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(50.0f, 0.0f, 0.0f),
		               &D3DXVECTOR3(0.0f, -10.0f, 10.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixIdentity(&m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

	PALETTESKIN_VERTEX *pMeshVertices;

	//Lock the vertex buffer, but allow writing.
	m_pMeshVertexBuffer->Lock(0, 
		                      m_pMesh->GetNumVertices() * sizeof(PALETTESKIN_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);

	//The cylinder is 36 units long, so it's broken up into three 12 unit
	//segments...
	for (long Vertex = 0; Vertex < m_pMesh->GetNumVertices(); Vertex++)
	{
		if (pMeshVertices[Vertex].z < 12.0f)
		{
			pMeshVertices[Vertex].Bone1 = FIRST_BONE + 4.0f; 
			pMeshVertices[Vertex].Weight1 = 1.0f;
			pMeshVertices[Vertex].Bone2 = FIRST_BONE + 4.0f;
			pMeshVertices[Vertex].Weight2 = 0.0f;
		}
		else if (pMeshVertices[Vertex].z < 24.0f)
		{
			pMeshVertices[Vertex].Bone1 = FIRST_BONE + 4.0f; 
			pMeshVertices[Vertex].Weight1 = 0.0f;
			pMeshVertices[Vertex].Bone2 = FIRST_BONE + 8.0f;
			pMeshVertices[Vertex].Weight2 = 1.0f;
		}
		else
		{
			pMeshVertices[Vertex].Bone1 = FIRST_BONE + 8.0f; 
			pMeshVertices[Vertex].Weight1 = 0.0f;
			pMeshVertices[Vertex].Bone2 = FIRST_BONE + 12.0f;
			pMeshVertices[Vertex].Weight2 = 1.0f;
		}
	}

	//This smoothens out the joints. In this case, I only smooth out one side and let the
	//inner part of the joint collapse on itself. Other cases may require a different setup.
	for (Vertex = 0; Vertex < m_pMesh->GetNumVertices(); Vertex++)
	{
		if (pMeshVertices[Vertex].z > 11.0f && 
			pMeshVertices[Vertex].z < 13.0f && 
			pMeshVertices[Vertex].y > 0.0f)
		{
			float Weight = (pMeshVertices[Vertex].z - 11.0f) / 2.0f;
			pMeshVertices[Vertex].Bone1 = FIRST_BONE + 4.0f; 
			pMeshVertices[Vertex].Weight1 = 1.0f - Weight;
			pMeshVertices[Vertex].Bone2 = FIRST_BONE + 8.0f;
			pMeshVertices[Vertex].Weight2 = Weight;
		}
		if (pMeshVertices[Vertex].z > 23.0f && 
			pMeshVertices[Vertex].z < 25.0f && 
			pMeshVertices[Vertex].y > 0.0f)
		{
			float Weight = (pMeshVertices[Vertex].z - 23.0f) / 2.0f;
			pMeshVertices[Vertex].Bone1 = FIRST_BONE + 8.0f; 
			pMeshVertices[Vertex].Weight1 = 1.0f - Weight;
			pMeshVertices[Vertex].Bone2 = FIRST_BONE + 12.0f;
			pMeshVertices[Vertex].Weight2 = Weight;
		}
	}

	//Give back the buffer.
	m_pMeshVertexBuffer->Unlock();

	return S_OK;
}

HRESULT CTechniqueApplication::CreateShaders()
{	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\PaletteSkin.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(PaletteSkinDeclaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_SkinShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::RenderBones()
{
	//use the ambient light to color the data (c5 is ambient light)
	D3DXVECTOR4 LineColor (1.0f,  0.0f, 0.0f, 1.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &LineColor, 1);
	
	m_pD3DDevice->SetStreamSource(0, m_pBoneVertexBuffer, sizeof(PALETTESKIN_VERTEX));

	m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 3);

	D3DXVECTOR4 PointColor (1.0f,  1.0f, 0.0f, 1.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &PointColor, 1);

	float PointSize = 5.0f;
	m_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&PointSize));
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 4);
}

HRESULT CTechniqueApplication::CreateBones()
{
	//Create as managed so we don't have to worry about recreating it
	//if the device is lost.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(PALETTESKIN_VERTEX), 
		                                        0, D3DFVF_PALETTESKINVERTEX,
									            D3DPOOL_MANAGED,
												&m_pBoneVertexBuffer)))
		return E_FAIL;

	//Render the control grid. Comment this out if you don't care.
	PALETTESKIN_VERTEX *pVertices;

	//Lock the vertex buffer, but allow writing.
	m_pBoneVertexBuffer->Lock(0, 4 * sizeof(PALETTESKIN_VERTEX), (BYTE **)&pVertices, 0);

	pVertices[0].x = 0.0f;	pVertices[0].y = 0.0f;	pVertices[0].z =  0.0f;
	pVertices[1].x = 0.0f;	pVertices[1].y = 0.0f;	pVertices[1].z =  12.0f;
	pVertices[2].x = 0.0f;	pVertices[2].y = 0.0f;	pVertices[2].z =  24.0f;
	pVertices[3].x = 0.0f;	pVertices[3].y = 0.0f;	pVertices[3].z =  36.0f;

	
	//The bone indices are 0 based, and there are three constants
	//per bone, so we set the vertices accordingly.
	pVertices[0].Bone1 = FIRST_BONE + 0.0f;  pVertices[0].Weight1 = 1.0f;
	pVertices[0].Bone2 = FIRST_BONE + 0.0f;  pVertices[0].Weight2 = 0.0f;

	pVertices[1].Bone1 = FIRST_BONE + 4.0f;  pVertices[1].Weight1 = 1.0f;
	pVertices[1].Bone2 = FIRST_BONE + 4.0f;  pVertices[1].Weight2 = 0.0f;

	pVertices[2].Bone1 = FIRST_BONE + 8.0f;  pVertices[2].Weight1 = 1.0f;
	pVertices[2].Bone2 = FIRST_BONE + 8.0f;  pVertices[2].Weight2 = 0.0f;

	pVertices[3].Bone1 = FIRST_BONE + 12.0f; pVertices[3].Weight1 = 1.0f;
	pVertices[3].Bone2 = FIRST_BONE + 12.0f; pVertices[3].Weight2 = 0.0f;

	m_pBoneVertexBuffer->Unlock();

	return S_OK;
}

void CTechniqueApplication::SetBoneConstants()
{
	D3DXMATRIX Bone1;
	D3DXMATRIX Bone2;
	D3DXMATRIX Bone3;
	D3DXMATRIX Bone4;

	//our good friend the general purpose animator...
	float Time = (float)GetTickCount() / 2000.0f;
	float Angle = (D3DX_PI / 2.0f) * fabs(sin(Time));

	D3DXMATRIX PosOffset;
	D3DXMATRIX NegOffset;
	D3DXMatrixTranslation(&PosOffset, 0.0f, 0.0f,  12.0f);
	D3DXMatrixTranslation(&NegOffset, 0.0f, 0.0f, -12.0f);


	//This code is just here to mimic the setup that would normally be done 
	//in a modelling tool. It generates a simple rotation matrix, and then...
	D3DXMATRIX SimpleRotate;
	D3DXMatrixRotationX(&SimpleRotate, Angle);

	//Move the base bone a little bit. If this were an arm, we'd be moving
	//the shoulder.
	D3DXMatrixTranslation(&Bone1, 0.0f, 5.0f * sin(Time), 0.0f);

	//Rotate the "shoulder"
	Bone2 = SimpleRotate * Bone1;

	//Offset the matrix along the arm so that the rotation happens
	//at the "elbow"
	Bone3 = SimpleRotate * PosOffset * Bone2;

	//Offset again and rotate at the "wrist"
	Bone4 = SimpleRotate * PosOffset * Bone3;

	//Undo the translation offsets so that the translation does not
	//affect the actual model.
	Bone3 = NegOffset * Bone3;
	Bone4 = NegOffset * NegOffset * Bone4;

	//Transpose the matrices and send them out...
	D3DXMatrixTranspose(&Bone1, &Bone1);
	D3DXMatrixTranspose(&Bone2, &Bone2);
	D3DXMatrixTranspose(&Bone3, &Bone3);
	D3DXMatrixTranspose(&Bone4, &Bone4);

	m_pD3DDevice->SetVertexShaderConstant(FIRST_BONE + 0,  &Bone1, 4);
	m_pD3DDevice->SetVertexShaderConstant(FIRST_BONE + 4,  &Bone2, 4);
	m_pD3DDevice->SetVertexShaderConstant(FIRST_BONE + 8,  &Bone3, 4);
	m_pD3DDevice->SetVertexShaderConstant(FIRST_BONE + 12, &Bone4, 4);

}
