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

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pRingVertexBuffer        = NULL;
	m_pRingIndexBuffer         = NULL;
	m_pRing                    = NULL;
	m_pBoxVertexBuffer         = NULL;
	m_pBoxIndexBuffer          = NULL;
	m_pBox                     = NULL;
	m_pRingMaterials           = NULL;
	m_pBoxMaterials            = NULL;
	m_NumRingMaterials         = 0;
	m_NumBoxMaterials          = 0;
	m_XRayShader               = 0;
	m_LightingShader           = 0;
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

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{		
	D3DXVECTOR4 EyePos(0.0, 5.0f, -30.0f, 0.0f);
	//Set the view matrix based on the position above.
	float EyeScan = 5.0f * sin((float)GetTickCount() / 1000.0f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(EyeScan, 5.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * 
		                      m_ViewMatrix * 
							  m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//These are all the constants for all the lights and the xray glasses
	D3DXVECTOR4 LightDir(0.0, -1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 LightColor(1.0, 1.0f, 1.0f, 1.0f);
	D3DXVECTOR4 Specular(1.0, 1.0f, 1.0f, 100.0f);
	D3DXVECTOR4 Ambient(0.1f, 0.1f, 0.1f, 1.0f);
	D3DXVECTOR4 XRayParams(1.0f, 127.0f, 1.0, 1.0f);
	D3DXMATRIX InverseWorld;

	//Transform the light position and direction to object space
	D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);

	D3DXVECTOR4 EyeDir;
	D3DXVec4Normalize(&EyeDir,   &D3DXVECTOR4(m_ViewMatrix._13, 
		                                      m_ViewMatrix._23, 
									          m_ViewMatrix._33, 
									          0.0f));


	//Transform the eye position to object space.
	D3DXVec4Transform(&EyePos, &EyePos, &InverseWorld);
	D3DXVec4Transform(&EyeDir, &EyeDir, &InverseWorld);
	D3DXVec4Normalize(&EyeDir, &EyeDir);

	//Set the constants...
	m_pD3DDevice->SetVertexShaderConstant(5, &LightDir, 1);
	m_pD3DDevice->SetVertexShaderConstant(6, &EyePos, 1);
	m_pD3DDevice->SetVertexShaderConstant(7, &Specular, 1);
	m_pD3DDevice->SetVertexShaderConstant(8, &LightColor, 1);
	m_pD3DDevice->SetVertexShaderConstant(9, &Ambient, 1);
	m_pD3DDevice->SetVertexShaderConstant(10, &EyeDir, 1);
	m_pD3DDevice->SetVertexShaderConstant(11, &XRayParams, 1);

	//Set the lighting shader and draw the ring.
	m_pD3DDevice->SetVertexShader(m_LightingShader);
	
	m_pD3DDevice->SetStreamSource(0, m_pRingVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pRingIndexBuffer, 0);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pRing->GetNumVertices(), 0,
									   m_pRing->GetNumFaces());


	//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//Now, draw the box with the XRay shader
	m_pD3DDevice->SetVertexShader(m_XRayShader);

	m_pD3DDevice->SetStreamSource(0, m_pBoxVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pBoxIndexBuffer, 0);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pBox->GetNumVertices(), 0,
									   m_pBox->GetNumFaces());

	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTechniqueApplication::LoadMesh()
{
	//This function is basically the same as many of the chapters,
	//only this time it's loading two meshes.
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

	if(FAILED(D3DXLoadMeshFromX("..\\media\\Ring.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumRingMaterials, &pOriginalMesh)))
        return FALSE;

	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pRingMaterials = new D3DMATERIAL8[m_NumRingMaterials];

    for(long MatCount = 0; MatCount < m_NumRingMaterials; MatCount++)
		m_pRingMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;

    pD3DXMtrlBuffer->Release();

	if (pOriginalMesh->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pRing);
		
		pOriginalMesh->Release();
		pOriginalMesh = NULL;
	}
	else
		m_pRing = pOriginalMesh;

	if(FAILED(D3DXLoadMeshFromX("..\\media\\Box.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumBoxMaterials, &pOriginalMesh)))
        return FALSE;

	d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pBoxMaterials = new D3DMATERIAL8[m_NumBoxMaterials];

    for(MatCount = 0; MatCount < m_NumBoxMaterials; MatCount++)
		m_pBoxMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;

    pD3DXMtrlBuffer->Release();

	if (pOriginalMesh->GetFVF() != D3DFVF_MESHVERTEX)
	{
		pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED,
									D3DFVF_MESHVERTEX,
                                    m_pD3DDevice, &m_pBox);
		
		pOriginalMesh->Release();
		pOriginalMesh = NULL;
	}
	else
		m_pBox = pOriginalMesh;

	return S_OK;
}

BOOL CTechniqueApplication::PreReset()
{
	//Delete the shaders
	m_pD3DDevice->DeleteVertexShader(m_XRayShader);
	m_pD3DDevice->DeleteVertexShader(m_LightingShader);

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
	m_pD3DDevice->DeleteVertexShader(m_XRayShader);
	m_pD3DDevice->DeleteVertexShader(m_LightingShader);
	
	//Clean up
	if (m_pRingVertexBuffer)
	{
		m_pRingVertexBuffer->Release();
		m_pRingVertexBuffer = NULL;
	}

	if (m_pRingIndexBuffer)
	{
		m_pRingIndexBuffer->Release();
		m_pRingIndexBuffer  = NULL;
	}

	if (m_pRing)
	{
		m_pRing->Release();
		m_pRing = NULL;
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

	if (m_pBox)
	{
		m_pBox->Release();
		m_pBox = NULL;
	}

	if (m_pRingMaterials)
	{
		delete m_pRingMaterials;
		m_pRingMaterials = NULL;
	}

	if (m_pBoxMaterials)
	{
		delete m_pBoxMaterials;
		m_pBoxMaterials = NULL;
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

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pRing->GetVertexBuffer(&m_pRingVertexBuffer);
	m_pRing->GetIndexBuffer(&m_pRingIndexBuffer);
	m_pBox->GetVertexBuffer(&m_pBoxVertexBuffer);
	m_pBox->GetIndexBuffer(&m_pBoxIndexBuffer);

	MESH_VERTEX *pMeshVertices;
	short       *pIndices;
	DWORD       *pAttribs;

	//Lock the vertex buffer, but allow writing.
	m_pRingVertexBuffer->Lock(0, m_pRing->GetNumVertices() * sizeof(MESH_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);


	//We only need to read the indices
	m_pRingIndexBuffer->Lock(0, 3 * m_pRing->GetNumFaces() * sizeof(short),
	                         (BYTE **)&pIndices, D3DLOCK_READONLY);

	//The attribute buffer maps the materials to each face.
	m_pRing->LockAttributeBuffer(D3DLOCK_READONLY, &pAttribs);

	//Loop through each face and set the vertex color based on the material.
	//This is a pretty simple example, but you could also use this to preload
	//other data, such as converting colors to data that the vertex shader
	//may use in computations.
	for (long Face = 0; Face < m_pRing->GetNumFaces(); Face++)
	{
		D3DXCOLOR Diffuse = (D3DXCOLOR)m_pRingMaterials[pAttribs[Face]].Diffuse;

		pMeshVertices[pIndices[Face * 3 + 0]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 1]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 2]].color = Diffuse;
	}

	//Give back all of our buffers.
	m_pRingVertexBuffer->Unlock();
	m_pRingIndexBuffer->Unlock();
	m_pRing->UnlockAttributeBuffer();


	//Repeat the above procedure for the box
	m_pBoxVertexBuffer->Lock(0, m_pBox->GetNumVertices() * sizeof(MESH_VERTEX),
		                     (BYTE **)&pMeshVertices, 0);

	m_pBoxIndexBuffer->Lock(0, 3 * m_pBox->GetNumFaces() * sizeof(short),
	                         (BYTE **)&pIndices, D3DLOCK_READONLY);
	m_pBox->LockAttributeBuffer(D3DLOCK_READONLY, &pAttribs);

	for (Face = 0; Face < m_pBox->GetNumFaces(); Face++)
	{
		D3DXCOLOR Diffuse = (D3DXCOLOR)m_pBoxMaterials[pAttribs[Face]].Diffuse;

		pMeshVertices[pIndices[Face * 3 + 0]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 1]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 2]].color = Diffuse;
	}

	m_pBoxVertexBuffer->Unlock();
	m_pBoxIndexBuffer->Unlock();
	m_pBox->UnlockAttributeBuffer();

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
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\XRayGlasses.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_XRayShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\DirLights.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_LightingShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}
