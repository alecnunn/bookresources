/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\Keyframe
File:  shader_KeyframeLinear.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "eb_effect.h"
#include "shader_KeyframeLinear.h"
#include "shader_KeyframeHermite.h"
#include "nvinc.h"
#include "MouseUI.h"
#include "KeyframeCommon.h"
#include "Constants.h"

#include <mmsystem.h>

using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 2; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{	
	switch(EffectNum)
	{
		case 0:
			return new CShaderKeyframeLinear();
			break;
		case 1:
			return new CShaderKeyframeHermite();
			break;
		default:
			return NULL;
	}
}

}


CShaderKeyframeLinear::CShaderKeyframeLinear()
	: m_pVertexBuffers(NULL),
	m_pIndexBuffer(NULL),
	m_pTexture(NULL),
	m_dwCurrentShader(0),
	m_pUI(NULL),
	m_frame0(0),
	m_frame1(1),
	m_bWireframe(false)
{
	m_strEffectLocation = "Vertex Shaders\\Animation";
	m_strEffectName = "Linear Keyframe Interpolation";
	m_strEffectVertexShader = GetFilePath("KeyframeLinear.nvv");
	m_strEffectPixelShader = "";
	
	m_md2Model = NULL;
}

void CShaderKeyframeLinear::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Keyframing", GetFilePath("KeyframeLinear.nvv"), EBTYPE_STRING_PROP));
}


CShaderKeyframeLinear::~CShaderKeyframeLinear()
{
	Free();	
}

void CShaderKeyframeLinear::SetupMatrices(const Extents& e)
{
	float dx = e.maxX - e.minX;
	float dy = e.maxY - e.minY;
	float dz = e.maxZ - e.minZ;
	
	float midX = (dx / 2) + e.minX;
	float midY = (dy / 2) + e.minY;
	float midZ = (dz / 2) + e.minZ;
	
	float maxExtent = (dx > dy) ? dx : dy;
	maxExtent = (maxExtent > dz) ? maxExtent : dz;
	
	D3DXMATRIX world;
	
	D3DXMatrixIdentity(&world);
	
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 up;
	D3DXVECTOR3 eye;
	
	eye.x    = midX; eye.y    = midY; eye.z    = e.minZ - (1.5f * maxExtent);
	lookAt.x = midX; lookAt.y = midY; lookAt.z = 0.0f;
	up.x     = 0.0f; up.y     = 1.0f; up.z     = 0.0f;
	
	//set view
	D3DXMatrixLookAtLH(&m_view, &eye, &lookAt, &up);
	
	D3DXMatrixPerspectiveFovLH(&m_proj, D3DXToRadian(60.0f), 1, 1.0f, 150.0f);

	D3DXMATRIX worldViewProj;
	D3DXMatrixMultiply(&worldViewProj, &world, &m_view);
	D3DXMatrixMultiply(&worldViewProj, &worldViewProj, &m_proj);

	D3DXMatrixTranspose(&worldViewProj, &worldViewProj);

	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &worldViewProj, 4);
}

void CShaderKeyframeLinear::SetMatricesBasedOnInput()
{
	D3DXMATRIX compoundMatrix;
	D3DXMATRIX worldMatrix;

	D3DXMatrixMultiply(&compoundMatrix, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix());
	
	D3DXMatrixTranspose(&worldMatrix, &compoundMatrix);

	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &worldMatrix, 4);

	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &m_view);

	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &m_proj);

	D3DXMatrixTranspose(&compoundMatrix, &compoundMatrix);

	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &compoundMatrix, 4);
}


void CShaderKeyframeLinear::SetExtents(const KeyframeVertex& vertex, Extents& e)
{
	if(vertex.Position.x > e.maxX)
		e.maxX = vertex.Position.x;
	if(vertex.Position.y > e.maxY)
		e.maxY = vertex.Position.y;
	if(vertex.Position.z > e.maxZ)
		e.maxZ = vertex.Position.z;
	if(vertex.Position.x < e.minX)
		e.minX = vertex.Position.x;
	if(vertex.Position.y < e.minY)
		e.minY = vertex.Position.y;
	if(vertex.Position.z < e.minZ)
		e.minZ = vertex.Position.z;
}


HRESULT CShaderKeyframeLinear::Initialize(LPDIRECT3DDEVICE8 pDev)
{
	HRESULT hr;

	m_pD3DDev = pDev;
	pDev->AddRef();

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_STREAM(1));
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("KeyframeLinear.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;

    m_md2Model = md2ReadModel(GetFilePath(FILENAME).c_str());
	
	m_numFrames = m_md2Model->header.numFrames - START_FRAME;
	if(m_numFrames % DISTANCE_BETWEEN_FRAMES != 0)
		m_numFrames = (m_numFrames / DISTANCE_BETWEEN_FRAMES) + 1;
	else
		m_numFrames = (m_numFrames / DISTANCE_BETWEEN_FRAMES);


	m_pVertexBuffers = new LPDIRECT3DVERTEXBUFFER8[m_numFrames];
	
	//extents that define the bounding box of the model
	Extents e;
	e.maxX = e.maxY = e.maxZ = -999999;
	e.minX = e.minY = e.minZ = 999999;

	//NON-INDEXED
	for(int i = 0; i < m_md2Model->header.numFrames; i++)
	{
		if( (i >= START_FRAME) && ((i % DISTANCE_BETWEEN_FRAMES) == 0) )
		{
			int bufferCtr = (i - START_FRAME) / DISTANCE_BETWEEN_FRAMES;

			//create vb for this keyframe
			hr = m_pD3DDev->CreateVertexBuffer( m_md2Model->header.numTriangles * 3 * sizeof(KeyframeVertex), D3DUSAGE_WRITEONLY, 0, 
				D3DPOOL_MANAGED, &m_pVertexBuffers[bufferCtr]);
			if (FAILED(hr))
				return hr;
			
			//lock and load
			KeyframeVertex* pBuff;
			hr = m_pVertexBuffers[bufferCtr]->Lock(0, sizeof(KeyframeVertex) * m_md2Model->header.numTriangles * 3, (BYTE**)&pBuff, 0);
			
			int vertCtr = 0;

			for(int j = 0; j < m_md2Model->header.numTriangles; j++)
			{
				//three vertices per triangle

				//vertex 1
				short currVertIndex = m_md2Model->triangles[j].vertexIndices[0];
				short currTexIndex = m_md2Model->triangles[j].textureIndices[0];
				pBuff[vertCtr].Position.x = m_md2Model->frames[i].vertices[currVertIndex].vertex[0];
				pBuff[vertCtr].Position.y = m_md2Model->frames[i].vertices[currVertIndex].vertex[1];
				pBuff[vertCtr].Position.z = m_md2Model->frames[i].vertices[currVertIndex].vertex[2];
				
				pBuff[vertCtr].Normal.x   = m_md2Model->frames[i].vertices[currVertIndex].normal[0];
				pBuff[vertCtr].Normal.y   = m_md2Model->frames[i].vertices[currVertIndex].normal[1];
				pBuff[vertCtr].Normal.z   = m_md2Model->frames[i].vertices[currVertIndex].normal[2];
				
				pBuff[vertCtr].Texture.x = (float)m_md2Model->texCoords[currTexIndex].s / 255.f;
				pBuff[vertCtr].Texture.y = (float)m_md2Model->texCoords[currTexIndex].t / 255.f;

				//set extents, based on the first frame
				if(i == (START_FRAME + (START_FRAME % DISTANCE_BETWEEN_FRAMES)))
					SetExtents(pBuff[vertCtr], e);

				vertCtr++;
				
				//vertex 2
				currVertIndex = m_md2Model->triangles[j].vertexIndices[1];
				currTexIndex = m_md2Model->triangles[j].textureIndices[1];
				pBuff[vertCtr].Position.x = m_md2Model->frames[i].vertices[currVertIndex].vertex[0];
				pBuff[vertCtr].Position.y = m_md2Model->frames[i].vertices[currVertIndex].vertex[1];
				pBuff[vertCtr].Position.z = m_md2Model->frames[i].vertices[currVertIndex].vertex[2];
				
				pBuff[vertCtr].Normal.x   = m_md2Model->frames[i].vertices[currVertIndex].normal[0];
				pBuff[vertCtr].Normal.y   = m_md2Model->frames[i].vertices[currVertIndex].normal[1];
				pBuff[vertCtr].Normal.z   = m_md2Model->frames[i].vertices[currVertIndex].normal[2];
				
				pBuff[vertCtr].Texture.x = (float)m_md2Model->texCoords[currTexIndex].s / 255.f;
				pBuff[vertCtr].Texture.y = (float)m_md2Model->texCoords[currTexIndex].t / 255.f;

				//set extents, based on the first frame
				if(i == (START_FRAME + (START_FRAME % DISTANCE_BETWEEN_FRAMES)))
					SetExtents(pBuff[vertCtr], e);

				vertCtr++;

				//vertex 3
				currVertIndex = m_md2Model->triangles[j].vertexIndices[2];
				currTexIndex = m_md2Model->triangles[j].textureIndices[2];
				pBuff[vertCtr].Position.x = m_md2Model->frames[i].vertices[currVertIndex].vertex[0];
				pBuff[vertCtr].Position.y = m_md2Model->frames[i].vertices[currVertIndex].vertex[1];
				pBuff[vertCtr].Position.z = m_md2Model->frames[i].vertices[currVertIndex].vertex[2];
				
				pBuff[vertCtr].Normal.x   = m_md2Model->frames[i].vertices[currVertIndex].normal[0];
				pBuff[vertCtr].Normal.y   = m_md2Model->frames[i].vertices[currVertIndex].normal[1];
				pBuff[vertCtr].Normal.z   = m_md2Model->frames[i].vertices[currVertIndex].normal[2];
				
				pBuff[vertCtr].Texture.x = (float)m_md2Model->texCoords[currTexIndex].s / 255.f;
				pBuff[vertCtr].Texture.y = (float)m_md2Model->texCoords[currTexIndex].t / 255.f;
			
				//set extents, based on the first frame
				if(i == (START_FRAME + (START_FRAME % DISTANCE_BETWEEN_FRAMES)))
					SetExtents(pBuff[vertCtr], e);

				vertCtr++;
			}
			m_pVertexBuffers[bufferCtr]->Unlock();
		}
	}
	
	SetupMatrices(e);
	
	//create texture
	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath(TEXTURE_FILENAME).c_str(), &m_pTexture);
	if(FAILED(hr))
		return hr;

	hr = m_pD3DDev->SetTexture(0, m_pTexture);
	if(FAILED(hr))
		return hr;

	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	
	D3DXVECTOR4 lightDir(0.0f, 0.0f, -1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &lightDir, 1);

	D3DXVECTOR4 constants(0.0f, 0.5f, 1.0f, -1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, &constants, 1);

	m_pD3DDev->SetStreamSource(0, m_pVertexBuffers[0], sizeof(KeyframeVertex));
	m_pD3DDev->SetStreamSource(1, m_pVertexBuffers[1], sizeof(KeyframeVertex));
	
	m_baseTime = timeGetTime();

	return S_OK;
}

HRESULT CShaderKeyframeLinear::Free()
{
	if(m_pVertexBuffers)
	{
		for(int i = 0; i < m_numFrames; i++)
		{
			SAFE_RELEASE(m_pVertexBuffers[i]);
		}
	}

	SAFE_DELETE_ARRAY(m_pVertexBuffers);
	
	SAFE_RELEASE(m_pTexture);

	if (m_pD3DDev)
	{
		if (m_dwCurrentShader)
			m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	SAFE_RELEASE(m_pIndexBuffer);
	
	md2FreeModel(m_md2Model);
	
	m_md2Model = NULL;

	SAFE_DELETE(m_pUI);

	return S_OK;
}

HRESULT CShaderKeyframeLinear::Start()
{
	return S_OK;
}

void CShaderKeyframeLinear::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pUI->OnLButtonUp(x, y);
		}
	}
	return;
}

void CShaderKeyframeLinear::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderKeyframeLinear::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	if(bDown)
	{
		switch(dwKey)
		{
			case 'W' :
            case 'w' :
            {
                m_bWireframe = !m_bWireframe;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            case 'H' :
            case VK_F1 :
            {
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
					   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
                break;
            }

            case VK_HOME :
            case VK_END :
			case VK_NUMPAD7 :
			case VK_NUMPAD1 :
            case '7' :
            case '1' :
            {
                m_pUI->Reset();
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            default :
            {
                break;
            }
        }
	}
	SetMatricesBasedOnInput();
	return;
}

HRESULT CShaderKeyframeLinear::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetMatricesBasedOnInput();

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
	
	//advance one keyframe every second
	int timeElapsed = timeGetTime() - m_baseTime;
	if(timeElapsed > PERIOD)
	{
		m_frame0++;
		m_frame1++;

		//clamp
		if(m_frame0 > (m_numFrames - 1))
			m_frame0 = 0;
		if(m_frame1 > (m_numFrames - 1))
			m_frame1 = 0;

		m_baseTime = timeGetTime();
		timeElapsed = 0;
	}
	
	//set blend factor
	float fTimeDelta = (float)timeElapsed / PERIOD;
	D3DXVECTOR4 blendFactor(fTimeDelta, 1.0f - fTimeDelta, 0.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_BLEND_FACTORS, blendFactor, 1);

	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffers[m_frame0], sizeof(KeyframeVertex));
	if (FAILED(hr))
		return hr;
	
	hr = m_pD3DDev->SetStreamSource(1, m_pVertexBuffers[m_frame1], sizeof(KeyframeVertex));
	if (FAILED(hr))
		return hr;
	
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_md2Model->header.numTriangles);

	return hr;
}

HRESULT CShaderKeyframeLinear::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}
