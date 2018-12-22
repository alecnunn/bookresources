// -----------------------------------------------------------------
// zfxModel
// -----------------------------------------------------------------
// http://www.zfx.info                       c. by Oliver Düvel 2003
// -----------------------------------------------------------------

// -----------------------------------------------------------------
#ifndef __ZFX_MODEL_STRUCT__                         // Define found?
#define __ZFX_MODEL_STRUCT__                         // no, then set it.
// -----------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "zfx3d.h"									 // Matheklasse
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------
#define	V1_HEADER             0x0100                 // Header
#define	V1_VERTEX             0x0200                 // Vertices
#define	V1_FACE               0x0300                 // Faces
#define	V1_MESH               0x0400                 // Meshs
#define	V1_MATERIAL           0x0500                 // Material
#define V1_JOINT              0x0600                 // Joints
#define V1_JOINT_MAIN         0x0610                 // Joints Main
#define V1_JOINT_KEYFRAME_ROT 0x0620                 // Keyframe rotation
#define V1_JOINT_KEYFRAME_POS 0x0630                 // Keyframe position
#define V1_ANIMATION          0x0700                 // Animation
#define V1_END                0x9999                 // Endchunk
#define	MAX( a, b ) (( a < b ) ? (b) : (a))          // Maxwert finden
#define	MIN( a, b ) (( a < b ) ? (a) : (b))          // Minwert finden
// -----------------------------------------------------------------

// --- Chunk ---
typedef struct _CHUNK{                               // Chunk
	WORD		wIdentifier;                         // Identifier
	ULONG		ulSize;                              // Grösse
} CHUNK_S;
typedef CHUNK_S*           LPCHUNK;                  // Chunk Header

// --- Header ---
typedef struct _CHUNKHEAD{                           // Chunk Header
	UCHAR        ucIdentifier[ 4 ];                  // Identifier
	UCHAR        ucName[ 32 ];                       // Name
	UCHAR        ucAuthor[ 32 ];                     // Autor
	UCHAR        ucEmail[ 32 ];                      // Email
	UCHAR        ucType;                             // Typ
	UCHAR        ucVersion;                          // Version
	ULONG        ulNumVertices;                      // Number of Vertices
	ULONG        ulNumIndices;                       // Number of Indices
	ULONG        ulNumFaces;                         // Number of Faces
	ULONG        ulNumMeshs;                         // Number of Meshs
	UINT         uiNumMaterials;                     // Number of Materials
	UINT		 uiNumJoints;			    		 // Number of Joints
	float        fAnimationFPS;                      // FPS
	float        fCurrentTime;                       // CurrentTime
	UINT	     uiNumFrames;                        // Number of Frames
	UINT		 uiNumAnimations;                    // Number of Animations
} CHUNKHEAD_S;
typedef CHUNKHEAD_S*       LPCHUNKHEAD;              // Chunk Header

// --- Vertex---
typedef struct _VERTEX{
   float        fXYZ[ 3 ];                           // Coordinates
   float        fUV0[ 2 ];                           // UV 0 Texturescoor.
   float        fUV1[ 2 ];                           // UV 1 Texturescoor.
   ZFXVector    fNormal;                             // Normalvector
   USHORT       usReferences;                        // References
   UINT         uiBoneID_A;                          // BoneID
   float        fWeight_A;                           // Weight
   UINT         uiBoneID_B;                          // BoneID
   float        fWeight_B;                           // Weight
   BYTE         byFlags;                             // Flags
} VERTEX_3F_S;  
typedef VERTEX_3F_S*          LPVERTEX_3F;           // Vertexpointer

// --- Face ---
typedef struct _FACE{
   ULONG        ulIndices[ 3 ];                      // Indices
   ZFXVector    fNormal;                             // Normalvector
   ULONG        ulMeshID;                            // MeshID
   UINT         uiMaterialID;                        // MaterialID
   BYTE         byFlags;                             // Flags
} FACE_S;
typedef FACE_S*            LPFACE;                   // Facepointer

// --- Material ---
typedef struct _MATERIAL{
   char         cName[ 32 ];                         // Name
   float        fAmbient[ 4 ];                       // Color Ambient
   float        fDiffuse[ 4 ];                       // Color Diffuse
   float        fSpecular[ 4 ];                      // Color Specular
   float        fEmissive[ 4 ];                      // Color Emissive
   float        fSpecularPower;                      // Specular Power
   float        fTransparency;                       // Transparency
   char         cTexture_1[ 128 ];                   // Texturename
   char         cTexture_2[ 128 ];                   // Texturename
   BYTE         byFlags;                             // Flags
} MATERIAL_S;
typedef MATERIAL_S*        LPMATERIAL;               // Materialspointer

// --- Mesh ---
typedef struct _MESH{
   char         cName[ 32 ];                         // Name
   WORD         wNumFaces;                           // Number of Faces
   PWORD        pIndices;                            // Faceindex
   UINT         uiMaterialID;                        // MaterialID
   BYTE         byFlags;                             // Flags
} MESH_S;
typedef MESH_S*            LPMESH;                   // Meshpointer

// --- Keyframe_Rotation ---
typedef struct _KF_ROT{
   float        fTime;                               // Time
   ZFXVector    vRotation;                           // Rotation
} KF_ROT_S;
typedef KF_ROT_S*          LPKF_ROT;                 // Modelpointer

// --- Keyframe_Position ---
typedef struct _KF_POS{
   float        fTime;                               // Time
   ZFXVector    vPosition;                           // Position
} KF_POS_S;
typedef KF_POS_S*          LPKF_POS;                 // Modelpointer

// --- Joints ---
typedef struct _JOINT{
   char         cName[ 32 ];                         // Descriptor
   char         cParentName[ 32 ];                   // Parentdescriptor
   WORD         wParentID;                           // ParentID
   ZFXVector    vRotation;                           // Rotation
   ZFXVector    vPosition;                           // Position
   WORD			wNumKF_Rotation;                     // Number of KF_Rots
   WORD         wNumKF_Position;                     // Number of KF_Pos
   LPKF_ROT     pKF_Rotation;                        // KF Rotations
   LPKF_POS     pKF_Position;                        // Position
   bool         bAnimated;                           // Animiert
   BYTE         byFlags;                             // Flags
   ZFXMatrix    sMatrix;                             // Matrix
   ZFXMatrix    sMatrix_absolute;                    // Matrix absolute
   ZFXMatrix    sMatrix_relative;                    // Matrix relativ
} JOINT_S;
typedef JOINT_S*           LPJOINT;                  // Jointpointer

// --- Animations ---
typedef struct _ANIMATION{
   char         cName[ 64 ];                         // Bezeichnung
   float        fStartFrame;                         // Startframe
   float        fEndFrame;                           // Endframe
   bool         bActive;                             // Aktiv
} ANIMATION_S;
typedef ANIMATION_S*       LPANIMATION;              // Animationpointer

// --- Model ---
typedef struct _MODEL{
   CHUNKHEAD_S  sHead;                               // Header
   LPVERTEX_3F  pVertices;                           // Vertices
   LPFACE       pFaces;                              // Faces
   LPMESH       pMeshs;                              // Meshs
   LPMATERIAL   pMaterials;                          // Materials
   LPJOINT		pJoints;                             // Joints
   LPANIMATION  pAnimation;                          // Animations
} MODEL_S;
typedef MODEL_S*           LPMODEL;                  // Modelpointer

// -----------------------------------------------------------------
#endif
