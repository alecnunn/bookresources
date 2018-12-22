/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_RGB_Plasmas
File:  Effect_Plasmas.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "eb_effect.h"

#include "CA_Water.h"

#include "NV_Error.h"

#include "Effect_Plasmas.h"

#include "Constants.h"
#include "PixelConstants.h"



DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new NVEffect_Plasmas();
}

}


HRESULT NVEffect_Plasmas::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		m_strLastError = "Device does not support cubemaps!";
		return E_FAIL;
	}

	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	if (!(pCaps->MaxTextureBlendStages >= 4))
	{
		m_strLastError = "Not enough texture blend stages!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}

    if(!(pCaps->DevCaps & D3DDEVCAPS_RTPATCHES) )
    {
        m_strLastError = "Device does not support RTPATCHES!";
        return E_FAIL;
    }

	return S_OK;
}


#define STR_INCREASEBUMPSCALE "Bump scale increase (+)"
#define STR_DECREASEBUMPSCALE "Bump scale decrease (-)"
#define STR_RESETPATCH "Reset patch offset (HOME)"
#define STR_MOREOPTION "Hit F1 for more options!"


void NVEffect_Plasmas::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBTriggerProperty(STR_INCREASEBUMPSCALE));
	AddProperty(new EBTriggerProperty(STR_DECREASEBUMPSCALE));
	AddProperty(new EBTriggerProperty(STR_RESETPATCH));
	AddProperty(new EBTriggerProperty(STR_MOREOPTION));

    EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);


	AddProperty(pEnumProp);


	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause geometry animation", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));


	// Vertex Shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Neighbor Sampling",
		GetFilePath("TexCoord_4_Offset.nvv"), EBTYPE_STRING_PROP));

	// Pixel Shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Force Step 1",
		GetFilePath("NeighborForceCalc.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Force Step 2",
		GetFilePath("NeighborForceCalc2.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Blur Texture",
		GetFilePath("EqualWeightCombine_PostMult.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Apply Force",
		GetFilePath("ApplyForceShader.nvp"), EBTYPE_STRING_PROP));
	
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Apply Velocity",
		GetFilePath("ApplyVelocityShader.nvp"), EBTYPE_STRING_PROP));

	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( _T("Date"), _T("May 2001"));
}


NVEffect_Plasmas::NVEffect_Plasmas()
:	m_eDisplayOption(DISPLAY_BLINN8BITSIGNED),
	m_pUI(NULL),
	m_bWireframe(false),
	m_bPause(true),
	m_pCA_Water(NULL)
{
	m_strEffectName = "RGB Waves";	// A string holding the name of the effect
	m_strEffectLocation = "Effects\\Procedural";
	m_strEffectPixelShader = GetFilePath("NeighborForceCalc.nvp");
	m_strEffectVertexShader = GetFilePath("TexCoord_4_Offset.nvv");
	m_strEffectVersion = "1.0";
}


NVEffect_Plasmas::~NVEffect_Plasmas()
{
	Free();	
}




HRESULT NVEffect_Plasmas::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	hr = S_OK;

	vector<DWORD> Declaration, PatchDeclaration;


	m_pD3DDev = pDev;
	pDev->AddRef();


	m_bDrawing = false;
	m_bDrawModeOn = true;

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;

	m_pUI = new MouseUI((const RECT)rect);
	assert( m_pUI != NULL );

	m_pUI->SetTranslationalSensitivityFactor(0.1f);


	//////////////////////////////////////////////////////
	//  allocate class for creating dynamic normal maps

	assert( m_pCA_Water == NULL );

	m_pCA_Water = new CA_Water();
	assert( m_pCA_Water != NULL );

	m_pCA_Water->Initialize( pDev );


	m_pCA_Water->SetSimMode( SM_RGB_GLOW );
	
	//////////////////////////////////////////////////////

	m_bShowProceduralMaps = false;



	D3DDEVICE_CREATION_PARAMETERS CreationParams;
	pDev->GetCreationParameters(&CreationParams);


	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO,   D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE,    D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);


	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	return S_OK;

}

HRESULT NVEffect_Plasmas::Free()
{
	SAFE_DELETE( m_pCA_Water );

	SAFE_DELETE(m_pUI);


	if (m_pD3DDev)
	{
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT NVEffect_Plasmas::Start()
{
	return S_OK;
}




HRESULT NVEffect_Plasmas::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;


	// Clear background if the water animation is not filling it
	if( m_pCA_Water->m_bDrawOutput == false )
	{
		assert( false );

		// Clear to grey
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);
	}

	//////////////////////////////////////////
	// Update dynamic normal map

	assert( m_pCA_Water != NULL );

	// Tick also draws the maps in background if m_bDrawOutput is on
	hr = m_pCA_Water->Tick();
	
	ASSERT_IF_FAILED(hr);

	////////////////////////////////////////////////////
	// Restore render state because it may have been polluted
	//		by water animation

	return hr;
}





void NVEffect_Plasmas::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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

	if( button == MOUSE_LEFTBUTTON && m_bDrawModeOn == true )
	{
		if(bDown)
		{
			m_bDrawing = true;
		}
		else
		{
			// Draw the last point clicked as the mouse button 
			//   comes up.
			
			TryDrawDroplet( x,y );

			m_bDrawing = false;
		}
	}

	return;
}


void NVEffect_Plasmas::TryDrawDroplet( float x, float y )
{

	float fx,fy;
	RECT winr;

	if( m_pUI->IsInWindow( x,y ))
	{
		winr = m_pUI->GetRECT();

		fx = 1.0f - ((float)x - (float)winr.left)/((float)winr.right - winr.left);
		fy =        ((float)y - (float)winr.top)/((float)winr.bottom - winr.top);

		float scale = 15.0f;
		scale += 7.0f * ((float)rand()/((float)RAND_MAX));

		m_pCA_Water->AddDroplet( fx, fy, scale );
	}

	RestoreRenderState();
}


void NVEffect_Plasmas::RestoreRenderState()
{
	// call to reset render modes to those appropriate for
	//  rendering the reflective surface

	int i;
	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT  );  // nearest level
	}

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,		true );

}


void NVEffect_Plasmas::MouseMove(HWND hWnd, int x, int y)
{

	if( m_bDrawing && m_bDrawModeOn )
	{
		if( m_pCA_Water != NULL )
		{
			TryDrawDroplet( x,y );
		}
	}
	else if( m_pUI != NULL )
	{
		m_pUI->OnMouseMove(x, y);
	}


	return;
}

void NVEffect_Plasmas::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{

	assert( m_pCA_Water != NULL );
	bool result;
	result = m_pCA_Water->Keyboard( dwKey, nFlags, bDown );

	if( result == true )
	{
		return;			// no more keyboard processing!
	}

	if( !bDown )
	{
		switch( dwKey )
		{
		case VK_NUMPAD8:
			m_pUI->Reset();
			m_pUI->Translate( 0.0f, 0.0f, -0.25f );
			m_pUI->OnLButtonDown( 50, 50 );
			m_pUI->OnMouseMove( 68, 62 );
			m_pUI->OnLButtonUp( 68, 62 );

            m_bWireframe = false;
			m_pCA_Water->m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;

			m_bShowProceduralMaps = false;

			break;

		case 'D':
			m_bDrawModeOn = !m_bDrawModeOn;
			FDebug("Set draw mode: %s\n", m_bDrawModeOn?"ON":"OFF");
			break;
		case '5':
			m_bShowProceduralMaps = !m_bShowProceduralMaps;
			break;
		case 'P':
			m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			break;
		}
	}


	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	

	if( result == true && Action != EB_WIREFRAME )
	{
		return;			// no more keyboard processing!
	}


	EBString msg;

    switch ( Action )
    {
		case EB_HELP:
		{
			msg  = "H  or  F1 for Help\n";
			msg += "\n";
			msg += "*** This demo is framerate limited.  ***\n";
			msg += "    Hit 'S' to run as fast as possible\n";
			msg += "\n";
			msg += "L       : Toggle logo in water\n";
			msg += "\n";
			msg += "NUMPAD7   : Reset to initial conditions\n";
			msg += "NUMPAD8   : Reset view\n";
			msg += "\n";
			msg += "4       : Display final texture\n";
			msg += "3       : Tile texture\n";
			msg += "1       : Display force\n";
			msg += "2       : Display velocity\n";
			msg += "\n";	
			msg += "W       : Wireframe\n";
			msg += "\n";
			msg += "SPACE     : Start/stop procedural animation       \n";
			msg += "ENTER     : Single step of animation\n";
			msg += "S          : Toggle animation rate limiting\n";
			msg += "  L arrow : Run animation faster\n";
			msg += "  R arrow : Run animation slower\n";
			msg += "\n";	
			msg += "Up arrow   : Increase droplet frequency\n";	
			msg += "Dwn arrow  : Decrease droplet frequency\n";	
			msg += "\n";
			msg += "M       : Toggle physical properties of water\n";
			msg += "\n";
			msg += "C       : Decrease height smoothing\n";	
			msg += "V       : Increase height smoothing\n";	
			msg += "<, >    : Dec/Inc velocity apply factor\n";
			msg += "[, ]    : Dec/Inc force apply factor\n";
			msg += "\n";
			msg += "\n";
			
			
			MessageBoxEx( NULL, msg.c_str(), "Dynamic Reflection Map Help",
						MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );

		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

		case EB_RESET:
        {
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;

			if( m_pCA_Water != NULL )
			{
				m_pCA_Water->m_bReset = true;
			}
        }
		break;

		case EB_PAUSE:
        {
            m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;

		case EB_ADD:
		{
		}
		break;

		case EB_SUBTRACT:
		{
		}		
		break;

        default :
            break;
    }
}

void NVEffect_Plasmas::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	EBString	name;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{

		name = pProperty->GetPropertyName();
		
		if(name == EBString(STR_INCREASEBUMPSCALE))
		{
			Keyboard(VK_ADD, 0, true);
		}
		else if (name == EBString(STR_DECREASEBUMPSCALE))
		{
			Keyboard(VK_SUBTRACT, 0, true);
		}
		else if (name == EBString(STR_RESETPATCH))
		{
			Keyboard(VK_HOME, 0, true);
		}
		else if (name == EBString(STR_MOREOPTION))
		{
			Keyboard(VK_F1, 0, true );
		}
	}
}



