/***************************************************************
* TransformApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* CTransformApplication class.								   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "TransformApplication.h"

#define D3DFVF_SIMPLEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct SIMPLE_VERTEX
{
	float x, y, z;
	DWORD color;
};

#define NUM_VERTICES 20

CTransformApplication::CTransformApplication()
{
	m_pVertexBuffer = NULL;
}

CTransformApplication::~CTransformApplication()
{
	DestroyVertexBuffer();
}

BOOL CTransformApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	//Because our vertices are no longer prelit, we need to turn
	//off lighting.
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//Set the view matrix to a default 
	//identity matrix.
	D3DXMatrixIdentity(&m_ViewMatrix);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_ViewMatrix);

	//Setup the sub-viewports
	InitializeViewports();

	return CreateVertexBuffer();
}

void CTransformApplication::InitializeViewports()
{
	D3DVIEWPORT8 MainViewport;
	m_pD3DDevice->GetViewport(&MainViewport);
	
	//Each viewport fills a quarter of the window
	m_RViewport.Width = m_SRTViewport.Width = m_TRViewport.Width = 
	m_RTRSViewport.Width = MainViewport.Width / 2;
	m_RViewport.Height = m_SRTViewport.Height = m_TRViewport.Height = 
	m_RTRSViewport.Height = MainViewport.Height / 2;

	m_RViewport.Y = m_SRTViewport.Y = 0;
	m_RViewport.X = m_TRViewport.X = 0;

	m_TRViewport.Y = m_RTRSViewport.Y = MainViewport.Height / 2;
	m_SRTViewport.X = m_RTRSViewport.X = MainViewport.Width / 2;

	//Set the full Z range for each viewport
	m_RViewport.MinZ = m_SRTViewport.MinZ = 
	m_TRViewport.MinZ = m_RTRSViewport.MinZ = 0.0f;
	m_RViewport.MaxZ = m_SRTViewport.MaxZ = 
	m_TRViewport.MaxZ = m_RTRSViewport.MaxZ = 1.0f;
}

BOOL CTransformApplication::PreReset()
{
	DestroyVertexBuffer();
	return TRUE;
}

BOOL CTransformApplication::PostReset()
{
	return CreateVertexBuffer();
}

BOOL CTransformApplication::CreateVertexBuffer()
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(
										NUM_VERTICES
										* sizeof(SIMPLE_VERTEX),
										D3DUSAGE_WRITEONLY,
										D3DFVF_SIMPLEVERTEX,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer)))
		return FALSE;

	m_pD3DDevice->SetVertexShader(D3DFVF_SIMPLEVERTEX);
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 
								  sizeof(SIMPLE_VERTEX));

	//Fill the vertex buffer once.
	FillVertexBuffer();

	return TRUE;
}

void CTransformApplication::DestroyVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CTransformApplication::Render()
{
	//Set the projection matrix.  We reset the projection
	//matrix here because the user may have resized the window
	//to a different aspect ratio.  To do this, we find the
	//new client rectangle of our window and figure out the 
	//new aspect ratio.  If we were writing a fullscreen app, 
	//we could do this when the device was created.
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);
    D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix,
					D3DX_PI / 4,
					(float)(WindowRect.right - WindowRect.left) / 
					(float)(WindowRect.bottom - WindowRect.top),
				    1.0f, 100.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_ProjectionMatrix);
	
	//Here we draw the points.  Notice we don't change the 
	//actual vertex data, but we do use the world 
	//transformation to move the vertices.
	D3DXMATRIX RotationMatrix1;
	D3DXMATRIX RotationMatrix2;
	D3DXMATRIX TranslationMatrix;
	D3DXMATRIX ScalingMatrix;

	//First, get the current viewport and save it.
	D3DVIEWPORT8  MainViewport;
	m_pD3DDevice->GetViewport(&MainViewport);

	//Create a rotation matrices using a timer to generate 
	//the angle.  (It doesn't matter what the angle is...)
	//The first matrix will rotate the geometry.  The second
	//one will be used in the RTR matrix to correct for the
	//first rotation.
	D3DXMatrixRotationZ(&RotationMatrix1, 
						(float)GetTickCount() / 1000.0f);
	D3DXMatrixRotationZ(&RotationMatrix2, 
						-(float)GetTickCount() / 1000.0f);

	//Create a translation matrix.  Translate the geometry
	//3 units to the right (+3 X).  (We don't *need* to
	//redo this every frame.)
	D3DXMatrixTranslation(&TranslationMatrix, 
						  3.0f, 0.0f, 0.0f);

	//Create the scaling matrix.
	D3DXMatrixScaling(&ScalingMatrix, 1.0f, 0.5f, 1.0f);

	//Draw the rotated data
	m_pD3DDevice->SetViewport(&m_RViewport);
	m_WorldMatrix = RotationMatrix1;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);
	
	//Draw the rotated-translated data
	m_pD3DDevice->SetViewport(&m_SRTViewport);
	m_WorldMatrix = TranslationMatrix * RotationMatrix1 * 
					ScalingMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);

	//Draw the translated-rotated data
	m_pD3DDevice->SetViewport(&m_TRViewport);
	m_WorldMatrix = RotationMatrix1 * TranslationMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);

	//Draw the rotated-translated-rotated data
	m_pD3DDevice->SetViewport(&m_RTRSViewport);
	m_WorldMatrix = ScalingMatrix * RotationMatrix2 * 
					TranslationMatrix * RotationMatrix1;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 
								NUM_VERTICES);

	//Set the viewport back to the full window.  This ensures
	//that any other processing (such as Clear) takes place
	//over the full window.
	m_pD3DDevice->SetViewport(&MainViewport);
}

BOOL CTransformApplication::PreTerminate()
{
	DestroyVertexBuffer();
	return TRUE;
}

BOOL CTransformApplication::HandleMessage(MSG *pMessage)
{
	
	return CHostApplication::HandleMessage(pMessage);
}

BOOL CTransformApplication::FillVertexBuffer()
{
	if (!m_pVertexBuffer)
		return FALSE;
	
	SIMPLE_VERTEX *pVertices;

	if (FAILED(m_pVertexBuffer->Lock(0,
					NUM_VERTICES * sizeof(SIMPLE_VERTEX),
					(BYTE **)&pVertices,
					0)))
	{
		DestroyVertexBuffer();
		return FALSE;
	}

	//Fill the set of vertices with a circle of points.
	//NOTE:  THIS IS NOT A GOOD WAY TO DRAW A CIRCLE!!!  We're
	//just doing it this way for now...
	for (long Index = 0; Index < NUM_VERTICES; Index++)
	{
		float Angle = 2.0f * D3DX_PI * (float)Index /
					  NUM_VERTICES;

		pVertices[Index].x = cos(Angle);
		pVertices[Index].y = sin(Angle);
		pVertices[Index].z = 10.0f;

		pVertices[Index].color = 0xffffffff;
	}

	m_pVertexBuffer->Unlock();

	return TRUE;	
}
