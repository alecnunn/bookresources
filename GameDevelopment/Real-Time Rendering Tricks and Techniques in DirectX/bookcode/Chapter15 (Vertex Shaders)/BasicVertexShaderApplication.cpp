/***************************************************************
* BasicVertexShaderApplication.cpp                             *
*                                                              *
* This file contains the implementation of the                 *
* CBasicVertexShaderApplication class.						   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "BasicVertexShaderApplication.h"

#define D3DFVF_SIMPLEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct SIMPLE_VERTEX
{
	float x, y, z;
	DWORD color;
};

#define NUM_VERTICES 20

CBasicVertexShaderApplication::CBasicVertexShaderApplication()
{
	m_pVertexBuffer = NULL;
}

CBasicVertexShaderApplication::~CBasicVertexShaderApplication()
{
	DestroyVertexBuffer();
}

BOOL CBasicVertexShaderApplication::PostInitialize()
{	
	//Asking the D3D object will give us the hardware device caps.
	//So, if it says it supports shaders, then it should support.
	//If not, then we should select software processing.
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

	//Set the view and projection matrices. Notice we don't call 
	//Set Transform - there's no need because we're using a shader!
	D3DXMatrixIdentity(&m_ViewMatrix);

	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
    D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 100.0f);

	//Create the shader
	if (FAILED(CreateShader()))
		return FALSE;

	return CreateVertexBuffer();
}

BOOL CBasicVertexShaderApplication::PreReset()
{
	DestroyVertexBuffer();

	//Delete the shader
	m_pD3DDevice->DeleteVertexShader(m_ShaderHandle);

	return TRUE;
}

BOOL CBasicVertexShaderApplication::PostReset()
{
	//Recreate the shader
	if (FAILED(CreateShader()))
		return FALSE;
	
	return CreateVertexBuffer();
}

BOOL CBasicVertexShaderApplication::CreateVertexBuffer()
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
										NUM_VERTICES
										* sizeof(SIMPLE_VERTEX),
										D3DUSAGE_WRITEONLY,
										D3DFVF_SIMPLEVERTEX,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer)))
		return FALSE;

	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 
								  sizeof(SIMPLE_VERTEX));

	FillVertexBuffer();

	return TRUE;
}

void CBasicVertexShaderApplication::DestroyVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CBasicVertexShaderApplication::Render()
{
	D3DXMATRIX RotationMatrix1;
	D3DXMATRIX RotationMatrix2;
	D3DXMATRIX TranslationMatrix;
	D3DXMATRIX ScalingMatrix;

	D3DXMatrixRotationZ(&RotationMatrix1,  (float)GetTickCount() / 1000.0f);
	D3DXMatrixRotationZ(&RotationMatrix2, -(float)GetTickCount() / 1000.0f);
	D3DXMatrixTranslation(&TranslationMatrix, 3.0f, 0.0f, 0.0f);
	D3DXMatrixScaling(&ScalingMatrix, 1.0f, 0.5f, 1.0f);

	//Create the world matrix
	m_WorldMatrix = ScalingMatrix * RotationMatrix2 * 
					TranslationMatrix * RotationMatrix1;

	//Create the complete matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * 
		                      m_ViewMatrix * 
							  m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Draw...
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);
}

BOOL CBasicVertexShaderApplication::PreTerminate()
{
	DestroyVertexBuffer();

	//Delete the shader
	m_pD3DDevice->DeleteVertexShader(m_ShaderHandle);

	return TRUE;
}

BOOL CBasicVertexShaderApplication::FillVertexBuffer()
{
	if (!m_pVertexBuffer)
		return FALSE;
	
	SIMPLE_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0,
					NUM_VERTICES * sizeof(SIMPLE_VERTEX),
					(BYTE **)&pVertices,
					0)))
	{
		DestroyVertexBuffer();
		return FALSE;
	}

	for (long Index = 0; Index < NUM_VERTICES; Index++)
	{
		float Angle = 2.0f * D3DX_PI * (float)Index /
					  NUM_VERTICES;

		pVertices[Index].x = cos(Angle);
		pVertices[Index].y = sin(Angle);
		pVertices[Index].z = 10.0f;

		pVertices[Index].color = 0xffffffff;
	}

	m_pVertexBuffer->Unlock();

	return TRUE;	
}

HRESULT CBasicVertexShaderApplication::CreateShader()
{
	//Because the shader is so simple, we will create it
	//in the code rather than read it in from a file.
	//This shader does the simple transform and emits the
	//position and the color.
	const char BasicShader[] =
	"vs.1.1                \n"
	"dp4 oPos.x, v0, c0    \n"
	"dp4 oPos.y, v0, c1    \n"
	"dp4 oPos.z, v0, c2    \n"
	"dp4 oPos.w, v0, c3    \n"
	"mov oD0,  v5          \n";

	//Set up the declaration to match the FVF and to
	//read from stream zero.
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_DIFFUSE,   D3DVSDT_D3DCOLOR),
		D3DVSD_END()
	};

	
	//Assemble and create the shader. Under real circumstances, you would 
	//probably want to do more error handling.

	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	if (FAILED(D3DXAssembleShader(BasicShader, sizeof(BasicShader) - 1, 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_ShaderHandle, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//As in Chapter 9, we only have one shader. 
	//Set it here and forget about it.
	return m_pD3DDevice->SetVertexShader(m_ShaderHandle);
}
