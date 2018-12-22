/***************************************************************
* GeometryApplication.cpp                                      *
*                                                              *
* This file contains the implementation of the                 *
* CGeometryApplication class.								   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* gdi32.lib                                                    *
* advapi32.lib                                                 *
*                                                              *
***************************************************************/

#include "GeometryApplication.h"

#define D3DFVF_GEOMETRYVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)

#define NUM_LIST_VERTICES  6

#define NUM_FAN_SIDES      10
#define NUM_FAN_VERTICES   NUM_FAN_SIDES + 2

#define NUM_STRIP_SIDES    10
#define NUM_STRIP_VERTICES (2 * NUM_STRIP_SIDES) + 2

#define FAN_OFFSET     NUM_LIST_VERTICES
#define STRIP_OFFSET   FAN_OFFSET + NUM_FAN_VERTICES

#define NUM_VERTICES (NUM_LIST_VERTICES + NUM_FAN_VERTICES + \
                      NUM_STRIP_VERTICES)

CGeometryApplication::CGeometryApplication()
{
	//Initialize everything
	m_pVertexBuffer  = NULL;
	m_pIndexBuffer   = NULL;
	m_pMesh          = NULL;
	m_pMeshMaterials = NULL;
	m_NumMaterials   = 0;
}

CGeometryApplication::~CGeometryApplication()
{
	DestroyGeometry();
}

BOOL CGeometryApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	SetupDevice();

	if (!CreateGeometry())
		return FALSE;

	return LoadMesh();
}

void CGeometryApplication::SetupDevice()
{
	//Now that we have to worry about lighting, renderstates, 
	//transforms, etc. we have to make sure that when we lose a
	//device, we recreate the device settings along with the 
	//vertex buffers!
	
	//Set the view matrix to a default 
	//identity matrix.
	D3DXMatrixIdentity(&m_ViewMatrix);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 100.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	InitializeLights();
}

BOOL CGeometryApplication::PreReset()
{
	DestroyGeometry();
	return TRUE;
}

BOOL CGeometryApplication::PostReset()
{
	SetupDevice();
	return CreateGeometry();
}

BOOL CGeometryApplication::PreTerminate()
{
	DestroyGeometry();

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

BOOL CGeometryApplication::CreateGeometry()
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
										NUM_VERTICES
										* sizeof(GEOMETRY_VERTEX),
										D3DUSAGE_WRITEONLY,
										D3DFVF_GEOMETRYVERTEX,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer)))
		return FALSE;

	//Since we are reusing the strip vertices, the number
	//of indices will be equal to the number of sides
	//times 2 triangles per side * 3 vertices per triangle.
	if (FAILED(m_pD3DDevice->CreateIndexBuffer(
						sizeof(short) * NUM_STRIP_SIDES * 2 * 3,
						D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
						D3DPOOL_DEFAULT, &m_pIndexBuffer)))
		return FALSE;

	//First, create the material that will be applied to 
	//every piece of sample geometry
	ZeroMemory( &m_ShapeMaterial, sizeof(D3DMATERIAL8) );
	m_ShapeMaterial.Diffuse.r = m_ShapeMaterial.Ambient.r = 1.0f;
	m_ShapeMaterial.Diffuse.g = m_ShapeMaterial.Ambient.g = 1.0f;
	m_ShapeMaterial.Diffuse.b = m_ShapeMaterial.Ambient.b = 1.0f;
	m_ShapeMaterial.Diffuse.a = m_ShapeMaterial.Ambient.a = 1.0f;


	//Fill the vertex buffer once.
	FillVertexBuffer();

	return TRUE;
}

BOOL CGeometryApplication::LoadMesh()
{
    //This is basically the same as many of the samples in the SDK. 
	//Load the mesh and retrieve the materials.  We do not use the 
	//textures because we haven't talked about textures yet!
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//Load the mesh.  We don't care about the adjacency info
	//because this will just show the simplest mesh usage.  Later,
	//we'll talk about mesh optimization.  Notice the mesh is in 
	//managed memory - it is not affected by lost devices.
    if(FAILED(D3DXLoadMeshFromX("..\\media\\TwoSpheres.x",
								D3DXMESH_MANAGED, 
                                m_pD3DDevice, NULL, &pD3DXMtrlBuffer,
								&m_NumMaterials, &m_pMesh)))
        return FALSE;

    //Retrieve the materials
	D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

    m_pMeshMaterials = new D3DMATERIAL8[m_NumMaterials];

    for(long MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
        //Copy the material member of the d3dx material structure
		//to the basic material structure and set the ambient color
		//to the diffuse color.
		m_pMeshMaterials[MatCount] = d3dxMaterials[MatCount].MatD3D;
        m_pMeshMaterials[MatCount].Ambient = 
								m_pMeshMaterials[MatCount].Diffuse;
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

	//This doesn't do anything, it's just for show...
	DWORD *pAttribBuffer;
	m_pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &pAttribBuffer);

	m_pMesh->UnlockAttributeBuffer();

	return TRUE;
}

void CGeometryApplication::DestroyGeometry()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}
}

void CGeometryApplication::Render()
{
	//These are our individual transforms
	D3DXMATRIX Translation;
	D3DXMATRIX Rotation;
	D3DXMATRIX Scaling;

	//Set up some arbitrary transform for our mesh.  It doesn't
	//really matter what we use, but we should push it away from
	//the camera and down, so that it does not interfere with the 
	//other geometry.
	D3DXMatrixRotationYawPitchRoll(&Rotation,
		                           (float)GetTickCount() / 100.0f,
								   0.0, 0.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, -1.0f, 5.0f);
	m_WorldMatrix = Rotation * Translation;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	RenderMesh();
	
	//Notice that we moved this code to the render function 
	//because the mesh object will set other sources and we
	//need to reset ours when we need them.
	m_pD3DDevice->SetVertexShader(D3DFVF_GEOMETRYVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 
								  sizeof(GEOMETRY_VERTEX));

	//These indices will be used for any indexed primitives.
	m_pD3DDevice->SetIndices(m_pIndexBuffer, 0);

	//Set the material
	m_pD3DDevice->SetMaterial(&m_ShapeMaterial);
		
	//Push the mesh away from the camera, tilt it away from
	//the camera (to give it a more interesting angle), and
	//spin it about the Y axis.
	D3DXMatrixRotationYawPitchRoll(&Rotation,
		                           (float)GetTickCount() / 1000.0f,
								   D3DX_PI / 8.0f, 0.0f);
	D3DXMatrixTranslation(&Translation, 0.0f, 0.0f, 5.0f);
	m_WorldMatrix = Rotation * Translation;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);


	//Draw the first two triangles as a list
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	//Draw the fan.  Notice that the number of triangles is always 
	//the number of vertices minus 2.
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,  FAN_OFFSET,
		                        NUM_FAN_SIDES);

	//Draw the strip.  Notice that the number of triangles is always 
	//the number of vertices minus 2.
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, STRIP_OFFSET,
		                        NUM_STRIP_SIDES * 2);

	D3DXMatrixScaling(&Scaling, 1.5f, 0.5f, 1.5f);
	m_WorldMatrix = Scaling * m_WorldMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_VERTICES, 0, NUM_STRIP_SIDES * 2);
}

void CGeometryApplication::RenderMesh()
{
    //The mesh is divided into subsets based on material.  Loop
	//through the materials and draw each subset.
	for(DWORD MatCount = 0; MatCount < m_NumMaterials; MatCount++)
    {
        // Set the material for this subset
        m_pD3DDevice->SetMaterial(&m_pMeshMaterials[MatCount]);
       
        // Draw the mesh subset
        m_pMesh->DrawSubset(MatCount);
    }
}

BOOL CGeometryApplication::FillVertexBuffer()
{
	if (!m_pVertexBuffer)
		return FALSE;
	
	GEOMETRY_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0,
					NUM_VERTICES * sizeof(GEOMETRY_VERTEX),
					(BYTE **)&pVertices,
					0)))
	{
		DestroyGeometry();
		return FALSE;
	}

	//Set everything to white
	memset(pVertices, 0xff, NUM_VERTICES * sizeof(GEOMETRY_VERTEX));

	//Initialize all of the sample primitives
	InitializeList(pVertices);
	InitializeFan(pVertices);
	InitializeStrip(pVertices);
	InitializeIndexed();

	m_pVertexBuffer->Unlock();

	return TRUE;	
}

void CGeometryApplication::InitializeFan(GEOMETRY_VERTEX *pVertices)
{
	//Define the center of the fan.  This is going to be centered on
	//the point (0, 1, 0). Also, set the normal to (0, 1, 0)
	pVertices[FAN_OFFSET].x = pVertices[FAN_OFFSET].z = 0.0f;
	pVertices[FAN_OFFSET].y = 1.0f;
	pVertices[FAN_OFFSET].nx = pVertices[FAN_OFFSET].nz = 0.0f;
	pVertices[FAN_OFFSET].ny = 1.0f;

	//Now, define the perimeter of the fan.  Here we will define
	//a circle with a radius of 0.5 units.
	//The reason for the -2 and +1 is to account for the first vertex 
	//(the center) and the last, which we set separately to ensure
	//that the circle is not incomplete due to rounding, etc.
	for (long FanIndex = 0; FanIndex < NUM_FAN_VERTICES - 2; FanIndex++)
	{
		float Angle = (float)(FanIndex) * (2.0f * D3DX_PI) / 
			          (float)(NUM_FAN_VERTICES - 2);
		
		//set the X and Z values
		pVertices[FAN_OFFSET + FanIndex + 1].x = 0.5f * cos(Angle);
		pVertices[FAN_OFFSET + FanIndex + 1].z = 0.5f * sin(Angle);

		//Set the Y Value
		pVertices[FAN_OFFSET + FanIndex + 1].y = 1.0f;
		
		//Set the normal
		pVertices[FAN_OFFSET + FanIndex + 1].nx = 
		pVertices[FAN_OFFSET + FanIndex + 1].nz = 0.0f;
		pVertices[FAN_OFFSET + FanIndex + 1].ny = 1.0f;
	}

	//Define the final slice (to complete the circle)
	//set the X and Z values
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].x = 0.5f;
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].z = 0.0f;

	//Set the Y Value
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].y = 1.0f;
	
	//Set the normal
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].nx = 
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].nz = 0.0f;
	pVertices[FAN_OFFSET + NUM_FAN_VERTICES - 1].ny = 1.0f;
}

void CGeometryApplication::InitializeStrip(GEOMETRY_VERTEX *pVertices)
{
	//Now, define the perimeter of the fan.  Here we will define
	//a circle with a radius of 0.5 units.
	//The reason for the -2 and +1 is to account for the first vertex 
	//(the center) and the last, which we set separately to ensure
	//that the circle is not incomplete due to rounding, etc.
	for (long StripIndex = 0; 
		 StripIndex < NUM_STRIP_VERTICES; 
		 StripIndex += 2)
	{
		float Angle = (float)(StripIndex) * (2.0f * D3DX_PI) / 
			          (float)(NUM_STRIP_SIDES * 2);
		
		//set the X and Z values
		pVertices[STRIP_OFFSET + StripIndex].x = 
		pVertices[STRIP_OFFSET + StripIndex + 1].x = 0.5f * cos(Angle);
		pVertices[STRIP_OFFSET + StripIndex].z = 
		pVertices[STRIP_OFFSET + StripIndex + 1].z = 0.5f * sin(Angle);

		//Set the Y Value
		pVertices[STRIP_OFFSET + StripIndex].y     = 0.0f;
		pVertices[STRIP_OFFSET + StripIndex + 1].y = 1.0f;
		
		//Set the normal
		pVertices[STRIP_OFFSET + StripIndex].nx = 
		pVertices[STRIP_OFFSET + StripIndex + 1].nx = cos(Angle);
		pVertices[STRIP_OFFSET + StripIndex].nz = 
		pVertices[STRIP_OFFSET + StripIndex + 1].nz = sin(Angle);
		pVertices[STRIP_OFFSET + StripIndex].ny = 
		pVertices[STRIP_OFFSET + StripIndex + 1].ny = 0.0f;
	}
}

void CGeometryApplication::InitializeList(GEOMETRY_VERTEX *pVertices)
{
	//This is one corner of the square
	pVertices[0].x = -1.0f;
	pVertices[0].z = -1.0f;

	//These are the shared corners.  Obviously an opportunity
	//for optimization!
	pVertices[1].x = pVertices[3].x = -1.0f;
	pVertices[1].z = pVertices[3].z =  1.0f;

	pVertices[2].x = pVertices[4].x =  1.0f;
	pVertices[2].z = pVertices[4].z = -1.0f;

	//This is the final corner of the square
	pVertices[5].x =  1.0f;
	pVertices[5].z =  1.0f;

	//all the Y values are the same
	pVertices[0].y = pVertices[1].y = 
	pVertices[2].y = pVertices[3].y =
	pVertices[4].y = pVertices[5].y = 0.0f;
	
	//all vertices have a common normal vector of (0, 1, 0)
	pVertices[0].nx = pVertices[1].nx = 
	pVertices[2].nx = pVertices[3].nx =
	pVertices[4].nx = pVertices[5].nx = 0.0f;
	pVertices[0].ny = pVertices[1].ny = 
	pVertices[2].ny = pVertices[3].ny =
	pVertices[4].ny = pVertices[5].ny = 1.0f;
	pVertices[0].nz = pVertices[1].nz = 
	pVertices[2].nz = pVertices[3].nz =
	pVertices[4].nz = pVertices[5].nz = 0.0f;
}

void CGeometryApplication::InitializeLights()
{
	//Create a directional light shining straight down
	D3DLIGHT8 light;
	ZeroMemory( &light, sizeof(D3DLIGHT8) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	light.Range       = 1000.0f;

	//Tell the device about the new light
	m_pD3DDevice->SetLight( 0, &light );
	m_pD3DDevice->LightEnable( 0, TRUE );
	
	//Make sure lighting is enabled and there is a little 
	//bit of ambient light.
	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00101010);
}

void CGeometryApplication::InitializeIndexed()
{
	//For the indexed primitive, we will reuse the vertices
	//we created from the strip, but reuse them as an indexed
	//triangle list.  The sequence for the strip looked like:
	//
	//   1   3   5...
	//
	//   0   2   4...
	//Which makes constructing our index fairly easy because they
	//are in sequence already.  This is just an exercise!

	//the indices are 16bit, so use shorts
	short *pIndices;
	HRESULT hr = m_pIndexBuffer->Lock(0, sizeof(short) * NUM_STRIP_SIDES * 2 * 3,
		                 (BYTE **)&pIndices, 0);

	//Loop through the triangles
	for (short Triangle = 0; 
	     Triangle < NUM_STRIP_SIDES * 2;
		 Triangle++)
	{
		pIndices[(Triangle * 3) + 0] = Triangle + STRIP_OFFSET;
		pIndices[(Triangle * 3) + 1] = Triangle + STRIP_OFFSET + 1;
		pIndices[(Triangle * 3) + 2] = Triangle + STRIP_OFFSET + 2;
	}

	m_pIndexBuffer->Unlock();
}
