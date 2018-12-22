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

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)
#define D3DFVF_BOXVERTEX  (D3DFVF_XYZ)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
};

struct BOX_VERTEX
{
	float x, y, z;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pBoxVertexBuffer         = NULL;
	m_pBoxIndexBuffer          = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_NumMaterials             = 0;
	m_ReflectRefractShader     = 0;
	m_EnvironmentShader        = 0;
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
	LoadMesh();

	//Create the buffers we're actually going to use
	ExtractBuffers();

	//Set up the box that will be the environment
	if (FAILED(SetupEnvironment()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	if (FAILED(D3DXCreateCubeTextureFromFile(m_pD3DDevice, 
		                                 "..\\media\\cubemap.dds", 
										 &m_pCubeTexture)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the main texture and all the texture stage states.
	m_pD3DDevice->SetTexture(0, m_pCubeTexture);

	//The second texture blending is determined by the blend factor
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);

	//Set the blendfactor. Right now it's about half reflective and
	//half transparent.
	m_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x80ffffff);

	//Basically, tell the device to enable mip maps
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	
	//Set the eye position so that the eye orbits the object
	float Time = (float)GetTickCount() / 5000.0f;
	D3DXVECTOR4 EyePos(2.0f * sin(Time), 0.0f, 2.0f * cos(Time), 0.0f);

	//Make sure that the room isn't spinning...
	D3DXMatrixIdentity(&m_WorldMatrix);
	
	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Create the concatenated transformation matrix
	//Get the transpose and Pass the transposed matrix to the shader
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
		                      m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);	
	
	//Now, render the environment
	m_pD3DDevice->SetVertexShader(m_EnvironmentShader);
	m_pD3DDevice->SetStreamSource(0, m_pBoxVertexBuffer, sizeof(BOX_VERTEX));
	m_pD3DDevice->SetIndices(m_pBoxIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 0, 8);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 10, 2);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 8, 14, 2);

	//Now, get ready to draw the object. Set the refraction texture.
	m_pD3DDevice->SetTexture(1, m_pCubeTexture);
	
    //Rotate the object (just because we can...)
	D3DXMatrixRotationY(&m_WorldMatrix, (float)GetTickCount() / 1000.0f);	

	//Account for the changed world matrix
	ShaderMatrix = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);	


	//Create the transposed world matrix and pass it to the shader
	D3DXMATRIX TransposedWorld;
	D3DXMatrixTranspose(&TransposedWorld, &m_WorldMatrix);
	m_pD3DDevice->SetVertexShaderConstant(4, &TransposedWorld, 4);

	//Handy constant
	D3DXVECTOR4 One(1.0, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 Refraction(0.4f, 0.0f, 0.0f, 0.0f);

	//Set the constants...
	m_pD3DDevice->SetVertexShaderConstant(8, &EyePos, 1);
	m_pD3DDevice->SetVertexShaderConstant(9, &One, 1);
	m_pD3DDevice->SetVertexShaderConstant(10, &Refraction, 1);

	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_ReflectRefractShader);
	
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//Disable mip maps. This is a limitation of the font class. We must "clean up"
	//here because we set up minimal state transitions in the font class itself.
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	//Set the refraction texture to NULL. This sets things up for the font
	//and also the next pass of the environment box.
	m_pD3DDevice->SetTexture(1, NULL);
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\unitsphere.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &pOriginalMesh)))
        return FALSE;

	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;
        m_pMeshMaterials[MatCount].Ambient = 
								m_pMeshMaterials[MatCount].Diffuse;
    }

    pD3DXMtrlBuffer->Release();

	//This is new. If the FVF doesn't match, clone the mesh and
	//create one that does. Then, release the loaded mesh. If the 
	//FVF does match, set the member mesh and move on.
	if (pOriginalMesh->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pMesh);
		
		pOriginalMesh->Release();
		pOriginalMesh = NULL;
	}
	else
		m_pMesh = pOriginalMesh;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_ReflectRefractShader);
	m_pD3DDevice->DeleteVertexShader(m_EnvironmentShader);

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
	m_pD3DDevice->DeleteVertexShader(m_ReflectRefractShader);
	m_pD3DDevice->DeleteVertexShader(m_EnvironmentShader);
	
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

	if (m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	if (m_pCubeTexture)
	{
		m_pCubeTexture->Release();
		m_pCubeTexture = NULL;
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

	D3DXMatrixIdentity(&m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

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
		D3DVSD_END()
	};

	
	ID3DXBuffer* pShaderBuffer;
	ID3DXBuffer* pShaderErrors;

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\reflectrefract.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_ReflectRefractShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	DWORD BoxDeclaration[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION,   D3DVSDT_FLOAT3),
		D3DVSD_END()
	};

	//Assemble and create the first shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\EnvironmentBox.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(BoxDeclaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_EnvironmentShader, 0)))
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