/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\Membrane
File:  shader_Membrane.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "shader_Membrane.h"
#include <NVFileDialog.h>


using namespace std;

DECLARE_EFFECT_MAIN()


#define   STR_LOADMESH     "Load .X File Mesh..."
#define   STR_LOADTEXTURE  "Load Texture Map..."
#define   STR_WIREFRAME    "Wireframe"


extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderMembrane();
			break;
		default:
			return NULL;
		}
	}
	
};

// ------------------------------------------------------------------------------
// CShaderMembrane Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderMembrane::CShaderMembrane()
: m_pVertexBuffer(NULL), 
  m_pIndexBuffer(NULL),
  m_pShadeTexture(NULL), 
  m_pEdgeTexture(NULL),
  m_pAttributes(NULL),
  m_bWireframe(false),
  m_pUI(NULL),
  m_bUseTrilinear(true),
  m_eDisplayOption(MembraneDisplay_SILHOUETTESANDSHADING)
{
	m_strEffectLocation = "Vertex Shaders\\Render Styles";
	m_strEffectName = "Membrane Lighting";
	m_strEffectVertexShader = GetFilePath("Membrane.nvv");
	m_strEffectPixelShader = "";

	D3DXMatrixIdentity(&m_world);
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);

	m_strFileName = "bigship1.x";

	m_bCycleTexture = true;
	m_bCycleTexCoords = false;
	m_bAppCycleTexCoords = false;

	m_tc[0]  = 0.0f;
	m_tc[1]  = 0.0f;
	m_tc[2]  = 0.0f;
	m_tc[3]  = 0.0f;

}


// ------------------------------------------------------------------------------
// CShaderMembrane Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderMembrane::~CShaderMembrane()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderMembrane::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 

void CShaderMembrane::UpdateProperties()
{
	EBEffect::UpdateProperties();

	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption),
														EBTYPE_DWORD_PROP);

	AddProperty(new EBProperty(STR_WIREFRAME, OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Cycle Texture", OBJECT_MEMBER(m_bCycleTexture), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Cycle Tex Coord", OBJECT_MEMBER(m_bCycleTexCoords), EBTYPE_BOOL_PROP));


	AddProperty(pEnumProp);

	AddProperty(new EBTriggerProperty(STR_LOADTEXTURE));
	AddProperty(new EBTriggerProperty(STR_LOADMESH));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Membrane Lighting",
										GetFilePath("Membrane.nvv"), EBTYPE_STRING_PROP));
}



// ------------------------------------------------------------------------------
// CShaderMembrane::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderMembrane::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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
		m_strLastError = "Device does not support two simultaneous textuers!";
		return E_FAIL;
	}

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderMembrane::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderMembrane::Initialize(IDirect3DDevice8* pDev)
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
	Declaration.push_back(D3DVSD_END());
	
	// Load the vertex shader object file and create it.
	m_dwCurrentShader = 0;
	hr = LoadAndCreateShader(GetFilePath("Membrane.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	// select the just created vertex shader as the thing to transform and light vertices
	hr = m_pD3DDev->SetVertexShader(m_dwCurrentShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadXFile(GetFilePath( m_strFileName ).c_str(), MembraneVertex::FVF_Flags );
	if(FAILED(hr))
		return hr;
	
	// use this vertex buffer as our source for vertices
	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(MembraneVertex));
	if (FAILED(hr))
		return hr;
	
	//set render states
	m_pD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS );	    // drawing transparent things
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );


	m_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );	// additive blending
	m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
	
	//load textures
	D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath("PurpleGreenBright.tga").c_str(), &m_pShadeTexture);
	
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



	SetDefaultView();


	return S_OK;
}


void CShaderMembrane::SetView()
{	
	m_pD3DDev->SetVertexShaderConstant(10, &m_vEye, 1);

	// Create view matrix
	D3DXMatrixLookAtLH( &m_view, &m_vEye, &m_vLookAt, &m_vUp);
	
	SetVertexShaderMatrices();
}



void CShaderMembrane::SetDefaultView()
{
	m_vEye.x    =   0.0f;
	m_vEye.y	=   0.0f;
	m_vEye.z	= -25.0f;

	m_vLookAt.x = 0.0f;
	m_vLookAt.y = 0.0f;
	m_vLookAt.z = 0.0f;

	m_vUp.x = 0.0f;
	m_vUp.y = 1.0f;
	m_vUp.z = 0.0f;


	// Set world matrix to view from an angle
	D3DXMatrixIdentity(&m_world);

	m_pUI->Reset();

	// rotate world matrix a bit:
	// We do it this way instead of changing eye position as the
	//   controls would not make sense for non-z axis eye position.
	m_pUI->OnLButtonDown( 40, 10 );
	m_pUI->OnMouseMove( 3, 30 );
	m_pUI->OnLButtonUp( 3, 30 );


	// Set field of view	
	D3DXMatrixPerspectiveFovLH(&m_proj,
		D3DXToRadian(60.0f),
		1,
		1.0f,
		5000.0f);

	m_pD3DDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
	
	SetView();
}



HRESULT CShaderMembrane::SetVertexShaderMatrices()
{
	D3DXMATRIX mvpMat, worldITMat, worldMat;
	
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


void CShaderMembrane::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	EBString	name;


	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{

		name = pProperty->GetPropertyName();

		
		if( name == EBString(STR_LOADMESH) )
		{
			// Now do file dialog
			NVXFileDialog aDialog;

			std::string theResult;
			bWritten = ( aDialog.Open( theResult ) );
			
			if ( bWritten )
			{
				m_strFileName = theResult;
				bWritten = ( LoadXFile( m_strFileName.c_str(), MembraneVertex::FVF_Flags ) == S_OK );
			}
		}
		else if( name == EBString(STR_LOADTEXTURE) )
		{
			// Now do file dialog
			NVTextureFileDialog aDialog;

			std::string theResult;
			bWritten = ( aDialog.Open( theResult ) );

			m_strFileName = theResult;

			if ( bWritten )
			{
				LoadTexture( &m_pShadeTexture, m_strFileName );				
			}
		}
	}
}


void CShaderMembrane::LoadTexture( LPDIRECT3DTEXTURE8 * ppTex, const EBString & filename )
{
	HRESULT hr;

	if( *ppTex != NULL )
	{
		SAFE_RELEASE( *ppTex );
	}

	//load texture
	hr = D3DXCreateTextureFromFile(m_pD3DDev, GetFilePath(filename.c_str()).c_str(), ppTex );

	if( !FAILED(hr))
	{
		m_pD3DDev->SetTexture(0, *ppTex);
		m_pD3DDev->SetTexture(1, *ppTex);
	}
}


// ------------------------------------------------------------------------------
// CShaderMembrane::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderMembrane::Free()
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


HRESULT CShaderMembrane::LoadXFile(const char* fileName, const DWORD dwFVF)
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

	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof( MembraneVertex ) );
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderMembrane::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderMembrane::Start()
{
	return S_OK;
}


HRESULT CShaderMembrane::SetTextureStageStates()
{

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE  );



	// Cycle between textures to show variety of effects

	static DWORD time = timeGetTime();
	static int count = 0;
	int limit = 3000;

	if( m_bCycleTexture )
	{
		if( timeGetTime() - time  > limit )
		{
			time = timeGetTime();

			switch( count )
			{
			case 0:
				LoadTexture( &m_pShadeTexture, "Inv_colors.tga" );
				SetTCCycle( false );
				count++;
				break;
			case 1:
				LoadTexture( &m_pShadeTexture, "BlueBands.tga" );
				SetTCCycle( true );
				count++;
				break;
			case 2:
				LoadTexture( &m_pShadeTexture, "Toonish_01.tga" );
				SetTCCycle( false );
				count++;
				break;
			case 3: 
				LoadTexture( &m_pShadeTexture, "YellowRed.tga" );
				SetTCCycle( false );
				count++;
				break;
			case 4:
				LoadTexture( &m_pShadeTexture, "PurpleGreenBright.tga" );
				SetTCCycle( false );
				count = 0;
				break;
			default:
				LoadTexture( &m_pShadeTexture, "PurpleGreenBright.tga" );
				SetTCCycle( false );
				count = 0;
				break;
			}

		}
	}

	if( m_bAppCycleTexCoords != m_bCycleTexCoords )
	{
		SetTCCycle( m_bCycleTexCoords );
	}

	if( m_bCycleTexCoords )
	{
		m_pD3DDev->SetVertexShaderConstant(20, m_tc, 1);

		#define TCINC 0.005f
		float inc = TCINC;

		m_tc[0] += inc;
		if( m_tc[0] > 1.0f )
			inc = -TCINC;
		else if( m_tc[0] < 0.0f )
			inc =  TCINC;
	}

	return S_OK;
}


void CShaderMembrane::SetTCCycle( bool on )
{
	if( on != m_bCycleTexCoords ||
		m_bAppCycleTexCoords != m_bCycleTexCoords )
	{
		m_bCycleTexCoords = on;
		m_bAppCycleTexCoords = on;

		m_tc[0] = 0.0f;
		m_pD3DDev->SetVertexShaderConstant(20, m_tc, 1);

	}	
}




// ------------------------------------------------------------------------------
// CShaderMembrane::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 

HRESULT CShaderMembrane::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetVertexShaderMatrices();

	SetTextureStageStates();
  
	if( m_bWireframe )
	{
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	}
	else
	{
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	}


	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

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


void CShaderMembrane::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

void CShaderMembrane::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderMembrane::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
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
				SetDefaultView();
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
}


