// teapot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
//#include "resource.h"

#include "SubDivSurf.h"

char g_helpMessage[] = "\
Subdivision Surface Sample Application\n\
Advanced 3D Game Programming using DirectX 8.0\n\
\n\
  Controls:\n\
      Keypad / Arrow Keys : control object\n\
      [X] : Subdivide Model (this can get slow!)\n\
      [C] : Change Rendering Mode (solid/wire/point)\n\
      [H] : Toggle Help\n\
";


class cSubDivApp : public cApplication,
	public iKeyboardReceiver
{

public:

	/**
	 * Controls for the object.
	 */
	float m_yaw;
	float m_pitch;
	float m_roll;
	float m_dist;

	/**
	 * how long the last frame took (the next one should take about as long)
	 */
	float m_timeDelta;

	/**
	 * If this is true, draw some help on the screen
	 */
	bool m_drawHelp;

	/**
	 * We keep track of how many triangles we have in our scene 
	 * and print the info
	 */
	int m_nTris;

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();
	virtual void SceneEnd(){ delete m_pSurf; }

	cSubDivApp() :
		cApplication()
	{
		m_title = string( "Subdivision Surface Sample" );
		m_pSurf = NULL;
		m_filename = string( "media\\rabbit.o3d" );
	}


	cSubDivSurf* m_pSurf;
	matrix4	m_surfMat;
	string m_filename;

	//==========-------------------------  iKeyboardReceiver

	virtual void KeyUp( int key );
	virtual void KeyDown( int key );

	//==========--------------------------  cSubDivApp

	void InitLights();
};

cApplication* CreateApplication()
{
	return new cSubDivApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}




void cSubDivApp::SceneInit()
{
	/**
	 * We're making the FOV less than 90 degrees.
	 * this is so the object doesn't warp as much
	 * when we're really close to it.
	 */
	Graphics()->SetProjectionData( PI/4.f, 0.1f, 8.f );
	Graphics()->MakeProjectionMatrix();

	/**
	 * Tell the keyboard object to send us state
	 * changes.
	 */
	Input()->GetKeyboard()->SetReceiver( this );

	/**
	 * initialize our scene
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_DITHERENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_AMBIENT, -1);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE );

	/**
	 * initialize the camera
	 */
	Graphics()->SetViewMatrix( matrix4::Identity );


	m_pSurf = new cSubDivSurf( m_filename.c_str() );

	m_yaw = 0;
	m_pitch = PI;
	m_roll = 0.0f;
	m_dist = 3.0f;
	m_drawHelp = false;
	m_nTris = m_pSurf->NumTris();

	m_timeDelta = 0.03f; // a 30th of a second is a good first value

	// set up our lighting system.
	InitLights();

}


float GetFrameRate( float timeDelta )
{
	if( timeDelta < 1/60.f )
		timeDelta = 1/60.f;

	float fps = (int)(1.f/timeDelta);

	return fps;
}


void cSubDivApp::DoFrame( float timeDelta )
{
	/**
	 * update the time
	 */
	m_timeDelta = timeDelta;
	static int count = 0;
	
	char nonHelpBuff[1024];
	sprintf( nonHelpBuff, "[H] for help. %i triangles; %2g FPS ", m_nTris, GetFrameRate( timeDelta ) );

	/**
	 * then, draw the frame.
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	if( pDevice )
	{
		Graphics()->Clear( true, true, 0x00E0E0E0, 1.f );
		pDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

		Graphics()->BeginScene();

		/**
		 * Here is where we actually draw our object
		 */
		m_surfMat.MakeIdent();
		m_surfMat.Rotate( m_yaw, m_pitch, m_roll );
		m_surfMat.Place( point3(0.001f, 0.0, m_dist) );

		m_pSurf->Draw( m_surfMat );

		
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
				nonHelpBuff); 
		}

		Graphics()->EndScene();
		/**
		 * flip the buffer.
		 */
		Graphics()->Flip();
	
	}


}

void cSubDivApp::InitLights()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
    
    sLight light;

	// Light 0
	light = sLight::Directional( 
		point3(0,-4,2).Normalized(),
		0.2f * color3::White + 0.2f * color3::Red,
		0.5f * color3::White + 0.5f * color3::Red );

    // Set the light
    pDevice->SetLight( 0, &light );
	pDevice->LightEnable(0, TRUE);

	// Light 1
	light = sLight::Directional( 
		point3(3,1,1).Normalized(),
		0.2f * color3::White + 0.2f * color3::Green,
		0.5f * color3::White + 0.5f * color3::Green );

    // Set the light
    pDevice->SetLight( 1, &light );
	pDevice->LightEnable(1, TRUE);

	// Light 2
	light = sLight::Directional( 
		point3(-3,3,5).Normalized(),
		0.2f * color3::White + 0.2f * color3::Blue,
		0.5f * color3::White + 0.7f * color3::Blue );

    // Set the light
    pDevice->SetLight( 2, &light );
	pDevice->LightEnable(2, TRUE);

    sMaterial mat( 
		20.f, 
		color3(0.7f,0.7f,0.7f),
		color3(0.9f,0.9f,0.9f),
		color3(0.1f,0.1f,0.1f) );

    pDevice->SetMaterial(&mat);
}

void cSubDivApp::KeyUp( int key )
{
	if( key == DIK_X )
	{
		m_pSurf->Subdivide();
		m_nTris = m_pSurf->NumTris();
	}
	if( key == DIK_C )
	{
		LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
		DWORD state;
		pDevice->GetRenderState( D3DRS_FILLMODE, &state );

		switch( state )
		{
		case D3DFILL_POINT:
			pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			break;
		case D3DFILL_WIREFRAME:
			pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
			pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			break;
		case D3DFILL_SOLID:
			pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
			break;
		}
			
	}

	if( key == DIK_F1 )
	{
		MessageBox(
			MainWindow()->GetHWnd(),
			g_helpMessage,
			"Help",
			MB_OK );
	}

	if( key == DIK_H )
	{
		m_drawHelp = !m_drawHelp;
	}
}


void cSubDivApp::KeyDown( int key )
{
	if( key == DIK_PRIOR || key == DIK_NUMPAD9 )
	{
		m_dist -= m_timeDelta * 3.f;
	}
	if( key == DIK_NEXT || key == DIK_NUMPAD3 )
	{
		m_dist += m_timeDelta * 3.f;
	}

	if( key == DIK_UP || key == DIK_NUMPAD8 )
	{
		m_yaw += m_timeDelta * 1.5f;
	}
	if( key == DIK_DOWN || key == DIK_NUMPAD2 )
	{
		m_yaw -= m_timeDelta * 1.5f;
	}

	if( key == DIK_RIGHT || key == DIK_NUMPAD6 )
	{
		m_pitch += m_timeDelta * 1.5f;
	}
	if( key == DIK_LEFT || key == DIK_NUMPAD4 )
	{
		m_pitch -= m_timeDelta * 1.5f;
	}

	if( key == DIK_HOME || key == DIK_NUMPAD7 )
	{
		m_roll += m_timeDelta * 1.5f;
	}
	if( key == DIK_END || key == DIK_NUMPAD1 )
	{
		m_roll -= m_timeDelta * 1.5f;
	}

}