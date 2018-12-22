// -----------------------------------------------------------------
// zfxMain.cpp
// -----------------------------------------------------------------
// http://www.zfx.info						 c. by Oliver Düvel 2002
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Include
// -----------------------------------------------------------------
#include "CZFXBasis.h"							// ZFX Basis Header
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Global access to the application for WNDProc
// -----------------------------------------------------------------
static CZFXBasis* g_pAppBasis = NULL;
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: CZFXBasis( void )
// Info: Normal constructor
// -----------------------------------------------------------------
CZFXBasis::CZFXBasis( void )
{
 g_pAppBasis          = this;                      // set Pointer
 m_hWnd               = NULL;                      // Windowshandle
 m_hInst              = NULL;                      // Instanz
 strcpy( m_szAppClass, "zfxBasis" );               // Title
 m_bIsActive          = false;                     // Activflag
 m_bIsInitiated       = false;                     // Initflag
 m_bDone              = false;                     // Doneflag
 m_pRenderer          = NULL;                      // Renderinterface
 m_pDevice            = NULL;                      // Renderdevice
 strcpy( m_cAPIName, "Direct3D" );                 // APIName
 m_pLog               = NULL;                      // Logfile
 m_byLog              = 1;                         // Loggen on
 strcpy( m_cLogFileName, "BASISLOG.TXT" );         // Logfilename
 m_uiLogLevel         = 10;                        // Loglevel
 m_pVertices          = NULL;                      // Vertices
 m_pIndices           = NULL;                      // Indices
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ~CZFXBasis( )
// Info: Normal donstructor
// -----------------------------------------------------------------
CZFXBasis::~CZFXBasis(){}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: WndProc( HWND hW, UINT uM, WPARAM wPa, LPARAM lPa )
// Info: Message handler, wich passes messages to the application
// -----------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hW, UINT uM, WPARAM wPa, LPARAM lPa )
{ return g_pAppBasis->MsgProc( hW, uM, wPa, lPa ); }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Create( HINSTANCE hInst )
// Info: Create the windowstuff
// -----------------------------------------------------------------
HRESULT CZFXBasis::Create( HINSTANCE hInst )
{
	// Variablen init
	WNDCLASSEX		wndclass;					// Windowsklasse
	HRESULT			hrRet		= ZFX_OK;		// Return
	HWND			hWnd;						// Windowshandle
	m_hInst         = hInst;                    // Instanz

	// Erstellen und betanken der Windowsklasse
	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= m_hInst;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= CZFXBasis::m_szAppClass;
	wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	// Jetzt können wir die Klasse registrieren
    if( RegisterClassEx( &wndclass ) == 0)		return 0;

    // Nun erstellen wir das Window
    if( !( hWnd = CreateWindowEx(	NULL, CZFXBasis::m_szAppClass,
									CZFXBasis::m_szAppClass,
									WS_OVERLAPPEDWINDOW | WS_VISIBLE,
									GetSystemMetrics(SM_CXSCREEN)/2 -250,
									GetSystemMetrics(SM_CYSCREEN)/2 -187,
									500, 375, NULL, NULL, NULL, NULL ) ) )
												return 0;
	// Wir verstecken das Window aber erst noch einmal
	ShowWindow( hWnd, SW_HIDE );

	// Speichern wir mal die Daten in unseren globalen Variablen
    m_hWnd   = hWnd;             // Das Windowshandle

	// Jetzt erstellen wir uns unser Logfile
	m_pLog	= fopen( CZFXBasis::m_cLogFileName, "w" );
	LOG( 0, true, "ZFXBasis Log started." );

	// Initialisieren wir mal die 3D Mathe Library
	ZFX3DInitCPU();

	// Jetzt versuchen wir mal unsere Engine zu starten
	if( FAILED( hrRet = Init() ) ){
		
		//
		// Leider konnte die Direct3D Engine nicht gestartet werden
		// überprüfen wir mal warum
		//

		// Checken der Fehlermeldung
		switch( hrRet ){
			case ZFX_FAIL:			// Undefinierter Fehler

				// Meldung schreiben
				LOG( 1, false, "ERROR: ProgramStartup() failed\n" );

				// und wech
				break;

			case ZFX_CANCELED:		// Benutzer hat abgebrochen

				// Meldung schreiben
				LOG( 1, false, "ERROR: ProgramStartup() canceled by user\n" );

				// und wech
				break;

			case ZFX_NOTCOMPATIBLE:	// Keine kompatible Hardware gefunden

				// Nachrichtenbox öffnen
				MessageBox( CZFXBasis::m_hWnd,
							"Sorry, no suitable graphics hardware found.",
							"ZFX Engine Report", MB_OK | MB_ICONERROR );

				// Meldung schreiben
				LOG( 1, false, "ERROR: no compatible graphics adapter\n" );

				// und wech
				break;
		}

	} else {

		//
		// Alles lief sehr gut, wir können also weiter machen
		//

		// Zeigen wir mal das Fenster
		ShowWindow( hWnd, SW_SHOW );

		// Set initflag
		m_bIsInitiated = true;

	}

	// und wech
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
// Info: Nachrichtenprozedur für Windows
// -----------------------------------------------------------------
LRESULT CZFXBasis::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// Was ist für eine Nachricht angekommen?
    switch( msg )
	{
		// Haben wir den Eingabefokus?
		case WM_ACTIVATE:	m_bIsActive = (bool)wParam;		break;
         
		// Wurde eine Taste gedrückt
		case WM_KEYDOWN:

			if( wParam == VK_ESCAPE )
			{
				// Wir sollen abschalten
				m_bDone = true;

				// Closenachricht senden
				PostMessage( hWnd, WM_CLOSE, 0, 0 );

				// und raus hier
				return 0;

			}

			// Virtuelle Methode aufrufen
			OnKeyDown( wParam );
		
			break;
		
		// Wir werden geschlossen
		case WM_DESTROY:
			
			// Flag setzen
			m_bDone = true;

			// Nachricht senden
			PostQuitMessage(0);

			// Also räumen wir mal auf
			Destroy();

			// und raus
			return 1;

			break;
    }

	// und wech
	return DefWindowProc( hWnd, msg, wParam, lParam );
}
// -----------------------------------------------------------------
 

// -----------------------------------------------------------------
// Name: Init( void )
// Info: Create and Init the Programm
// -----------------------------------------------------------------
HRESULT CZFXBasis::Init( void )
{
	// We don't have OpenGL yet
	if( strcmp( m_cAPIName, "OpenGL" ) == 0 )               return S_OK;

	// Create the Renderer
	m_pRenderer	= new ZFXRenderer( m_hInst );

	// Creat the Device from the given API
	if( FAILED( m_pRenderer->CreateDevice( m_cAPIName ) ) )	return E_FAIL;
	
    // Get Devicepointer from Renderer
    m_pDevice   = m_pRenderer->GetDevice();

	// everything went smoth?
	if( m_pDevice == NULL )                                 return E_FAIL;

	// Init the Renderdevice
	if( FAILED( m_pDevice->Init( m_hWnd, NULL, 0, 16, 0, true ) ) )
	{
		// something went wrong
		LOG( 0, true,  "Init(): failed to init the Renderdevice" );
        
		// bye
		return E_FAIL;
	}

	// Call our virtual function
	OnInit();

	// bye
	return ZFX_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ProgramCleanup( void )
// Info: cleanup and deinit
// -----------------------------------------------------------------
HRESULT CZFXBasis::Destroy( void )
{
	// Renderer freigeben
	if( m_pRenderer )
	{
      delete m_pRenderer;						// Speicher freigeben
      m_pRenderer = NULL;						// Zeiger löschen
	}

	// Das Logfile schliessen
	if( m_pLog )
	{
        fclose( m_pLog );						// Schliessen
		m_pLog = NULL;							// Dereferenzieren
	}

	// Clear the Vertices
	if( m_pVertices )
	{
		delete [] m_pVertices;                  // free memory
		m_pVertices = NULL;                     // reset pointer
	}

	// Clear the Indices
	if( m_pIndices )
	{
		delete [] m_pIndices;                   // free memory
		m_pIndices = NULL;                      // reset pointer
	}

	// Entregistrieren noch kurz unsere Windowsklasse
	UnregisterClass( m_szAppClass, m_hInst );

	// und wech
    return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Run( void )
// Info: Run the application
// -----------------------------------------------------------------
int CZFXBasis::Run( void )
{
    // Variablen init
	MSG			 msg = { 0, 0, 0 };				// Windowsnachricht
    
	// Mainloop
	while( !m_bDone )
	{
		
		// Nachrichten bearbeiten
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// Render the enviroment
		if( m_bIsActive ) Render();
	}

	// und raus
	return (int)msg.wParam;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Render( void )
// Info: Hier wird alles für den Frame bearbeitet.
// -----------------------------------------------------------------
HRESULT CZFXBasis::Render( void )
{
   // Variablen init
   ZFXMatrix  mWorld;							// Weltmatrix

   // Masterwindows setzen
   m_pDevice->UseWindow( 0 );

   // View ausrichten
   m_pDevice->SetView3D( ZFXVector( 1.0, 0.0, 0.0 ),
                         ZFXVector( 0.0, 1.0, 0.0 ),
                         ZFXVector( 0.0, 0.0, 1.0 ),
                         ZFXVector( 0.0, 0.0, 0.0 ) );

	// Weltmatrix resetten
	mWorld.Identity();
	m_pDevice->SetWorldTransform( &mWorld );

	// Den ersten Viewport auf das gesamte Fenster setzen
	m_pDevice->SetMode( EMD_PERSPECTIVE, 0 );
	m_pDevice->SetClearColor( 0.7f, 0.7f, 1.0f );
   
	// Alles mögliche für die neue Scene löschen
	m_pDevice->BeginRendering( true, true, true);

    // Do something

	// und die Buffer flippen, wir sind dann auch fertig
	m_pDevice->EndRendering();

	// Bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: HrToStr( HRESULT hr )
// Info: Gibt die Fehlermeldunge zu den Fehler IDs zurück
// -----------------------------------------------------------------
TCHAR* CZFXBasis::HrToStr( HRESULT hr )
{
	// Alle Fehler durch gehen
	switch( hr )
	{
		case ZFX_OK:				return TEXT("ZFX_OK");
		case ZFX_FAIL:				return TEXT("ZFX_FAIL");
		case ZFX_CREATEAPI:			return TEXT("ZFX_CREATEAPI");
		case ZFX_CREATEDEVICE:		return TEXT("ZFX_CREATEDEVICE");
		case ZFX_INVALIDPARAM:		return TEXT("ZFX_INVALIDPARAM");
		case ZFX_INVALIDID:			return TEXT("ZFX_INVALIDID");
		case ZFX_BUFFERSIZE:		return TEXT("ZFX_BUFFERSIZE");
		case ZFX_BUFFERLOCK:		return TEXT("ZFX_BUFFERLOCK");
		case ZFX_NOTCOMPATIBLE:		return TEXT("ZFX_NOTCOMPATIBLE");
		case ZFX_OUTOFMEMORY:		return TEXT("ZFX_OUTOFMEMORY");
		case ZFX_FILENOTFOUND:		return TEXT("ZFX_FILENOTFOUND");
		case ZFX_INVALIDFILE:		return TEXT("ZFX_INVALIDFILE");
		default:					return TEXT("unknown");
	}
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: LOG( UINT iLevel, bool bCR, PCHAR pcText, ... )
// Info: writes the line into the logfile
//
// iLevel    = (UINT)   our current loglevel
// bCR       = (bool)   Carriage Return wanted?
// pcText    =	        text to write
// ...       =          variable argumentelist
// -----------------------------------------------------------------
void CZFXBasis::LOG( UINT iLevel, bool bCR, PCHAR pcText, ... )
{
   // Variablen init
   va_list  Argumente;                                // Arguments
   char     cBuffer[4096];                            // linebuffer

   // do logging?
   if( m_byLog == 0 ) return;

   // correct loglevel?
   if( iLevel <= m_uiLogLevel )
	{
       // something to log?
       if( pcText )
		{
           // set the pointer to the beginning
           va_start( Argumente, pcText );

           // copy all data to cBuffer
           vsprintf( cBuffer, pcText, Argumente );

           // reset the list
           va_end( Argumente );
		}

        // open the logfile
        m_pLog = fopen( m_cLogFileName, "a" );

        // file opened?
        if( !m_pLog ) return;

        // write the wanted text into the logfile
        if( bCR )
            fprintf( m_pLog, "%s\n", cBuffer );
        else
            fprintf( m_pLog, "%s", cBuffer );

        // file close
        fclose( m_pLog );
	}
}
//------------------------------------------------------------------
