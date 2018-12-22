/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MultiTex.cpp
 *    Desc: More multitexture than you can shake a stick at
 *          
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <string>
#include <vector>
using namespace std;

/**
 * This is the type of vertex we'll be using
 * location, normal, and 2 sets of uv coords.
 */
struct sMTVertex
{
	point3 loc;
	point3 norm;
	// 2 sets of u,v coordinates
	float tex[2][2];
};


class cMultiTexApp : public cApplication,
	public iKeyboardReceiver
{

public:

	float m_timeDelta;

	matrix4	m_earthMat;
	matrix4	m_cloudMat;

	vector< sMTVertex > m_earthVerts;
	vector< sMTVertex > m_cloudVerts;
	cTexture* m_pTextures[8];

	string m_helpText;

	bool m_bDrawHelp;

	/**
	 * Are we currently rendering a pass?
	 */
	bool m_bDoBasePass;
	bool m_bDoGlowPass;
	bool m_bDoGlossPass;
	bool m_bDoDetailPass;
	bool m_bDoEnvyPass;
	bool m_bDoCloudPass;

	/**
	 * Can we render a pass at all?
	 */
	bool m_bCanDoBasePass;
	bool m_bCanDoGlowPass;
	bool m_bCanDoGlossPass;
	bool m_bCanDoDetailPass;
	bool m_bCanDoEnvyPass;
	bool m_bCanDoCloudPass;

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();

	bool CheckCaps();

	cMultiTexApp() :
		cApplication()
	{
		m_title = string( "MultiTex - 6 passes of multitexture madness" );
		m_glFlags &= ~GLF_ZBUFFER; // turn off Z

		for( int i=0; i<8; i++ )
		{
			m_pTextures[i] = NULL;
		}

		
	}

	virtual ~cMultiTexApp()
	{
		/**
		 * Delete all of the textures
		 */
		for( int i=0; i<8; i++ )
		{
			delete m_pTextures[i];
		}
	}

	virtual void KeyUp( int key );
	virtual void KeyDown( int key ){}

	void CreateSphereVertices( float radius, vector<sMTVertex>* pVec, float* scales );
	void InitLights();

	void LoadTextures();

	void DoBasePass();
	void DoGlowPass();
	void DoGlossPass();
	void DoDetailPass();
	void DoEnvyPass();
	void DoCloudPass();

	void DrawBackground();

};

cApplication* CreateApplication()
{
	return new cMultiTexApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}

bool cMultiTexApp::CheckCaps()
{
	// certain base functionality is assumed, like MODULATE
	// and SELECTARGx

	m_bCanDoBasePass = true; // lord help us if the card can't do the base pass.
	m_bCanDoDetailPass = false;
	m_bCanDoGlowPass = false;
	m_bCanDoGlossPass = false;
	m_bCanDoEnvyPass = false;
	m_bCanDoCloudPass = false;

	D3DCAPS8 DevCaps;

	Graphics()->GetDevice()->GetDeviceCaps( &DevCaps );

	bool bCanDoMultitexture = (DevCaps.MaxSimultaneousTextures >= 2);
	bool bCanDoMod2x = (DevCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X)? true : false; 
	
	
	/**
	 * check detail mapping
	 */
	if( (DevCaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR) &&
		(DevCaps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR) )
	{
		m_bCanDoDetailPass = true;
	}

	/**
	 * check glow mapping
	 */
	if( bCanDoMultitexture &&
		(DevCaps.SrcBlendCaps & D3DPBLENDCAPS_ONE) &&
		(DevCaps.DestBlendCaps & D3DPBLENDCAPS_ONE) )
	{
		m_bCanDoGlowPass = true;
	}

	/**
	 * check envy mapping
	 */
	if( (DevCaps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN ) &&
		(DevCaps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR) &&
		(DevCaps.DestBlendCaps & D3DPBLENDCAPS_ONE) )
	{
		m_bCanDoEnvyPass = true;
	}

	/**
	 * check gloss mapping
	 */
	if( bCanDoMod2x &&
		(DevCaps.SrcBlendCaps & D3DPBLENDCAPS_ONE) &&
		(DevCaps.DestBlendCaps & D3DPBLENDCAPS_ONE) )
	{
		m_bCanDoGlossPass = true;
	}

	/**
	 * check cloud mapping
	 */
	if( (DevCaps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR) &&
		(DevCaps.DestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR) )
	{
		m_bCanDoCloudPass = true;
	}

	return true;
}


void cMultiTexApp::SceneInit()
{
	/**
	 * We're making the FOV less than 90 degrees.
	 * this is so the object doesn't warp as much
	 * when we're close to it.
	 */
	CheckCaps();

	Graphics()->SetProjectionData( PI/4.f, 1.f, 150.f );
	Graphics()->MakeProjectionMatrix();

	/**
	 * Request keyboard updates
	 */
	Input()->GetKeyboard()->SetReceiver( this );

	/**
	 * initialize our scene
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFF );

	/**
	 * Turn on bilinear filtering
	 */

	pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );

	pDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_POINT );
	

	/**
	 * initialize the camera
	 * the 'up' vector is tilted to make the earth appear off-axis
	 */
	Graphics()->SetViewMatrix( 
		matrix4::CameraLookAt(
			point3(4, 10, -25 ), 
			point3(0,0,0) )
	);

	m_timeDelta = 0.03f; // a 30th of a second is a good first value

	/**
	 * We're wasting a bit of space, since the cloud vertices
	 * only use one set of texture coordinates.  Fixing that
	 * would introduce code complexity, which I'd rather avoid
	 */
	float scales[2] = {1.0f, 11.0f};
	CreateSphereVertices( 10.f, &m_earthVerts, &scales[0] );
	CreateSphereVertices( 10.2f, &m_cloudVerts, &scales[0] );

	InitLights();
	LoadTextures();

	m_helpText = "Multitexture Sample Application\n" +
	m_helpText += "Advanced 3D Game Programming using DirectX 8.0\n";
	m_helpText += "  available keys: (availability depends on device features)\n";

	m_helpText += (( m_bCanDoBasePass ) ? 
			"   [1]: Toggle base pass\n" : 
			"   [1]: (Not available)\n");

	m_helpText += (( m_bCanDoDetailPass ) ? 
			"   [2]: Toggle detail pass\n" : 
			"   [2]: (Not available)\n");

	m_helpText += (( m_bCanDoGlowPass ) ? 
			"   [3]: Toggle glow pass\n" : 
			"   [3]: (Not available)\n");

	m_helpText += (( m_bCanDoEnvyPass ) ? 
			"   [4]: Toggle envy pass\n" : 
			"   [4]: (Not available)\n");

	m_helpText += (( m_bCanDoGlossPass ) ? 
			"   [5]: Toggle gloss pass\n" : 
			"   [5]: (Not available)\n");

	m_helpText += (( m_bCanDoCloudPass ) ? 
			"   [6]: Toggle cloud pass\n" : 
			"   [6]: (Not available)\n");

	/**
	 * Initially, don't draw help
	 */
	m_bDrawHelp = false;

	/**
	 * Initially, just display the base map.
	 */
	m_bDoBasePass = true;
	m_bDoGlowPass = false;
	m_bDoGlossPass = false;
	m_bDoDetailPass = false;
	m_bDoEnvyPass = false;
	m_bDoCloudPass = false;

}

void cMultiTexApp::DoFrame( float timeDelta )
{
	/**
	 * update the time
	 */
	m_timeDelta = timeDelta;
	
	static float rotAmt = 3*PI/4;
	rotAmt += 0.75f * m_timeDelta;

	/**
	 * then, draw the frame.
	 */
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	if( pDevice )
	{
		pDevice->SetVertexShader( 
			D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2 );

		Graphics()->Clear( true, true, 0, 1.f );

		Graphics()->BeginScene();

		Graphics()->SetWorldMatrix( matrix4::Identity );
		DrawBackground();

		point3 from( 0, 1, 0 );

		m_earthMat = 
			matrix4::AxisAngle( point3::j, rotAmt );

		/**
		 * The cloud matrix rotates a tiny bit faster
		 * than the earth matrix
		 */
		m_cloudMat = 
			matrix4::AxisAngle( point3::j, rotAmt * 0.7f );

		Graphics()->SetWorldMatrix( m_earthMat );

		if( m_bCanDoBasePass && m_bDoBasePass )
		{
			DoBasePass();
		}

		if( m_bCanDoDetailPass && m_bDoDetailPass )
		{
			DoDetailPass();
		}

		if( m_bCanDoGlowPass && m_bDoGlowPass )
		{
			DoGlowPass();
		}

		if( m_bCanDoEnvyPass && m_bDoEnvyPass )
		{
			DoEnvyPass();
		}

		if( m_bCanDoGlossPass && m_bDoGlossPass )
		{
			DoGlossPass();
		}

		if( m_bCanDoCloudPass && m_bDoCloudPass )
		{
			Graphics()->SetWorldMatrix( m_cloudMat );
			DoCloudPass();
		}

		

		if( m_bDrawHelp )
		{
			Graphics()->DrawTextString( 2, 2, D3DCOLOR_XRGB( 0, 255, 0 ), 
				(char*)m_helpText.c_str() );
		}
		else
		{
			Graphics()->DrawTextString( 2, 2, D3DCOLOR_XRGB( 0, 0, 255 ), 
				"Press [H] for help" );
		}

		Graphics()->EndScene();

		/**
		 * flip the buffer.
		 */
		Graphics()->Flip();
	
	}

}

void cMultiTexApp::KeyUp( int key )
{
	if( key == DIK_1 )
	{
		m_bDoBasePass = !m_bDoBasePass;
	}
	if( key == DIK_2 )
	{
		m_bDoDetailPass = !m_bDoDetailPass;
	}
	if( key == DIK_3 )
	{
		m_bDoGlowPass = !m_bDoGlowPass;
	}
	if( key == DIK_4 )
	{
		m_bDoEnvyPass = !m_bDoEnvyPass;
	}
	if( key == DIK_5 )
	{
		m_bDoGlossPass = !m_bDoGlossPass;
	}
	if( key == DIK_6 )
	{
		m_bDoCloudPass = !m_bDoCloudPass;
	}
	if( key == DIK_H )
	{
		m_bDrawHelp = !m_bDrawHelp;
	}
}


void cMultiTexApp::InitLights()
{
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
    
    sLight light;

	// Light 0
	light = sLight::Directional( 
		point3(8,-3,5).Normalized(),
		0.9f * color3::White + 0.1f * color3::Yellow,
		0.9f * color3::White + 0.1f * color3::Yellow );

    // Set the light
    lpDevice->SetLight( 0, &light );
	lpDevice->LightEnable(0, TRUE);

	// Light 1
	light = sLight::Directional( 
		point3(8,-3,5).Normalized(),
		color3::White );

    // Set the light
    lpDevice->SetLight( 1, &light );
}


/**
 * Based off of InitSphereVertices from the
 * BumpEarth D3D SDK application
 */
void cMultiTexApp::CreateSphereVertices( float radius, vector<sMTVertex>* pVec, float* scales )
{
    int numSphereRings    = 30;
    int numSphereSegments = 30;

	sMTVertex curr;
	int i;

    // Establish constants used in sphere generation
    float fDeltaRingAngle = ( PI / numSphereRings );
    float fDeltaSegAngle  = ( 2.0f * PI / numSphereSegments );

    // Generate the group of rings for the sphere
    for( int ring = 0; ring < numSphereRings; ring++ )
    {    
        float r0 = (float)sin( (ring+0) * fDeltaRingAngle );
        float r1 = (float)sin( (ring+1) * fDeltaRingAngle );
        float y0 = (float)cos( (ring+0) * fDeltaRingAngle );
        float y1 = (float)cos( (ring+1) * fDeltaRingAngle );

        // Generate the group of segments for the current ring
        for( int seg = 0; seg < (numSphereSegments+1); seg++ )
        {
            float x0 =  r0 * (float)sin( seg * fDeltaSegAngle );
            float z0 =  r0 * (float)cos( seg * fDeltaSegAngle );
            float x1 =  r1 * (float)sin( seg * fDeltaSegAngle );
            float z1 =  r1 * (float)cos( seg * fDeltaSegAngle );

			curr.loc = radius * point3(x0,y0,z0);
			curr.norm = curr.loc;
			curr.norm.Normalize();
			for( i=0; i<2; i++ )
			{
				curr.tex[i][0] = scales[i] * -((FLOAT)seg)/numSphereSegments;
				curr.tex[i][1] = scales[i] * ((ring+0)/(FLOAT)numSphereRings);
			}
			pVec->push_back( curr );

			curr.loc = radius * point3(x1,y1,z1);
			curr.norm = curr.loc;
			curr.norm.Normalize();
			for( i=0; i<2; i++ )
			{
				curr.tex[i][0] = scales[i] * -((FLOAT)seg)/numSphereSegments;
				curr.tex[i][1] = scales[i] * ((ring+1)/(FLOAT)numSphereRings);
			}
			pVec->push_back( curr );
        }
    }
}


void cMultiTexApp::LoadTextures()
{
	char* names[7] = { 
		"media\\earth.dds", 
		"media\\glowmap.dds", 
		"media\\detail.dds", 
		"media\\stars.dds",
		"media\\glownoise.dds",
		"media\\spheremap.dds",
		"media\\specmap.dds"
	};
	int stages[7] = { 0, 0, 0, 1, 1, 1, 1 };

	for( int i=0; i<7; i++ )
	{
		m_pTextures[i] = new cTexture( names[i], stages[i] );
	}
}

void cMultiTexApp::DoBasePass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * first pass should modulate with the diffuse color
	 */
	pDevice->SetTexture( 0, m_pTextures[0]->GetTexture() );
	SetColorStage( 0, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE );

	/**
	 * first pass doesn't use alpha blending.
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    sMaterial mat( 
		0.f, 
		color3(0.8f,0.8f,0.8f),
		color3(0.0f,0.0f,0.0f),
		color3(0.0f,0.0f,0.0f) );
    pDevice->SetMaterial(&mat);
	
	pDevice->DrawPrimitiveUP(
		D3DPT_TRIANGLESTRIP,
		m_earthVerts.size() - 2,
		&m_earthVerts[0],
		sizeof( sMTVertex ) );

}

void cMultiTexApp::DoDetailPass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * set up modulate 2x style alpha blending
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );

	/**
	 * first stage is the detail map
	 */
	pDevice->SetTexture( 0, m_pTextures[2]->GetTexture() );
	SetColorStage( 0, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_SELECTARG1 );

	/**
	 * The detail map needs the second pair of coordinates
	 */
	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1 );

	pDevice->DrawPrimitiveUP(
		D3DPT_TRIANGLESTRIP,
		m_earthVerts.size() - 2,
		&m_earthVerts[0],
		sizeof( sMTVertex ) );

	/**
	 * Restore the texture coordinates
	 */
	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0 );
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 );
}


void cMultiTexApp::DoGlowPass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * glow map the glow map mask gets modulated with the
	 * inverse diffuse color, that way it fades as light
	 * hits it.
	 */
	pDevice->SetTexture( 0, m_pTextures[1]->GetTexture() );
	SetColorStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_DIFFUSE | D3DTA_COMPLEMENT, 
		D3DTOP_MODULATE );

	/**
	 * The second pass is the noise map, to give the
	 * illusion of millions of little lights.  just
	 * modulate with whatever made it through the 
	 * first pass
	 */
	pDevice->SetTexture( 1, m_pTextures[4]->GetTexture() );
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 );
	SetColorStage( 
		1, 
		D3DTA_TEXTURE, 
		D3DTA_CURRENT, 
		D3DTOP_MODULATE );


	/**
	 * set up add style blending
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	/**
	 * Turn up diffuse all the way to accentuate the
	 * effect
	 */
    sMaterial mat( 
		0.f, 
		color3(1.0f,1.0f,1.0f),
		color3(0.0f,0.0f,0.0f),
		color3(0.0f,0.0f,0.0f) );
    pDevice->SetMaterial(&mat);

	/**
	 * The second light is to help accentuate the light hitting the earth.
	 * This helps the little lights fade away as they hit sunlight.
	 */
	pDevice->LightEnable(1, TRUE);


	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLESTRIP,
		m_earthVerts.size() - 2,
		&m_earthVerts[0],
		sizeof( sMTVertex ) );

	/**
	 * Restore the basic state
	 */
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP ,D3DTOP_DISABLE );
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );
	pDevice->LightEnable(1, FALSE);

}


void cMultiTexApp::DoEnvyPass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * The first color pass is just the inverse diffuse color.
	 * the first alpha pass takes the earth mask used in
	 * the gloss pass.  This will be modulated with the 
	 * final color before being alpha blended onto the 
	 * frame buffer.
	 */
	pDevice->SetTexture( 0, m_pTextures[1]->GetTexture() );
	SetColorStage( 
		0, 
		D3DTA_DIFFUSE | D3DTA_COMPLEMENT, 
		D3DTA_CURRENT,
		D3DTOP_SELECTARG1 );

	SetAlphaStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_CURRENT,
		D3DTOP_SELECTARG1 );

	/**
	 * The second pass is the envy map.  Sure, a nice
	 * star pattern would have worked, but using the 
	 * coffeeshop texturemap really points out that
	 * the envymapping is working correctly.
	 */
	pDevice->SetTexture( 1, m_pTextures[5]->GetTexture() );
	SetColorStage( 
		1, 
		D3DTA_TEXTURE, 
		D3DTA_CURRENT, 
		D3DTOP_MODULATE );

	/**
	 * Set up texture transformations.
	 */
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL  );
	pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	/**
	 * Set up the environment mapping matrix.
	 * This performs the calculation we want:
	 * u=n_x/2 + 0.5
	 * v=-n_y/2 + 0.5
	 */
	matrix4 texMat;
	texMat.MakeIdent();
	texMat._11 = 0.5;
	texMat._41 = 0.5;
	texMat._22 = -0.5;
	texMat._42 = 0.5;
	pDevice->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX*)&texMat );

	/**
	 * Reflect lots of the diffuse light again
	 */
    sMaterial mat( 
		0.f, 
		color3(1.0f,1.0f,1.0f),
		color3(0.0f,0.0f,0.0f),
		color3(0.0f,0.0f,0.0f) );
    pDevice->SetMaterial(&mat);

	// Turn on that extra light we used in the glow pass too.
	pDevice->LightEnable(1, TRUE);

	/**
	 * set up add style blending
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );


	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLESTRIP,
		m_earthVerts.size() - 2,
		&m_earthVerts[0],
		sizeof( sMTVertex ) );

	/**
	 * Fix up all of our esoteric states
	 */
	pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP ,D3DTOP_DISABLE );
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );
	pDevice->LightEnable(1, FALSE);
}


void cMultiTexApp::DoGlossPass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * The first color pass is just the diffuse color.
	 * the first alpha pass uses the gloss map texture.
	 * This will be modulated with the 
	 * final color before being alpha blended onto the 
	 * frame buffer.
	 */
	pDevice->SetTexture( 0, m_pTextures[1]->GetTexture() );
	SetColorStage( 
		0, 
		D3DTA_DIFFUSE, 
		D3DTA_CURRENT,
		D3DTOP_SELECTARG1 );

	SetAlphaStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_CURRENT,
		D3DTOP_SELECTARG1 );

	/**
	 * The second pass is the specular map.  It isn't even
	 * close to being correct, but it looks good enough.
	 */
	pDevice->SetTexture( 1, m_pTextures[6]->GetTexture() );
	SetColorStage( 
		1, 
		D3DTA_TEXTURE, 
		D3DTA_CURRENT, 
		D3DTOP_SELECTARG1 );

	/**
	 * Set up texture transformations.
	 */
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL  );
	pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	/**
	 * Set up the environment mapping matrix.
	 * This performs the calculation we want:
	 * u=n_x/2 + 0.5
	 * v=-n_y/2 + 0.5
	 */
	matrix4 texMat;
	texMat.MakeIdent();
	texMat._11 = 0.5;
	texMat._41 = 0.5;
	texMat._22 = -0.5;
	texMat._42 = 0.5;
	pDevice->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX*)&texMat );

	/**
	 * Reflect lots of the diffuse light again
	 */
    sMaterial mat( 
		0.f, 
		color3(1.0f,1.0f,1.0f),
		color3(0.0f,0.0f,0.0f),
		color3(0.0f,0.0f,0.0f) );
    pDevice->SetMaterial(&mat);

	// Turn on that extra light we used in the glow pass too.
	pDevice->LightEnable(1, TRUE);

	/**
	 * set up add style blending
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLESTRIP,
		m_earthVerts.size() - 2,
		&m_earthVerts[0],
		sizeof( sMTVertex ) );

	/**
	 * Fix up all of our esoteric states
	 */
	pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP ,D3DTOP_DISABLE );
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );
	pDevice->LightEnable(1, FALSE);
}


void cMultiTexApp::DoCloudPass()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	pDevice->SetTexture( 0, m_pTextures[0]->GetTexture() );

	SetColorStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_DIFFUSE, 
		D3DTOP_SELECTARG2 );

	SetAlphaStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_DIFFUSE, 
		D3DTOP_SELECTARG1 );


	/**
	 * Reflect lots of the diffuse light again
	 */
    sMaterial mat( 
		0.f, 
		color3(1.0f,1.0f,1.0f),
		color3(0.0f,0.0f,0.0f),
		color3(0.3f,0.3f,0.3f) );
    pDevice->SetMaterial(&mat);

	/**
	 * Alpha blending modulates with source color, so
	 * the brighter the texture is the more it is seen.
	 */
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLESTRIP,
		m_cloudVerts.size() - 2,
		&m_cloudVerts[0],
		sizeof( sMTVertex ) );
}


/**
 * Draw a simple starry background.
 * uses the camera's location to set itself
 * up so that it's always facing towards
 * the viewer.
 */
void cMultiTexApp::DrawBackground()
{
	matrix4 mat;
	Graphics()->GetViewMatrix( &mat );

	matrix4 viewInv;
	viewInv.ToInverse( mat );

	Graphics()->SetWorldMatrix( viewInv );

	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	pDevice->SetTexture( 0, m_pTextures[3]->GetTexture() );

	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	SetColorStage( 
		0, 
		D3DTA_TEXTURE, 
		D3DTA_DIFFUSE, 
		D3DTOP_SELECTARG1 );

	sLitVertex v[4];

	v[0] = sLitVertex( point3( -150,  150, 80.f ), 0, 0, 0.f, 0.f );
	v[1] = sLitVertex( point3(  150,  150, 80.f ), 0, 0, 8.f, 0.f );
	v[2] = sLitVertex( point3(  150, -150, 80.f ), 0, 0, 8.f, 8.f );
	v[3] = sLitVertex( point3( -150, -150, 80.f ), 0, 0, 0.f, 8.f );


	pDevice->SetVertexShader(
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );

	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLEFAN,
		2,
		v,
		sizeof( sLitVertex ) );

	pDevice->SetVertexShader(
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2 );
}