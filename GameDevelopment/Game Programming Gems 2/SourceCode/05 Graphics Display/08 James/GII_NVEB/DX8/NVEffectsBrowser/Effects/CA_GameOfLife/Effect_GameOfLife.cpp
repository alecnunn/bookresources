/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_GameOfLife
File:  Effect_GameOfLife.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include <crtdbg.h>
#include <string.h>

#include "eb_effect.h"
#include "Effect_GameOfLife.h"
#include "..\..\..\CommonSrc\NV_Error.h"			// for FDebug

#include "Constants.h"
#include "PixelConstants.h"

#include "ShaderManager.h"

#include "CA_GameOfLife.h"



////////////////////////////////////////////////////////////////////


DECLARE_EFFECT_MAIN()
DECLARE_EFFECT_COUNT(1)
DECLARE_EFFECT_CREATE_BEG()
DECLARE_EFFECT_CREATE(0, Effect_GameOfLife())
DECLARE_EFFECT_CREATE_END()



////////////////////////////////////////////////////////////////////

Effect_GameOfLife::Effect_GameOfLife()
{
	m_pCA_GameOfLife	= NULL;
	m_pShaderManager	= NULL;


	m_strEffectLocation     = "Effects\\Procedural";
	m_strEffectVertexShader = "";


	m_strEffectName        = "Game of Life";
	m_strEffectPixelShader = "";

	m_strEffectVersion = "1.0";

}


Effect_GameOfLife::~Effect_GameOfLife()
{
	Free();	
}

/////////////////////////////////////////////////////////


HRESULT Effect_GameOfLife::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if( m_pCA_GameOfLife == NULL )
	{
		m_pCA_GameOfLife = new CA_GameOfLife();
		assert( m_pCA_GameOfLife );
	}

	HRESULT hr = m_pCA_GameOfLife->ConfirmDevice( pCaps, dwBehavior, Format );

	delete m_pCA_GameOfLife;
	m_pCA_GameOfLife = NULL;

	return( hr );
}


void Effect_GameOfLife::UpdateProperties()
{
	EBEffect::UpdateProperties();

    AddProperty(new EBProperty("W  - Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	AddProperty(new EBTriggerProperty("Hit F1 for more options"));

	// Vertex shaders
	AddShaderCodeFile( SHADERTYPE_VERTEX, "Neighbor Calc Offsets",		"TexCoord_4_Offset.nvv" );

	// Pixel shaders
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Center to Blue",	"TexelsToBlueWithBias.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Equal Weight(4)",	"EqualWeightCombine.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Dependent GB",	"DependentGB.nvp");

    std::string aboutText = "file://";
    aboutText += GetFilePath("NVEffectsExplained.htm");
    aboutText += "#GameOfLife";
	SetAboutInfo( NULL, _T("Game of Life"), _T(aboutText.c_str()));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( _T("Date"), _T("May 2001"));

}



HRESULT Effect_GameOfLife::Initialize( LPDIRECT3DDEVICE8 pDev )
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

	if( m_pCA_GameOfLife == NULL )
	{
		m_pCA_GameOfLife = new CA_GameOfLife();
		assert( m_pCA_GameOfLife );
	}


	m_pCA_GameOfLife->Initialize( m_pD3DDev, m_pShaderManager );


	m_bWireframe = m_pCA_GameOfLife->m_bWireframe;

	////////////////////////////////////


	return( hr );

}



HRESULT Effect_GameOfLife::Free()
{
	SAFE_DELETE( m_pCA_GameOfLife );
	SAFE_DELETE( m_pShaderManager );

	SAFE_RELEASE( m_pD3DDev );

	return S_OK;
}


HRESULT Effect_GameOfLife::Start()
{
	return S_OK;
}



void Effect_GameOfLife::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	// Note - pressing tidle key will not cause this to be called
	//  until the window is resized - Something happens then to 
	//  trigger the Keyboard function.


	bool processed;

	if( m_pCA_GameOfLife == NULL )
	{
		FDebug("*** Keyboard: m_pCA_GameOfLife class doesn't exist!\n");
		return;
	}

	processed = m_pCA_GameOfLife->Keyboard( dwKey, nFlags, bDown );

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

			str += " Home - Restart from initial frame  \n\n";

			str += " R    - Reload \"rules\" and \"output\" maps      \n";
			str += "          from files\n";
			str += " SPC  - Stop/Go animation toggle\n";
			str += " RET  - Single frame step\n";
			str += " B    - Toggle border wrapping\n";
			str += "\n";

			str += " 1    - Draw neighbor accumulation map\n";
			str += " 2    - Draw current generation\n";
			str += " 3    - Draw initial, temp, and output frames\n";
			str += "\n";

			str += " M    - Increase # frames to skip display\n";
			str += " N    - Decrease # frames to skip display\n";
			str += " S    - Slow updates to ~20 fps\n\n";

			MessageBoxEx( NULL, str.c_str(),
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
			m_pCA_GameOfLife->m_bWireframe = m_bWireframe;

            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

		case EB_RESET:
        {
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

void Effect_GameOfLife::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{

}


void Effect_GameOfLife::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{

}


void Effect_GameOfLife::MouseMove(HWND hWnd, int x, int y)
{

}



HRESULT Effect_GameOfLife::Tick(EBTimer* pTimer)
{	
	HRESULT hr = E_FAIL;


	// Run a step of the game
	// The game class draws it's output to the
	//   screen or not depending on its m_eRenderMode
	//   variable

	if( m_pCA_GameOfLife != NULL )
	{
		hr = m_pCA_GameOfLife->Tick();
	}


	return( hr );
}



