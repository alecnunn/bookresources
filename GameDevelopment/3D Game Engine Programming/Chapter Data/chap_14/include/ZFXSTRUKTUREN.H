// ------------------------------------------------------------------------------------------------
// zfxStrukturen
// ------------------------------------------------------------------------------------------------
// http://www.zfx.info								   c. by Oliver Düvel and Stefan Zerbst in 2002
// ------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
#ifndef __ZFX_STRUKTUREN__													// Define gesetzt?
#define __ZFX_STRUKTUREN__													// nein dann setzen!
//-------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Include
// ------------------------------------------------------------------------------------------------
#include "zfx3d.h"															// ZFX 3D lib
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// Definitionen
// ------------------------------------------------------------------------------------------------
typedef	struct	_IDENT								IDENT_S;				// Identifier
typedef struct	_RGB								RGB_S;					// Farbe RGB
typedef struct	_RGBA								RGBA_S;					// Farbe RGBA
typedef	struct	_VERTEX2I							VERTEX2I_S;				// Vertex (U,V)
typedef	struct	_VERTEX2F							VERTEX2F_S;				// Vertex (U,V)
typedef	struct	_VERTEX3I							VERTEX3I_S;				// Vertex (X,Y,Z)
typedef	struct	_VERTEX3F							VERTEX3F_S;				// Vertex (X,Y,Z)
typedef	struct	_VERTEX								VERTEX_S;				// Vertex
typedef	struct  _MATERIAL							MATERIAL_S;				// Material
typedef	struct	_FACE								FACE_S;					// Face
typedef	struct _MESH								MESH_S;					// Mesh
typedef	struct _NODE								NODE_S;					// Node
// ------------------------------------------------------------------------------------------------


typedef struct TRI_TYPE {
   WORD i0, i1, i2;  // original indices
   WORD n0, n1, n2;  // new indices
   UINT nMat;
   } TRI;

// ------------------------------------------------------------------------------------------------
// Zeiger und Enumeration
// ------------------------------------------------------------------------------------------------

// Zeiger -----------------------------------------------------------------------------------------
typedef IDENT_S*				LPIDENT;									// Identifier
typedef	RGB_S*					LPRGB;										// Farbe RGB
typedef	RGBA_S*					LPRGBA;										// Farbe RGBA
typedef	VERTEX2I_S*				LPVERTEX2I;									// Vertex (U,V)
typedef	VERTEX2F_S*				LPVERTEX2F;									// Vertex (U,V)
typedef	VERTEX3I_S*				LPVERTEX3I;									// Vertex (X,Y,Z)
typedef	VERTEX3F_S*				LPVERTEX3F;									// Vertex (X,Y,Z)
typedef	VERTEX_S*				LPVERTEX;									// Vertexstruktur
typedef	MATERIAL_S*				LPMATERIAL;									// Materialstruktur
typedef FACE_S*					LPFACE;										// Facestruktur
typedef	MESH_S*					LPMESH;										// Meshstruktur
typedef NODE_S*					LPNODE;										// Nodestruktur

// Enumeration ------------------------------------------------------------------------------------
enum{
	FT_TRIANGLE					= 0,										// Triangle
	FT_POLYGON,																// Polygon
	FT_PATCH,																// Patch
	FT_MESH,																// Mesch
	FT_BILLBOARD															// Billboard	
};

#define	ZFXCURRENT				-1											// Aktuell
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Strukturen 
// ------------------------------------------------------------------------------------------------

// Ident -
struct _IDENT{
	USHORT			usType;													// Typ für die Selektion
};

// RGB -
struct _RGB{
	union
	{
		struct{
			float	fR;														// Rotanteil
			float	fG;														// Grünanteil
			float	fB;														// Blauanteil
		};
		float fV[3];														// RGB Values
	};
};

// RGBA -
struct _RGBA{
	union
	{
		struct{
			float	fR;														// Rotanteil
			float	fG;														// Grünanteil
			float	fB;														// Blauanteil
			float	fA;														// Alphaanteil
		};
		float fV[4];														// RGBA Values
	};
};

// Vertex 2i -
struct _VERTEX2I{ int iU, iV; };											// Koordinaten U und V

// Vertex 2f -
struct _VERTEX2F{ float fU, fV; };											// Koordinaten U und V

// Vertex 3i -
struct _VERTEX3I{ int iX, iY, iZ; };										// Koordinaten X, Y und Z

// Vertex 3f -
struct _VERTEX3F{ float fX, fY, fZ; };										// Koordinaten X, Y und Z

// Vertex -
struct _VERTEX{
	USHORT			usType;													// Typ für die Selektion
	// Koordinaten ------- X ------- Y - ----- Z --/------- XYZ -----------------------------------
	union{ struct{	float fX; float fY; float fZ; }; float fXYZ[ 3 ]; };	// Koordinaten

	// Texturkoodrinaten - U - V - ----------------------------------------------------------------
	union{	struct{	float fU0; float fV0; };								// Koordinaten UV
			float fUV0[ 2 ]; };												// Koordinaten array

	union{	struct{	float fU1; float fV1; };								// Koordinaten UV
			float fUV1[ 2 ]; };												// Koordinaten array

	union{	struct{	float fU2; float fV2; };								// Koordinaten UV
			float fUV2[ 2 ]; };												// Koordinaten array

	union{	struct{	float fU3; float fV3; };								// Koordinaten UV
			float fUV3[ 2 ]; };												// Koordinaten array
	
	// Farbe uns Sonstiges														
	RGBA_S			sRGBA;													// Farbe
	ZFXVector		sNormal;												// Normalvektor
	BYTE			byFlags;												// Flags
	BYTE			byBoneID;												// Bone Identifier
	BYTE			byREFCount;												// Referenzzähler
	UINT			uiID;													// Identifier
};

// Material -
struct _MATERIAL{
	USHORT			usType;													// Typ für die Selektion
	char			cName[ 32 ];											// Bezeichnung
	RGBA_S			sAmbient;												// Farbe Ambient
	RGBA_S			sDiffuse;												// Farbe Diffuse
	RGBA_S			sEmessive;												// Farbe Emessive
	RGBA_S			sSpecular;												// Farbe Spekular
	float			fSpecularPower;											// Spekularstärke
	float			fTransparency;											// Transparenz
	char			cTexture[ 32 ];											// Texturname
	BYTE			byFlags;												// Flags
	UINT			uiID;													// Identifier
};

// Face -
struct _FACE{
	USHORT			usType;													// Typ für die Selektion
	UINT			uiFaceType;												// Type 0=Tri 1=polygon, 2=patch, 3=mesh, 4=billboard
	UINT			uiSumVertices;											// Anzahl der Vertices
	PUINT			puiIndexVertices;										// Index der Vertices
	UINT			uiIndexVertexOffset;									// Starindex des ersten Vertices
	ZFXVector		vNormal;												// Normalvektor des Faces
	UINT			uiGroupID;												// Gruppen Index
	RGBA_S			sRGBA;													// Farbe
	UINT			uiNummer_Material;										// Materialnummer
	BYTE			byFlags;												// Flags
	UINT			uiParent;												// Elternteil
	UINT			uiID;													// Identifier
};

// Mesh -
struct _MESH{
	USHORT			usType;													// Typ für die Selektion
	char			cName[ 32 ];											// Bezeichnung
	UINT			uiSumFaces;												// Anzahl der Faces
	PUINT			puiIndexFace;											// Index zu den Faces
	UINT			uiFaceID;												// Face das hinzugefügt wird
	BYTE			byFlags;												// Flags
	RGBA_S			sRGBA;													// Farbe
	UINT			uiMaterialIndex;										// Material index
	UINT			uiParent;												// Elternteil
	UINT			uiID;													// Identifier
};

// Node -
struct _NODE{
	USHORT			usType;													// Typ für die Selektion
	char			cName[ 32 ];											// Bezeichnung
	UINT			uiSumMeshs;												// Anzahl der Meshes
	PUINT			puiIndexMesh;											// Index zu den Meshes
	UINT			uiIndexVertexOffset;									// Start der Vertices
	UINT			uiIndexFaceOffset;										// Start der Faces
	UINT			uiMeshID;												// Mesh das hinzugefügt wird
	BYTE			byFlags;												// Flags
	RGBA_S			sRGBA;													// Farbe
	UINT			uiNodeParent;											// Elternnode
	UINT			uiNodeChild;											// Kindnode
	UINT			uiNodeNext;												// Nachfolger
	UINT			uiNodePrev;												// Vorgänger
	UINT			uiID;													// Identifier
};
// ------------------------------------------------------------------------------------------------
#endif
