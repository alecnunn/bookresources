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

#define D3DFVF_BBVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct BB_VERTEX
{
	float x, y, z;
	float RightDir, UpDir;
	DWORD d;
	float u, v;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pVertexBuffer          = NULL;
	m_pMesh                  = NULL;
	m_BillboardShaderHandle  = 0;
	m_pMeshMaterials         = NULL;
	m_NumMaterials           = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	//Figure out which device to use
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

	//Create the vertex buffer
	if (FAILED(CreateBuffer()))
		return FALSE;

	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\light.dds", 
										 &m_pLightTexture)))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShader()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//orbit around the scene...
	float Time = (float)GetTickCount() / 1000.0f;
	
	D3DXMatrixLookAtLH(&m_ViewMatrix, 
		      &D3DXVECTOR3(-100.0f * sin(Time), 20.0f, -100.0f * cos(Time)),
		      &D3DXVECTOR3(0.0f, 20.0f, 0.0f),
              &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	D3DXMatrixTranslation(&m_WorldMatrix, 10.0f, -15.0f, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	//draw the lamp post.
	m_pD3DDevice->SetMaterial(&m_pMeshMaterials[0]);
	m_pMesh->DrawSubset(0);


	//extract the right and up vectors from the view matrix.
	D3DXVECTOR4 Right;
	D3DXVECTOR4 Up;
	D3DXVECTOR4 Eye;
	D3DXVec4Normalize(&Right, &D3DXVECTOR4(m_ViewMatrix._11, 
		                                   m_ViewMatrix._21, 
										   m_ViewMatrix._31, 
										   0.0f));
	D3DXVec4Normalize(&Up,    &D3DXVECTOR4(m_ViewMatrix._12, 
		                                   m_ViewMatrix._22, 
										   m_ViewMatrix._32, 
										   0.0f));
	D3DXVec4Normalize(&Eye,   &D3DXVECTOR4(m_ViewMatrix._13, 
		                                   m_ViewMatrix._23, 
										   m_ViewMatrix._33, 
										   0.0f));

	//These factors set how much we want to scale the billboard
	float Pulse = 2.0f * (sin(Time * 2.0f) + 1.0f) + 4.0f;
	D3DXVECTOR4 ScaleFactors(Pulse, Pulse, 1.0f, 0.0f);

	m_pD3DDevice->SetVertexShaderConstant(4,  &Right,          1);
	m_pD3DDevice->SetVertexShaderConstant(5,  &Up,             1);
	m_pD3DDevice->SetVertexShaderConstant(6,  &Eye,            1);
	m_pD3DDevice->SetVertexShaderConstant(7,  &ScaleFactors,   1);

	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
							  m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Do the same for the world matrix. We do this because
	//the model must be transformed to the world position
	//before we can do eye space operations
	D3DXMatrixTranspose(&ShaderMatrix, &m_WorldMatrix);
	m_pD3DDevice->SetVertexShaderConstant(8, &ShaderMatrix, 4);

	m_pD3DDevice->SetVertexShader(m_BillboardShaderHandle);

	//Set up the texture, blending, and Z
	m_pD3DDevice->SetTexture(0, m_pLightTexture);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(BB_VERTEX));
    m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//reset the tests for the mesh
	m_pD3DDevice->SetTexture(0, NULL);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\lamppost.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &m_pMesh)))
        return E_FAIL;

	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;
		m_pMeshMaterials[MatCount].Ambient = m_pMeshMaterials[MatCount].Diffuse;
    }

    pD3DXMtrlBuffer->Release();

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shader
	m_pD3DDevice->DeleteVertexShader(m_BillboardShaderHandle);

	return TRUE;
}

BOOL CTechniqueApplication::PostReset()
{
	//SetupDevice also creates the lights...
	SetupDevice();

	//Recreate the shader
	if (FAILED(CreateShader()))
		return FALSE;

	return TRUE;
}


BOOL CTechniqueApplication::PreTerminate()
{
	//Delete the shader
	m_pD3DDevice->DeleteVertexShader(m_BillboardShaderHandle);
	
	//Clean up
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
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

	if (m_pLightTexture)
	{
		m_pLightTexture->Release();
		m_pLightTexture = NULL;
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
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);

	D3DXMatrixIdentity(&m_WorldMatrix);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	//Create a point light where the light is supposed to be
	D3DLIGHT8 PointLight;
	ZeroMemory(&PointLight, sizeof(D3DLIGHT8));
	PointLight.Type         = D3DLIGHT_POINT;
	PointLight.Diffuse.r    = PointLight.Diffuse.g = 
							  PointLight.Diffuse.b = 1.0f;
	PointLight.Position     = D3DXVECTOR3(9.0f, 48.0f, 0.0f);
	PointLight.Range        = 50.0f;
	PointLight.Attenuation1 = 0.25f;

	//turn all the lighting on
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00555555);
	m_pD3DDevice->SetLight(0, &PointLight);
	m_pD3DDevice->LightEnable(0, TRUE);

	//Make sure everything is rendered
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//Alpha stuff
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000000);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
}

HRESULT CTechniqueApplication::CreateBuffer()
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(BB_VERTEX), 
		                                        D3DUSAGE_WRITEONLY, 
												D3DFVF_BBVERTEX,
									            D3DPOOL_MANAGED, 
												&m_pVertexBuffer)))
		return E_FAIL;

    // Fill the quad VB
    BB_VERTEX* pVertices = NULL;
    m_pVertexBuffer->Lock(0, 4 * sizeof(BB_VERTEX), (BYTE**)&pVertices, 0);

	//Set everything to zero
	memset(pVertices, 0, 4 * sizeof(BB_VERTEX));

	//Set all the colors to white
	pVertices[0].d = pVertices[1].d = pVertices[2].d = pVertices[3].d = 0xFFFFFFFF;

	//Set the position. Notice that all components are the same. The 
	//individual positions are determined with the data in the normal
	//vector and the vectors passed to the shader.
	pVertices[0].x = pVertices[1].x = pVertices[2].x = pVertices[3].x = 9.0f;
	pVertices[0].y = pVertices[1].y = pVertices[2].y = pVertices[3].y = 47.0f;
	pVertices[0].z = pVertices[1].z = pVertices[2].z = pVertices[3].z = 0.0f;

	//Set the texture coordinates
	pVertices[0].u = pVertices[1].u = 0.0f;
	pVertices[2].u = pVertices[3].u = 1.0f;

	pVertices[0].v = pVertices[2].v = 1.0f;
	pVertices[1].v = pVertices[3].v = 0.0f;

	//The normal vector doesn't hold the normal, it holds the
	//vector that determines how the vertex follows the up and right
	//vectors. These can be used to reshape the billboard dynamically.
	pVertices[0].RightDir = -1.0f; pVertices[0].UpDir =  1.0f;
	pVertices[1].RightDir = -1.0f; pVertices[1].UpDir = -1.0f;
	pVertices[2].RightDir =  1.0f; pVertices[2].UpDir =  1.0f;
	pVertices[3].RightDir =  1.0f; pVertices[3].UpDir = -1.0f;

    m_pVertexBuffer->Unlock();

	return S_OK;
}

HRESULT CTechniqueApplication::CreateShader()
{
	//Set up the declaration to match the FVF and to
	//read from stream zero.
	DWORD Declaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_NORMAL,    D3DVSDT_FLOAT2),
		D3DVSD_REG(D3DVSDE_DIFFUSE,   D3DVSDT_D3DCOLOR),
		D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};
	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\Billboard.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BillboardShaderHandle, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}
