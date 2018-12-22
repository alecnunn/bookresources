// -----------------------------------------------------------------
// zfxModel
// -----------------------------------------------------------------
// http://www.zfx.info                       c. by Oliver Düvel 2003
// -----------------------------------------------------------------


// -----------------------------------------------------------------
#ifndef __ZFX_MODEL__                                 // Define found?
#define __ZFX_MODEL__                                 // no, then set it.
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Include
// -----------------------------------------------------------------
#include <windows.h>                                  // windows
#include <stdio.h>                                    // Std. IO
#include "ZFX.h"                                      // ZFX Header
#include "zfx3d.h"                                    // ZFX AABB Include
#include "ZFXRenderer.h"                              // Renderinterface
#include "ZFXRenderDevice.h"                          // Renderdevice
#include "ZFXModelStructs.h"                          // ZFX Modell
// -----------------------------------------------------------------


// -----------------------------------------------------------------
class CZFXModel
{
public:
   UINT          m_uiLogLevel;                        // Loglevel
   bool          m_bLog;                              // Logging
   char          m_cLogFileName[ 32 ];                // Logfilename

protected:
   CVERTEX      *m_pVertices;                         // CA Vertices
   CVERTEX      *m_pVertices_Orig;                    // CA Vertices
   LPFACE        m_pFaces;                            // Faces
   LPMESH        m_pMeshs;                            // Mesh
   LPMATERIAL    m_pMaterials;                        // Materials
   LPJOINT       m_pJoints;                           // Joints
   LPANIMATION   m_pAnimations;                       // Animations
   PUINT         m_puiSkinBuffer;                     // Skinbuffer
   PVOID         *m_ppIndices;                        // Indices
   PWORD         m_pIndices;                          // Indices
   PUINT         m_puiNumIndices;                     // Number of Indices
   CHUNKHEAD_S   m_sHeader;                           // Modelheader
   PCHAR         m_pcFileName;                        // Filename
   UINT          m_uiStartVertexID;                   // Startvertex ID
   UINT          m_uiStartFaceID;                     // StartFace ID
   FILE          *m_pFile;                            // File
   UINT          m_uiCurrentAnimation;                // Animation
   float         m_fTime;                             // Time
   float         m_fStartTime;                        // Time
   float         m_fFrame;                            // current frame
   float         m_fAnimationSpeed;                   // Animationsspeed
   bool          m_bAnimationChanged;                 // Animation geändert
   bool          m_bAnimationComplete;                // Animationflag
   bool          m_bAnimationRunOnce;                 // Single animation
   bool          m_bRenderBones;                      // Renderbones
   bool          m_bRenderNormals;                    // Rendernormals
   VERTEX        m_sBBoxMin;                          // Min Vertex
   VERTEX        m_sBBoxMax;                          // Max Vertex
   ZFXAabb       m_sAabb;                             // Aabb
   ZFXRenderDevice *m_pRenderDevice;                  // Renderdevice
   CHUNK_S       m_sChunk;                            // Chunk
   
public:
   CZFXModel( const PCHAR pcName, ZFXRenderDevice *pDevice, float fSize );// Construktor
   virtual ~CZFXModel();                              // Destruktor
   void          Init( void );                        // Initialisierung
   void          Destroy( void );                     // Deinitialisierung
   bool          Execute( const PCHAR pcName, float fSize ); // PlugIn ausführen
   HRESULT       Prepare( void );                     // Prepare Data
   HRESULT       Update( float fTime );               // Updaten
   HRESULT       Render( void );                      // Render
   void          GetFileName( PCHAR pcName, PCHAR pcModelName );// Filenamen holen
   bool          IsAnimationCompleted( void );        // Animaationsdurchlauf
   void          SetAnimationSpeed( float fSpeed );   // Animationsspeed
   void          SetAnimation( UINT uiAnim );         // Animationssquenze
   void          SetAnimation( UINT uiAnim,float fSpeed ); // Animationssquenze
   void          SetAnimation( bool bSingle, UINT uiAnim );// Singleanimation
   void          SetAnimation( bool bSingle, UINT uiAnim, float fSpeed );// Singleanimation
   void          SetRenderBones( void );              // Bones
   void          SetRenderNormals( void);             // Normals
   UINT          GetAnimation( void );                // Animationssquenze
   float         GetAnimationSpeed( void );           // Animationsspeed
   ULONG         GetVertices( CVERTEX* *pVertices );  // Modellvertices holen
   ULONG         GetIndices( PWORD *pIndices );       // Indices
   VERTEX        GetBBoxMin( void );                  // Min Vertex
   VERTEX        GetBBoxMax( void );                  // Max Vertex
   ZFXAabb       GetAabb( void );                     // Get Aabb

protected:
   bool          CloseFile( bool rBool );             // File schliessen
   HRESULT       ReadHeader( void );                  // Header schreiben
   HRESULT       ReadVertices( void );                // Vertices einlesen
   HRESULT       ReadMaterials( void );               // Materialien einlesen
   HRESULT       ReadFaces( void );                   // Faces einlesen
   HRESULT       ReadMesh( void );                    // Meshs einlesen
   HRESULT       ReadJoints( void );                  // Joints einlesen
   HRESULT       ReadJoint_Main( LPJOINT pJoint );    // Joints einlesen
   HRESULT       ReadJoint_KeyFrame_Rot( LPJOINT pJoint );// KeyFrame Rotations
   HRESULT       ReadJoint_KeyFrame_Pos( LPJOINT pJoint );// KeyFrame Positions
   HRESULT       ReadAnimations( void );              // Animations einlesen
   HRESULT       RenderBones( void );                 // Render
   HRESULT       RenderNormals( void );               // Normals
   HRESULT       SetupBones( void );                  // preparing the Bones
   HRESULT       Animation( void );                   // Animation
   HRESULT       AnimationPrepare( void );            // Animation Preparing
   HRESULT       AnimationVertices( void );           // Animate the Vertices
   void          SetScaling( float fScale = 0.0f );   // Scaling
   WORD          GetNextChunk( CHUNK_S &pChunk );     // ChunkID auslesen
   HRESULT       CheckForChunks( void );              // auf Chunks prüfen
   void          LOG( UINT iLevel, bool bCR, PCHAR pcText, ... );// Log

   ZFXMatrix	 CreateRotationMatrix( ZFXVector *pVector );
};
// -----------------------------------------------------------------
#endif