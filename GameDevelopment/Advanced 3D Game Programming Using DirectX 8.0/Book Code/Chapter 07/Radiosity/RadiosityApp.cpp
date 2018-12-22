/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: RadiosityApp.cpp
 *    Desc: 
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "RadiosityCalc.h"

#include <functional>
#include <string>

using namespace std;

cRadiosityCalc radCalc;

HWND g_hWnd;

string g_helpMessage("\
Radiosity Sample Application\n\
Advanced 3D Game Programming using DirectX 8.0\n\
\n\
  Controls:\n\
      Keypad / Arrow Keys : control object\n\
");


class cRadiosityApp : public cApplication
{

public:

	//==========--------------------------  cRadiosityApp

	cRadiosityApp() : cApplication() 
	{
		m_title = string( "Radiosity Sample" );
	}

	void SceneInit();
	void DoFrame( float timeDelta );
};

cApplication* CreateApplication()
{
	return new cRadiosityApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}


void cRadiosityApp::SceneInit()
{
	// initialize our scene
	Graphics()->SetProjectionData( PI/3, 1.f, 200.f );
	Graphics()->MakeProjectionMatrix();

	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();

	// initialize the camera
	matrix4 camMat = matrix4::CameraLookAt( point3(0,0,22), point3(0,0,0) );
	Graphics()->SetViewMatrix( camMat );
	Graphics()->SetWorldMatrix( matrix4::Identity );

	lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
	lpDevice->SetRenderState(D3DRS_DITHERENABLE, true );
	lpDevice->SetRenderState(D3DRS_LIGHTING, false );
	
	lpDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );

	cFile file;
	file.Open( "media\\complex.rad" );
	radCalc.Load( file );
	file.Close();

	g_hWnd = MainWindow()->GetHWnd();
}


void cRadiosityApp::DoFrame( float timeDelta )
{

	static float rho = 0.f, theta = 0.0f, dist = 30.0f;

	static float wind = 0.0;
	wind += 0.1f;

	static bool bCWasUp = false;

	// first, run our key checks.
	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD9 ) )
		dist -= 3.f;
	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD3 ) )
		dist += 3.f;

	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD8 ) )
		rho += 0.05f;
	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD2 ) )
		rho -= 0.05f;

	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD6 ) )
		theta += 0.05f;
	if( Input()->GetKeyboard()->Poll( DIK_NUMPAD4 ) )
		theta -= 0.05f;

	Snap( rho, -PI/2.f, PI/2.f );
	Snap( dist, 2.f, 100.f );

	point3 temp = point3::Spherical( theta, rho, dist );

	Graphics()->SetViewMatrix( matrix4::CameraLookAt(temp, point3(0,0,0) ));

	static bool keepIterating = true;

	if( keepIterating )
	{
		keepIterating = radCalc.CalcNextIteration();
	}

	// draw the scene
	Graphics()->Clear( true, true, 0, 1.f );

	Graphics()->BeginScene();
	
	radCalc.Draw();	

	Graphics()->DrawTextString(
		4,
		4,
		D3DCOLOR_XRGB( 100, 100, 100 ),
		g_helpMessage.c_str()); 

	Graphics()->EndScene();

	// flip the buffer.
	Graphics()->Flip();
}

