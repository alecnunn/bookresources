/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: zfxplane.cpp                                              *
 * part of static math lib implementing basic 3D math objects      *
 *******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include "zfx3d.h"      // class definitions and stuff


// G L O B A L S ///////////////////////////////////////////////////

extern bool g_bSSE;



// F U N C T I O N S ///////////////////////////////////////////////

inline void ZFXPlane::Set(const ZFXVector &vcN, const ZFXVector &vcP) {
   m_fD      = - ( vcN * vcP);
   m_vcN     = vcN;
   m_vcPoint = vcP;
   }
/*----------------------------------------------------------------*/

inline void ZFXPlane::Set(const ZFXVector &vcN, const ZFXVector &vcP,
                          float fD) {
   m_vcN     = vcN;
   m_fD      = fD;
   m_vcPoint = vcP;
   }
/*----------------------------------------------------------------*/

inline void ZFXPlane::Set(const ZFXVector &v0, const ZFXVector &v1, 
                          const ZFXVector &v2) {
   ZFXVector vcEdge1 = v1 - v0;
   ZFXVector vcEdge2 = v2 - v0;

   m_vcN.Cross(vcEdge1, vcEdge2);
   m_vcN.Normalize();
   m_fD = - (m_vcN * v0);
   m_vcPoint = v0;
   }
/*----------------------------------------------------------------*/

// Calculate distance to point. Plane normal must be normalized.
inline float ZFXPlane::Distance(const ZFXVector &vcPoint) {
   return ( _fabs((m_vcN*vcPoint) - m_fD) );
   }
/*----------------------------------------------------------------*/


// Classify point to plane.
inline int ZFXPlane::Classify(const ZFXVector &vcPoint) {
   float f = (vcPoint * m_vcN) + m_fD;
   
   if (f >  0.00001) return ZFXFRONT;
   if (f < -0.00001) return ZFXBACK;
   return ZFXPLANAR;
   }
/*----------------------------------------------------------------*/


// Classify polygon with respect to this plane
int ZFXPlane::Classify(const ZFXPolygon &Poly) {
   int NumFront=0, NumBack=0, NumPlanar=0;
   int nClass;

   // cast away const
   ZFXPolygon *pPoly = ((ZFXPolygon*)&Poly);

   int NumPoints = pPoly->GetNumPoints();

   // loop through all points
   for (int i=0; i < NumPoints; i++) {
      nClass = Classify( pPoly->m_pPoints[i] );
      
      if (nClass == ZFXFRONT)     NumFront++;
      else if (nClass == ZFXBACK) NumBack++;
      else {
         NumFront++;
         NumBack++;
         NumPlanar++;
         }
      } // for
   
   // all points are planar
   if (NumPlanar == NumPoints)
      return ZFXPLANAR;
   // all points are in front of plane
   else if (NumFront == NumPoints)
      return ZFXFRONT;
   // all points are on backside of plane
   else if (NumBack == NumPoints)
      return ZFXBACK;
   // poly is intersecting the plane
   else
      return ZFXCLIPPED;
   } // Classify
/*----------------------------------------------------------------*/



// clips a ray into two segments if it intersects the plane
bool ZFXPlane::Clip(const ZFXRay *_pRay, float fL, ZFXRay *pF, ZFXRay *pB) {
   ZFXVector vcHit(0.0f,0.0f,0.0f);
   
   ZFXRay *pRay = (ZFXRay*)_pRay;

   // ray intersects plane at all?
   if ( !pRay->Intersects( *this, false, fL, NULL, &vcHit) ) 
      return false;

   int n = Classify( _pRay->m_vcOrig );

   // ray comes fron planes backside
   if ( n == ZFXBACK ) {
      if (pB) pB->Set(pRay->m_vcOrig, pRay->m_vcDir);
      if (pF) pF->Set(vcHit, pRay->m_vcDir);
      }
   // ray comes from planes front side
   else if ( n == ZFXFRONT ) {
      if (pF) pF->Set(pRay->m_vcOrig, pRay->m_vcDir);
      if (pB) pB->Set(vcHit, pRay->m_vcDir);
      }

   return true;
   } // Clip [ray]
/*----------------------------------------------------------------*/


// Intersection of two planes. If third parameter is given the line
// of intersection will be calculated. (www.magic-software.com)
bool ZFXPlane::Intersects(const ZFXPlane &plane, ZFXRay *pIntersection) {
   ZFXVector vcCross;
   float     fSqrLength;
   
   // if crossproduct of normals 0 than planes parallel
   vcCross.Cross(this->m_vcN, plane.m_vcN);
   fSqrLength = vcCross.GetSqrLength();

   if (fSqrLength < 1e-08f) 
      return false;

   // find line of intersection
   if (pIntersection) {
      float fN00 = this->m_vcN.GetSqrLength();
      float fN01 = this->m_vcN * plane.m_vcN;
      float fN11 = plane.m_vcN.GetSqrLength();
      float fDet = fN00*fN11 - fN01*fN01;

      if (_fabs(fDet) < 1e-08f) 
         return false;

      float fInvDet = 1.0f/fDet;
      float fC0 = (fN11*this->m_fD - fN01*plane.m_fD) * fInvDet;
      float fC1 = (fN00*plane.m_fD - fN01*this->m_fD) * fInvDet;

      (*pIntersection).m_vcDir  = vcCross;
      (*pIntersection).m_vcOrig = this->m_vcN*fC0 + plane.m_vcN*fC1;
      }

   return true;
   } // Intersects(Plane)
/*----------------------------------------------------------------*/


// Intersection of a plane with a triangle. If all vertices of the
// triangle are on the same side of the plane, no intersection occured. 
bool ZFXPlane::Intersects(const ZFXVector &vc0, const ZFXVector &vc1, 
                          const ZFXVector &vc2) {
   int n = this->Classify(vc0);

   if ( (n == this->Classify(vc1)) && 
        (n == this->Classify(vc2)) )
      return false;
   return true;
   } // Intersects(Tri)
/*----------------------------------------------------------------*/


// Intersection with AABB. Search for AABB diagonal that is most
// aligned to plane normal. Test its two vertices against plane.
// (Möller/Haines, "Real-Time Rendering")
bool ZFXPlane::Intersects(const ZFXAabb &aabb) {
   ZFXVector Vmin, Vmax;

   // x component
   if (m_vcN.x >= 0.0f) {
      Vmin.x = aabb.vcMin.x;
      Vmax.x = aabb.vcMax.x;
      }
   else {
      Vmin.x = aabb.vcMax.x;
      Vmax.x = aabb.vcMin.x;
      }

   // y component
   if (m_vcN.y >= 0.0f) {
      Vmin.y = aabb.vcMin.y;
      Vmax.y = aabb.vcMax.y;
      }
   else {
      Vmin.y = aabb.vcMax.y;
      Vmax.y = aabb.vcMin.y;
      }
   
   // z component
   if (m_vcN.z >= 0.0f) {
      Vmin.z = aabb.vcMin.z;
      Vmax.z = aabb.vcMax.z;
      }
   else {
      Vmin.z = aabb.vcMax.z;
      Vmax.z = aabb.vcMin.z;
      }

   if ( ((m_vcN * Vmin) + m_fD) > 0.0f)
      return false;
   
   if ( ((m_vcN * Vmax) + m_fD) >= 0.0f)
      return true;
  
   return false;
   } // Intersects(AABB)
/*----------------------------------------------------------------*/


// Intersection with OBB. Same as obb culling to frustrum planes.
bool ZFXPlane::Intersects(const ZFXObb &obb) {
    float fRadius = _fabs( obb.fA0 * (m_vcN * obb.vcA0) ) 
                  + _fabs( obb.fA1 * (m_vcN * obb.vcA1) ) 
                  + _fabs( obb.fA2 * (m_vcN * obb.vcA2) );

    float fDistance = this->Distance(obb.vcCenter);
    return (fDistance <= fRadius);
   } // Intersects(OBB)
/*----------------------------------------------------------------*/
