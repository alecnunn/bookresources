
#include "stdafx.h"

char g_helpMessage[] = "\
Detail Texture example app\n\
Advanced 3D Game Programming using DirectX 8.0\n\
\n\
  Controls:\n\
      [D] : Turn detail textures on/off\n\
      [H] : Toggle Help\n\
";

struct sDetailVertex
{
	point3 loc;
	DWORD diff;
	// 2 pairs of u,v coordinates
	float tex[2][2];
};

struct sDetailPoly
{
	sDetailVertex v[4];

	// Convenience function to set up textures for us.
	void SetCoords( int num, float minu, float maxu, float minv, float maxv )
	{
		v[0].tex[num][0] = minu;
		v[0].tex[num][1] = minv;
		v[1].tex[num][0] = maxu;
		v[1].tex[num][1] = minv;
		v[2].tex[num][0] = maxu;
		v[2].tex[num][1] = maxv;
		v[3].tex[num][0] = minu;
		v[3].tex[num][1] = maxv;
	}
};

class cDetailApp : public cApplication,
	public iKeyboardReceiver

{
	// Data for the square
	sDetailPoly m_square;
	void LoadTextures();

	void SetStage1();
	void SetStage2();

	cTexture*	m_pTextures[3];

	// If this is true, draw some help on the screen
	bool m_drawHelp;

	void InitSquare();
	void DrawSquare();

	bool m_bCanDoMultitexture;
	bool m_bCanDoAddSigned;
	bool m_bDrawDetail;

public:

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();

	cDetailApp() :
		cApplication()
	{
		m_pTextures[0] = NULL;
		m_pTextures[1] = NULL;
		m_pTextures[2] = NULL;
		m_title = string( "Detail Textures - press [H] for help" );
	}

	~cDetailApp()
	{
		delete m_pTextures[0];
		delete m_pTextures[1];
		delete m_pTextures[2];
	}

	//==========--------------------------  iKeyboardReceiver

	virtual void KeyUp( int key );
	virtual void KeyDown( int key );

	//==========--------------------------  cDetailApp

	bool CheckCaps()
	{
		D3DCAPS8 DevCaps;

		Graphics()->GetDevice()->GetDeviceCaps( &DevCaps );
	
		m_bCanDoMultitexture = false;
		if( DevCaps.MaxSimultaneousTextures > 1 )
		{
			m_bCanDoMultitexture = true;
		}

		m_bCanDoAddSigned = false;
		if( DevCaps.TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED )
		{
			m_bCanDoAddSigned = true;
		}
		
		if( !(DevCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X) )
		{
			// the device can't do mod 2x.  If we also can't do add signed,
			// we have no way to do the multitexture.
			if( !m_bCanDoAddSigned )
			{
				// turn off multitexture and just go with the one detail texture
				m_bCanDoMultitexture = false;
			}
		}

		bool bSrcColor = DevCaps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR;
		bool bDestColor = DevCaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR;
		if( !m_bCanDoMultitexture && !(bSrcColor && bDestColor) )
		{
			// device couldn't do the alpha blending we wanted.
			return false;
		}

		return true;
	}
};

cApplication* CreateApplication()
{
	return new cDetailApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}

void cDetailApp::SceneInit()
{
	CheckCaps();

	Graphics()->SetProjectionData( PI/3.5f, 1.f, 200.f );
	Graphics()->MakeProjectionMatrix();

	Input()->GetKeyboard()->SetReceiver( this );

	// initialize our scene
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
	lpDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	lpDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	lpDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	lpDevice->SetRenderState( D3DRS_AMBIENT, 0x00404040 );
	lpDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// initialize the camera
	Graphics()->SetViewMatrix( matrix4::Identity );

	// don't draw help
	m_drawHelp = false;
	// do draw the detail
	m_bDrawDetail = true;

	// init our geometry
	InitSquare();

	// load our textures
	LoadTextures();
}


void cDetailApp::DoFrame( float timeDelta )
{
	static float total =0.f;
	total += timeDelta;
	
	float dist = 1.1f + 80.f*(1.f + sin(total));

	matrix4 mat;
	mat.ToTranslation( point3(0.0f, 0.0f, dist) );
	Graphics()->SetWorldMatrix( mat );
	
	// then, draw the frame.
	Graphics()->BeginScene();

	Graphics()->Clear( true, true, 0xE0E0E0, 1.f );

	// Draw the square
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	if( m_bCanDoMultitexture )
	{
		// Do one pass, two textures.
		pDevice->SetTexture( 0, m_pTextures[0]->GetTexture() );	

		if( m_bDrawDetail )
		{
			pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			pDevice->SetTexture( 1, m_pTextures[2]->GetTexture() );	

			if( m_bCanDoAddSigned )
			{
				pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
			}
			else
			{
				pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			}
		}
		else
		{
			pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		}

		DrawSquare();
	}
	else
	{
		// Do two passes, blended.

		// Pass 1: no alpha blending, base texture.
		pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pDevice->SetTexture( 0, m_pTextures[0]->GetTexture() );	
		DrawSquare();

		if( m_bDrawDetail )
		{
			// Pass 2: Alpha-blend in the detail texture
			pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			pDevice->SetTexture( 0, m_pTextures[1]->GetTexture() );	

			pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
			DrawSquare();
			// restore the texture index
			pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

		}
	}

	

	if( m_drawHelp )
	{
		Graphics()->DrawTextString(
			4,
			4,
			D3DCOLOR_XRGB( 256, 0, 0 ),
			g_helpMessage); 
	}
	else
	{
		Graphics()->DrawTextString(
			4,
			4,
			D3DCOLOR_XRGB( 256, 0, 0 ),
			"Press [H] for help" ); 
	}

	Graphics()->EndScene();

	// flip the buffer.
	Graphics()->Flip();
}



void cDetailApp::KeyUp( int key )
{
	if( key == DIK_H )
	{
		m_drawHelp = !m_drawHelp;
	}

	else if( key == DIK_D )
	{
		m_bDrawDetail = !m_bDrawDetail;
	}
}


void cDetailApp::KeyDown( int key )
{
}


void cDetailApp::InitSquare()
{
	point3 v[4];
	v[0] = point3(-10.f, 10.f,0.f);
	v[1] = point3( 10.f, 10.f,0.f);
	v[2] = point3( 10.f,-10.f,0.f);
	v[3] = point3(-10.f,-10.f,0.f);

	DWORD diff = -1; // white

	m_square.v[0].loc = v[0];
	m_square.v[0].diff = diff;
	m_square.v[1].loc = v[1];
	m_square.v[1].diff = diff;
	m_square.v[2].loc = v[2];
	m_square.v[2].diff = diff;
	m_square.v[3].loc = v[3];
	m_square.v[3].diff = diff;
	m_square.SetCoords( 0, 0.f, 1.f, 0.f, 1.f );
	m_square.SetCoords( 1, 0.5f, 17.9f, 0.f, 17.9f );
}

void cDetailApp::DrawSquare()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	int fvfFlags = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;

	pDevice->SetVertexShader( fvfFlags );

	pDevice->DrawPrimitiveUP(
		D3DPT_TRIANGLEFAN,
		2,
		&m_square,
		sizeof( sDetailVertex ) );
}

void cDetailApp::LoadTextures()
{
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();

	/**
	 * turn on bi-lin
	 */
    lpDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    lpDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	lpDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );
	lpDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

    lpDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    lpDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	lpDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_POINT );
	lpDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	/**
	 * base map in texture stage 0
	 */
	m_pTextures[0] = new cTexture( "media\\base.dds", 0 ); 

	/**
	 * because of the multitexture, and because some cards (3dfx) can't
	 * share texture memory across stages, we need two copies of the 
	 * detail map, one for each stage.
	 * InitDetail generates mip-levels, and also blurs each mip upon
	 * creation, to help the high-frequency detail fade away quicker.
	 */
	m_pTextures[1] = new cTexture( "media\\detail.dds", 0 ); 
	if( m_bCanDoMultitexture )
	{
		m_pTextures[2] = new cTexture( "media\\detail.dds", 1 ); 
	}
	
}


void cDetailApp::SetStage1()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	// Set the texture state's for single-texturing mode
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	// In a real-world app we'd do something more meaningful with the 
	// second stage if we had multitexture, such as a lightmap
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pDevice->SetTexture( 0, m_pTextures[0]->GetTexture() );	

	// this stage doesn't use alpha blending, turn it off.
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void cDetailApp::SetStage2()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	// Set the texture state's for single-texturing mode
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pDevice->SetTexture( 0, m_pTextures[1]->GetTexture() );	

	if( m_bCanDoMultitexture )
	{
		pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		if( m_bCanDoAddSigned )
		{
			pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED );
		}
		else
		{
			pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		}
		
		pDevice->SetTexture( 1, m_pTextures[2]->GetTexture() );	
	}


}

