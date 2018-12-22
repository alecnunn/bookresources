// ------------------------------------------------------------------------------------------------
// zfxModell_C
// Das ist die Basisklasse für alle Loader.
// ------------------------------------------------------------------------------------------------
// http://www.zfx.info														c. by Oliver Düvel 2002
// ------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
#ifndef __ZFX_MODELL__														// Define gesetzt?
#define __ZFX_MODELL__														// nein dann setzen!
//-------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "ZFX.h"															// return values and stuff
#include "ZFXRenderer.h"	 											// Renderdevice
#include "zfxStrukturen.h"												// ZFX Strukturen
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// QSort Prozedur
// ------------------------------------------------------------------------------------------------
typedef int (*CMPFUNC) ( const void *arg1, const void *arg2 );				// Definition f. QSORT
int			SortFacesByMaterial(  const TRI *pFace_A, const TRI *pFace_B  );// Faces sortieren
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
class zfxModell_C
{

// Membervariablen --------------------------------------------------------------------------------
protected:
	ZFXRenderDevice			*m_pRenderDevice;								// unser Renderdevice
	PUINT					m_puiBufferID;									// static buffers
	UINT					m_uiSumMaterials;								// Summe Materialien
	LPMATERIAL				m_pMaterials;									// Zeiger auf Materialien
	PUINT					m_pMaterialIndex;								// Zeiger auf Materialindex
	UINT					m_uiSumVertices;								// Anzahl Vertices
	VERTEX					*m_pVertices;									// Zeiger auf Vertices
	UINT					m_uiSumFaces;									// Anzahl der Faces
	TRI						*m_pFaces;										// Zeiger auf Faces
	UINT					m_uiSumIndices;									// Anzahl Indices
	PUINT					m_puiIndexCount;								// Indexzählarray
	PWORD					m_pwIndices;									// Zeiger Indices
	FILE					*m_pLogFile;									// unser Logfile

// Methoden ---------------------------------------------------------------------------------------
public:
	zfxModell_C();															// Default Konstruktor
	~zfxModell_C();															// Destructor
	void			Init( void );											// Initialisierung
	void			Destroy( void );										// Deinitialisierung

	HRESULT			Render( bool bStatic = true );							// Renderaufruf
	HRESULT			Prepare( void );										// Vorbereiten

};
// ------------------------------------------------------------------------------------------------
#endif
