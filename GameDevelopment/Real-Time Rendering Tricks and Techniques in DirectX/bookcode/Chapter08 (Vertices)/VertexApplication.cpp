/***************************************************************
* VertexApplication.cpp                                        *
*                                                              *
* This file contains the implementation of the                 *
* CVertexApplication class.									   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

//Include the class definition
#include "VertexApplication.h"

//Define our vertex format and structure.  We use the
//pretransformed vertices here because we haven't discussed 
//transformations yet!
#define D3DFVF_SIMPLEVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct SIMPLE_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

//Define a number of vertices to work with
#define NUM_VERTICES 1000

//Define a random number macro
#define RANDOM_NUMBER (float)rand() / (float)RAND_MAX

//The constructor initializes the vertx buffer pointer to NULL
CVertexApplication::CVertexApplication()
{
	m_pVertexBuffer = NULL;
}

//The Destructor does any final cleanup
CVertexApplication::~CVertexApplication()
{
	DestroyVertexBuffer();
}

//Once the D3D object is created, create a device and a vertex 
//buffer for rendering.
BOOL CVertexApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL,
					     D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	return CreateVertexBuffer();
}

//If the device is going to be reset, release the buffer
BOOL CVertexApplication::PreReset()
{
	DestroyVertexBuffer();
	return TRUE;
}

//If the device was reset, recreate the buffer.
BOOL CVertexApplication::PostReset()
{
	return CreateVertexBuffer();
}

//This function creates and fills the vertex buffer.  It is its 
//own function so that it can be called at the beginning or
//whenever the device is reset.
BOOL CVertexApplication::CreateVertexBuffer()
{
	//Create the vertex buffer in video memory.  This buffer
	//is dynamic so that we change change it frequently and
	//it's being used for points.
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
						NUM_VERTICES * sizeof(SIMPLE_VERTEX),
						D3DUSAGE_WRITEONLY | 
						D3DUSAGE_DYNAMIC | 
						D3DUSAGE_POINTS,
						D3DFVF_SIMPLEVERTEX,
						D3DPOOL_DEFAULT,
						&m_pVertexBuffer)))
		return FALSE;

	//This is an  optimization...  Because we are never going
	//to use a different FVF or different vertex buffer, we set
	//it here and forget about it.
	m_pD3DDevice->SetVertexShader(D3DFVF_SIMPLEVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 
								  sizeof(SIMPLE_VERTEX));

	return TRUE;
}

void CVertexApplication::DestroyVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CVertexApplication::Render()
{
	//Here we regenerate the data and draw the points
	FillVertexBuffer();
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);
}

//Release the vertex buffer before the device is destroyed
BOOL CVertexApplication::PreTerminate()
{
	DestroyVertexBuffer();
	return TRUE;
}

//This function places a circle in the vertex buffer.
//NOTE: THIS IS NOT THE BEST WAY TO DRAW A CIRCLE, 
//THIS IS JUST A DEMOSTRATION...
BOOL CVertexApplication::FillVertexBuffer()
{
	//If we don't have a buffer, stop
	if (!m_pVertexBuffer)
		return FALSE;
	
	//Create a pointer to hold the locked vertices
	SIMPLE_VERTEX *pVertices;

	//Lock the vertices.  If we fail, release the buffer and
	//return false;
	if (FAILED(m_pVertexBuffer->Lock(0,
						NUM_VERTICES * sizeof(SIMPLE_VERTEX),
						(BYTE **)&pVertices,
						D3DLOCK_DISCARD)))
	{
		DestroyVertexBuffer();
		return FALSE;
	}

	float XOffset = 640.0f * RANDOM_NUMBER;
	float YOffset = 480.0f * RANDOM_NUMBER;

	for (long Index = 0; Index < NUM_VERTICES; Index++)
	{
		float Angle = (float)Index / (float)NUM_VERTICES * 
					  2.0f * D3DX_PI;
		
		pVertices[Index].x = XOffset + 50.0f * cos(Angle);
		pVertices[Index].y = YOffset + 50.0f * sin(Angle);
		
		//Z and RHW are set to a default value of 1.0f.
		pVertices[Index].z = 1.0f;
		pVertices[Index].rhw = 1.0f;

		//Set the color to white
		pVertices[Index].color = 0xffffffff;
	}

	//Unlock the buffer and return.
	m_pVertexBuffer->Unlock();

	return TRUE;	
}
