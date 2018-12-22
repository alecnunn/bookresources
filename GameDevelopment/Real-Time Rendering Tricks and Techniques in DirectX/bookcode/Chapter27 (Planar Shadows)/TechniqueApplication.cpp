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
	m_pPlaneVertexBuffer       = NULL;
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_NumMaterials             = 0;
	m_BasicShader              = 0;
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

	if (FAILED(CreatePlaneBuffer()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the eye position
	D3DXVECTOR4 EyePos(0.0, 60.0f, -60.0f, 0.0f);

	//Set the view matrix based on the position above.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));


	//Set the current light shader
	m_pD3DDevice->SetVertexShader(m_BasicShader);


    //The following matrix operations will transform the mesh
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//These matrices will rotate and translate the mesh
	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, 15.0f, 0.0f);
	m_WorldMatrix = Rotation * Translation;

	//Helper constant
	D3DXVECTOR4 OneAndZero (1.0,  1.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(6, &OneAndZero, 1);

	//Render the shadow
	RenderPlaneAndShadow();

	//The ambient light constant gets set by the shadow code. 
	//Set it to a real value. The light direction gets set in the 
	//shadow code.
	D3DXVECTOR4 Ambient    (0.1,  0.1f, 0.1f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

	//Create the concatenated transformation matrix
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;

	//Get the transpose
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);

	//Pass the transposed matrix to the shader
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

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
	if(FAILED(D3DXLoadMeshFromX("..\\media\\fattorus.x",
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
	m_pD3DDevice->DeleteVertexShader(m_BasicShader);

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
	
	//Clean up
	if (m_pPlaneVertexBuffer)
	{
		m_pPlaneVertexBuffer->Release();
		m_pPlaneVertexBuffer = NULL;
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
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
						D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
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
	pVertices[1].x = -30.0f; pVertices[1].ny = 1.0f; pVertices[1].z =  30.0f;
	pVertices[2].x =  30.0f; pVertices[2].ny = 1.0f; pVertices[2].z = -30.0f;
	pVertices[3].x =  30.0f; pVertices[3].ny = 1.0f; pVertices[3].z =  30.0f;

	//Setting the alpha value of the plane to zero allows use to interact more
	//easily with the existing shader. This is not a general purpose requirement,
	//just a little bit of a hack to give us some shortcuts with the way we
	//use the plane with the shader.
	pVertices[0].color = pVertices[1].color = 
		                 pVertices[2].color = pVertices[3].color = 0x00ffffff;

	m_pPlaneVertexBuffer->Unlock();

	return S_OK;
}

void CTechniqueApplication::RenderPlaneAndShadow()
{
    //Enable the stencil testing and set the comparison function so that the 
	//plane always increments the buffer
    m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE,   TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,     D3DCMP_ALWAYS);

    // Increment the stencil buffer for each pixel drawn. It will always
	//pass the way it is set up...
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	
	//The light is moving side to side over the model.
	float Time = (float)GetTickCount() / 1000.0f;
	D3DXVECTOR4 LightPosition(150.0f * sin(Time), 70.0f, 0.0f, 1.0f);

	//Set the light direction based on what was computed for the shadow
	D3DXVECTOR4 LightDir = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f) - LightPosition;
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);
	
	//Set up the shader matrix. We're not using the world matrix, which is the
	//same as setting the world matrix to an identity matrix...  This MAY NOT
	//be a generally good idea, but I'm just showing how you can take shortcuts.
	D3DXMATRIX ShaderMatrix = m_ViewMatrix * 
							  m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Draw the plane.
	m_pD3DDevice->SetStreamSource(0, m_pPlaneVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	
	//Setup the shadow pass
	D3DXMATRIX  ShadowMatrix;

	//This shader matrix has the shadow matrix built in
	D3DXPLANE   ShadowPlane(0.0f, 1.0f, 0.0f, 0.0);
	D3DXMatrixShadow(&ShadowMatrix, &LightPosition, &ShadowPlane);
	ShaderMatrix = m_WorldMatrix * ShadowMatrix *  
		           m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//draw the shadow pass of the mesh
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);

    //Set up the Z and stencil states so that depth testing is disabled and
	//any pixel that is drawn "on top of the plane" will draw and increment
	//the stencil value.
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,   FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_INCR);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_LESS);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,   0);

	//Set up the alpha blending so that no color values are written
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
									   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());

	//Now, this is a little bit of a hack, but rather than change the data 
	//for the plane, I am setting up the light parameters so that
	//the plane turns black...
	D3DXVECTOR4 HackLightDir(0.0, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 Ambient (0.0,  0.0f, 0.0f, 0.25f);
	m_pD3DDevice->SetVertexShaderConstant(4, &HackLightDir, 1);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

	//Draw the plane again, only this time, only draw where the
	//shadow should be.
    m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_LESS);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,   1);
	
    //The alpha ambient value lets the plane draw with some blending.
	//in this case, the alpha ambient value controls the darkness of
	//the shadow. In a way, it's the inverse of the real ambient lighting.
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Again, set everything up and render the plane
	ShaderMatrix = m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);
	m_pD3DDevice->SetStreamSource(0, m_pPlaneVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//Turn off all these testing states
    m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,          TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE,    FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//Set the light direction so that the mesh rendering code can use it.
	//Convert the light direction to object space.
	D3DXMATRIX InverseWorld;
	D3DXMatrixInverse(&InverseWorld, NULL, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);
}