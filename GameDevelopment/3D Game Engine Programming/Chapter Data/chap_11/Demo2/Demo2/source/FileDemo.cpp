//-----------------------------------------------------------------------------
// ZFX FileDemo for the ZFXNetwork.DLL
//-----------------------------------------------------------------------------
// by Oliver Düvel mailto: Oliver.Duevel@zfx.info			date: 22.02.03
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "FileDemo.h"										// our Header
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Winmain
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, PSTR szCMD, int iCMD )
{
	// Variablen init
	MSG				iMSG;									// Messages

	// create our Dialog
	g_hWnd = CreateDialog( hInst, MAKEINTRESOURCE( IDD_ZFX_FILEDEMO ), NULL, DLGProc );

	// show the Dialog
	ShowWindow( g_hWnd, iCMD );

	// temp IP
	SetDlgItemText( g_hWnd, IDC_IPADRESS, "192.168.100.42" );

	// Message Pump
	while( GetMessage( &iMSG, NULL, 0, 0 ) )
	{
		TranslateMessage( &iMSG );
		DispatchMessage( &iMSG );
	}

	// bye
	return (int)iMSG.wParam;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Callback for the Dialog
//-----------------------------------------------------------------------------
BOOL CALLBACK DLGProc( HWND hWnd, UINT iMSG, WPARAM wParam, LPARAM lParam )
{
	// Variablen init

	// Messages for us?
	switch( iMSG )
	{
	// Init us?
	case WM_INITDIALOG:					Init();					break;

	// destroy us?
	case WM_DESTROY:		Destroy();	PostQuitMessage( 0 );	return 0;

	// any commands for us?
	case WM_COMMAND:
		switch( LOWORD( wParam ))
		{
			// quit our session?
			case IDCANCEL:	Destroy();	PostQuitMessage( 0 );	return 0;

			// send a file?
			case IDC_SENDFILE:			FileSend();				break;

			// receive a file?
			case IDC_RECEIVEFILE:		FileReceive();			break;

			// start as Server?
			case IDC_BESERVER:

				// Start as Server
				g_nmd	= NMD_SERVER;

				// set the ID
				g_iID	= 1;

				// Start network
				NetworkInit();

				// bye
				break;

		} // WM_COMMAND

	// Network activity
	case WM_ZFXCLIENT:
	case WM_ZFXSERVER:

		// update Network.DLL
		g_pDevice->MsgProc( wParam, lParam );

		// Update Demo
		NetworkReceive();
		
		// break
		break;

	} // Messages

	// bye
	return FALSE;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Init the Dialog
//-----------------------------------------------------------------------------
HRESULT Init( void )
{
	// Settings for the Filedialog
	g_ofn.lStructSize		= sizeof( OPENFILENAME );
	g_ofn.hwndOwner			= g_hWnd;
	g_ofn.lpstrTitle		= g_szAppName;
	g_ofn.hInstance			= g_hInst;
	g_ofn.lpstrFilter		= TEXT( "All Files (*.*)\0*.*\0\0");
	g_ofn.lpstrCustomFilter	= NULL;
	g_ofn.nMaxCustFilter	= 0;
	g_ofn.nFilterIndex		= 0;
	g_ofn.nMaxFile			= MAX_PATH;
	g_ofn.nMaxFileTitle		= MAX_PATH;
	g_ofn.lpstrInitialDir	= NULL;
	g_ofn.nFileOffset		= 0;
	g_ofn.nFileExtension	= 0;
	g_ofn.lpstrDefExt		= TEXT( "*.*" );
	g_ofn.lCustData			= 0L;
	g_ofn.lpfnHook			= NULL;
	g_ofn.lpTemplateName	= NULL;

	// run the ZFXNetworkdevice
	g_pNetwork	= new ZFXNetwork( g_hInst );
	
	// Check
	if( FAILED( g_pNetwork->CreateDevice() ) )		return E_FAIL;

	// get a pointer on that device
	g_pDevice	= g_pNetwork->GetDevice();

	// Check
	if( g_pDevice == NULL )							return E_FAIL;

	// default package
	g_sData.nSender	= 0;
	g_sData.nType	= 0;
	g_sData.pData	= new BYTE[ MAX_PACKAGE_SIZE ];
	ZeroMemory( g_sData.pData, MAX_PACKAGE_SIZE );

	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Destroy the Dialog
//-----------------------------------------------------------------------------
HRESULT Destroy( void )
{
	// the ZFXNetworkdevice
	if( g_pNetwork )
	{
		delete g_pNetwork;								// delete it
		g_pNetwork	= NULL;								// reset pointer
		g_pDevice	= NULL;								// reset pointer
	}

	// cleanup the package
	if( g_sData.pData )
	{
		delete [] g_sData.pData;						// delete it
		g_sData.pData = NULL;							// reset pointer
	}
	g_sData.nType	= 0;								// Filepackage
	g_sData.nLength	= 0;								// Filelength

	// Shutdownflag
	g_bDone = true;

	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Start the Network
//-----------------------------------------------------------------------------
HRESULT NetworkInit( void )
{
	// Network online?
	if( !g_bNetOK )
	{
		// Setup the Network

		// Start as client?
		if( g_nmd == NMD_CLIENT )
		{
			// Get the IP-Adress
			GetWindowText( GetDlgItem( g_hWnd, IDC_IPADRESS ), g_cIPADRESS, 16 );
		} else {

			// Show state
			SetDlgItemText( g_hWnd, IDC_IPADRESS, " ...waiting for files" );
		}

		// start the device
		if( FAILED( g_pDevice->Init( g_hWnd, g_nmd, g_iPort, g_cIPADRESS, MAX_PACKAGE_SIZE, true ) ) )
		{
			// Sorry no network
			MessageBox( g_hWnd, "Network init failed", "Systemrequest", MB_ICONERROR );

			// Bye
			return E_FAIL;
		}

		// Init done
		g_bNetOK = true;

		// Disable the Startserverbutton
		EnableWindow( GetDlgItem( g_hWnd, IDC_BESERVER ), false );
		
		// Disable the IP-Adressfield
		EnableWindow( GetDlgItem( g_hWnd, IDC_IPADRESS ), false );
	}

	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Receive date from the Network
//-----------------------------------------------------------------------------
HRESULT NetworkReceive( void )
{
	// Check
	if( !g_bNetOK )									return E_FAIL;

	// waiting
	while( g_pDevice->IsPkgWaiting() )
	{
		// Paket correct?
		if( FAILED( g_pDevice->GetNextPkg( &g_sData ) ) )
		{
			// MB
			MessageBox( g_hWnd, "GetNextPkg() failed", "Systemrequest", MB_ICONERROR );
			
			// bye
			return E_FAIL;
		} else {

			// receiving our Data?
			if( (g_sData.nType == 0) && ( g_iID != g_sData.nSender ) )
			{
				// Show that we can save a file
				EnableWindow( GetDlgItem( g_hWnd, IDC_RECEIVEFILE ), true );
			} // Package done
		} // receiving package
	} // waiting for package

	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Send a File
//-----------------------------------------------------------------------------
HRESULT FileSend( void )
{
	// Variablen init
	FILE*	pFile	= NULL;								// File
			
    // Settings for the Filedialog
	g_ofn.Flags				= OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	g_ofn.lpstrTitle		= TEXT("Send a File");
	g_ofn.lpstrFile			= (PTSTR)&g_szFile;

	// Open the Filedialog
    if( !GetOpenFileName( &g_ofn ) )				return S_OK;

	// Network online?
	if( !g_bNetOK )
	{
		// Start the Network
		if( FAILED( NetworkInit() ) )				return E_FAIL;
	}

	// now we got the wanted file
	pFile = fopen( g_szFile, "rb" );

	// check
	if( pFile == NULL )		return E_FAIL;

	// Filesize
	g_sData.nLength = _filelength( _fileno( pFile ) );

	// correct Filesize?
	if( g_sData.nLength > MAX_PACKAGE_SIZE )
	{
		// close the file
		fclose( pFile );

		// Sorry nothing send
		MessageBox( g_hWnd, "Unexpected filesize", "Systemrequest", MB_ICONERROR );

		// bye
		return E_FAIL;
	}
	
	// clear buffer
	ZeroMemory( g_sData.pData, MAX_PACKAGE_SIZE );
	
	// read the file into the Databuffer
	fread( g_sData.pData, sizeof(BYTE), g_sData.nLength, pFile );

	// close the file
	fclose( pFile );

	// sending the file over the network ---
	
	// Set ID
	g_sData.nSender	= g_iID;
	g_sData.nType	= 0;

	// client or server?
	if( g_nmd == NMD_CLIENT )
	{
		// sending our package
		if( FAILED( g_pDevice->SendToServer( &g_sData ) ) )
		{
			// Sorry nothing send
			MessageBox( g_hWnd, "Sending data failed", "Systemrequest", MB_ICONERROR );

			// bye
			return E_FAIL;
		}
	} else {
		// sending our package
		if( FAILED( g_pDevice->SendToClients( &g_sData ) ) )
		{
			// Sorry nothing send
			MessageBox( g_hWnd, "Sending data failed", "Systemrequest", MB_ICONERROR );

			// bye
			return E_FAIL;
		}
	}
	
	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Receive a File
//-----------------------------------------------------------------------------
HRESULT FileReceive( void )
{
	// Variablen init
	FILE*	pFile	= NULL;								// File

	// Settings for the Filedialog
	g_ofn.Flags				= OFN_OVERWRITEPROMPT;
	g_ofn.lpstrTitle		= TEXT("Receive a File");
	g_ofn.lpstrFile			= (PTSTR)&g_szFile;

	// Check for data
	if( g_sData.nLength == 0 )						return S_OK;

	// Open the Filedialog
    if( !GetOpenFileName( &g_ofn ) )				return S_OK;

	// now we got the wanted file
	pFile = fopen( g_szFile, "wb" );
    
	// check
	if( pFile == NULL )		return E_FAIL;

	// read the file into the Databuffer
	fwrite( g_sData.pData, sizeof(BYTE), g_sData.nLength, pFile );

	// close the file
	fclose( pFile );

	// Show that we have saved a file
	EnableWindow( GetDlgItem( g_hWnd, IDC_RECEIVEFILE ), false );

	// bye
	return S_OK;
}
//-----------------------------------------------------------------------------