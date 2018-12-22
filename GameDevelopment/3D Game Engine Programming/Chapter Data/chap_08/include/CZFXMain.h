// -----------------------------------------------------------------
// zfxMain
// -----------------------------------------------------------------
// http://www.zfx.info                       c. by Oliver Düvel 2002
// -----------------------------------------------------------------


// -----------------------------------------------------------------
#ifndef __ZFX_MAIN__                                 // Define gesetzt?
#define __ZFX_MAIN__                                 // nein dann setzen!
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// Include
// -----------------------------------------------------------------
#include "CZFXBasis.h"                               // Basisclass
#include "CZFXModel.h"                               // Model Header
// -----------------------------------------------------------------
#pragma comment( lib, "winmm.lib" )                  // Windows Multimedia
// -----------------------------------------------------------------


// -----------------------------------------------------------------
class CZFXMain : public CZFXBasis
{

// -----------------------------------------------------------------
protected:
	UINT       m_iVShaderID_A;                      // Shader A
	UINT       m_iVShaderID_B;                      // Shader B
	UINT       m_iPShaderID_A;                      // Shader A
	UINT       m_iPShaderID_B;                      // Shader B
	ZFXVector  m_sLight_1;                          // Light 1
    ZFXVector  m_sLight_2;                          // Light 2
	CZFXModel  *m_pModel;                           // Model
	UINT       m_uiFPS;					    		// FPS
	UINT       m_uiFrameCount;                      // Framecount
	ULONG      m_ulCurrentTime;						// Current Time
	ULONG      m_ulOldTime;							// Old Time
	
// Methoden --------------------------------------------------------
public:
   CZFXMain();                                      // Constructor
   HRESULT Render( void );                          // Render
 

protected:
   HRESULT OnInit( void );                          // Init
   HRESULT OnKeyDown( WPARAM wParam );              // KeyDown
};
// -----------------------------------------------------------------
#endif
