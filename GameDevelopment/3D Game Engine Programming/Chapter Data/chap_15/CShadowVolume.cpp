// FILE: CShadowVolume.cpp

#include "include/CShadowVolume.h"
#include "include/zfx.h"


// constructor
CShadowVolume::CShadowVolume(void) {
   m_pDevice    = NULL;
   m_pVerts     = NULL;
   m_pIndis     = NULL;
   m_pTris      = NULL;
   m_pVolume    = NULL;
   m_pAdjacency = NULL;
   m_pIsCap     = NULL;
   m_pEdges     = NULL;
   m_NumV       = 0;
   m_NumI       = 0;
   m_NumT       = 0;
   m_NumE       = 0;
   m_NumVV      = 0;
   m_nSize      = 0;
   } // constructor
/*----------------------------------------------------------------*/
 

// destructor
CShadowVolume::~CShadowVolume(void) {
   SAFE_DELETE_A(m_pTris);
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pIndis);
   SAFE_DELETE_A(m_pIsCap);
   SAFE_DELETE_A(m_pVolume);
   SAFE_DELETE_A(m_pAdjacency);
   SAFE_DELETE_A(m_pEdges);
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Create the mesh for this object from vertex and index list.
 */
HRESULT CShadowVolume::Init(const void *pVerts, UINT NumV,
                            const WORD *pIndis, UINT NumI,
                            UINT nSize, ZFXAabb aabb,
                            ZFXRenderDevice *pDevice) {
   m_pDevice = pDevice;
   m_Aabb    = aabb;
   m_nSize   = nSize;

   // better save than sorry
   if (!pVerts || !pIndis || (NumV==0) || (NumI==0))
      return ZFX_FAIL;

   m_pVerts = new BYTE[NumV*nSize];
   m_pIndis = new WORD[NumI];
   m_pTris  = new TRIANGLE[NumI/3];
   m_pIsCap = new bool[NumI/3];
   m_NumV   = NumV;
   m_NumI   = NumI;
   m_NumT   = NumI/3;

   m_pEdges = new WORD[m_NumT*6];

   // in a closed mesh each triangle has 
   // exactly three neighboring triangles
   m_pAdjacency = new int[m_NumT*3];

   // the shadow volume has in worst case
   // a quad of 4 vertices for each edge
   // of all triangles: NumT*3*4
   m_pVolume = new PVERTEX[m_NumT*12];

   // copy original mesh data
   memcpy(m_pVerts, pVerts, m_nSize*NumV);
   memcpy(m_pIndis, pIndis, sizeof(WORD) * NumI);

   int nFact = m_nSize/sizeof(BYTE);

   // generate tri structures
   for (UINT i=0; i<m_NumT; i++) {
      m_pTris[i].pV0 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+0] * nFact ];
      m_pTris[i].pV1 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+1] * nFact ];
      m_pTris[i].pV2 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+2] * nFact ];
  
      ZFXVector vc0( m_pTris[i].pV0->x,
                     m_pTris[i].pV0->y,
                     m_pTris[i].pV0->z);

      ZFXVector vc1( m_pTris[i].pV1->x,
                     m_pTris[i].pV1->y,
                     m_pTris[i].pV1->z);

      ZFXVector vc2( m_pTris[i].pV2->x,
                     m_pTris[i].pV2->y,
                     m_pTris[i].pV2->z);

      // precalculate normal
      m_pTris[i].vcN.Cross( (vc2 - vc1),
                            (vc1 - vc0));
      m_pTris[i].vcN.Normalize();
      }

   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Change vertex and index lists which must be the same size as
 * the original list supplied to Init() call of this instance.
 */
HRESULT CShadowVolume::UpdateGeometry(const void *pVerts,
                                      const WORD *pIndis,
                                      ZFXAabb aabb) {

   if ( !m_pVerts || !m_pIndis) return ZFX_FAIL;

   memcpy(m_pVerts, pVerts, m_nSize*m_NumV);
   memcpy(m_pIndis, pIndis, sizeof(WORD)*m_NumI);

   int nFact = m_nSize/sizeof(BYTE);

   // generate tri structures
   for (UINT i=0; i<m_NumT; i++) {
      m_pTris[i].pV0 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+0] * nFact ];
      m_pTris[i].pV1 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+1] * nFact ];
      m_pTris[i].pV2 = (VERTEX*)&m_pVerts[ m_pIndis[i*3+2] * nFact ];
  
      ZFXVector vc0( m_pTris[i].pV0->x,
                     m_pTris[i].pV0->y,
                     m_pTris[i].pV0->z);

      ZFXVector vc1( m_pTris[i].pV1->x,
                     m_pTris[i].pV1->y,
                     m_pTris[i].pV1->z);

      ZFXVector vc2( m_pTris[i].pV2->x,
                     m_pTris[i].pV2->y,
                     m_pTris[i].pV2->z);

      // precalculate normal
      m_pTris[i].vcN.Cross( (vc2 - vc1),
                            (vc1 - vc0));
      m_pTris[i].vcN.Normalize();
      }


   return ZFX_OK;
   } // UpdateGeometry
/*----------------------------------------------------------------*/


// just a little helper
inline PVERTEX CShadowVolume::Extrude(const VERTEX *pv, 
                                      const ZFXVector &vc) {
   PVERTEX v;
   v.x = pv->x - vc.x;
   v.y = pv->y - vc.y;
   v.z = pv->z - vc.z;
   return v;
   } // extrude
/*----------------------------------------------------------------*/


/**
 * This method builds the actual shadow volume for a given light
 */
void CShadowVolume::BuildShadowVolume(ZFXVector vcLight) {
   ZFXVector vc;
   PVERTEX v1, v2, v3, v4;
   
   vc = vcLight * 5.0f;

   // reset counters
   m_NumVV = 0;
   m_NumE  = 0;

   // 1st pass: determine which tris make up the
   //           front and back caps of the volume
   for (UINT i=0; i<m_NumT; i++) {

      WORD wFace0 = m_pIndis[3*i+0];
      WORD wFace1 = m_pIndis[3*i+1];
      WORD wFace2 = m_pIndis[3*i+2];

      if ( (m_pTris[i].vcN * vcLight) >= 0.0f ) {

         AddEdge(wFace0, wFace1 );
         AddEdge(wFace1, wFace2 );
         AddEdge(wFace2, wFace0 );

         memcpy(&v1, m_pTris[i].pV0, sizeof(PVERTEX));
         memcpy(&v2, m_pTris[i].pV2, sizeof(PVERTEX));
         memcpy(&v3, m_pTris[i].pV1, sizeof(PVERTEX));

         // backfacing tris form front cap
         m_pVolume[m_NumVV++] = v1;
         m_pVolume[m_NumVV++] = v2;
         m_pVolume[m_NumVV++] = v3;

         // back cap is extruded to infinity
         m_pVolume[m_NumVV++] = Extrude(m_pTris[i].pV0, vc);
         m_pVolume[m_NumVV++] = Extrude(m_pTris[i].pV1, vc);
         m_pVolume[m_NumVV++] = Extrude(m_pTris[i].pV2, vc);

         m_pIsCap[i] = true;
         }
      else m_pIsCap[i] = false;
      } // for

   int nFact = m_nSize/sizeof(BYTE);

   // loop through edge list and extrude edges
   // to full rects along the light direction
   for (UINT j=0; j<m_NumE; j++) {
      v3 = Extrude( (VERTEX*)&m_pVerts[ m_pEdges[2*j+0] * nFact ], vc);
      v4 = Extrude( (VERTEX*)&m_pVerts[ m_pEdges[2*j+1] * nFact ], vc);

      memcpy(&v1, (VERTEX*)&m_pVerts[ m_pEdges[2*j+0] * nFact ], sizeof(PVERTEX));
      memcpy(&v2, (VERTEX*)&m_pVerts[ m_pEdges[2*j+1] * nFact ], sizeof(PVERTEX));

      // Add a quad (two triangles) to the vertex list
      m_pVolume[m_NumVV++] = v1;
      m_pVolume[m_NumVV++] = v2;
      m_pVolume[m_NumVV++] = v3;

      m_pVolume[m_NumVV++] = v2;
      m_pVolume[m_NumVV++] = v4;
      m_pVolume[m_NumVV++] = v3;
      }
   } // BuildShadowVolume
/*----------------------------------------------------------------*/


/**
 * Add indices of an edge to the given list of edges
 */
void CShadowVolume::AddEdge(WORD v0, WORD v1) {
    // Remove interior edges (which appear in the list twice)
    for( UINT i=0; i < m_NumE; i++ ) {
       if( ( m_pEdges[2*i+0] == v0 && m_pEdges[2*i+1] == v1 ) ||
           ( m_pEdges[2*i+0] == v1 && m_pEdges[2*i+1] == v0 ) ) 
          {
          if( m_NumE > 1 ) {
             m_pEdges[2*i+0] = m_pEdges[2*(m_NumE-1)+0];
             m_pEdges[2*i+1] = m_pEdges[2*(m_NumE-1)+1];
             }
          m_NumE--;
          return;
          }
       }
   m_pEdges[2*m_NumE+0] = v0;
   m_pEdges[2*m_NumE+1] = v1;
   m_NumE++;
   } // AddEdge
/*----------------------------------------------------------------*/


/**
 * Render the shadow volume utilizing the stencil buffer also. 
 * Note that the application itself needs to switch off pixel
 * buffer and set init states for stencil buffer prior to this.
 */
HRESULT CShadowVolume::Render( CGameLight *pLight ) {
   HRESULT hr=ZFX_OK;

   ZFXVector vcLightPos = pLight->GetPosition();
   ZFXVector vcLookAtLight = vcLightPos - m_Aabb.vcCenter;

   // light shining on object at all
   float fDist_2 = vcLookAtLight.GetSqrLength();
   float fRadi_2 = pLight->GetSqrRadius()*2;

   if ( fDist_2 > fRadi_2 ) return ZFX_OK;

   // build shadow volume
   BuildShadowVolume(vcLookAtLight);

   // FIRST PASS RENDERING BACKFACES FIRT
   m_pDevice->SetStencilBufferMode(RS_STENCIL_ZFAIL_INCR, 0);
   m_pDevice->SetBackfaceCulling(RS_CULL_CW);
   hr = m_pDevice->GetVertexManager()->RenderNaked(m_NumVV, m_pVolume, false);

   // SECOND PASS RENDERING FRONT FACES
   m_pDevice->SetStencilBufferMode(RS_STENCIL_ZFAIL_DECR, 0);
   m_pDevice->SetBackfaceCulling(RS_CULL_CCW);
   hr = m_pDevice->GetVertexManager()->RenderNaked(m_NumVV, m_pVolume, false);

   return hr;
   } // Render
/*----------------------------------------------------------------*/




/**
 * Generates information about all three neighbors of one tri
 *
void CShadowVolume::GenerateAdjacency(void) {
   char c=0;

   // loop through all triangles
   for (UINT t=0; t<m_NumT; t++) {
      c=0;

      // invalidate neighbor fields
      m_pAdjacency[t*3+0] = -1;
      m_pAdjacency[t*3+1] = -1;
      m_pAdjacency[t*3+2] = -1;

      for (UINT i=0; i<m_NumT; i++) {
         // don't test against itself
         if (i==t) continue;

         if ( Adjacent(m_pTris[t], m_pTris[i]) ) {
            // triangle i is a neighbor of t 
            m_pAdjacency[t*3 + c] = i;
            c++;

            // all three neighbors found?
            if (c==3) break;
            }
         }
      }
   } // GenerateAdjacency
/*----------------------------------------------------------------*/


/* just a little helper
inline bool CShadowVolume::Adjacent(const TRIANGLE &t0, 
                                    const TRIANGLE &t1) {
   
   if ( HasEqualEdge(t0, t1.pV0, t1.pV1) ||
        HasEqualEdge(t0, t1.pV1, t1.pV2) ||
        HasEqualEdge(t0, t1.pV2, t1.pV0) )
      return true;
   
   return false;
   } // TrisAreAdjacent
/*----------------------------------------------------------------*/

/* just a little helper
inline bool CShadowVolume::HasEqualEdge(const TRIANGLE &t, 
                                        const VERTEX *pv0,
                                        const VERTEX *pv1) {

   if ( Adjacent(pv0, pv1, t.pV0, t.pV1) ||
        Adjacent(pv0, pv1, t.pV1, t.pV2) ||
        Adjacent(pv0, pv1, t.pV2, t.pV0) )
      return true;

   return false;
   } // HasEqualEdge
/*----------------------------------------------------------------*/

/* just a little helper
inline bool CShadowVolume::Adjacent(const VERTEX *pAv0, 
                                    const VERTEX *pAv1,
                                    const VERTEX *pBv0, 
                                    const VERTEX *pBv1) {
   if ( EqualPosition(pAv0, pBv0) &&
        EqualPosition(pAv1, pBv1) ) {
      if ( (pAv0 != pBv0) && (pAv0 != pBv0) )
         return true;
      }

   if ( EqualPosition(pAv0, pBv1) &&
        EqualPosition(pAv1, pBv0) ) {
      if ( (pAv0 != pBv1) && (pAv1 != pBv0) ) 
         return true;
      }

   return false;
   } // EqualPosition
/*----------------------------------------------------------------*/

/* just a little helper
inline bool CShadowVolume::EqualPosition(const VERTEX *pv0, 
                                     const VERTEX *pv1) {
   return ( (pv0->x == pv1->x) &&
            (pv0->y == pv1->y) &&
            (pv0->z == pv1->z) );
   } // EqualPosition
/*----------------------------------------------------------------*/

/* just a little helper
inline void CShadowVolume::AddExtrudedEdge(const VERTEX *pv0, 
                                           const VERTEX *pv1,
                                           const ZFXVector &vc) {
   PVERTEX v0, v1;
   PVERTEX v2 = Extrude(pv0, vc);
   PVERTEX v3 = Extrude(pv1, vc);

   memcpy(&v0, pv0, sizeof(PVERTEX));
   memcpy(&v1, pv1, sizeof(PVERTEX));

   m_pVolume[m_NumVV++] = v0;
   m_pVolume[m_NumVV++] = v1;
   m_pVolume[m_NumVV++] = v2;

   m_pVolume[m_NumVV++] = v1;
   m_pVolume[m_NumVV++] = v3;
   m_pVolume[m_NumVV++] = v2;
   } // AddExtrudedEdge
/*----------------------------------------------------------------*/