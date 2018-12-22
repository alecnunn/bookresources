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
#define D3DFVF_MASKVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct MESH_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

struct MASK_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

CTechniqueApplication::CTechniqueApplication()
{
	m_pPlaneVertexBuffer       = NULL;
	m_pMaskVertexBuffer        = NULL;
	m_pMeshVertexBuffer        = NULL;
	m_pMeshIndexBuffer         = NULL;
	m_pMesh                    = NULL;
	m_pMeshMaterials           = NULL;
	m_NumMaterials             = 0;
	m_BasicShader              = 0;
	m_VolumeShader             = 0;
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

	if (FAILED(CreatePlaneBuffers()))
		return FALSE;

	//Create the shader
	if (FAILED(CreateShaders()))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
	//Set the view parameters
	D3DXVECTOR4 EyePos(0.0, 60.0f, -60.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &(D3DXVECTOR3)EyePos,
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	//The light is moving side to side over the model.
	float Time = (float)GetTickCount() / 2000.0f;
	D3DXVECTOR4 LightPos(150.0f * sin(Time), 90.0f, 0.0f, 1.0f);

	//Set the light direction based on what was computed for the shadow.
	//Assume the light is pointer toward the origin.
	D3DXVECTOR4 LightDir = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f) - LightPos;
	D3DXVec4Normalize(&LightDir, &LightDir);

	//Set the ambient light value.
	D3DXVECTOR4 Ambient    (0.0f,  0.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(5, &Ambient, 1);

	//Set the expansion amount in c8.x. 
	//Also, the shader expects the Y value to be 0.0
	D3DXVECTOR4 PushDistance(100.0f, 0.0f, 0.0f, 0.0f);
	m_pD3DDevice->SetVertexShaderConstant(8, &PushDistance, 1);

	//Set up the basic shader first
	m_pD3DDevice->SetVertexShader(m_BasicShader);

	//The real objects are always in front of the shadow passes
	m_pD3DDevice->SetRenderState(D3DRS_ZBIAS, 1);
		
	//First, Draw the plane
	RenderPlane();

	//Some reusable matrices.
	D3DXMATRIX Rotation;
    D3DXMATRIX Translation;

	//This first instance of the mesh is just an object for the 
	//shadow caster to cast shadows on. In this case, I'm not
	//setting it up to cast it's own shadows.
	D3DXMatrixTranslation(&m_WorldMatrix, 20.0f, 10.0f, 0.0f);
	D3DXMATRIX ShaderMatrix = m_WorldMatrix * m_ViewMatrix * 
				              m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Set the light direction so that the mesh rendering code can use it.
	//Convert the light direction to object space.
	D3DXMATRIX InverseWorld;
	float Det;
	D3DXMatrixInverse(&InverseWorld, &Det, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);

	//Do the same for the light position
	D3DXVec4Transform(&LightPos, &LightPos, &InverseWorld);
	m_pD3DDevice->SetVertexShaderConstant(7, &LightPos, 1);

	//Draw the mesh object that does not cast shadoww, but does "receive" them.
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());


	D3DXMatrixRotationY(&Rotation, (float)GetTickCount() / 1000.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, 20.0f, 0.0f);
	m_WorldMatrix = Rotation * Translation;
	
	//Set the world matrix and the shader matrix as usual. Also,
	//transform the light parameters to object space. This is all the
	//same as what we've seen before...
	ShaderMatrix = m_WorldMatrix * m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);
	D3DXMatrixInverse(&InverseWorld, &Det, &m_WorldMatrix);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);
	D3DXVec4Normalize(&LightDir, &LightDir);
	m_pD3DDevice->SetVertexShaderConstant(4, &LightDir, 1);
	D3DXVec4Transform(&LightPos, &LightPos, &InverseWorld);
	m_pD3DDevice->SetVertexShaderConstant(7, &LightPos, 1);

	//Draw the mesh - this is the mesh that is actually seen.
	m_pD3DDevice->SetStreamSource(0, m_pMeshVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->SetIndices(m_pMeshIndexBuffer, 0);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    								   m_pMesh->GetNumVertices(), 0,
									   m_pMesh->GetNumFaces());


	//Clear the stencil buffer and set it up to always pass.
	//When it passes, have it increment the stencil value.
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_ALWAYS);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_INCR);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    1);
   
	//Make sure that the shadow volume steps do not "corrupt" the
	//real depth values.
	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    //Don't write to the color buffer
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//Set the volume shader
	m_pD3DDevice->SetVertexShader(m_VolumeShader);

	//The shadows are always under the real objects
	m_pD3DDevice->SetRenderState(D3DRS_ZBIAS, 0);

	//First, draw only the front facing triangles. Any pixel that passes
	//the depth test will set that stencil value to 1
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,         D3DCULL_CCW);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    							   m_pMesh->GetNumVertices(), 0,
								   m_pMesh->GetNumFaces());


	//Now, draw only the back facing triangles. They will decrement
	//the stencil value. This will essentially clear all the values
	//except for the real surfaces that intersect the shadow volume.
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,         D3DCULL_CW);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_DECR);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 
    							   m_pMesh->GetNumVertices(), 0,
    							   m_pMesh->GetNumFaces());


    //Now, we set up the stencil test to draw the big shadow rectangle.
	//This test will draw a "shadow" everywhere that the stencil value is 1
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_LESSEQUAL);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    1);

	//Turn off depth testing and culling for our big rectangle. Also,
	//set the alpha blending to use the transparency of the rectangle. The 
	//more transparent it is, the lighter the shadow.
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Draw..
	m_pD3DDevice->SetVertexShader(D3DFVF_MASKVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pMaskVertexBuffer, sizeof(MASK_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    //Set all the tests back to "normal" for the next drawing pass
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
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
	m_pD3DDevice->DeleteVertexShader(m_VolumeShader);

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
	m_pD3DDevice->DeleteVertexShader(m_VolumeShader);
	
	//Clean up
	if (m_pPlaneVertexBuffer)
	{
		m_pPlaneVertexBuffer->Release();
		m_pPlaneVertexBuffer = NULL;
	}

	if (m_pMaskVertexBuffer)
	{
		m_pMaskVertexBuffer->Release();
		m_pMaskVertexBuffer = NULL;
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

	//Assemble and create the volume shader. Under real circumstances, you would 
	//probably want to do more error handling.
	if (FAILED(D3DXAssembleShaderFromFile("..\\media\\Shaders\\shadowvolume.vsh", 
		                            0, NULL, &pShaderBuffer, &pShaderErrors)))
		return E_FAIL;

	if (FAILED(m_pD3DDevice->CreateVertexShader(Declaration, 
		                           (DWORD *)pShaderBuffer->GetBufferPointer(),
								   &m_VolumeShader, 0)))
		return E_FAIL;

	//release the working buffers
	pShaderBuffer->Release();

	return S_OK;
}

void CTechniqueApplication::PreRender()
{
	//Clear the device
	m_pD3DDevice->Clear(0, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);

	//Call BeginScene to set up the device
	m_pD3DDevice->BeginScene();
		
	return;
}


HRESULT CTechniqueApplication::CreatePlaneBuffers()
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
	pVertices[0].x = -50.0f; pVertices[0].ny = 1.0f; pVertices[0].z = -50.0f;
	pVertices[1].x =  50.0f; pVertices[1].ny = 1.0f; pVertices[1].z = -50.0f;
	pVertices[2].x = -50.0f; pVertices[2].ny = 1.0f; pVertices[2].z =  50.0f;
	pVertices[3].x =  50.0f; pVertices[3].ny = 1.0f; pVertices[3].z =  50.0f;

	pVertices[0].color = pVertices[1].color = 
		                 pVertices[2].color = pVertices[3].color = 0xffffffff;

	m_pPlaneVertexBuffer->Unlock();

	//Also, create the mask vertex buffer
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(MASK_VERTEX), 
		                                        0, D3DFVF_MASKVERTEX,
									            D3DPOOL_MANAGED,
												&m_pMaskVertexBuffer)))
		return E_FAIL;

	//Create a set of 4 vertices for the plane
	MASK_VERTEX *pMaskVertices;

	//Lock the vertex buffer, but allow writing.
	m_pMaskVertexBuffer->Lock(0, 4 * sizeof(MASK_VERTEX), (BYTE **)&pMaskVertices, 0);

	//Initialize everything to zero. If I don't set a certain attribute,
	//assume it's zero..
	memset(pMaskVertices, 0x00, 4 * sizeof(MASK_VERTEX));

	pMaskVertices[0].x = 0.0f; pMaskVertices[0].y = 0.0f;
	pMaskVertices[1].x = (float)m_WindowWidth; pMaskVertices[1].y = 0.0f;
	pMaskVertices[2].x = 0.0f; pMaskVertices[2].y = (float)m_WindowHeight;
	pMaskVertices[3].x = (float)m_WindowWidth; pMaskVertices[3].y = (float)m_WindowHeight;

	pMaskVertices[0].color = pMaskVertices[1].color = 
		                     pMaskVertices[2].color = 
							 pMaskVertices[3].color = 0x88000000;

	m_pMaskVertexBuffer->Unlock();

	return S_OK;
}

void CTechniqueApplication::RenderPlane()
{
	//This time, the plane doesn't have as much of a role in shadow generation
	//draw it once and move on. Most of the work is done on the mesh.
	D3DXMATRIX ShaderMatrix = m_ViewMatrix * m_ProjectionMatrix;
	D3DXMatrixTranspose(&ShaderMatrix, &ShaderMatrix);
	m_pD3DDevice->SetVertexShaderConstant(0, &ShaderMatrix, 4);

	//Draw the mesh
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice->SetStreamSource(0, m_pPlaneVertexBuffer, sizeof(MESH_VERTEX));
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}