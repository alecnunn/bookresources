/*******************************************************************
* ZFXEngine!                                                      *
* (c)2003 by Stefan Zerbst | www.zfx.info                         *
*-----------------------------------------------------------------*
* File: ZFXBspTree.cpp                                            *
* part of static math lib implementing basic 3D math objects      *
*******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include <stdlib.h>     // malloc, free, realloc
#include "zfx3d.h"      // class definitions and stuff


// F U N C T I O N S ///////////////////////////////////////////////


// init static attribute
unsigned int ZFXBspTree::m_sNum = 0;


/**
 * Constructor
 */
ZFXBspTree::ZFXBspTree(void) {
   m_NumPolys  = 0;
   m_pBack     = NULL;
   m_pFront    = NULL;
   m_pRoot     = NULL;
   m_pParent   = NULL;
   m_pPolys    = NULL;
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXBspTree::~ZFXBspTree(void) {
   m_NumPolys = 0;

   if (m_pPolys) {
      free(m_pPolys);
      m_pPolys = NULL;
      }

   if (m_pFront) {
      delete m_pFront;
      m_pFront = NULL;
      }

   if (m_pBack) {
      delete m_pBack;
      m_pBack = NULL;
      }
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Set root node and parent for the current 'this' object.
 */ 
void ZFXBspTree::SetRelationships(ZFXBspTree *pRoot, ZFXBspTree *pDad) {
   m_pParent = pDad;
   m_pRoot   = pRoot;
   } // SetRelationships
/*----------------------------------------------------------------*/


/**
 * Start the building process for the bsp tree
 */
void ZFXBspTree::BuildTree(const ZFXPolygon *pPolys, unsigned int Num) {
   m_pRoot   = this;
   m_pParent = NULL;

   if (Num<1) return;

   // allocate memory for plygon list
   int nSize = sizeof(ZFXPolygon)*Num;
   m_pPolys = (ZFXPolygon*)malloc(nSize);
   memset(m_pPolys, 0, nSize);
   m_NumPolys = Num;

   for (unsigned int i=0; i<Num; i++)
      m_pPolys[i].CopyOf( pPolys[i] );

   // recursively create childs
   CreateChilds();
   } // BuildTree
/*----------------------------------------------------------------*/


/**
 * Recursively build the childs for node represented by 'this' object
 */
void ZFXBspTree::CreateChilds(void) {
   float fDot=0.0f;

   ZFXPolygon plyFront, plyBack;
   
   int nFront=0, nBack=0, nClass=0;

   // calculate aabb for this node
   CalcBoundingBox(m_pPolys, m_NumPolys);

   // if we cant find a splitter this is a leaf
   if ( !FindBestSplitter() ) { 
      ZFXBspTree::m_sNum += m_NumPolys;
      return; 
      }
   
   // create new objects for front- and backlist
   m_pFront = new ZFXBspTree;
   m_pBack  = new ZFXBspTree;
   m_pFront->SetRelationships(m_pRoot, this);
   m_pBack->SetRelationships(m_pRoot, this);

   // now split polygonlist to childs
   for (unsigned int i=0; i<m_NumPolys; i++) {

      nClass = m_Plane.Classify( m_pPolys[i] );

      if (nClass == ZFXFRONT) {
         m_pFront->AddPolygon( m_pPolys[i] );
         }
      else if (nClass == ZFXBACK) {
         m_pBack->AddPolygon( m_pPolys[i] );
         }
      else if (nClass == ZFXCLIPPED) {
         // split the poly at the plane
         m_pPolys[i].Clip(m_Plane, &plyFront, &plyBack);

         m_pFront->AddPolygon( plyFront );
         m_pBack->AddPolygon( plyBack );
         }
      else if (nClass == ZFXPLANAR) {

         fDot = m_Plane.m_vcN * m_pPolys[i].GetPlane().m_vcN;

         if ( fDot >= 0.0f ) {
            m_pFront->AddPolygon( m_pPolys[i] );
            }
         else {
            m_pBack->AddPolygon( m_pPolys[i] );
            }
         }
      } // for

   // delete polylist from inner nodes
   if (m_pPolys) {
      free(m_pPolys);
      m_pPolys = NULL;
      }

   // recursive calls
   m_pFront->CreateChilds();
   m_pBack->CreateChilds();
   } // CreateChilds
/*----------------------------------------------------------------*/


/**
 * Find the best splitter for a given node, return false if there 
 * ain't any suited splitter in the polygon list.
 */
bool ZFXBspTree::FindBestSplitter(void) {

   ZFXPolygon *pBestSplitter = NULL;
   ZFXPolygon *pSplitter = NULL;
   ZFXPlane    Plane;

   int     lFront  = 0,       // Wie viele Polys liegen
           lBack   = 0,       // bei jedem potentiellen
           lPlanar = 0,       // Splitter in Front, Back
           lSplits = 0;       // Planar oder spannen?
   int     nClass;
   int     lScore,
           lBestScore = 1000000;
   bool    bFound = false;


   for (unsigned int i=0; i<m_NumPolys; i++) {

      pSplitter = &m_pPolys[i];
      Plane     = pSplitter->GetPlane();

      // reset counters for this splitter
      lFront = lBack = lPlanar = lSplits = 0;

      // if flag then poly already used as splitter
      if ( pSplitter->GetFlag() == 1 ) continue;

      // loop through all polys for this splitter
      for (unsigned int j=0; j<m_NumPolys; j++) {
         if (i==j) continue;

         // classify current polygon with splitter
         nClass = Plane.Classify( m_pPolys[j] );

         // add result to count
         if      ( nClass == ZFXFRONT  ) lFront++;
         else if ( nClass == ZFXBACK   ) lBack++;
         else if ( nClass == ZFXPLANAR ) lPlanar++;
         else                            lSplits++;
         } // for

      // count splitter's score heuristic
      lScore = abs(lFront - lBack) + (lSplits * 3);

      // do we have a new best score?
      if (lScore < lBestScore) {
         if ( ((lFront > 0) && (lBack > 0)) || 
              (lSplits > 0) ) {
            lBestScore = lScore;
            pBestSplitter = pSplitter;
            bFound = true;
            } 
         } // if [ulScore]

      } // for

   // we can't find a splitter at all
   if ( !bFound ) return false;

   // mark polygon as used for split
   pBestSplitter->SetFlag(1);

   // save splitting plane
   m_Plane = pBestSplitter->GetPlane();
   
   return true;
   } // FindBestSplitter
/*----------------------------------------------------------------*/


/**
 * Add a polygon to the current list of polygons in this node.
 */ 
void ZFXBspTree::AddPolygon(const ZFXPolygon &Poly) {
   
   m_pPolys = (ZFXPolygon*)realloc(m_pPolys,
                              sizeof(ZFXPolygon)*
                              (m_NumPolys+1));

   memset(&m_pPolys[m_NumPolys], 0, sizeof(ZFXPolygon));

   m_pPolys[m_NumPolys].CopyOf( Poly );
   m_NumPolys++;
   } // SetRelationships
/*----------------------------------------------------------------*/


/**
 * Traverse the tree with given viewer position and view frustum. This
 * is back to front traversal so polylist is ordered from back to front
 * in respect to given viewpoint position.
 */
void ZFXBspTree::TraverseBtF(ZFXPolylist *pList, ZFXVector vcPos, 
                           const ZFXPlane *Frustum) {

   // frustum culling for this node
   if (m_Aabb.Cull(Frustum,6 ) == ZFXCULLED) return;

   // if leaf then add polys to return list
   if ( IsLeaf() ) {
      for (unsigned int i=0; i<m_NumPolys; i++) {
         pList->AddPolygon( m_pPolys[i] );
         }
      }

   // traverse tree
   else {
      int nClass = m_Plane.Classify( vcPos );

      if (nClass == ZFXBACK) {
         m_pFront->TraverseBtF(pList, vcPos, Frustum);
         m_pBack->TraverseBtF(pList,  vcPos, Frustum);
         }
      else {
         m_pBack->TraverseBtF(pList,  vcPos, Frustum);
         m_pFront->TraverseBtF(pList, vcPos, Frustum);
         }
      }
   } // TraverseBtF
/*----------------------------------------------------------------*/


/**
 * Traverse the tree with given viewer position and view frustum. This
 * is front to back traversal so polylist is ordered from front to back
 * in respect to given viewpoint position.
 */
void ZFXBspTree::TraverseFtB(ZFXPolylist *pList, ZFXVector vcPos, 
                           const ZFXPlane *Frustum) {

   // frustum culling for this node
   if (m_Aabb.Cull(Frustum,6 ) == ZFXCULLED) return;

   // if leaf then add polys to return list
   if ( IsLeaf() ) {
      for (unsigned int i=0; i<m_NumPolys; i++) {
         pList->AddPolygon( m_pPolys[i] );
         }
      }

   // traverse tree
   else {
      int nClass = m_Plane.Classify( vcPos );

      if (nClass == ZFXBACK) {
         m_pBack->TraverseFtB(pList,  vcPos, Frustum);
         m_pFront->TraverseFtB(pList, vcPos, Frustum);
         }
      else {
         m_pFront->TraverseFtB(pList, vcPos, Frustum);
         m_pBack->TraverseFtB(pList,  vcPos, Frustum);
         }
      }
   } // TraverseFtB
/*----------------------------------------------------------------*/


/**
 * Calculates the aabb for this node
 */
void ZFXBspTree::CalcBoundingBox(const ZFXPolygon *_pPolys_, unsigned int Num) {
   ZFXVector   vcMax, vcMin, vcTemp;
   ZFXAabb     Aabb;

   // cast away const
   ZFXPolygon *pPolys = ((ZFXPolygon*)_pPolys_);

   if (Num < 1)  return;

   // get arbitrary sub bounding box
   Aabb = pPolys[0].GetAabb();
   vcMax = vcMin = Aabb.vcCenter;

   for (unsigned int i=0; i<Num; i++) {
      Aabb = pPolys[i].GetAabb();

      // get obb one side's extreme values
      vcTemp = Aabb.vcMax;

      if      ( vcTemp.x > vcMax.x ) vcMax.x = vcTemp.x;
      else if ( vcTemp.x < vcMin.x ) vcMin.x = vcTemp.x;
      if      ( vcTemp.y > vcMax.y ) vcMax.y = vcTemp.y;
      else if ( vcTemp.y < vcMin.y ) vcMin.y = vcTemp.y;
      if      ( vcTemp.z > vcMax.z ) vcMax.z = vcTemp.z;
      else if ( vcTemp.z < vcMin.z ) vcMin.z = vcTemp.z;

      // get obb other side's extreme values
      vcTemp = Aabb.vcMin;

      if      ( vcTemp.x > vcMax.x ) vcMax.x = vcTemp.x;
      else if ( vcTemp.x < vcMin.x ) vcMin.x = vcTemp.x;
      if      ( vcTemp.y > vcMax.y ) vcMax.y = vcTemp.y;
      else if ( vcTemp.y < vcMin.y ) vcMin.y = vcTemp.y;
      if      ( vcTemp.z > vcMax.z ) vcMax.z = vcTemp.z;
      else if ( vcTemp.z < vcMin.z ) vcMin.z = vcTemp.z;
      }

   // now calculate maximum extension
   float fMax = vcMax.x - vcMin.x;
   if (fMax < (vcMax.y - vcMin.y) ) fMax = vcMax.y - vcMin.y;
   if (fMax < (vcMax.z - vcMin.z) ) fMax = vcMax.z - vcMin.z;

   // make box cubic
   m_Aabb.vcCenter = (vcMax + vcMin) / 2.0f;
   m_Aabb.vcMax    = m_Aabb.vcCenter + (fMax/2.0f);
   m_Aabb.vcMin    = m_Aabb.vcCenter - (fMax/2.0f);
   } // CalcBoundingBox
/*----------------------------------------------------------------*/


/**
 * Check if the given ray sits at least partially in solid space.
 * Then a collision must have occured with geometry in tree.
 */
bool ZFXBspTree::TestCollision(const ZFXRay &Ray, float fL, 
                             float *pfD, ZFXVector *pvcN) {
   ZFXRay rayFront, rayBack;
   int    nFront=0;

   // IF WE ARE IN A LEAF
   if ( IsLeaf() ) {

      for (unsigned int i=0; i<m_NumPolys; i++) {
         
         // collision with any polygon in leaf?
         if ( m_pPolys[i].Intersects(Ray, false, fL, NULL) )
            {
            if (pvcN) *pvcN = m_pPolys[i].GetPlane().m_vcN;  
            return true;
            }
         } // for
      return false;
      }

   // ELSE IF WE ARE IN AN INNER NODE
   int nClass = m_Plane.Classify( Ray.m_vcOrig );

   // ray intersects this node's splitting plane
   if ( m_Plane.Clip(&Ray, fL, &rayFront, &rayBack) ) {

      // search front to back order
      if ( nClass == ZFXBACK ) 
         return m_pBack->TestCollision(rayBack,   fL, pfD, pvcN)
            ||  m_pFront->TestCollision(rayFront, fL, pfD, pvcN);
      else 
         return m_pFront->TestCollision(rayFront, fL, pfD, pvcN)
            ||  m_pBack->TestCollision(rayBack,   fL, pfD, pvcN);

      }
   else {
      // ray totally on back side
      if ( nClass == ZFXBACK ) {
         return m_pBack->TestCollision(Ray, fL, pfD, pvcN);
         }
      // ray totally on front side or planar
      else {
         return m_pFront->TestCollision(Ray, fL, pfD, pvcN);
         }
      }
   } // TestCollision [ray]
/*----------------------------------------------------------------*/


/**
 * Test if there is a free line of sight between the two given points
 */
bool ZFXBspTree::LineOfSight(const ZFXVector &vcA, const ZFXVector &vcB) {
   ZFXRay Ray;

   // build a ray from A to B
   ZFXVector vcDir = vcB - vcA;
   vcDir.Normalize();
   Ray.Set( vcA, vcDir );

   // check ray for collision
   return !TestCollision(Ray, vcDir.GetLength(), NULL, NULL);
   } // LineOfSight
/*----------------------------------------------------------------*/

