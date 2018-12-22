// ------------------------------------------------------------------------------------------------
// zfxModellCBF
// ------------------------------------------------------------------------------------------------
// http://www.zfx.info														c. by Oliver Düvel 2002
// ------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
#ifndef __ZFX_MODELL_CBF__													// Define gesetzt?
#define __ZFX_MODELL_CBF__													// nein dann setzen!
//-------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include <stdio.h>															// Std. IO
#include "zfxModell.h"														// ZFX Modell
// ------------------------------------------------------------------------------------------------

#define	V1_HEADER					0x0100									// Header
// --- Vertices ---
#define	V1_VERTEX					0x0200									// Vertices
#define V1_VERTEX_XYZ				0x0201									// Vertexkoordinaten
#define V1_VERTEX_UV0				0x0202									// UV 0 Koordinaten
#define V1_VERTEX_UV1				0x0203									// UV 1 Koordinaten
#define V1_VERTEX_UV2				0x0204									// UV 2 Koordinaten
#define V1_VERTEX_UV3				0x0205									// UV 3 Koordinaten
#define V1_VERTEX_NORMAL			0x0206									// Normalvektor
#define V1_VERTEX_FLAG				0x0207									// Flags
#define V1_VERTEX_BONEID			0x0208									// Jointnummer
// --- Materials ---
#define	V1_MATERIAL					0x0300									// Material
#define	V1_MATERIAL_NAME			0x0301									// Materialname
#define V1_MATERIAL_AMBIENT			0x0302									// Farbe Ambient
#define V1_MATERIAL_DIFFUSE			0x0303									// Farbe Diffuse
#define V1_MATERIAL_SPECULAR		0x0304									// Farbe Spekular
#define V1_MATERIAL_SPECULAR_POWER	0x0305									// Farbe Spekularstärke
#define V1_MATERIAL_EMISSIVE		0x0306									// Farbe Emessive
#define V1_MATERIAL_TEXTURE			0x0307									// Texturname
#define V1_MATERIAL_TRANSPARENCY	0x0308									// Transparenz
#define	V1_MATERIAL_FLAG			0x0309									// Flags
// --- Faces ---
#define	V1_FACE						0x0400									// Faces
#define V1_FACE_TYPE				0x0401									// Typ Tri/Poly
#define	V1_FACE_MATERIALID			0x0402									// Material ID
#define	V1_FACE_INDEX				0x0403									// Index
#define	V1_FACE_INDEXCOUNT			0x0404									// Indexcount
#define	V1_FACE_FLAG				0x0405									// Flags
// --- Mesh ---
#define	V1_MESH						0x0500									// Meshs
#define	V1_MESH_TYPE				0x0501									// Typ
#define	V1_MESH_INDEXCOUNT			0x0502									// Indexcount
#define	V1_MESH_INDEX				0x0503									// Index
#define	V1_MESH_FLAG				0x0504									// Flags
#define	V1_MESH_NAME				0x0505									// Bezeichnung
// --- Nodes ---
#define	V1_NODE						0x0600									// Nodes
#define	V1_NODE_TYPE				0x0601									// Typ
#define	V1_NODE_INDEXCOUNT			0x0602									// Indexcount
#define	V1_NODE_INDEX				0x0603									// Index
#define V1_NODE_INDEXVERTEXOFFSET	0x0604									// Index Vertexoffset
#define V1_NODE_INDEXFACEOFFSET		0x0605									// Index Faceoffset
#define	V1_NODE_FLAG				0x0606									// Flags
#define	V1_NODE_NAME				0x0607									// Bezeichnung
#define V1_NODE_PARENT				0x0608									// Eltern ID
#define V1_NODE_CHILD				0x0609									// Kind ID
#define V1_NODE_NEXT				0x060A									// Nachfolger ID
#define V1_NODE_PREV				0x060B									// Vorgänger ID
// --- Sonstige --
#define V1_END						0x9999									// Endmarke

// - Vorspulen -
#define FW_BYTE						sizeof( BYTE )							// Byte
#define FW_CHAR						sizeof( CHAR )							// Char
#define FW_SHORT					sizeof( USHORT )						// Short Integer
#define FW_INT						sizeof( UINT )							// Integer
#define FW_LONG						sizeof( ULONG )							// Long
#define FW_FLOAT					sizeof( FLOAT )							// Float
#define FW_WORD						sizeof( WORD )							// Word
#define FW_DWORD					sizeof( DWORD )							// DWORD
#define FW_DOUBLE					sizeof( DOUBLE )						// DOUBLE
#define FW_VECTOR3					3 * sizeof( FLOAT )						// Vector 3
#define FW_VECTOR4					4 * sizeof( FLOAT )						// Vector 4


// ------------------------------------------------------------------------------------------------
class zfxModellCBF_C : public zfxModell_C
{
// Strukturen der Chunks --------------------------------------------------------------------------
protected:
	typedef struct _CHUNKHEAD{												// Chunk Header
		UCHAR		ucIdentifier[ 4 ];										// Identifier
		UCHAR		ucAuthor[ 32 ];											// Autor
		UCHAR		ucEmail[ 32 ];											// Email
		UCHAR		ucType;													// Typ
		ULONG		ulSumVertices;											// Anzahl Vertices
		ULONG		ulSumFaces;												// Anzahl Faces
		ULONG		ulSumMeshs;												// Anzahl Meshs
		ULONG		ulSumNodes;												// Anzahl Nodes
	} CHUNKHEAD_S;
	typedef CHUNKHEAD_S*			LPCHUNKHEAD;							// Chunk Header

	UINT			m_uiStartVertexID;										// Startvertex ID
	UINT			m_uiStartFaceID;										// StartFace ID
	char			m_cTexturePath[ 32 ];									// Texturpfad
	bool			m_bRightHandedSystem;									// Koordinatensystem

protected:
	FILE			*m_pFile;												// File
	PCHAR			m_pcFileBuffer;											// Filebuffer

public:
	zfxModellCBF_C ( const PCHAR pcName, ZFXRenderDevice *pDevice, FILE *pLog ); // Konstruktor
    virtual ~zfxModellCBF_C ();												// Destruktor
	void			Init( void );											// Initialisierung
	void			Destroy( void );										// Deinitialisierung
	bool			Execute( const PCHAR pcName );							// PlugIn ausführen
	void			GetFileName( PCHAR pcName, PCHAR pcModelName );			// Filenamen holen

protected:
	bool			CloseFile( bool rBool );								// File schliessen
	void			ReadString( PCHAR pString, UINT uiSize );				// String einlesen
	void			ReadFloat( PFLOAT pFloat );								// Float einlesen
	void			ReadULONG( PULONG pULong );								// ULONG einlesen
	void			ReadUINT( PUINT pUInteger );							// UINT einlesen
	void			ReadUSHORT( PUSHORT pUShort );							// USHORT einlesen
	void			ReadBYTE( PBYTE pByte );								// Byte einlese
	void			ReadColor( LPRGBA pColor );								// Farbe einlesen
	void			Forward( UINT uiSize );									// Vorspulen
	HRESULT			ReadHeader( void );										// Header schreiben
	
	// --- Vertices ---
	HRESULT			ReadVertices( void );									// Vertices einlesen
	HRESULT			ReadVertexXYZ( VERTEX *pVertex );						// Vertexkoordinaten einlesen
	HRESULT			ReadVertexUV( LPVERTEX2F pVertex2F );					// UV Mapping einlesen
	HRESULT			ReadVertexNormal( VERTEX *pVertex );					// Normalvektor einlesen
	
	// --- Materials ---
	HRESULT			ReadMaterials( void );									// Materialien einlesen
	
	// --- Faces ---
	HRESULT			ReadFaces( void );										// Faces einlesen
	HRESULT			ReadFaceIndex( TRI *pFace );							// Indices
	
	// --- Chunks ---
	WORD			GetNextChunk( void );									// ChunkID auslesen
	HRESULT			CheckForChunks( void );									// auf Chunks prüfen

};
// ------------------------------------------------------------------------------------------------
#endif