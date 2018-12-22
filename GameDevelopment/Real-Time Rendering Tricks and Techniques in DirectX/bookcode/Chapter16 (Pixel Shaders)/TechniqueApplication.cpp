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
	D3DVSD_END()
};

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pPlaneVertexBuffer       = NULL;
	m_pTexture                 = NULL;
	m_SetupShader              = 0;
	m_SimplePixelShader        = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	D3DCAPS8 Caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
	if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1) && 
		Caps.VertexShaderVersion == D3DPS_VERSION(1,1))
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
	
	//Create the plane
	if (FAILED(CreatePlaneBuffer()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\Tile.dds", 
										 &m_pTexture)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_SetupShader);

	float Time = (float)GetTickCount() / 1000.0f;

	//Set the light direction and the ambient light value
	D3DXVECTOR4 LightDir = D3DXVECTOR4(sin(Time), -fabs(cos(Time)), 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);
	D3DXVECTOR4 Ambient    (0.1,  0.1f, 0.1f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);
	
	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	m_pD3DDevice->SetTexture(0, m_pTexture);

	m_pD3DDevice->SetPixelShader(m_SimplePixelShader);

	m_pD3DDevice->SetStreamSource(0, m_pPlaneVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	m_pD3DDevice->SetPixelShader(0);
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_SetupShader);
	m_pD3DDevice->DeletePixelShader(m_SimplePixelShader);

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
	m_pD3DDevice->DeleteVertexShader(m_SetupShader);
	m_pD3DDevice->DeletePixelShader(m_SimplePixelShader);
	
	//Clean up
	if (m_pPlaneVertexBuffer)
	{
		m_pPlaneVertexBuffer->Release();
		m_pPlaneVertexBuffer = NULL;
	}

	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
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

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(-60.0, 30.0f, 0.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}

HRESULT CTechniqueApplication::CreateShaders()
{	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\shaders\\PixelSetup.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_SetupShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Assemble and create the pixel shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\shaders\\Simple.psh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreatePixelShader(
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_SimplePixelShader)))
	{
		MessageBox(NULL, "The pixel shader could not be created. Quitting...", "", 0);
		return E_FAIL;
	}

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

HRESULT CTechniqueApplication::CreatePlaneBuffer()
{
	//Create as managed so we don't have to worry about recreating it
	//if the device is lost.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(MESH_VERTEX), 
		                                        0, D3DFVF_MESHVERTEX,
									            D3DPOOL_MANAGED,
												&m_pPlaneVertexBuffer)))
		return E_FAIL;

	//Create a set of 4 vertices for the plane
	MESH_VERTEX *pVertices;

	//Lock the vertex buffer, but allow writing.
	m_pPlaneVertexBuffer->Lock(0, 4 * sizeof(MESH_VERTEX), (BYTE **)&pVertices, 0);

	//Initialize everything to zero. If I don't set a certain attribute,
	//assume it's zero..
	memset(pVertices, 0x00, 4 * sizeof(MESH_VERTEX));

	//WARNING - this is a bit confusing. I'm setting X and Z positions, but
	//I'm setting the y *Normal* value. If you don't read closely, it 
	//might be very confusing.
	pVertices[0].x = -30.0f; pVertices[0].ny = 1.0f; pVertices[0].z = -30.0f;
	pVertices[0].u = 0.0f; pVertices[0].v = 0.0f;
	pVertices[1].x = -30.0f; pVertices[1].ny = 1.0f; pVertices[1].z =  30.0f;
	pVertices[1].u = 0.0f; pVertices[1].v = 5.0f;
	pVertices[2].x =  30.0f; pVertices[2].ny = 1.0f; pVertices[2].z = -30.0f;
	pVertices[2].u = 5.0f; pVertices[2].v = 0.0f;
	pVertices[3].x =  30.0f; pVertices[3].ny = 1.0f; pVertices[3].z =  30.0f;
	pVertices[3].u = 5.0f; pVertices[3].v = 5.0f;

	m_pPlaneVertexBuffer->Unlock();

	return S_OK;
}

