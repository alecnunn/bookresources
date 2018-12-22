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

#define TARGET_DIMENSION 1024

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_pPickSurface             = NULL;
	m_pPickZSurface            = NULL;
	m_pBackBuffer              = NULL;
	m_pZBuffer                 = NULL;
	m_NumMaterials             = 0;
	m_BasicShader              = 0;
	m_PickShader               = 0;
	m_CurrentObject            = 0;
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
	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0, 0.0f, -100.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Set the current shader
	m_pD3DDevice->SetVertexShader(m_BasicShader);

    //The following matrix operations will transform the mesh
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//These matrices will rotate and translate the mesh
	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);

	for (long i = 0; i < 3; i++)
	{
		D3DXMatrixTranslation(&Translation, (float)i * 10.0f  - 10.0f, 0.0f, 0.0f);
		m_WorldMatrix = Rotation * Translation;

		//Set some lighting constants
		float AmbientValue = 0.1f;

		if (m_CurrentObject == i + 1)
			AmbientValue = 0.5;

		D3DXVECTOR4 Ambient(AmbientValue,  AmbientValue, 
			                AmbientValue, 0.0f);
		m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

		//Set the light direction
		D3DXVECTOR4 LightDir = D3DXVECTOR4(0.0f, -1.0f, 0.0f, 0.0f);
		D3DXMATRIX InverseWorld;
		D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
		D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
		D3DXVec4Normalize(&LightDir, &LightDir);
		m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);

		//Create the concatenated transformation matrix
		D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
								  m_ProjectionMatrix;
		D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
		m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

		//Draw the object
		m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
		m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    									   m_pMesh->GetNumVertices(), 0,
										   m_pMesh->GetNumFaces());
	}
}

HRESULT CTechniqueApplication::LoadMesh()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPD3DXMESH   pOriginalMesh;

    //Load and initialize the mesh. This is a repeat of the code
	//from Chapter 10.
	if(FAILED(D3DXLoadMeshFromX("..\\media\\fattorus.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &pOriginalMesh)))
        return FALSE;

	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;

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
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);
	m_pD3DDevice->DeleteVertexShader(m_PickShader);

	//Get rid of all the render target objects
	CleanUpTarget();

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
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);
	m_pD3DDevice->DeleteVertexShader(m_PickShader);

	//Get rid of all the render target objects
	CleanUpTarget();
	
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


HRESULT CTechniqueApplication::SetupDevice()
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

	//Create the render target.
	if (FAILED(m_pD3DDevice->CreateRenderTarget(TARGET_DIMENSION,
		                                        TARGET_DIMENSION, 
		                                        D3DFMT_X8R8G8B8,
												D3DMULTISAMPLE_NONE,
												TRUE, &m_pPickSurface)))
		return E_FAIL;


	//Create the target depth buffer
	if (FAILED(m_pD3DDevice->CreateDepthStencilSurface(TARGET_DIMENSION, 
		                                               TARGET_DIMENSION,
													   D3DFMT_D24S8,
													   D3DMULTISAMPLE_NONE,
													   &m_pPickZSurface)))
		return E_FAIL;


    //Keep a handle to the back buffer for easy swapping
	if (FAILED(m_pD3DDevice->GetRenderTarget(&m_pBackBuffer)))
		return E_FAIL;

    //Same for the depth buffer
	if (FAILED(m_pD3DDevice->GetDepthStencilSurface(&m_pZBuffer)))
		return E_FAIL;

	return S_OK;
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
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\Basic.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_BasicShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	//Create the pick shader
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\PerPixelPick.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_PickShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::CleanUpTarget()
{
	if (m_pPickSurface)
	{
		m_pPickSurface->Release();
		m_pPickSurface = NULL;
	}

	if (m_pPickZSurface)
	{
		m_pPickZSurface->Release();
		m_pPickZSurface = NULL;
	}

	if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

	if (m_pZBuffer)
	{
		m_pZBuffer->Release();
		m_pZBuffer = NULL;
	}
}

long CTechniqueApplication::GetPickObject(long X, long Y)
{
	//Set the viewport so that the rendered image matches the device.
	//Get the viewport here and then set it when the render target changes
	D3DVIEWPORT8 NormalViewport;
	m_pD3DDevice->GetViewport(&NormalViewport);

	//Set up the renderTarget
	m_pD3DDevice->SetRenderTarget(m_pPickSurface, m_pPickZSurface);
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		                D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//Set the matching viewport
	m_pD3DDevice->SetViewport(&NormalViewport);

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0, 0.0f, -100.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//Set the current shader
	m_pD3DDevice->SetVertexShader(m_PickShader);

    //The following matrix operations will transform the mesh
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//These matrices will rotate and translate the mesh
	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);

	//Call beginscene to begin drawing
	m_pD3DDevice->BeginScene();

	for (long i = 0; i < 3; i++)
	{
		D3DXMatrixTranslation(&Translation, (float)i * 10.0f  - 10.0f, 0.0f, 0.0f);
		m_WorldMatrix = Rotation * Translation;

		//Pick values start at one so that black means "no object"
		D3DXVECTOR4 PickConstants(((float)i + 1.0f) / 255.0f, 0.5f, 0.3f, 0.0f);
		m_pD3DDevice->SetVertexShaderConstant(4, &PickConstants, 1);

		//Create the concatenated transformation matrix
		D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
								  m_ProjectionMatrix;
		D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
		m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

		//Draw the object
		HRESULT hr = m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
		hr = m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
		hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    									   m_pMesh->GetNumVertices(), 0,
										   m_pMesh->GetNumFaces());
	}

	//End the scene
	m_pD3DDevice->EndScene();

	//Set the renderTarget back to the normal back buffer
	m_pD3DDevice->SetRenderTarget(m_pBackBuffer, m_pZBuffer);

	//The texture now contains the rendered scene. Lock the rectangle around
	//the mouse point to get the value.
	RECT MouseRect;
	MouseRect.left = X; MouseRect.right  = X + 1;
	MouseRect.top = Y;  MouseRect.bottom = Y + 1;

    D3DLOCKED_RECT LockedRect;
    if (FAILED(m_pPickSurface->LockRect(&LockedRect, &MouseRect, D3DLOCK_READONLY)))
        return 0;

	BYTE  *pPickValue;
    pPickValue = static_cast<byte *>(LockedRect.pBits);
	long ObjectID = *(pPickValue + 2);
    
    //unlock the surface
	if (FAILED(m_pPickSurface->UnlockRect()))
        return 0;

	return ObjectID;
}

BOOL CTechniqueApplication::HandleMessage(MSG *pMessage)
{
	if (pMessage->message == WM_LBUTTONDOWN)
	{
		m_CurrentObject = GetPickObject(LOWORD(pMessage->lParam), 
			                            HIWORD(pMessage->lParam));
	}

	return CHostApplication::HandleMessage(pMessage);
}

