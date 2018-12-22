/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  Effect_Fire.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#include <crtdbg.h>


#include "eb_effect.h"
#include "Effect_Fire.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"

#include "ShaderManager.h"

#include "CA_Fire.h"
#include "CA_GameOfLife.h"



////////////////////////////////////////////////////////////////////


DECLARE_EFFECT_MAIN()
DECLARE_EFFECT_COUNT(1)
DECLARE_EFFECT_CREATE_BEG()
DECLARE_EFFECT_CREATE(0, Effect_Fire())
DECLARE_EFFECT_CREATE_END()



////////////////////////////////////////////////////////////////////

Effect_Fire::Effect_Fire()
{
	m_pCA_Fire			= NULL;
	m_pCA_GameOfLife	= NULL;
	m_pShaderManager	= NULL;


	m_strEffectLocation     = "Effects\\Procedural";
	m_strEffectVertexShader = GetFilePath("TexCoord_4_Offset.nvv");


	m_strEffectName        = "Fire #1";
	m_strEffectPixelShader = GetFilePath("TwoWeightCombine_PostMult.nvp");
	m_strEffectVersion     = "1.0";

}


Effect_Fire::~Effect_Fire()
{
	Free();	
}

/////////////////////////////////////////////////////////


HRESULT Effect_Fire::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{

	if( m_pCA_Fire == NULL )
	{
		m_pCA_Fire = new CA_Fire();
		assert( m_pCA_Fire );
	}

	return( m_pCA_Fire->ConfirmDevice( pCaps, dwBehavior, Format ));
}


void Effect_Fire::UpdateProperties()
{
	EBEffect::UpdateProperties();

    AddProperty(new EBProperty("W  - Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	AddProperty(new EBTriggerProperty("Hit F1 for more options"));


	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Neighbor Calc Offsets",
										GetFilePath("TexCoord_4_Offset.nvv"), EBTYPE_STRING_PROP));

	// Pixel shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Equal Weight",
										GetFilePath("EqualWeightCombine.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Dependent GB",
										GetFilePath("DependentGB.nvp"), EBTYPE_STRING_PROP));

	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Weighted Combine",
										GetFilePath("TwoWeightCombine_PostMult.nvp"), EBTYPE_STRING_PROP));

	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( _T("Date"), _T("May 2001"));
	
}



HRESULT Effect_Fire::Initialize( LPDIRECT3DDEVICE8 pDev )
{
	// Called when effect is selected from list in browser
	// Free() is called when effect is de-selected

	assert( pDev != NULL );


	HRESULT hr;

	Free();					// Make sure nothing is init already

	// get the device
	m_pD3DDev = pDev;
	pDev->AddRef();			// Released on Free()


	////////////////////////////////////
	// Initialize shader manager

	m_pShaderManager = new ShaderManager();
	assert( m_pShaderManager );

	hr = m_pShaderManager->Initialize( m_pD3DDev );

	////////////////////////////////////
	// Init GameOfLife effect to provide the 
	//  source "embers" for the fire effect

	m_pCA_GameOfLife = new CA_GameOfLife();
	assert( m_pCA_GameOfLife );

	m_pCA_GameOfLife->Initialize( m_pD3DDev, m_pShaderManager );

	m_pCA_GameOfLife->m_eRenderMode = CA_GameOfLife::DO_NOT_RENDER;

	m_pCA_GameOfLife->m_bOccasionalExcitation = true;

	////////////////////////////////////
	// Initialize fire effect:

	m_pCA_Fire = new CA_Fire();
	assert( m_pCA_Fire );

	m_pCA_Fire->Initialize( m_pD3DDev, m_pShaderManager, m_pCA_GameOfLife->GetOutputTexture() );


	m_bWireframe = m_pCA_Fire->m_bWireframe;

	////////////////////////////////////


	return( hr );

}



HRESULT Effect_Fire::Free()
{
	SAFE_DELETE( m_pCA_GameOfLife );
	SAFE_DELETE( m_pCA_Fire );
	SAFE_DELETE( m_pShaderManager );

	SAFE_RELEASE( m_pD3DDev );

	return S_OK;
}


HRESULT Effect_Fire::Start()
{
	return S_OK;
}



void Effect_Fire::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	// Note - pressing tidle key will not cause this to be called
	//  until the window is resized - Something happens then to 
	//  trigger the Keyboard function.


	bool processed;

	if( m_pCA_Fire == NULL )
	{
		FDebug("*** Keyboard: CA_Fire class doesn't exist!\n");
		return;
	}

	processed = m_pCA_Fire->Keyboard( dwKey, nFlags, bDown );

	if( processed )
	{
		return;
	}


	if( bDown )
	{

		
	}


	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			EBString str;
			str =  " Help : F1 - Help\n\n";
			str += " W    - Wireframe toggle\n\n";

			str += " Home - Restart from nothing\n\n";

			str += " R    - Restart from nothing\n";
			str += " G    - Stop/Go animation toggle\n";
			str += " SPC  - Single frame step when anim stopped     \n";
			str += " B    - Toggle border wrapping\n\n";

			str += " 1    - Draw final output texture\n";
			str += " 2    - Draw input ember texture\n";
			str += " 3    - Draw ember, output, and color re-map\n";
			str += " F    - Toggle additional color re-map step\n\n";

			str += " C    - Toggle between color attenuation factors\n";
			str += " [,]  - Inc. / Dec. color attenuation factor\n";
			str += " +,-  - Inc. / Dec. scrolling amount per frame\n\n";

			str += " M    - Increase # frames to skip drawing\n";
			str += " N    - Decrease # frames to skip drawing\n";
			str += " S    - Slow framerate\n\n";

			str += " L ARROW - Reduce framerate delay\n";
			str += " R ARROW - Increase framerate delay\n\n";

			MessageBoxEx( NULL, str.c_str(),
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
			m_pCA_Fire->m_bWireframe = m_bWireframe;

            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_RESET:
        {
			m_pCA_Fire->m_pUI->Reset();
			m_pCA_Fire->m_bReset = true;
			m_pCA_GameOfLife->m_bReset = true;
        }
		break;

        case EB_ADD:
            break;

        case EB_SUBTRACT:
            break;

        default:
            break;
    }
}





// Our properties were updated, set the correct shader to match

void Effect_Fire::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{

}



void Effect_Fire::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if( m_pCA_Fire != NULL )
	{
		m_pCA_Fire->MouseButton( hWnd, button, bDown, x, y );
	}
}


void Effect_Fire::MouseMove(HWND hWnd, int x, int y)
{
	if( m_pCA_Fire != NULL )
	{
		m_pCA_Fire->MouseMove( hWnd, x, y );
	}
}



HRESULT Effect_Fire::Tick(EBTimer* pTimer)
{	
	// Render a new frame

	HRESULT hr = E_FAIL;


	// Update the game of life which is generating the 
	//   source ember pattern for the fire effect

	if( m_pCA_GameOfLife != NULL )
	{
		m_pCA_GameOfLife->Tick();

		// Get it's texture & set it as the fire's source
		//  texture

		if( m_pCA_Fire != NULL )
		{
			m_pCA_Fire->SetInputTexture( m_pCA_GameOfLife->GetOutputTexture() );

			hr = m_pCA_Fire->Tick();
		}
		

	}


	return( hr );
}



