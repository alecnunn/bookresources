//-----------------------------------------------------------------------------
// ZFX FileDemo for the ZFXNetwork.DLL
//-----------------------------------------------------------------------------
// by Oliver Düvel mailto: Oliver.Duevel@zfx.info			date: 22.02.03
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#ifndef MAIN_H
#define MAIN_H
#define WIN32_MEAN_AND_LEAN
#define	MAX_PACKAGE_SIZE		65536//32768
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <windows.h>										// Windows stuff
#include <io.h>												// IO stuff
#include "ZFXNetwork.h"										// Network interface
#include "resource.h"										// dialogbox resource
// ---
#pragma comment( lib, "ZFXNetwork.lib" )					// Network interface
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Variablen
//-----------------------------------------------------------------------------
// windows stuff
HWND				g_hWnd				= NULL;				// Windowshandle
HINSTANCE			g_hInst				= NULL;				// Windowsinstance
TCHAR				g_szAppName[]		= TEXT("ZFXFiletransfer"); // Name
TCHAR				g_szFile[ MAX_PATH ]= { 0 };			// our File
OPENFILENAME		g_ofn;									// Filedialog

// application stuff
BOOL				g_bIsActive			= FALSE;			// Activeflag
bool				g_bDone				= false;			// Shutdownflag

// input objects
LPZFXNETWORK		g_pNetwork			= NULL;				// Network interface
LPZFXNETWORKDEVICE	g_pDevice			= NULL;				// Networkdevice
ZFXNETMODE			g_nmd				= NMD_CLIENT;		// Networkmode
CHAR				g_cIPADRESS[ 16 ]	= { 0 };			// IP-Adress
int					g_iPort				= 10042;			// Port
BOOL				g_bNetOK			= FALSE;			// Network OK
ZFXPACKAGE			g_sData;								// our Package
int					g_iID				= 0;				// our ID

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
HRESULT				Init( void );							// Init
HRESULT				Destroy( void );						// Cleanup
BOOL CALLBACK		DLGProc( HWND,UINT,WPARAM,LPARAM );		// Message Proc
HRESULT				FileSend( void );						// Filesend
HRESULT				FileReceive( void );					// Filereceiving
HRESULT				NetworkInit( void );					// Run the Network
HRESULT				NetworkReceive( void );					// Data for us
//-----------------------------------------------------------------------------
#endif