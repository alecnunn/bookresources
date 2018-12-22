/*******************************************************************
* ZFXEngine!                                                      *
* (c)2003 by Stefan Zerbst | www.zfx.info                         *
*-----------------------------------------------------------------*
* File: ZFXOctree.cpp                                             *
* part of static math lib implementing basic 3D math objects      *
*******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include "zfx3d.h"      // class definitions and stuff


// D E F I N E S ///////////////////////////////////////////////////

// polys allowed per leaf
#define POLYS_PER_LEAF 10

// name the childs
#define UP_NE  0     // upper north east child
#define UP_NW  1     // upper north west child
#define UP_SE  2     // upper south east child
#define UP_SW  3     // upper south west child
#define LW_NE  4     // lower north east child
#define LW_NW  5     // lower north west child
#define LW_SE  6     // lower south east child
#define LW_SW  7     // lower south west child



// F U N C T I O N S ///////////////////////////////////////////////


// constructor
ZFXOctree::ZFXOctree(void) {
   m_NumPolys = 0;
   m_Pos      = -1;
   m_pPolys   = NULL;
   m_pRoot    = NULL;
   m_pParent  = NULL;

   for (int i=0; i<8; i++)
      m_pChild[i] = NULL;

   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // constructor
/*----------------------------------------------------------------*/


// destructor
ZFXOctree::~ZFXOctree(void) {
   m_NumPolys = 0;

   if (m_pPolys) {
      delete [] m_pPolys;
      m_pPolys = NULL;
      }

   for (int i=0; i<8; i++) {
      if (m_pChild[i]) {
         delete m_pChild[i];
         m_pChild[i] = NULL;
         }
      }
   } // destructor
/*----------------------------------------------------------------*/


// start the building process for the octree object
void ZFXOctree::BuildTree(const ZFXPolygon *pPolys, unsigned int Num) {
   m_pRoot = this;

   if (Num<1) return;

   // calculate aabb for the root
   CalcBoundingBox(pPolys, Num);

   m_pPolys = new ZFXPolygon[Num];
   m_NumPolys = Num;

   for (unsigned int i=0; i<Num; i++)
      m_pPolys[i].CopyOf( pPolys[i] );

   // recursively create childs
   CreateChilds(this);
 
   if (m_pPolys) {
      delete [] m_pPolys;
      m_pPolys = NULL;
      }
   } // BuildTree
/*----------------------------------------------------------------*/
  

/**
 * Constructs the subtree from the list of polygons, set NULL for the
 * root node on calling this method from external function. This will
 * calculate the bounding box for the list for the root object.
 * Otherwise the list is assumed to be inside of this object's aabb
 * when performing the recursive call. 
 */
void ZFXOctree::CreateChilds(ZFXOctree *pRoot) {
   // save pointer to root
   m_pRoot = pRoot;

   // do we need to subdivide this node
   if ( (pRoot == this) || (m_NumPolys > POLYS_PER_LEAF) )
      {
      // init childs and their aabb
      for (int i=0; i<8; i++) {
         InitChildObject( i, this );
 
         // BUILD POLY LISTS FOR CHILDS ANS COPY TO SUBLIST
         m_pChild[i]->ChopListToMe(m_pPolys, m_NumPolys); 
         m_pChild[i]->CreateChilds(pRoot);
         }

      // now we don't need this list for inner nodes any
      // more cause we only save lists in the leaf nodes
      if (m_pPolys) {
         delete [] m_pPolys;
         m_pPolys = NULL;
         }
      }
   // this is going to be a leaf 
   else return;
   } // CreateChilds
/*----------------------------------------------------------------*/
    
 
/**
 * Takes the list of polygons and builds a new list containing all
 * polygons that are insed the given aabb. Note that this will also
 * clip polygons which intersect and are not totally included in aabb
 */
void ZFXOctree::ChopListToMe(ZFXPolygon *pList, unsigned int Num) {
   ZFXPolygon  ChoppedPoly;
   int  nClass;

   if (Num < 1) return;

   // better save than sorry
   if (m_pPolys) { delete [] m_pPolys; m_pPolys = NULL; }
   m_NumPolys = 0;
 
   // temporary list
   ZFXPolygon *TempMem = new ZFXPolygon[Num];

   // loop through the list
   for (unsigned int i=0; i<Num; i++) {
      if (pList[i].GetFlag() == 1) continue;

      ChoppedPoly.CopyOf( pList[i] ); 

      nClass = ChoppedPoly.Cull(m_Aabb);

      // this poly is outside the aabb
      if (nClass == ZFXCULLED) continue;
       
      // poly is inside or intersecting
      else {
         // if totally inside this node then set flag to
         // prevent multiple checks in neighboring nodes
         if ( nClass != ZFXCLIPPED) pList[i].SetFlag(1);
         // else we need to chop it accordingly
         else ChoppedPoly.Clip(m_Aabb);
    
         // add to list
         TempMem[m_NumPolys].CopyOf(ChoppedPoly);
         m_NumPolys++;
         }
      }

   // copy new polygonlist
   m_pPolys = new ZFXPolygon[m_NumPolys];
   for (unsigned int j=0; j<m_NumPolys; j++) {
      m_pPolys[j].CopyOf( TempMem[j] );
      }
   delete [] TempMem;
   } // ChopListToAabb
/*----------------------------------------------------------------*/
 

// test if this node intersects a vertical ray going downwards
bool ZFXOctree::IntersectsDownwardsRay(const ZFXVector &vcOrig, float f) {
   // ray starts below this aabb anyway
   if (vcOrig.y < m_Aabb.vcMin.y) return false;

   // not inside on x axis
   if (vcOrig.x < m_Aabb.vcMin.x) return false;
   if (vcOrig.x > m_Aabb.vcMax.x) return false;

   // not inside on z axis
   if (vcOrig.z < m_Aabb.vcMin.z) return false;
   if (vcOrig.z > m_Aabb.vcMax.z) return false;

   // we already have a closer hitpoint that possible here
   if (f < (fabs(m_Aabb.vcMax.y - vcOrig.y))) return false;

   return true;
   } // IntersectsDownwardsRay
/*----------------------------------------------------------------*/


// check aabb for collision with polygons in the octree
bool ZFXOctree::TestCollision(const ZFXAabb &aabb, ZFXPlane *pP) {

   // check collision with this node
   if (this != m_pRoot) {
      if (!m_Aabb.Intersects(aabb))
         return false;
      }

   // no geometry here, just a node
   if ( !IsLeaf() ) {
      for (int i=0; i<8; i++) {
         // seek for any collision no matter where
         if ( m_pChild[i]->TestCollision(aabb, pP) )
            return true;
         } // for

      // none of this node's childs collide
      return false;
      } // if [!leaf]

   // this is a leaf with geometry
   else {
      if (!m_pPolys) return false;

      // loop through polys and check'em out
      for (unsigned int i=0; i<m_NumPolys; i++) {
         if (m_pPolys[i].GetAabb().Intersects(aabb)) {
            if (pP) *pP = m_pPolys[i].GetPlane();
            return true;
            }
         }

      // no collision in this leaf
      return false;
      }
   } // TestCollision
/*----------------------------------------------------------------*/


// check aabb for collision with polygons in the octree
bool ZFXOctree::TestCollision(const ZFXRay &Ray, float fL, float *pfD) {
   bool  blnCollision=false;
   float _fD=0.0f;

   // check collision with this node
   if (this != m_pRoot) {
      if ( !m_Aabb.Intersects(Ray, fL, pfD) &&
           !m_Aabb.Contains(Ray, fL) )
         return false;
      }

   // no geometry here, just a node
   if ( !IsLeaf() ) {
      for (int i=0; i<8; i++) {
         // seek for any collision no matter where
         if ( m_pChild[i]->TestCollision(Ray, fL, pfD) )
            return true;
         } // for

      // none of this node's childs collide
      return false;
      } // if [!leaf]

   // this is a leaf with geometry
   else {
      if (!m_pPolys) return false;

      // loop through polys and check'em out
      for (unsigned int i=0; i<m_NumPolys; i++) {
         if ( m_pPolys[i].Intersects(Ray, false, fL, &_fD) ) {
            blnCollision = true;

            // find nearest collision point
            if (pfD) {
               if ( (*pfD<=0.0f) || (_fD < *pfD)) 
                  *pfD = _fD;
               }
            }
         }

      // no collision in this leaf
      return blnCollision;
      }

   return false;
   } // TestCollision [ray]
/*----------------------------------------------------------------*/


// find distance from ray origin to closest floor below, returns
// plane of this floor in second parameter for slide purposes.
bool ZFXOctree::GetFloor(const ZFXVector &vcPos, float *pf, ZFXPlane *pPlane) {
   float   fAabbDist=0, fHitDist=0;
   bool    bHit=false;
   ZFXAabb aabb;
   ZFXRay  Ray;

   // is this the root node?
   if ( this == m_pRoot) *pf = 99999.0f;

   // no geometry here, just a node
   if ( !IsLeaf() ) {
      for (int i=0; i<8; i++) {
         // ray intersects with child node at all
         if ( m_pChild[i]->IntersectsDownwardsRay(vcPos, *pf) ) {
            // is intersection closer than current hitpoint
            if (m_pChild[i]->GetFloor(vcPos, pf, pPlane)) 
               bHit = true;
            }
         } // for
      return bHit;
      } // if [!leaf]

   // this is a leaf with geometry
   else {
      if (!m_pPolys) return false;

      Ray.Set(vcPos, ZFXVector(0.0f,-1.0f,0.0f));
      
      // loop through polys and check'em out
      for (unsigned int i=0; i<m_NumPolys; i++) {
         aabb = m_pPolys[i].GetAabb();

         if ((Ray.m_vcOrig.x < aabb.vcMin.x) ||
             (Ray.m_vcOrig.x > aabb.vcMax.x) ||
             (Ray.m_vcOrig.z < aabb.vcMin.z) ||
             (Ray.m_vcOrig.z > aabb.vcMax.z) ||
             (Ray.m_vcOrig.y < aabb.vcMin.y) )
            continue;

         if (m_pPolys[i].Intersects(Ray, true, *pf, &fHitDist)) {
               *pf = fHitDist;
               bHit = true;
            }
         } // for
      return bHit;
      }
   } // GetFloor
/*----------------------------------------------------------------*/


// initializes child objects that they are ready to receive polys
void ZFXOctree::InitChildObject(int ChildID, ZFXOctree *pParent) {
   ZFXAabb aabb;

   float xmin = m_Aabb.vcMin.x, 
         xcen = m_Aabb.vcCenter.x, 
         xmax = m_Aabb.vcMax.x; 

   float ymin = m_Aabb.vcMin.y, 
         ycen = m_Aabb.vcCenter.y, 
         ymax = m_Aabb.vcMax.y; 

   float zmin = m_Aabb.vcMin.z, 
         zcen = m_Aabb.vcCenter.z, 
         zmax = m_Aabb.vcMax.z; 

   switch(ChildID) {
      case UP_NW:
         aabb.vcMax = ZFXVector(xcen, ymax, zmax);
         aabb.vcMin = ZFXVector(xmin, ycen, zcen);
         break;
      case UP_NE:
         aabb.vcMax = m_Aabb.vcMax;
         aabb.vcMin = m_Aabb.vcCenter;
         break;
      case UP_SW:
         aabb.vcMax = ZFXVector(xcen, ymax, zcen);
         aabb.vcMin = ZFXVector(xmin, ycen, zmin);
         break;
      case UP_SE:
         aabb.vcMax = ZFXVector(xmax, ymax, zcen);
         aabb.vcMin = ZFXVector(xcen, ycen, zmin);
         break;
      case LW_NW:
         aabb.vcMax = ZFXVector(xcen, ycen, zmax);
         aabb.vcMin = ZFXVector(xmin, ymin, zcen);
         break;
      case LW_NE:
         aabb.vcMax = ZFXVector(xmax, ycen, zmax);
         aabb.vcMin = ZFXVector(xcen, ymin, zcen);
         break;
      case LW_SW:
         aabb.vcMax = m_Aabb.vcCenter;
         aabb.vcMin = m_Aabb.vcMin;
         break;
      case LW_SE:
         aabb.vcMax = ZFXVector(xmax, ycen, zcen);
         aabb.vcMin = ZFXVector(xcen, ymin, zmin);
         break;
      default: break;
      }

   aabb.vcCenter = (aabb.vcMax + aabb.vcMin) / 2.0f;
   m_pChild[ChildID] = new ZFXOctree();
   m_pChild[ChildID]->SetBoundingBox(aabb);
   m_pChild[ChildID]->m_pParent = pParent;
   m_pChild[ChildID]->m_Pos     = ChildID;
   } // InitChildObjects
/*----------------------------------------------------------------*/


// calculates the aabb for this node
void ZFXOctree::CalcBoundingBox(const ZFXPolygon *_pPolys_, unsigned int Num) {
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


// walk through the tree and collect visible polygons
void ZFXOctree::Traverse(ZFXPolylist *pList, ZFXPolylist *pAabbList,
                         const ZFXPlane *pFrustum) {

   if (m_Aabb.Cull(pFrustum, 6) == ZFXCULLED) return;

   if ( IsLeaf() ) {

      if (pList) {
         for (unsigned int i=0; i<m_NumPolys; i++)
            pList->AddPolygon(m_pPolys[i]);
         }
      if (pAabbList) GetAabbAsPolygons(pAabbList);
      }
   else {
      m_pChild[0]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[1]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[2]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[3]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[4]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[5]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[6]->Traverse(pList, pAabbList, pFrustum);
      m_pChild[7]->Traverse(pList, pAabbList, pFrustum);
      }
   } // Traverse
/*----------------------------------------------------------------*/


// get all bounding boxes as polygons from the octree
void ZFXOctree::GetAabbAsPolygons(ZFXPolylist *pList) {
   ZFXPolygon   Poly;
   ZFXVector    vcPoints[24];
   unsigned int nIndis[6] = { 0, 1, 2, 2, 3, 0 };
   
   float fW = m_Aabb.vcMax.x - m_Aabb.vcMin.x;
   float fH = m_Aabb.vcMax.y - m_Aabb.vcMin.y;
   float fD = m_Aabb.vcMax.z - m_Aabb.vcMin.z;

   // top rectangle
   vcPoints[0].Set( m_Aabb.vcCenter.x - (fW / 2.0f),
                    m_Aabb.vcCenter.y + (fH / 2.0f),
                    m_Aabb.vcCenter.z - (fD / 2.0f) );
   vcPoints[1].Set( m_Aabb.vcCenter.x - (fW / 2.0f),
                    m_Aabb.vcCenter.y + (fH / 2.0f),
                    m_Aabb.vcCenter.z + (fD / 2.0f) );
   vcPoints[2].Set( m_Aabb.vcCenter.x + (fW / 2.0f),
                    m_Aabb.vcCenter.y + (fH / 2.0f),
                    m_Aabb.vcCenter.z + (fD / 2.0f) );
   vcPoints[3].Set( m_Aabb.vcCenter.x + (fW / 2.0f),
                    m_Aabb.vcCenter.y + (fH / 2.0f),
                    m_Aabb.vcCenter.z - (fD / 2.0f) );
   Poly.Set( &vcPoints[0], 4, nIndis, 6 );
   pList->AddPolygon( Poly );

   // right side rectanlge
   vcPoints[4] = vcPoints[3];
   vcPoints[5] = vcPoints[2];
   vcPoints[6].Set( m_Aabb.vcCenter.x + (fW / 2.0f),
                    m_Aabb.vcCenter.y - (fH / 2.0f),
                    m_Aabb.vcCenter.z + (fD / 2.0f) );
   vcPoints[7].Set( m_Aabb.vcCenter.x + (fW / 2.0f),
                    m_Aabb.vcCenter.y - (fH / 2.0f),
                    m_Aabb.vcCenter.z - (fD / 2.0f) );
   Poly.Set( &vcPoints[4], 4, nIndis, 6 );
   pList->AddPolygon( Poly );

   // left side rectangle
   vcPoints[8] = vcPoints[0];
   vcPoints[9] = vcPoints[1];
   vcPoints[10].Set( m_Aabb.vcCenter.x - (fW / 2.0f),
                     m_Aabb.vcCenter.y - (fH / 2.0f),
                     m_Aabb.vcCenter.z + (fD / 2.0f) );
   vcPoints[11].Set( m_Aabb.vcCenter.x - (fW / 2.0f),
                     m_Aabb.vcCenter.y - (fH / 2.0f),
                     m_Aabb.vcCenter.z - (fD / 2.0f) );
   Poly.Set( &vcPoints[8], 4, nIndis, 6 );
   pList->AddPolygon( Poly );

   // back side rectanlge
   vcPoints[12] = vcPoints[2];
   vcPoints[13] = vcPoints[1];
   vcPoints[14] = vcPoints[10];
   vcPoints[15] = vcPoints[6];
   Poly.Set( &vcPoints[12], 4, nIndis, 6 );
   pList->AddPolygon( Poly );

   // front side rectangle
   vcPoints[16] = vcPoints[0];
   vcPoints[17] = vcPoints[3];
   vcPoints[18] = vcPoints[7];
   vcPoints[19] = vcPoints[11];
   Poly.Set( &vcPoints[16], 4, nIndis, 6 );
   pList->AddPolygon( Poly );

   // bottom side rectangle
   vcPoints[20] = vcPoints[7];
   vcPoints[21] = vcPoints[6];
   vcPoints[22] = vcPoints[10];
   vcPoints[23] = vcPoints[11];
   Poly.Set( &vcPoints[20], 4, nIndis, 6 );
   pList->AddPolygon( Poly );
   } // GetAabbAsPolygons
/*----------------------------------------------------------------*/

