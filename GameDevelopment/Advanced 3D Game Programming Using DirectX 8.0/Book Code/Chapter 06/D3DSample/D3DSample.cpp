/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: D3DSample.cpp
 *    Desc: An extremely simple D3D app, using the framework
 *          we have made
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

class cD3DSampleApp : public cApplication
{

public:

	string m_filename;
	cModel* m_pModel;

	void InitLights();

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();

	virtual void SceneEnd()
	{
		delete m_pModel;
	}

	cD3DSampleApp() :
		cApplication()
	{
		m_title = string( "D3DSample - Objects Spinning in D3D" );
		m_pModel = NULL;
		m_filename = "..\\BIN\\Media\\rabbit.o3d";
	}
};

cApplication* CreateApplication()
{
	return new cD3DSampleApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}

void cD3DSampleApp::SceneInit()
{
	/**
	 * We're making the FOV less than 90 degrees.
	 * this is so the object doesn't warp as much
	 * when we're really close to it.
	 */
	Graphics()->SetProjectionData( PI/4.f, 0.5f, 10.f );
	Graphics()->MakeProjectionMatrix();

	/**
	 * initialize our scene
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_AMBIENT, 0x404040);

	/**
	 * initialize the camera
	 */
	Graphics()->SetViewMatrix( matrix4::Identity );

	/**
	 * Create a model with the given filename,
	 * and resize it so it fits inside a unit sphere.
	 */
	m_pModel = new cModel( m_filename.c_str() );
	m_pModel->Scale( 1.f / m_pModel->GenRadius() );

	InitLights();
}


void cD3DSampleApp::InitLights()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
    
    sLight light;

	// Light 0
	light = sLight::Directional( 
		point3(0,-4,2).Normalized(),
		0.5f * color3::White + 0.2f * color3::Red,
		0.7f * color3::White + 0.2f * color3::Red,
		0.2f * color3::White + 0.2f * color3::Red);

    // Set the light
    pDevice->SetLight( 0, &light );
	pDevice->LightEnable(0, TRUE);

	// Light 1
	light = sLight::Directional( 
		point3(3,1,1).Normalized(),
		0.5f * color3::White + 0.2f * color3::Green,
		0.7f * color3::White + 0.2f * color3::Green,
		0.2f * color3::White + 0.2f * color3::Green);

    // Set the light
    pDevice->SetLight( 1, &light );
	pDevice->LightEnable(1, TRUE);

	// Light 2
	light = sLight::Directional( 
		point3(-3,3,5).Normalized(),
		0.5f * color3::White + 0.2f * color3::Blue,
		0.7f * color3::White + 0.2f * color3::Blue,
		0.2f * color3::White + 0.2f * color3::Blue);

    // Set the light
    pDevice->SetLight( 2, &light );
	pDevice->LightEnable(2, TRUE);

    sMaterial mat( 
		16.f, 
		color3(0.5f,0.5f,0.5f),
		color3(0.7f,0.7f,0.7f),
		color3(0.1f,0.1f,0.1f) );

    pDevice->SetMaterial(&mat);
}


void cD3DSampleApp::DoFrame( float timeDelta )
{
	/**
	 * update the time
	 */
	static float rotAmt = 0.f;
	rotAmt += timeDelta;

	/**
	 * then, draw the frame.
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	if( pDevice )
	{
		Graphics()->Clear( true, true, 0x000000, 1.f );

		Graphics()->BeginScene();

		/**
		 * Build a simple matrix for the object,
		 * just spin around all three axes.
		 */
		matrix4 mat;
		mat.MakeIdent();
		mat.Rotate( rotAmt, 1.1f * rotAmt, 1.4f * rotAmt );
		mat.Place( point3(0,0,3.f) );
		Graphics()->SetWorldMatrix( mat );

		/**
		 * Here is where we actually draw our object
		 */
		m_pModel->Draw( mat );
		
		Graphics()->EndScene();

		/**
		 * flip the buffer.
		 */
		Graphics()->Flip();
	}
}