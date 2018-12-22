/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Teapot.cpp
 *    Desc: Controlling code for the Teapot application
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "bezier.h"

char g_helpMessage[] = "\
Parametric Surface Sample Application\n\
Advanced 3D Game Programming using DirectX 8.0\n\
\n\
  Controls:\n\
      Keypad / Arrow Keys : control teapot\n\
      [Z] : Increase Tesselation level\n\
      [X] : Decrease Tesselation level\n\
      [C] : Cycle rendering mode (filled/point/wireframe)\n\
      [V] : Toggle Contrl mesh\n\
      [H] : Toggle Help\n\
";

class cBezierApp : public cApplication,
	public iKeyboardReceiver

{

	// Controls for the object.
	float m_yaw;
	float m_pitch;
	float m_roll;
	float m_dist;

	// how long the last frame took (the next one should take about as long)
	float m_timeDelta;

	// The actual object
	cBezierObject m_bezObj;

	// matrix for the teapot
	matrix4	m_objMat;

	// If this is true, draw some help on the screen
	bool m_drawHelp;

	// If this is true, draw the control net
	bool m_drawNet;

public:

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();


	cBezierApp() :
		cApplication()
	{
		m_title = string( "Parametric Surface Sample Application" );
	}

	//==========--------------------------  iKeyboardReceiver

	virtual void KeyUp( int key );
	virtual void KeyDown( int key );

	//==========--------------------------  cBezierApp
	void InitLights();
};

cApplication* CreateApplication()
{
	return new cBezierApp();
	
}

void cBezierApp::SceneInit()
{
	Graphics()->SetProjectionData( PI/3, 1.f, 1000.f );
	Graphics()->MakeProjectionMatrix();

	Input()->GetKeyboard()->SetReceiver( this );

	// initialize our scene
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
	lpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	lpDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
	lpDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );

	// initialize the camera
	Graphics()->SetViewMatrix( matrix4::Identity );

	// we could load any mesh file we wanted here 
	m_bezObj.Load( "media\\teapot.bez" );

	m_yaw = -0.0f;
	m_pitch = -0.0f;
	m_roll = 0.0f;
	m_dist = 50.0f;
	m_drawHelp = false;
	m_drawNet = false;

	m_timeDelta = 0.03f; // a 30th of a second is a good first value

	// set up our Lights system.
	InitLights();

}


void cBezierApp::DoFrame( float timeDelta )
{
	// update the time
	m_timeDelta = timeDelta;

	m_objMat.MakeIdent();
	m_objMat.Rotate( m_yaw, m_pitch, m_roll );
	m_objMat.Place( point3(0.001f, -15.0, m_dist) );

	Graphics()->GetDevice()->SetVertexShader( D3DFVF_VERTEX );
	
	// then, draw the frame.
	Graphics()->BeginScene();

	Graphics()->Clear( true, true, -1, 1.f );

	m_bezObj.Draw( m_objMat, m_drawNet );

	

	if( m_drawHelp )
	{
		Graphics()->DrawTextString(
			0,
			0,
			0x40404040,
			g_helpMessage); 
	}
	else
	{
		Graphics()->DrawTextString(
			0,
			0,
			0x40404040,
			"Press [H] for help..."); 
	}

	Graphics()->EndScene();

	// flip the buffer.
	Graphics()->Flip();
}

void cBezierApp::InitLights()
{

	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
    
    sLight light;

	// Light 0
	light = sLight::Directional( 
		point3(0,-4,2).Normalized(),
		0.2f * color3::White + 0.2f * color3::Red,
		0.5f * color3::White + 0.5f * color3::Red );

    // Set the light
    lpDevice->SetLight( 0, &light );
	lpDevice->LightEnable(0, TRUE);

	// Light 1
	light = sLight::Directional( 
		point3(4,1,1).Normalized(),
		0.2f * color3::White + 0.2f * color3::Green,
		0.5f * color3::White + 0.5f * color3::Green );

    // Set the light
    lpDevice->SetLight( 1, &light );
	lpDevice->LightEnable(1, TRUE);

	// Light 2
	light = sLight::Directional( 
		point3(-3,3,5).Normalized(),
		0.2f * color3::White + 0.2f * color3::Blue,
		0.5f * color3::White + 0.7f * color3::Blue );

    // Set the light
    lpDevice->SetLight( 2, &light );
	lpDevice->LightEnable(2, TRUE);

    sMaterial mat( 
		40.f, 
		color3(0.9f,0.9f,0.9f),
		color3(0.9f,0.9f,0.9f),
		color3(0.4f,0.4f,0.4f) );

    lpDevice->SetMaterial(&mat);
}


void cBezierApp::KeyUp( int key )
{
	if( key == DIK_C )
	{
		LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
		DWORD state;
		lpDevice->GetRenderState( D3DRS_FILLMODE, &state );

		switch( state )
		{
		case D3DFILL_POINT:
			lpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			break;
		case D3DFILL_WIREFRAME:
			lpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			break;
		case D3DFILL_SOLID:
			lpDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
			break;
		}
			
	}

	if( key == DIK_H )
	{
		m_drawHelp = !m_drawHelp;
	}

	if( key == DIK_V )
	{
		m_drawNet = !m_drawNet;
	}
}


void cBezierApp::KeyDown( int key )
{

	switch( key )
	{
	case DIK_Z:
		m_bezObj.IncTesselation();
		break;

	case DIK_X:
		m_bezObj.DecTesselation();
		break;

	case DIK_PRIOR:
	case DIK_NUMPAD9:
		m_dist -= m_timeDelta * 30.f;
		break;

	case DIK_NEXT:
	case DIK_NUMPAD3:
		m_dist += m_timeDelta * 30.f;
		break;

	case DIK_UP:
	case DIK_NUMPAD8:
		m_yaw += m_timeDelta * 1.f;
		break;

	case DIK_DOWN:
	case DIK_NUMPAD2:
		m_yaw -= m_timeDelta * 1.f;
		break;

	case DIK_RIGHT:
	case DIK_NUMPAD6:
		m_pitch += m_timeDelta * 1.f;
		break;

	case DIK_LEFT:
	case DIK_NUMPAD4:
		m_pitch -= m_timeDelta * 1.f;
		break;

	case DIK_HOME:
	case DIK_NUMPAD7:
		m_roll += m_timeDelta * 1.f;
		break;

	case DIK_END:
	case DIK_NUMPAD1:
		m_roll -= m_timeDelta * 1.f;
		break;
	}

}

