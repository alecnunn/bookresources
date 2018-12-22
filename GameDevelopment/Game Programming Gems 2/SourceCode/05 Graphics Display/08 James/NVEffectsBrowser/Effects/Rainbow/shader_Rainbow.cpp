/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\Rainbow
File:  shader_Rainbow.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "shader_Rainbow.h"
#include <NVFileDialog.h>
using namespace std;

DECLARE_EFFECT_MAIN()


extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderRainbow();
			break;
		default:
			return NULL;
		}
	}
	
};

// ------------------------------------------------------------------------------
// CShaderRainbow Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderRainbow::CShaderRainbow()
: m_pVertexBuffer(NULL), 
  m_pIndexBuffer(NULL),
  m_pShadeTexture(NULL), 
  m_pEdgeTexture(NULL),
  m_pAttributes(NULL),
  m_bWireframe(false),
  m_pUI(NULL),
  m_bUseTrilinear(true),
  m_eDisplayOption(RainbowDisplay_SILHOUETTESANDSHADING)
{
	m_strEffectLocation = "Vertex Shaders\\Render Styles";
	m_strEffectName = "Rainbow Rendering";
	m_strEffectVertexShader = GetFilePath("Rainbow.nvv");
	m_strEffectPixelShader = "";

	D3DXMatrixIdentity(&m_world);
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);

	m_strFileName = "bigship1.x";
}

// ------------------------------------------------------------------------------
// CShaderRainbow Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderRainbow::~CShaderRainbow()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderRainbow::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
void CShaderRainbow::UpdateProperties()
{
	EBEffect::UpdateProperties();

	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	
	AddProperty(pEnumProp);

	AddProperty(new EBTriggerProperty("Load Another X File Mesh..."));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Rainbow Lighting", GetFilePath("Rainbow.nvv"), EBTYPE_STRING_PROP));
}



// ------------------------------------------------------------------------------
// CShaderRainbow::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderRainbow::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP))
	{
		m_strLastError = "Device does not support mipmaps!";
		return E_FAIL;
	}

	if (!(pCaps->TextureCaps & D3DPTFILTERCAPS_MIPFLINEAR))
	{
		m_bUseTrilinear = false;
	}

	if(pCaps->MaxSimultaneousTextures < 2)
	{
		m_strLastError = "Device does not support two simultaneous textures!";
		return E_FAIL;
	}

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderRainbow::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderRainbow::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	
	// get the D3D device 
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
	Declaration.push_back(D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3)); // position
	Declaration.push_back(D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3)); // normal
	Declaration.push_back(D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT1)); // texture coords 0
	Declaration.push_back(D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT1)); // texture coords 1
	Declaration.push_back(D3DVSD_END());
	
	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("Rainbow.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	// select the just created vertex shader as the thing to transform and light vertices
	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadXFile(GetFilePath( m_strFileName ).c_str(), RainbowVertex::FVF_Flags );
	if(FAILED(hr))
		return hr;
	
	// use this vertex buffer as our source for vertices
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(RainbowVertex));
	if (FAILED(hr))
		return hr;
	
	//set render states
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
	
	//load textures
	D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("Colors2.tga").c_str(), &m_pShadeTexture);
	
	m_pD3DDev->SetTexture(0, m_pShadeTexture);
	m_pD3DDev->SetTexture(1, m_pShadeTexture);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	
	if(m_bUseTrilinear)
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
		
	// write the constant constants to constant memory now
	m_pD3DDev->SetVertexShaderConstant(9, D3DXVECTOR4(0.0f, 0.5f, 1.0f, -1.0f), 1);
	D3DXVECTOR4 lightDir(1.0f, 0.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&lightDir, &lightDir);
	
	m_pD3DDev->SetVertexShaderConstant(4, &lightDir, 1);
	
	D3DXVECTOR4 constNums(0.0f, 0.5f, 1.0f, -1.0f);
	m_pD3DDev->SetVertexShaderConstant(9, &constNums, 1);
	
	D3DXVECTOR3 eye, lookAt, up;
	
	eye.x    = 0.0f; eye.y	  = 0.0f; eye.z	   = -28.0f;
	lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
	up.x	 = 0.0f; up.y 	  = 1.0f; up.z	   = 0.0f;
	
	m_pD3DDev->SetVertexShaderConstant(10, &eye, 1);
	
	D3DXMatrixIdentity(&m_world);
	D3DXMatrixLookAtLH(&m_view, &eye, &lookAt, &up);
	
	D3DXMatrixPerspectiveFovLH(&m_proj,
		D3DXToRadian(60.0f),
		1,
		1.0f,
		50.0f);
	m_pD3DDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
	
	//matrices
	SetVertexShaderMatrices();
	
	return S_OK;
}

HRESULT CShaderRainbow::SetVertexShaderMatrices()
{
	D3DXMATRIX mvpMat, worldITMat, worldMat;
	
	//D3DXMatrixTranspose(&worldMat, &m_world);
	D3DXMATRIX tempMat;
	D3DXMatrixMultiply( &tempMat, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );

	D3DXMatrixTranspose(&worldMat, &tempMat);

	D3DXMatrixIdentity(&mvpMat);
	
	D3DXMatrixMultiply(&mvpMat, &tempMat, &m_view);
	
	D3DXMatrixInverse(&worldITMat, NULL, &tempMat );
	
	D3DXMatrixMultiply(&mvpMat, &mvpMat, &m_proj);
	D3DXMatrixTranspose(&mvpMat, &mvpMat);
	
	m_pD3DDev->SetVertexShaderConstant(0, &mvpMat, 4);
	
	m_pD3DDev->SetVertexShaderConstant(5, &worldITMat, 4);
	m_pD3DDev->SetVertexShaderConstant(11, &worldMat, 4);
	
	return S_OK;
}


void CShaderRainbow::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;
}

// ------------------------------------------------------------------------------
// CShaderRainbow::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderRainbow::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	SAFE_RELEASE(m_pShadeTexture);
	SAFE_RELEASE(m_pEdgeTexture);
	
	SAFE_DELETE_ARRAY(m_pAttributes);
	
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	delete m_pUI, m_pUI = 0;
	
	return S_OK;
}

HRESULT CShaderRainbow::LoadXFile(const char* fileName, const DWORD dwFVF)
{
	ID3DXMesh *tempMesh, *tempMeshFVF, *tempMeshOpt;
	
	HRESULT hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM , m_pD3DDev, NULL, 
		NULL, &m_dwNumSections, &tempMesh);
	
	tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	DWORD attribSize = m_dwNumSections;
	hr = tempMeshOpt->GetAttributeTable(NULL, &attribSize);

	SAFE_DELETE_ARRAY( m_pAttributes );

	m_pAttributes = new D3DXATTRIBUTERANGE[attribSize];
	
	hr = tempMeshOpt->GetAttributeTable(m_pAttributes, &attribSize);

	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pIndexBuffer );

	//convert to our format
	hr = tempMeshOpt->CloneMeshFVF(D3DXMESH_WRITEONLY, dwFVF, m_pD3DDev, &tempMeshFVF);
	
	tempMeshFVF->GetVertexBuffer(&m_pVertexBuffer);
	tempMeshFVF->GetIndexBuffer(&m_pIndexBuffer);

	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshFVF);
	SAFE_RELEASE(tempMeshOpt);
	
	//set index buffer
	m_pD3DDev->SetIndices( m_pIndexBuffer, 0);

	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof( RainbowVertex ) );
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderRainbow::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderRainbow::Start()
{
	return S_OK;
}

HRESULT CShaderRainbow::SetTextureStageStates()
{
	//we're going to pass the material color in thru tfactor
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE |D3DTA_ALPHAREPLICATE);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	
	//and modulate this resulting color with the edge texture (which should be white everywhere but on the edge)
	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	return S_OK;
}
// ------------------------------------------------------------------------------
// CShaderRainbow::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderRainbow::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetVertexShaderMatrices();

	SetTextureStageStates();
  
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	for(int i = 0; i < m_dwNumSections; i++)
	{
		m_pD3DDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
										 m_pAttributes[i].VertexStart,
										 m_pAttributes[i].VertexCount, 
											m_pAttributes[i].FaceStart*3, 
											m_pAttributes[i].FaceCount );
	}

	return hr;
}


void CShaderRainbow::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderRainbow::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderRainbow::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
    if ( bDown )
    {
        switch ( dwKey )
        {

            case 'H' :
            case VK_F1 :
            {
		        ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
					   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
                break;
            }

            case 'W' :
            {
                m_bWireframe = !m_bWireframe;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
                break;
            }

            case 'R' :
            {
                m_bWireframe = false;
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
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

            case VK_LEFT :
			case VK_NUMPAD4 :
            case '4' :
            {
                m_pUI->Translate( -1.0f, 0.0f, 0.0f );
                break;
            }

            case VK_RIGHT :
			case VK_NUMPAD6 :
            case '6' :
            {
                m_pUI->Translate(  1.0f, 0.0f, 0.0f );
                break;
            }

            case VK_UP :
			case VK_NUMPAD8 :
            case '8' :
            {
                m_pUI->Translate(  0.0f, 0.0f, -1.0f );
                break;
            }

            case VK_DOWN :
			case VK_NUMPAD2 :
            case '2' :
            {
                m_pUI->Translate( 0.0f, 0.0f, 1.0f );
                break;
            }

            case VK_PRIOR :
			case VK_NUMPAD9 :
            case '9' :
            {
                m_pUI->Translate(  0.0f, 1.0f, 0.0f );
                break;
            }
            case VK_NEXT :
			case VK_NUMPAD3 :
            case '3' :
            {
                m_pUI->Translate(  0.0f, -1.0f, 0.0f );
                break;
            }

            case VK_ADD :
            {
                m_pUI->Translate( 0.0f, 0.0f, 1.0f );
                break;
            }
            case VK_SUBTRACT :
            {
                m_pUI->Translate( 0.0f, 0.0f, -1.0f );
                break;
            }

            default :
            {
                break;
            }
        };
    }

	return;
}