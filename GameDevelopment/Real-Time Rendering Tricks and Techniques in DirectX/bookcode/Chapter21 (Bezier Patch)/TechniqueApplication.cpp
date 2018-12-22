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

struct BEZIER_VERTEX
{
	float x, y, z;
	float Bs0, Bs1, Bs2, Bs3;
	float Bt0, Bt1, Bt2, Bt3;
	float dBs0, dBs1, dBs2, dBs3;
	float dBt0, dBt1, dBt2, dBt3;
};

DWORD BezierDeclaration[] =
{
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_TEXCOORD0,D3DVSDT_FLOAT4),
	D3DVSD_REG(D3DVSDE_TEXCOORD1,D3DVSDT_FLOAT4),
	D3DVSD_REG(D3DVSDE_TEXCOORD2,D3DVSDT_FLOAT4),
	D3DVSD_REG(D3DVSDE_TEXCOORD3,D3DVSDT_FLOAT4),
	D3DVSD_END()
};

struct CONTROL_VERTEX
{
	float x, y, z;
};

#define D3DFVF_CONTROLVERTEX (D3DFVF_XYZ)

DWORD ControlDeclaration[] =
{
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_END()
};


CTechniqueApplication::CTechniqueApplication()
{
	m_pMeshVertexBuffer     = NULL;
	m_pControlVertexBuffer  = NULL;
	m_pMeshIndexBuffer      = NULL;
	m_pControlIndexBuffer   = NULL;
	m_pMesh                 = NULL;
	m_pMeshMaterials        = NULL;
	m_NumMaterials          = 0;
	m_BezierShader          = 0;
	m_ControlShader         = 0;
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
	if (FAILED(CreateGridVisuals()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	D3DXMatrixScaling(&m_WorldMatrix, 5.0f, 5.0f, 5.0f);

	D3DXMATRIX Rotation;

	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);

	m_WorldMatrix *= Rotation;
	
	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * 
		                      m_ViewMatrix * 
							  m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	D3DXVECTOR4 LightDir(0.0f, -1.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &LightDir, 1);

	D3DXVECTOR4 Correction(-0.5f, 0.0f, -0.5f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(6, &Correction, 1);

	float Warp1 = 2.0f * sin((float)GetTickCount() / 1000.0f);
	float Warp2 = 2.0f * cos((float)GetTickCount() / 1000.0f);

	//Set the control grid constants and rows in the T direction
	D3DXVECTOR4 ControlS0T0(0.0f, 0.25f * Warp1, 0.0f, 1.0f);
	D3DXVECTOR4 ControlS0T1(0.0f, 0.0f, 0.33f, 1.0f);
	D3DXVECTOR4 ControlS0T2(0.0f, 0.0f, 0.66f, 1.0f);
	D3DXVECTOR4 ControlS0T3(0.0f, 0.25f * Warp1, 1.0f, 1.0f);

	D3DXVECTOR4 ControlS1T0(0.33f, 0.0f, 0.0f, 1.0f);
	D3DXVECTOR4 ControlS1T1(0.33f, 0.33f * Warp1 + 0.33 * Warp2, 0.33f, 1.0f);
	D3DXVECTOR4 ControlS1T2(0.33f, -0.33f * Warp1 + 0.66f * Warp2, 0.66f, 1.0f);
	D3DXVECTOR4 ControlS1T3(0.33f, 0.0f, 1.0f, 1.0f);

	D3DXVECTOR4 ControlS2T0(0.66f, 0.0f, 0.0f, 1.0f);
	D3DXVECTOR4 ControlS2T1(0.66f, 0.66f * Warp1 + 0.33 * Warp2, 0.33f, 1.0f);
	D3DXVECTOR4 ControlS2T2(0.66f, -0.66f * Warp1 + 0.66f * Warp2, 0.66f, 1.0f);
	D3DXVECTOR4 ControlS2T3(0.66f, 0.0f, 1.0f, 1.0f);

	D3DXVECTOR4 ControlS3T0(1.0f, 0.25f * Warp2, 0.0f, 1.0f);
	D3DXVECTOR4 ControlS3T1(1.0f, 0.0f, 0.33f, 1.0f);
	D3DXVECTOR4 ControlS3T2(1.0f, 0.0f, 0.66f, 1.0f);
	D3DXVECTOR4 ControlS3T3(1.0f, 0.25f * Warp2, 1.0f, 1.0f);

	m_pD3DDevice->SetVertexShaderConstant(10, &ControlS0T0, 1);
	m_pD3DDevice->SetVertexShaderConstant(11, &ControlS0T1, 1);
	m_pD3DDevice->SetVertexShaderConstant(12, &ControlS0T2, 1);
	m_pD3DDevice->SetVertexShaderConstant(13, &ControlS0T3, 1);
	m_pD3DDevice->SetVertexShaderConstant(14, &ControlS1T0, 1);
	m_pD3DDevice->SetVertexShaderConstant(15, &ControlS1T1, 1);
	m_pD3DDevice->SetVertexShaderConstant(16, &ControlS1T2, 1);
	m_pD3DDevice->SetVertexShaderConstant(17, &ControlS1T3, 1);
	m_pD3DDevice->SetVertexShaderConstant(18, &ControlS2T0, 1);
	m_pD3DDevice->SetVertexShaderConstant(19, &ControlS2T1, 1);
	m_pD3DDevice->SetVertexShaderConstant(20, &ControlS2T2, 1);
	m_pD3DDevice->SetVertexShaderConstant(21, &ControlS2T3, 1);
	m_pD3DDevice->SetVertexShaderConstant(22, &ControlS3T0, 1);
	m_pD3DDevice->SetVertexShaderConstant(23, &ControlS3T1, 1);
	m_pD3DDevice->SetVertexShaderConstant(24, &ControlS3T2, 1);
	m_pD3DDevice->SetVertexShaderConstant(25, &ControlS3T3, 1);


	m_pD3DDevice->SetVertexShader(m_BezierShader);

	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(BEZIER_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

	//Uncomment this to see the wireframe view
	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//make sure the fillmode is reset for other things (like font rendering)
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//Render the control grid. Comment this out if you'd like
	RenderControlGrid();
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\BezierPlane.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &pOriginalMesh)))
        return E_FAIL;

	if(FAILED(pOriginalMesh->CloneMesh(D3DXMESH_MANAGED,
							           BezierDeclaration,
                                       m_pD3DDevice, &m_pMesh)))
        return E_FAIL;
		
	pOriginalMesh->Release();
	pOriginalMesh = NULL;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_BezierShader);
	m_pD3DDevice->DeleteVertexShader(m_ControlShader);

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
	m_pD3DDevice->DeleteVertexShader(m_BezierShader);
	m_pD3DDevice->DeleteVertexShader(m_ControlShader);
	
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

	if (m_pControlVertexBuffer)
	{
		m_pControlVertexBuffer->Release();
		m_pControlVertexBuffer = NULL;
	}

	if (m_pControlIndexBuffer)
	{
		m_pControlIndexBuffer->Release();
		m_pControlIndexBuffer  = NULL;
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

	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0f, 5.0f, -10.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixIdentity(&m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

	BEZIER_VERTEX *pMeshVertices;

	//Lock the vertex buffer, but allow writing.
	m_pMeshVertexBuffer->Lock(0, 
		                      m_pMesh->GetNumVertices() * sizeof(BEZIER_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);

	for (long Vertex = 0; Vertex < m_pMesh->GetNumVertices(); Vertex++)
	{
		//The X and Z values contain the s and t values for the patch.
		//We will use those values to write the bezier basis functions.
		//This could theoretically work for any model in a unit cube.
		float S = pMeshVertices[Vertex].x + 0.5;
		float T = pMeshVertices[Vertex].z + 0.5;

		pMeshVertices[Vertex].Bs0 = (1.0f - S) * (1.0f - S) * (1.0f - S);
		pMeshVertices[Vertex].Bs1 = 3.0f * S * (1.0f - S) * (1.0f - S);
		pMeshVertices[Vertex].Bs2 = 3.0f * S * S * (1.0f - S);
		pMeshVertices[Vertex].Bs3 = S * S * S;

		pMeshVertices[Vertex].Bt0 = (1.0f - T) * (1.0f - T) * (1.0f - T);
		pMeshVertices[Vertex].Bt1 = 3.0f * T * (1.0f - T) * (1.0f - T);
		pMeshVertices[Vertex].Bt2 = 3.0f * T * T * (1.0f - T);
		pMeshVertices[Vertex].Bt3 = T * T * T;

		//For the normals
		pMeshVertices[Vertex].dBs0 = (6.0f * S) - (3.0f * S * S) - 3.0f;
		pMeshVertices[Vertex].dBs1 = 3.0f - (12.0f * S) + (9.0f * S * S);
		pMeshVertices[Vertex].dBs2 = (6.0f * S) - (9.0f * S * S);
		pMeshVertices[Vertex].dBs3 = 3.0f * S * S;

		pMeshVertices[Vertex].dBt0 = (6.0f * T) - (3.0f * T * T) - 3.0f;
		pMeshVertices[Vertex].dBt1 = 3.0f - (12.0f * T) + (9.0f * T * T);
		pMeshVertices[Vertex].dBt2 = (6.0f * T) - (9.0f * T * T);
		pMeshVertices[Vertex].dBt3 = 3.0f * T * T;
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
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\bezier.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(BezierDeclaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BezierShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Create the shader that will show the control grid
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\beziercontrol.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(ControlDeclaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_ControlShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::RenderControlGrid()
{
	CONTROL_VERTEX *pVertices;

	//Lock the vertex buffer, but allow writing.
	m_pControlVertexBuffer->Lock(0, 16 * sizeof(CONTROL_VERTEX),
                                 (BYTE **)&pVertices, 0);

	for (long Vertex = 0; Vertex < 16; Vertex++)
	{
		//Retrieve the constant. A bit heavy handed, but we're guaranteed consistency
		//and speed really isn't the issue...
		D3DXVECTOR4 Temp;
		m_pD3DDevice->GetVertexShaderConstant(10 + Vertex, &Temp, 1);
		pVertices[Vertex].x = Temp.x - 0.5f;
		pVertices[Vertex].y = Temp.y;
		pVertices[Vertex].z = Temp.z - 0.5f;
	}

	m_pControlVertexBuffer->Unlock();

	//Set the color (we assume that the transformation matrices
	//were set somewhere else.
	D3DXVECTOR4 LineColor(1.0f, 0.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(4, &LineColor, 1);

	m_pD3DDevice->SetVertexShader(m_ControlShader);
	m_pD3DDevice->SetIndices(m_pControlIndexBuffer, 0);
	m_pD3DDevice->SetStreamSource(0, m_pControlVertexBuffer, sizeof(CONTROL_VERTEX));

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 
									   16, 0, 24);

	D3DXVECTOR4 PointColor(1.0f, 1.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(4, &PointColor, 1);

	float PointSize = 5.0f;
	m_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&PointSize));
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 16);
}

HRESULT CTechniqueApplication::CreateGridVisuals()
{
	//Create as managed so we don't have to worry about recreating it
	//if the device is lost.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(16 * sizeof(CONTROL_VERTEX), 
		                                        0, D3DFVF_CONTROLVERTEX,
									            D3DPOOL_MANAGED,
												&m_pControlVertexBuffer)))
		return E_FAIL;

	//Create the Index Buffer for the control points
	if (FAILED(m_pD3DDevice->CreateIndexBuffer(48 * sizeof(short), 
		                                       0, D3DFMT_INDEX16,
									           D3DPOOL_MANAGED,
											&m_pControlIndexBuffer)))
		return E_FAIL;

	//lock the index buffer once - these are hardcoded values
	short *pIndex;
	m_pControlIndexBuffer->Lock(0, 48 * sizeof(short), (BYTE**)&pIndex, 0);
	short Indices[] = {0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10,
		               10, 11, 12, 13, 13, 14, 14, 15, 0, 4, 4, 8, 8, 12,
					   1, 5, 5, 9, 9, 13, 2, 6, 6, 10, 10, 14, 3, 7, 7,
					   11, 11, 15};
	memcpy(pIndex, &Indices, 48 * sizeof(short));
	m_pControlIndexBuffer->Unlock();
}
