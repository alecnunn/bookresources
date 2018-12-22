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
	m_pMeshVertexBuffer = NULL;
	m_pMeshIndexBuffer  = NULL;
	m_pMesh             = NULL;
	m_pMeshMaterials    = NULL;
	m_NumMaterials      = 0;
	m_WhichShader       = 0;
	m_SineShaderHandle  = 0;
	m_ScaleShaderHandle = 0;
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
    //Create the concatenated transfotmation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * 
		                      m_ViewMatrix * 
							  m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

    //The various wave parameters for the sine shader. 
	//The first value is the phase, 
	//the second value alters the wavelength. The third value is
	//the amplitude and the last value helps to bias the color.
	D3DXVECTOR4 Wave((float)GetTickCount() / 100.0f, 0.25f, 3.0f, 0.1f);
    m_pD3DDevice->SetVertexShaderConstant(4, &Wave, 1);

	//These are the values for the scaling shader. 
	//The first value is an oscillating value.
	float ScaleFactor = 3.0f * sinf((float)GetTickCount() / 1000.0f);
	D3DXVECTOR4 Scale(ScaleFactor, ScaleFactor, ScaleFactor, 1.0f); 
    m_pD3DDevice->SetVertexShaderConstant(5, &Scale, 1);
	
	//Decide which shader
	if (!m_WhichShader)
		m_pD3DDevice->SetVertexShader(m_SineShaderHandle);
	else
		m_pD3DDevice->SetVertexShader(m_ScaleShaderHandle);

	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\chapter18.x",
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
	m_pD3DDevice->DeleteVertexShader(m_SineShaderHandle);
	m_pD3DDevice->DeleteVertexShader(m_ScaleShaderHandle);

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
	m_pD3DDevice->DeleteVertexShader(m_SineShaderHandle);
	m_pD3DDevice->DeleteVertexShader(m_ScaleShaderHandle);
	
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
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);

	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0f, 10.0f, -50.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	D3DXMatrixIdentity(&m_WorldMatrix);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
}

HRESULT CTechniqueApplication::ExtractBuffers()
{
	//Get the buffers
	m_pMesh->GetVertexBuffer(&m_pMeshVertexBuffer);
	m_pMesh->GetIndexBuffer(&m_pMeshIndexBuffer);

	MESH_VERTEX *pMeshVertices;
	short       *pIndices;
	DWORD       *pAttribs;

	//Lock the vertex buffer, but allow writing.
	m_pMeshVertexBuffer->Lock(0, 
		                      m_pMesh->GetNumVertices() * sizeof(MESH_VERTEX),
		                      (BYTE **)&pMeshVertices, 0);


	//We only need to read the indices
	m_pMeshIndexBuffer->Lock(0, 3 * m_pMesh->GetNumFaces() * sizeof(short),
	                         (BYTE **)&pIndices, D3DLOCK_READONLY);

	//The attribute buffer maps the materials to each face.
	m_pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &pAttribs);

	//Loop through each face and set the vertex color based on the material.
	//This is a pretty simple example, but you could also use this to preload
	//other data, such as converting colors to data that the vertex shader
	//may use in computations.
	for (long Face = 0; Face < m_pMesh->GetNumFaces(); Face++)
	{
		D3DXCOLOR Diffuse = (D3DXCOLOR)m_pMeshMaterials[pAttribs[Face]].Diffuse;

		pMeshVertices[pIndices[Face * 3 + 0]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 1]].color = Diffuse;
		pMeshVertices[pIndices[Face * 3 + 2]].color = Diffuse;
	}

	//Give back all of our buffers.
	m_pMeshVertexBuffer->Unlock();
	m_pMeshIndexBuffer->Unlock();
	m_pMesh->UnlockAttributeBuffer();

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
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\SineWave.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_SineShaderHandle, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Assemble and create the second shader.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\ScaleNormals.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_ScaleShaderHandle, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//These are the Taylor coefficients used to approximate the 
	//sine. These only need to be set once.
	D3DXVECTOR4 Sine(1.0f, -1.0f/6.0f, 1.0f/120.0f, -1.0f/5040.0f);

	//These constants are used to constrain the range of values.
	D3DXVECTOR4 RangeConstants(D3DX_PI, 1.0f/(2.0f*D3DX_PI), 
		                       2.0f*D3DX_PI, 0.0f);


	//Set the constants that are not going to change.
	m_pD3DDevice->SetVertexShaderConstant(6, &RangeConstants, 1);
	m_pD3DDevice->SetVertexShaderConstant(7, &Sine, 1);

	return S_OK;
}

BOOL CTechniqueApplication::HandleMessage(MSG *pMessage)
{
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_F2)
		m_WhichShader = !m_WhichShader;
	
	return CHostApplication::HandleMessage(pMessage);
}


