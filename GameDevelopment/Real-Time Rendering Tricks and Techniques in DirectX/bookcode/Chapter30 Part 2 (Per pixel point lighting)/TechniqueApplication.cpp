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

#define D3DFVF_BOXVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

struct BOX_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pBoxVertexBuffer         = NULL;
	m_pBoxIndexBuffer          = NULL;
	m_PointLightShader         = 0;
	m_AttenuationShader        = 0;
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
		if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_REF, 
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING)))
			return FALSE;
	}

	//Do the basic camera positioning, etc.
	SetupDevice();
	
	//Set up the box that will be the environment
	if (FAILED(SetupEnvironment()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\PointLight.dds",
										 &m_pPointLightTexture)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the main texture and all the texture stage states.
	m_pD3DDevice->SetTexture(0, m_pPointLightTexture);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	//Only use the alpha channel for the second texture stage
	m_pD3DDevice->SetTexture(1, m_pPointLightTexture);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	//Set the pixel shader
	m_pD3DDevice->SetPixelShader(m_AttenuationShader);

	// for animation...
	float Time = (float)GetTickCount() / 5000.0f;

	//Set the view matrix based on the position.
	D3DXVECTOR3 EyePos(10.0f, 0.0f, 10.0f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Create the concatenated transformation matrix
	//Get the transpose and Pass the transposed matrix to the shader
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
		                      m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);
	
	//Set the light position for the attenuation calculation
	D3DXVECTOR4 LightPos(sin(Time), 0.0f, 0.0f, 0.0f);
	D3DXMATRIX InversesWorld;
	D3DXMatrixInverse(&InversesWorld, NULL, &m_WorldMatrix);
	D3DXVec3Transform(&LightPos, &(D3DXVECTOR3)LightPos, &InversesWorld);
	m_pD3DDevice->SetVertexShaderConstant(8, &LightPos, 1);


	//Set the range
	D3DXVECTOR4 Range(1.0f / 4.0f, 0.5f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(9, &Range, 1);
	
	//Now, render the environment
	m_pD3DDevice->SetVertexShader(m_PointLightShader);
	m_pD3DDevice->SetStreamSource(0, m_pBoxVertexBuffer, sizeof(BOX_VERTEX));
	m_pD3DDevice->SetIndices(m_pBoxIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 0, 8);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 10, 2);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 14, 2);

	//Set everything back to normal
	m_pD3DDevice->SetPixelShader(NULL);

}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_PointLightShader);
	m_pD3DDevice->DeletePixelShader(m_AttenuationShader);

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
	m_pD3DDevice->DeleteVertexShader(m_PointLightShader);
	m_pD3DDevice->DeletePixelShader(m_AttenuationShader);
	
	//Clean up
	if (m_pBoxVertexBuffer)
	{
		m_pBoxVertexBuffer->Release();
		m_pBoxVertexBuffer = NULL;
	}

	if (m_pBoxIndexBuffer)
	{
		m_pBoxIndexBuffer->Release();
		m_pBoxIndexBuffer  = NULL;
	}

	if (m_pPointLightTexture)
	{
		m_pPointLightTexture->Release();
		m_pPointLightTexture  = NULL;
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

HRESULT CTechniqueApplication::CreateShaders()
{
	//Set up the declaration to match the FVF and to
	//read from stream zero.
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_NORMAL,   D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR),
		D3DVSD_END()
	};

	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\PerPixelPointLight.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_PointLightShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Assemble and create the pixel shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\PointLight.psh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreatePixelShader(
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_AttenuationShader)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();


	return S_OK;
}


HRESULT CTechniqueApplication::SetupEnvironment()
{
	//Create as managed so we don't have to worry about recreating it
	//if the device is lost.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(8 * sizeof(BOX_VERTEX), 
		                                        0, D3DFVF_BOXVERTEX,
									            D3DPOOL_MANAGED,
												&m_pBoxVertexBuffer)))
		return E_FAIL;

	//Render the control grid. Comment this out if you don't care.
	BOX_VERTEX *pVertices;

	//Lock the vertex buffer, but allow writing.
	m_pBoxVertexBuffer->Lock(0, 8 * sizeof(BOX_VERTEX), (BYTE **)&pVertices, 0);

	pVertices[0].x = -2.0f;	pVertices[0].y = -2.0f;	pVertices[0].z =  2.0f;
	pVertices[1].x = -2.0f;	pVertices[1].y =  2.0f;	pVertices[1].z =  2.0f;
	pVertices[2].x =  2.0f;	pVertices[2].y = -2.0f;	pVertices[2].z =  2.0f;
	pVertices[3].x =  2.0f;	pVertices[3].y =  2.0f;	pVertices[3].z =  2.0f;
	pVertices[4].x =  2.0f;	pVertices[4].y = -2.0f;	pVertices[4].z = -2.0f;
	pVertices[5].x =  2.0f;	pVertices[5].y =  2.0f;	pVertices[5].z = -2.0f;
	pVertices[6].x = -2.0f;	pVertices[6].y = -2.0f;	pVertices[6].z = -2.0f;
	pVertices[7].x = -2.0f;	pVertices[7].y =  2.0f;	pVertices[7].z = -2.0f;

	//Set the vertex colors
	pVertices[0].color = pVertices[1].color = pVertices[2].color = 
	pVertices[3].color = pVertices[4].color = pVertices[5].color =
	pVertices[6].color = pVertices[7].color = 0xffffffff;

	m_pBoxVertexBuffer->Unlock();

	//Create the Index Buffer for the control points
	if (FAILED(m_pD3DDevice->CreateIndexBuffer(18 * sizeof(short), 
		                                       0, D3DFMT_INDEX16,
									           D3DPOOL_MANAGED,
											   &m_pBoxIndexBuffer)))
		return E_FAIL;

	//lock the index buffer once - these are hardcoded values
	short *pIndex;
	m_pBoxIndexBuffer->Lock(0, 18 * sizeof(short), (BYTE**)&pIndex, 0);
	short Indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 1, 7, 3, 5, 0, 2, 6, 4};
	memcpy(pIndex, &Indices, 18 * sizeof(short));
	m_pBoxIndexBuffer->Unlock();

	return S_OK;
}

void CTechniqueApplication::PreRender()
{
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}

