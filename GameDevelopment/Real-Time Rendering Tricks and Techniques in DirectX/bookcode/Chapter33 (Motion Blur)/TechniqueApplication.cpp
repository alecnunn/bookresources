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

#define TEX_DIMENSION 1024

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define D3DFVF_DISPLAYVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

struct DISPLAY_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
	float u, v;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pDisplayVertexBuffer     = NULL;
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_pDisplayTexture          = NULL;
	m_pDisplayTextureSurface   = NULL;
	m_pDisplayZSurface         = NULL;
	m_pBackBuffer              = NULL;
	m_pZBuffer                 = NULL;
	m_NumMaterials             = 0;
	m_BasicShader              = 0;
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{
	D3DDISPLAYMODE CurrentMode;
	if (SUCCEEDED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
								                &CurrentMode)))
	{
		ZeroMemory(&m_PresentParameters, 
				   sizeof(D3DPRESENT_PARAMETERS));
		m_PresentParameters.Windowed = TRUE;
		m_PresentParameters.SwapEffect = D3DSWAPEFFECT_COPY;
		m_PresentParameters.BackBufferFormat = CurrentMode.Format;
		m_PresentParameters.EnableAutoDepthStencil = TRUE;
		m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

		m_CreationParameters.AdapterOrdinal = 
											D3DADAPTER_DEFAULT;
		m_CreationParameters.DeviceType     = D3DDEVTYPE_HAL;
		m_CreationParameters.hFocusWindow   = m_hWnd;


		D3DCAPS8 Caps;
		m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
		if (Caps.VertexShaderVersion == D3DVS_VERSION(1,1))
			m_CreationParameters.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			m_CreationParameters.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		if (FAILED(CreateDevice(&m_CreationParameters, &m_PresentParameters)))
			return FALSE;
	}


	//Do the basic camera positioning, etc.
	SetupDevice();
	
	//Load the mesh object
	LoadMesh();

	//Create the buffers we're actually going to use
	ExtractBuffers();

	if (FAILED(CreateDisplayBuffer()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	//Clear Once
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		            D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);


	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0, 0.0f, -40.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

    //The following matrix operations will transform the mesh
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//These matrices will rotate and translate the mesh
	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, 0.0f, 0.0f);
	m_WorldMatrix = Rotation * Translation;

	//Set some lighting constants
	D3DXVECTOR4 Ambient    (0.1,  0.1f, 0.1f, 0.0f);
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

	//Set the viewport so that the rendered image matches the device.
	//Get the viewport here and then set it when the render target changes
	D3DVIEWPORT8 NormalViewport;
	m_pD3DDevice->GetViewport(&NormalViewport);

	//Set up the renderTarget
	HRESULT hr = m_pD3DDevice->SetRenderTarget(m_pDisplayTextureSurface, m_pDisplayZSurface);

	//Set the matching viewport
	m_pD3DDevice->SetViewport(&NormalViewport);
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		                D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);
		
	//Set the current shader
	hr = m_pD3DDevice->SetVertexShader(m_BasicShader);
	
	//Draw the object
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//Set the renderTarget back to the normal back buffer
	m_pD3DDevice->SetRenderTarget(m_pBackBuffer, m_pZBuffer);

	//Set up the new texture to be drawn
	m_pD3DDevice->SetTexture(0, m_pDisplayTexture);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	m_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x55ffffff);

	//Set display modes for a simple alpha blended rectangle
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//This ensures everything is drawn
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	//Draw a rectangle showing the rendering
	m_pD3DDevice->SetVertexShader(D3DFVF_DISPLAYVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pDisplayVertexBuffer, sizeof(DISPLAY_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//Set things back to normal
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);
	m_pD3DDevice->SetTexture(0, NULL);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
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

	//Get rid of all the render target objects
	CleanUpTarget();
	
	//Clean up
	if (m_pDisplayVertexBuffer)
	{
		m_pDisplayVertexBuffer->Release();
		m_pDisplayVertexBuffer = NULL;
	}

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

	//Create the shadow render target. This will be used to render the light
	//view depth.
	if (FAILED(D3DXCreateTexture(m_pD3DDevice, TEX_DIMENSION, TEX_DIMENSION, 1, 
		                         D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
								 D3DPOOL_DEFAULT, &m_pDisplayTexture)))
		return E_FAIL;

	//Create the target depth buffer
	if (FAILED(m_pD3DDevice->CreateDepthStencilSurface(TEX_DIMENSION, 
		                                               TEX_DIMENSION,
													   D3DFMT_D24S8,
													   D3DMULTISAMPLE_NONE,
													   &m_pDisplayZSurface)))
		return E_FAIL;


    //Keep a handle to the back buffer for easy swapping
	if (FAILED(m_pD3DDevice->GetRenderTarget(&m_pBackBuffer)))
		return E_FAIL;

    //Same for the depth buffer
	if (FAILED(m_pD3DDevice->GetDepthStencilSurface(&m_pZBuffer)))
		return E_FAIL;

	//Get the top level surface of the target texture.
	if (FAILED(m_pDisplayTexture->GetSurfaceLevel(0, &m_pDisplayTextureSurface)))
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

	return S_OK;
}

void CTechniqueApplication::PreRender()
{
	//Don't Clear...

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}


HRESULT CTechniqueApplication::CreateDisplayBuffer()
{
	//Also, create the mask vertex buffer
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(DISPLAY_VERTEX), 
		                                        0, D3DFVF_DISPLAYVERTEX,
									            D3DPOOL_MANAGED,
												&m_pDisplayVertexBuffer)))
		return E_FAIL;

	//Create a set of 4 vertices for the texture viewing surface
	DISPLAY_VERTEX *pDisplayVertices;

	//Lock the vertex buffer, but allow writing.
	m_pDisplayVertexBuffer->Lock(0, 4 * sizeof(DISPLAY_VERTEX), (BYTE **)&pDisplayVertices, 0);

	//Base the vertex extents on the viewport size. This does not handle cases where the
	//viewport may change size.
	D3DVIEWPORT8 Viewport;
	m_pD3DDevice->GetViewport(&Viewport);
	DWORD X1 = Viewport.X;
	DWORD Y1 = Viewport.Y;
	DWORD X2 = Viewport.X + Viewport.Width;
	DWORD Y2 = Viewport.Y + Viewport.Height;

	//Also, use the viewport specs to define texture coordinates
	float UMax = (float)Viewport.Width  / (float)TEX_DIMENSION;
	float VMax = (float)Viewport.Height / (float)TEX_DIMENSION;

	//Set up the 4 corners of a small square
	pDisplayVertices[0].x = X1;     pDisplayVertices[0].y = Y1;
	pDisplayVertices[0].z = 1.0f;   pDisplayVertices[0].rhw = 1.0f;
	pDisplayVertices[0].u = 0.0f;   pDisplayVertices[0].v = 0.0f;

	pDisplayVertices[1].x = X1;     pDisplayVertices[1].y = Y2;
	pDisplayVertices[1].u = 0.0f;   pDisplayVertices[1].v = VMax;
	pDisplayVertices[1].z = 1.0f;   pDisplayVertices[1].rhw = 1.0f;

	pDisplayVertices[2].x = X2;     pDisplayVertices[2].y = Y1;
	pDisplayVertices[2].u = UMax;   pDisplayVertices[2].v = 0.0f;
	pDisplayVertices[2].z = 1.0f;   pDisplayVertices[2].rhw = 1.0f;

	pDisplayVertices[3].x = X2;     pDisplayVertices[3].y = Y2;
	pDisplayVertices[3].u = UMax;   pDisplayVertices[3].v = VMax;
	pDisplayVertices[3].z = 1.0f;   pDisplayVertices[3].rhw = 1.0f;

	pDisplayVertices[0].color = pDisplayVertices[1].color = 
	pDisplayVertices[2].color = pDisplayVertices[3].color = 0xffffffff;

	m_pDisplayVertexBuffer->Unlock();

	return S_OK;
}

void CTechniqueApplication::CleanUpTarget()
{
	if (m_pDisplayTextureSurface)
	{
		m_pDisplayTextureSurface->Release();
		m_pDisplayTextureSurface = NULL;
	}

	if (m_pDisplayTexture)
	{
		m_pDisplayTexture->Release();
		m_pDisplayTexture = NULL;
	}

	if (m_pDisplayZSurface)
	{
		m_pDisplayZSurface->Release();
		m_pDisplayZSurface = NULL;
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