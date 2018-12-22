// -----------------------------------------------------------------
// zfxBasis - Header
// -----------------------------------------------------------------
// http://www.zfx.info						 c. by Oliver Düvel 2002
// -----------------------------------------------------------------


//------------------------------------------------------------------
#ifndef __ZFX_BASIS__                                   // Define
#define __ZFX_BASIS__                                   // Set
//------------------------------------------------------------------

// -----------------------------------------------------------------
// Include
//------------------------------------------------------------------
#include "ZFX.h"                                       // ZFX Header
#include "ZFXRenderer.h"                               // Renderinterface
#include "ZFXRenderDevice.h"                           // Renderdevice
// - Libraries -
#pragma comment(lib, "ZFXRenderer.lib")
#pragma comment(lib, "zfx3d.lib")
//------------------------------------------------------------------


// -----------------------------------------------------------------
// Definitionen
// -----------------------------------------------------------------
#define WIN32_MEAN_AND_LEAN
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Typendefinitionen
// -----------------------------------------------------------------
typedef class   LPCZFXBasis*  CZFXBASIS;              // Pointer
// -----------------------------------------------------------------


// -----------------------------------------------------------------
class CZFXBasis
{
// Membervariablen -------------------------------------------------
public:

protected:
 HWND              m_hWnd;                             // Windowshandle
 HINSTANCE         m_hInst;                            // Instanz
 TCHAR             m_szAppClass[16];                   // Title
 bool              m_bIsActive;                        // Activflag
 bool              m_bIsInitiated;                     // Initflag
 bool              m_bDone;                            // Doneflag
 LPZFXRENDERER     m_pRenderer;                        // Renderinterface
 LPZFXRENDERDEVICE m_pDevice;                          // Renderdevice
 CHAR              m_cAPIName[16];                     // APIName
 FILE              *m_pLog;                            // Logfile
 BYTE              m_byLog;                            // Loggen on
 char              m_cLogFileName[ 32 ];               // Logfilename
 UINT              m_uiLogLevel;                       // Loglevel
 UINT              m_uiNumVertices;                    // Num Vertices
 VERTEX            *m_pVertices;                       // Vertices
 UINT              m_uiNumIndices;                     // Num Indices
 PWORD             m_pIndices;                         // Indices

// Methoden --------------------------------------------------------
public:
 CZFXBasis();                                          // Constructor
 virtual    ~CZFXBasis();                              // Destructor

 virtual    LRESULT MsgProc( HWND, UINT, WPARAM, LPARAM );// Entry
 HRESULT    Init( void );                              // Init
 HRESULT    Create( HINSTANCE hInst );                 // Create
 HRESULT    Destroy( void );                           // Deinit
 int        Run( void );                               // Update
 
 virtual HRESULT Render( void );                       // Render
 virtual HRESULT OnInit( void ) {  return S_OK; };     // Init
 virtual HRESULT OnDestroy( void ) { return S_OK; };   // Deinit
 virtual HRESULT OnKeyDown( WPARAM wParam ) { return S_OK; }; // KeyDown
 
 void       LOG( UINT iLevel, bool bCR, PCHAR pcText, ... );// Log

protected:
 TCHAR*     HrToStr( HRESULT hr );                     // FehlerMsg

};
// -----------------------------------------------------------------
#endif