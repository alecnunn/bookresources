/***************************************************************
* LightingApplication.cpp                                      *
*                                                              *
* This file contains the implementation of the                 *
* CLightingApplication class.								   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* gdi32.lib                                                    *
* advapi32.lib                                                 *
*                                                              *
***************************************************************/

#include "LightingApplication.h"

//define the lights
#define SPOT_LIGHT        0
#define DIR_LIGHT         1
#define POINT_LIGHT       2
#define MAX_LIGHT         2


//define the number of vertices to be used
#define NUM_CIRCLE_POINTS    40
#define NUM_CONE_POINTS      6
#define NUM_VERTICES (NUM_CIRCLE_POINTS + NUM_CONE_POINTS)

//Our new vertex structure for the simple lighting visuals
struct VISUALS_VERTEX
{
	float x, y, z;
	DWORD diffuse;
};

//The FVF for the lighting visuals
#define D3DFVF_VISUALSVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

//Initialize all the pointers
CLightingApplication::CLightingApplication()
{
	m_pMesh          = NULL;
	m_pMeshMaterials = NULL;
	m_NumMaterials   = 0;
	m_CurrentSubset  = 0;
	m_CurrentLight   = 0;
	m_pLightsBuffer  = NULL;
}

CLightingApplication::~CLightingApplication()
{
}

BOOL CLightingApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	//Set the device states
	SetupDevice();

	//Initialize the data for the lighting visuals
	if (!InitializeLightVisuals())
		return FALSE;

	//Load the sample mesh
	return LoadMesh();
}

void CLightingApplication::SetupDevice()
{
	D3DXMatrixLookAtLH(&m_ViewMatrix, &D3DXVECTOR3(0.0f, 20.0f, -20.0f),
		               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					   &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	D3DXMatrixIdentity(&m_WorldMatrix);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 8,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 500.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);

	//Tell the device about the new light
	m_pD3DDevice->LightEnable(0, TRUE);

	//Get the number of lights.  We don't actually use this number
	//for anything.
	D3DCAPS8 Caps;
	m_pD3DDevice->GetDeviceCaps(&Caps);
	DWORD NumLights = Caps.MaxActiveLights;
	
	//Make sure lighting is enabled and there is a little 
	//bit of ambient light.
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00101010);

	InitializeLights();
}

BOOL CLightingApplication::PreReset()
{
	//Destroy our own viuals (the mesh is "managed")
	DestroyLightVisuals();
	
	return TRUE;
}

BOOL CLightingApplication::PostReset()
{
	//Make sure the states are reset
	SetupDevice();

	//reinitialize the light visuals
	return InitializeLightVisuals();
}

BOOL CLightingApplication::PreTerminate()
{
	//Destroy everything
	DestroyLightVisuals();
	
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

BOOL CLightingApplication::HandleMessage(MSG *pMessage)
{
	//Handle user input.  F1 sets the current submesh.
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_F1)
	{
		if (++m_CurrentSubset > m_NumMaterials - 1)
			m_CurrentSubset = 0;
	}

	//F2 runs through the different light types
	if (pMessage->message == WM_KEYDOWN && pMessage->wParam == VK_F2)
	{
		if (++m_CurrentLight > MAX_LIGHT)
			m_CurrentLight = 0;
	}

	//Mke sure the base class receives any messages
	return CHostApplication::HandleMessage(pMessage);
}


BOOL CLightingApplication::LoadMesh()
{
	//Our basic mesh loading function.  In this case, each sub mesh
	//uses a different material, so we denote submeshes by material.
	LPD3DXBUFFER pD3DXMtrlBuffer;

    if(FAILED(D3DXLoadMeshFromX("..\\media\\planes.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &m_pMesh)))
        return FALSE;

	D3DXMATERIAL* d3dxMaterials;
	d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;
        m_pMeshMaterials[MatCount].Ambient = 
								m_pMeshMaterials[MatCount].Diffuse;
    }

    pD3DXMtrlBuffer->Release();

	return TRUE;
}

void CLightingApplication::Render()
{
	//Make sure lighting is enabled.  That's the point!
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//Make sure the current light is set (in case parameters have changed)
	m_pD3DDevice->SetLight(0, &m_Light[m_CurrentLight]);
	
    //Set the current material
	m_pD3DDevice->SetMaterial(&m_pMeshMaterials[m_CurrentSubset]);
   
    //Draw the current subset
	m_pMesh->DrawSubset(m_CurrentSubset);

	//Render the lighting visuals
	RenderLightVisuals();
}

//This function builds each light.  Experiment with different values to
//see the effect.
void CLightingApplication::InitializeLights()
{
	ZeroMemory(&m_Light[POINT_LIGHT], sizeof(D3DLIGHT8));
	m_Light[POINT_LIGHT].Type         = D3DLIGHT_POINT;
	m_Light[POINT_LIGHT].Diffuse.r    = m_Light[POINT_LIGHT].Diffuse.g = 
							            m_Light[POINT_LIGHT].Diffuse.b = 1.0f;
	m_Light[POINT_LIGHT].Position     = D3DXVECTOR3(0.0f, 1.5f, 0.0f);
	m_Light[POINT_LIGHT].Range        = 5.0f;
	m_Light[POINT_LIGHT].Attenuation0 = 0.0f;
	m_Light[POINT_LIGHT].Attenuation1 = 1.0f;

	ZeroMemory(&m_Light[DIR_LIGHT], sizeof(D3DLIGHT8));
	m_Light[DIR_LIGHT].Type         = D3DLIGHT_DIRECTIONAL;
	m_Light[DIR_LIGHT].Diffuse.r    = m_Light[DIR_LIGHT].Diffuse.g = 
							          m_Light[DIR_LIGHT].Diffuse.b = 1.0f;
	m_Light[DIR_LIGHT].Direction    = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);

	ZeroMemory(&m_Light[SPOT_LIGHT], sizeof(D3DLIGHT8));
	m_Light[SPOT_LIGHT].Type         = D3DLIGHT_SPOT;
	m_Light[SPOT_LIGHT].Diffuse.r    = m_Light[SPOT_LIGHT].Diffuse.g = 
							           m_Light[SPOT_LIGHT].Diffuse.b = 1.0f;
	m_Light[SPOT_LIGHT].Direction    = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_Light[SPOT_LIGHT].Position     = D3DXVECTOR3(0.0f,  5.0f, 0.0f);
	m_Light[SPOT_LIGHT].Theta        = D3DXToRadian(10.0f);
	m_Light[SPOT_LIGHT].Phi          = D3DXToRadian(60.0f);
	m_Light[SPOT_LIGHT].Falloff      = 1.0f;
	m_Light[SPOT_LIGHT].Range        = 10.0f;
	m_Light[SPOT_LIGHT].Attenuation0 = 1.0f;
}

//This function creates visuals for point lights and spot lights.
//There is no visualization for directional lights because it is
//a simple vector.
BOOL CLightingApplication::InitializeLightVisuals()
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
										NUM_VERTICES
										* sizeof(VISUALS_VERTEX),
										D3DUSAGE_WRITEONLY,
										D3DFVF_VISUALSVERTEX,
										D3DPOOL_DEFAULT,
										&m_pLightsBuffer)))
		return FALSE;

	VISUALS_VERTEX *pVertices;

	if (FAILED(m_pLightsBuffer->Lock(0,
					NUM_VERTICES * sizeof(VISUALS_VERTEX),
					(BYTE **)&pVertices,
					0)))
	{
		DestroyLightVisuals();
		return FALSE;
	}

	//an easy way to set all the colors to white
	memset(pVertices, 0xFF, NUM_VERTICES * sizeof(VISUALS_VERTEX));

	//Here we set the vertices for the sides of the spotlight cone.
	//They are set to simple unit values so that they can be scaled
	//easily with transformation matrices.
	pVertices[0].x = -1.0f;	pVertices[0].y = -1.0f; pVertices[0].z = 0.0f;
	pVertices[1].x =  0.0f;	pVertices[1].y =  0.0f; pVertices[1].z = 0.0f;
	pVertices[2].x =  1.0f; pVertices[2].y = -1.0f; pVertices[2].z = 0.0f;

	pVertices[3].x = 0.0f; pVertices[3].y = -1.0f; pVertices[3].z = -1.0f;
	pVertices[4].x = 0.0f; pVertices[4].y =  0.0f; pVertices[4].z =  0.0f;
	pVertices[5].x = 0.0f; pVertices[5].y = -1.0f; pVertices[5].z =  1.0f;

	//Here we create a circle of vertices that will be reused for the point
	//light sphere and the umbra and penumbra of the spotlight. We set several
	//vertices in a lkoop and set the last one separately to ensure the 
	//circle is complete.
	long Counter;
	for (Counter = 0; Counter < NUM_CIRCLE_POINTS - 1; Counter++)
	{
		float Angle = 2.0f * D3DX_PI / (NUM_CIRCLE_POINTS - 1) * Counter;
		pVertices[Counter + NUM_CONE_POINTS].x = cos(Angle);
		pVertices[Counter + NUM_CONE_POINTS].y = sin(Angle);
		pVertices[Counter + NUM_CONE_POINTS].z = 0.0f;
	}
	pVertices[Counter + NUM_CONE_POINTS].x = 1.0f;
	pVertices[Counter + NUM_CONE_POINTS].y = 0.0f;
	pVertices[Counter + NUM_CONE_POINTS].z = 0.0f;

	m_pLightsBuffer->Unlock();

	return TRUE;
}

void CLightingApplication::DestroyLightVisuals()
{
	//Destroy the visuals vertex buffer
	if (m_pLightsBuffer)
	{
		m_pLightsBuffer->Release();
		m_pLightsBuffer = NULL;
	}
}

void CLightingApplication::RenderLightVisuals()
{
	//Here are several reusable matrices.
	D3DXMATRIX Translation;
	D3DXMATRIX Rotation;
	D3DXMATRIX Scaling;
	D3DXMATRIX Transform;
	
	//Here we make sure that lighting is disabled so that it
	//does not affect the simple lines.  We also set the 
	//vertex shader and source.
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDevice->SetVertexShader(D3DFVF_VISUALSVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pLightsBuffer, sizeof(VISUALS_VERTEX));
	
	//Render the spotlight visuals
	if (m_CurrentLight == SPOT_LIGHT)
	{
		//Translate to the light's position
		D3DXMatrixTranslation(&Translation, m_Light[SPOT_LIGHT].Position.x, 
			                                m_Light[SPOT_LIGHT].Position.y, 
									        m_Light[SPOT_LIGHT].Position.z);

		//We'll scale the spotlight cone according to it's distance
		//from the origin.  That defines the "cone of interest" and
		//we can then scale the width of the cone according to the angle.
		//Scale for the penumbra
		FLOAT ConeLength = D3DXVec3Length(&D3DXVECTOR3(m_Light[SPOT_LIGHT].Position));
		FLOAT ConeWidth = ConeLength * tan(m_Light[SPOT_LIGHT].Phi / 2.0f);
		D3DXMatrixScaling(&Scaling, ConeWidth, ConeLength, ConeWidth);
		Transform = Scaling * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 2);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 3, 2);

		//Now draw the circle - translate to the new position, but keep the
		//circle flush with the mesh.
		D3DXMatrixTranslation(&Translation, m_Light[SPOT_LIGHT].Position.x, 
			                                0.0f, 
									        m_Light[SPOT_LIGHT].Position.z);
		//Rotate the circle so that it is aligned with the mesh. Scale it 
		//according to the cone width.  Set the transform and render it.
		D3DXMatrixRotationX(&Rotation, D3DX_PI / 2.0f);
		D3DXMatrixScaling(&Scaling, ConeWidth, ConeWidth, ConeWidth);
		Transform = Rotation * Scaling * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONE_POINTS,
			                        NUM_CIRCLE_POINTS - 1);

		//Now repeat for the umbra
		D3DXMatrixTranslation(&Translation, m_Light[SPOT_LIGHT].Position.x, 
			                                m_Light[SPOT_LIGHT].Position.y, 
									        m_Light[SPOT_LIGHT].Position.z);
		ConeWidth = ConeLength * tan(m_Light[SPOT_LIGHT].Theta / 2.0f);
		D3DXMatrixScaling(&Scaling, ConeWidth, ConeLength, ConeWidth);

		Transform = Scaling * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);

		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 2);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 3, 2);

		D3DXMatrixTranslation(&Translation, m_Light[SPOT_LIGHT].Position.x, 
			                                0.0f, 
									        m_Light[SPOT_LIGHT].Position.z);
		D3DXMatrixRotationX(&Rotation, D3DX_PI / 2.0f);
		D3DXMatrixScaling(&Scaling, ConeWidth, ConeWidth, ConeWidth);
		Transform = Rotation * Scaling * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONE_POINTS,
			                        NUM_CIRCLE_POINTS - 1);
	}

	//For the point light, we position it, scale it, and draw two instances of
	//the circle, rotating the second circle by 90 degrees to create a very
	//simple wireframe.
	if (m_CurrentLight == POINT_LIGHT)
	{	
		D3DXMatrixTranslation(&Translation, m_Light[POINT_LIGHT].Position.x, 
			                                m_Light[POINT_LIGHT].Position.y, 
									        m_Light[POINT_LIGHT].Position.z);
		D3DXMatrixScaling(&Scaling, m_Light[POINT_LIGHT].Range, 
			                        m_Light[POINT_LIGHT].Range, 
									m_Light[POINT_LIGHT].Range);
		Transform = Scaling * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONE_POINTS,
			                        NUM_CIRCLE_POINTS - 1);
		D3DXMatrixRotationY(&Rotation, D3DX_PI / 2.0f);
		Transform = Scaling * Rotation * Translation;
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, NUM_CONE_POINTS,
			                        NUM_CIRCLE_POINTS - 1);
	}

	//Make sure that the world transform is set back to the identity matrix.
	D3DXMatrixIdentity(&Transform);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &Transform);
}