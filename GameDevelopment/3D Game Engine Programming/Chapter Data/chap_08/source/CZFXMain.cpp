// -----------------------------------------------------------------
// zfxMain
// -----------------------------------------------------------------
// http://www.zfx.info                       c. by Oliver DŸvel 2003
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// Include
// -----------------------------------------------------------------
#include "CZFXMain.h"                                 // Main Header
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
// Info: Windowshauptprozedur
// -----------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
    // Create our application
	CZFXMain ZFXApp;

	// init
	if( FAILED( ZFXApp.Create( hInst ) ) ) return 0;

	// und wech
	return (int)ZFXApp.Run();
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: CZFXMain( void )
// Info: Normal constructor
// -----------------------------------------------------------------
CZFXMain::CZFXMain( void ){}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: OnInit( void )
// Info: Initialisiert die Klasse
// -----------------------------------------------------------------
HRESULT CZFXMain::OnInit( void )
{
	m_uiLogLevel         = 10;                  // Loglevel

	// Can use shaders?
	if( !m_pDevice->CanDoShaders() )
	{
		LOG( 1, true, "ERROR: Shaders not supported. STOP" );
		PostQuitMessage(0);
		return E_FAIL;
	}

	// Load Vertex Shader A
	if( FAILED( m_pDevice->CreateVShader( "SHADER\\VERTEX_1.VSH", 0, true,
		                                  false, &m_iVShaderID_A ) ) )
	{
		LOG( 1, true, "ERROR: Vertex-Shader not found" );
		return ZFX_FAIL;
	}

	// Load Pixel Shader A
	if( FAILED( m_pDevice->CreatePShader( "SHADER\\PIXEL_1.PSH", 0, true,
		                                  false, &m_iPShaderID_A ) ) )
	{
		LOG( 1, true, "ERROR: Pixel-Shader not found" );
		return ZFX_FAIL;
	}

	// load a model
	m_pModel = new CZFXModel( "DATA\\MODEL\\ALIEN\\ALIEN.CBF", m_pDevice, 60.0f );

	// Settings
	m_ulOldTime         = -1.0f;                       // Time

	// Bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: OnInit( void )
// Info: Initialisiert die Klasse
// -----------------------------------------------------------------
HRESULT CZFXMain::OnKeyDown( WPARAM wParam )
{
	// Tasten prŸfen
	if( wParam == VK_UP )	m_pModel->SetAnimation( m_pModel->GetAnimation() + 1 );
	if( wParam == VK_DOWN )	m_pModel->SetAnimation( m_pModel->GetAnimation() - 1 );
	if( wParam == VK_RIGHT )m_pModel->SetAnimation( true, m_pModel->GetAnimation() + 1 );
	if( wParam == VK_LEFT )	m_pModel->SetAnimation( true, m_pModel->GetAnimation() - 1 );
	if( wParam == VK_SPACE )  m_pModel->SetRenderNormals();
	if( wParam == VK_RETURN )  m_pModel->SetRenderBones();

	// Bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Render( void )
// Info: Hier wird alles fŸr den Frame bearbeitet.
// -----------------------------------------------------------------
HRESULT CZFXMain::Render( void )
{
    // Variablen init
    ZFXMatrix  mWorld;							// Weltmatrix
	ZFXMatrix  mInverse;                        // Inverse matrix
	float      fDeltaTime	= 0.0f;		    	// Deltazeit

	// Timer abgleichen
	m_ulCurrentTime		= timeGetTime();

	// First time?
	if( m_ulOldTime == -1 ) m_ulOldTime = m_ulCurrentTime;

	// Deltazeit berechnen
	fDeltaTime = (float)m_ulCurrentTime - m_ulOldTime;

	// ist schon eine Sekunde vergangen?
	if( fDeltaTime >= 1000 )
	{
		m_uiFPS			= m_uiFrameCount;
		m_ulOldTime		= m_ulCurrentTime;
		m_uiFrameCount	= 0;
	} else {
		// Den Frame zŠhlen
		m_uiFrameCount++;
	}

	// Deltatime vorbereiten
	fDeltaTime /= 1000;

    // Masterwindows setzen
    m_pDevice->UseWindow( 0 );

    // View ausrichten
    m_pDevice->SetView3D( ZFXVector( 1.0, 0.0, 0.0 ),
                          ZFXVector( 0.0, 1.0, 0.0 ),
                          ZFXVector( 0.0, 0.0, 1.0 ),
                          ZFXVector( 0.0, -10.0, -150.0 ) );

	// Den ersten Viewport auf das gesamte Fenster setzen
	m_pDevice->SetMode( EMD_PERSPECTIVE, 0 );
	m_pDevice->SetBackfaceCulling( RS_CULL_CCW );
	m_pDevice->SetClearColor( 0.5f, 0.3f, 0.3f );

	// Use shaders
	m_pDevice->UseShaders( true );

	// Set and use our shader
	m_pDevice->ActivateVShader( m_iVShaderID_A, VID_CA );
	m_pDevice->ActivatePShader( m_iPShaderID_A );

	// Set Lightposition
	m_sLight_1.x = 0.0f; m_sLight_1.y = 0.0f; m_sLight_1.z = 2000.0f;
	mInverse.InverseOf( mWorld );
	m_sLight_1 = m_sLight_1 * mInverse;
	m_pDevice->SetShaderConstant( SHT_VERTEX, DAT_FLOAT, 21, 1, &m_sLight_1 );

	// Set Ambientcolor
	m_pDevice->SetAmbientLight( 0.8f, 0.8f, 0.8f );

	// Weltmatrix resetten
	mWorld.Identity();
	mWorld.RotaY( GetTickCount()*0.0005f );
	mWorld.Translate( 0, -20,0 );
	m_pDevice->SetWorldTransform( &mWorld );
	   
	// Alles mšgliche fŸr die neue Scene lšschen
	m_pDevice->BeginRendering( true, true, true);

	// Do something
	m_pModel->Update( fDeltaTime );
	m_pModel->Render();

	// und die Buffer flippen, wir sind dann auch fertig
	m_pDevice->EndRendering();

	// Bye
	return S_OK;
}
// -----------------------------------------------------------------


