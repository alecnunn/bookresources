//--------------------------------------------------------------------------------------------------------------------//
//                                                VECTORS & MATRICES CLASSES                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VECTORMATRIX_INL_
#define   _GPG4_VECTORMATRIX_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <math.h>
#include  <memory.h>
#include  "MyAssert.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                2D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

inline CVect2D::CVect2D()                                                       // do nothing
  {}

inline CVect2D::CVect2D(const float fD)                                         // x=y=fD
  { x = y = fD; }

inline CVect2D::CVect2D(const float fX,const float fY)                          // x=fX; y=fY
  { x = fX; y = fY; }

inline CVect2D::CVect2D(const CVect2D& v2V)                                     // copy
  { x = v2V.x; y =v2V.y; }

inline CVect2D::CVect2D(const CVect3D& v3V,const ui uiDropAxis)                 // cast v3 to v2 (drop an axis)
  { 
  switch (uiDropAxis)
    { 
    case X  : x = v3V.y; y = v3V.z; break;
    case Y  : x = v3V.x; y = v3V.z; break;
    default : x = v3V.x; y = v3V.y; break; 
    } 
  }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CVect2D& CVect2D::operator  = (const CVect2D& v2V)                       // assignment
  { x  = v2V.x; y  = v2V.y; return *this; }

inline CVect2D& CVect2D::operator += (const CVect2D& v2V)                       // incrementation
  { x += v2V.x; y += v2V.y; return *this; }

inline CVect2D& CVect2D::operator -= (const CVect2D& v2V)                       // decrementation
  { x -= v2V.x; y -= v2V.y; return *this; }

inline CVect2D& CVect2D::operator *= (const float fK)                           // multiplication by a constant
  { x *= fK;    y *= fK;    return *this; }

inline CVect2D& CVect2D::operator *= (const CVect2D& v2V)                       // term by term *
  { x *= v2V.x; y *= v2V.y; return *this; }

inline CVect2D& CVect2D::operator /= (const float fK)                           // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    x *= fKinv; y *= fKinv; return *this; }

inline CVect2D& CVect2D::operator /= (const CVect2D& v2V)                       // term by term /
  { MY_ASSERT(v2V.x != 0.f,"division by zero !");
    MY_ASSERT(v2V.y != 0.f,"division by zero !");
    x /= v2V.x; y /= v2V.y; return *this; }

inline float&   CVect2D::operator [] (const ui uiIndex)                         // indexing
  { MY_ASSERT(uiIndex <= Y,"invalid index");
    return V [uiIndex]; }

inline const float CVect2D::operator [] (const ui uiIndex) const                // indexing
  { MY_ASSERT(uiIndex <= Y,"invalid index");
    return V [uiIndex]; }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CVect2D  operator  - (const CVect2D& v2V)                                // -V
  { return CVect2D(-v2V.x,-v2V.y); }

inline CVect2D  operator  * (const float fK,const CVect2D& v2V)                 // k*V
  { return CVect2D(fK*v2V.x,fK*v2V.y); }

inline CVect2D  operator  * (const CVect2D& v2V,const float fK)                 // V*k
  { return CVect2D(fK*v2V.x,fK*v2V.y); }

inline CVect2D  operator  / (const CVect2D& v2V,const float fK)                 // V/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CVect2D(fKinv*v2V.x,fKinv*v2V.y); }

inline CVect2D  operator  - (const CVect2D& v2V1,const CVect2D& v2V2)           // V1-V2
  { return CVect2D(v2V1.x-v2V2.x,v2V1.y-v2V2.y); }

inline CVect2D  operator  + (const CVect2D& v2V1,const CVect2D& v2V2)           // V1+V2
  { return CVect2D(v2V1.x+v2V2.x,v2V1.y+v2V2.y); }

inline float operator  * (const CVect2D& v2V1,const CVect2D& v2V2)              // V1*V2 (dot product)
  { return(v2V1.x*v2V2.x + v2V1.y*v2V2.y); }

inline float operator  ^ (const CVect2D& v2V1,const CVect2D& v2V2)              // V1^V2 (cross product)
  { return(v2V1.x*v2V2.y - v2V1.y*v2V2.x); }

inline bool  operator == (const CVect2D& v2V1,const CVect2D& v2V2)              // V1==V2
  { return((v2V1.x == v2V2.x) && (v2V1.y == v2V2.y)); }

inline bool  operator != (const CVect2D& v2V1,const CVect2D& v2V2)              // V1!=V2
  { return((v2V1.x != v2V2.x) || (v2V1.y != v2V2.y)); }

inline ostream& operator << (ostream& s,CVect2D& v2V)                           // output
  { return s << '(' << v2V.x << ';' << v2V.y << ')'; }

inline ostream& operator << (ostream& s,const CVect2D& v2V)                     // output
  { CVect2D v2Tmp(v2V);
    return s << '(' << v2Tmp.x << ';' << v2Tmp.y << ')'; }

//----------------------------------------------- friend functions ---------------------------------------------------//

inline void Swap(CVect2D& v2V1,CVect2D& v2V2)                                   // swap
  { CVect2D v2Tmp(v2V1); v2V1 = v2V2; v2V2 = v2Tmp; }

inline CVect2D Smallest(const CVect2D& v2V1,const CVect2D& v2V2)                // mini(V1,V2) (smallest vector)
  { float f1 = v2V1.Length2(); float f2 = v2V2.Length2();
    if(f1 < f2) return v2V1; else return v2V2; }

inline CVect2D Longest (const CVect2D& v2V1,const CVect2D& v2V2)                // maxi(V1,V2) (longest  vector) 
  { float f1 = v2V1.Length2(); float f2 = v2V2.Length2();
    if(f1 > f2) return v2V1; else return v2V2; }

inline CVect2D Prod    (const CVect2D& v2V1,const CVect2D& v2V2)                // term by term *
  { return CVect2D(v2V1.x*v2V2.x,v2V1.y*v2V2.y); }

inline CVect2D Div     (const CVect2D& v2V1,const CVect2D& v2V2)                // term by term /
  { MY_ASSERT(v2V2.x != 0.f,"division by zero !");
    MY_ASSERT(v2V2.y != 0.f,"division by zero !");
    return CVect2D(v2V1.x/v2V2.x,v2V1.y/v2V2.y); }

inline float Dot  (const CVect2D& v2V1,const CVect2D& v2V2)                     // dot product
  { return(v2V1.x*v2V2.x + v2V1.y*v2V2.y); }

inline float Cross(const CVect2D& v2V1,const CVect2D& v2V2)                     // cross product
  { return(v2V1.x*v2V2.y - v2V1.y*v2V2.x); }

//----------------------------------------------- special functions --------------------------------------------------//

inline void  CVect2D::Zero()                                                    // set to (0;0)
  { x = y = 0.f; }

inline float CVect2D::Length () const                                           // vector length d
  { return(float)sqrt(Length2()); }

inline float CVect2D::Length2() const                                           // vector squared length d^2
  { return(x*x + y*y); }

inline CVect2D CVect2D::Normalized() const                                      // return normalized copy
  { CVect2D v2Tmp(*this);
    v2Tmp.Normalize1();
    return v2Tmp; }

inline ui CVect2D::MainAxis() const                                             // main axis index
  { return(fabs(x) >= fabs(y) ? X : Y); }

inline bool CVect2D::Equals      (const CVect2D& v2V,const float fDelta) const  // == with a tolerance
  { return((fabs(x-v2V.x) <= fDelta) && (fabs(y-v2V.y) <= fDelta)); }

inline bool CVect2D::IsParallel  (const CVect2D& v2V,const float fDelta) const  // 'almost parallel' test
  { float fProd = Normalized()*v2V.Normalized();
    return(fabs(fProd) > 1.f-fDelta); }

inline bool CVect2D::IsOrthogonal(const CVect2D& v2V,const float fDelta) const  // 'almost orthogonal' test
  { float fProd = Normalized()*v2V.Normalized();
    return(fabs(fProd) < fDelta); }

inline void CVect2D::Rotate(const float fAngle)                                 // rotate by an angle
  {
  float fSin = (float)sin(fAngle);
  float fCos = (float)cos(fAngle);
  float fX   = x*fCos-y*fSin;
  y = x*fSin+y*fCos;
  x = fX;
  }

inline CVect2D CVect2D::Rotated(const float fAngle)                             // return rotated copy
  {
  float fSin = (float)sin(fAngle);
  float fCos = (float)cos(fAngle);
  return CVect2D(x*fCos-y*fSin,x*fSin+y*fCos);
  }

//----------------------------------------------- set ----------------------------------------------------------------//

inline void CVect2D::Set(const float fD)
  { x = y = fD; }

inline void CVect2D::Set(const float fX,const float fY)
  { x = fX; y = fY; }

inline void CVect2D::Set(const CVect2D& v2V)
  { x = v2V.x; y = v2V.y; }

inline void CVect2D::Set(const CVect3D& v3V,const ui uiDropAxis)
  { 
  switch (uiDropAxis)
    { 
    case X  : x = v3V.y; y = v3V.z; break;
    case Y  : x = v3V.x; y = v3V.z; break;
    default : x = v3V.x; y = v3V.y; break; 
    } 
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                3D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

inline CVect3D::CVect3D()                                                       // do nothing
  {}

inline CVect3D::CVect3D(const float fD)                                         // x=y=z=fD
  { x = y = z = fD; }

inline CVect3D::CVect3D(const float fX,const float fY,const float fZ)           // x=fX; y=fY; z=fZ
  { x = fX; y = fY; z = fZ; }

inline CVect3D::CVect3D(const CVect3D& v3V)                                     // copy
  { x = v3V.x; y = v3V.y; z = v3V.z; }

inline CVect3D::CVect3D(const CVect4D& v4V,const ui uiDropAxis)                 // cast v4 to v3 (drop an axis)
  { 
  switch (uiDropAxis)
    { 
    case X  : x = v4V.y; y = v4V.z; z = v4V.w; break;
    case Y  : x = v4V.x; y = v4V.z; z = v4V.w; break;
    case Z  : x = v4V.x; y = v4V.y; z = v4V.w; break;
    default : x = v4V.x; y = v4V.y; z = v4V.z; break; 
    } 
  }

inline CVect3D::CVect3D(const CVect2D& v2V,const float fZ)                      // cast v2 to v3; z=fZ
  { x = v2V.x; y = v2V.y; z = fZ; }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CVect3D& CVect3D::operator  = (const CVect3D& v3V)                       // assignment
  { x  = v3V.x; y  = v3V.y; z  = v3V.z; return *this; }

inline CVect3D& CVect3D::operator += (const CVect3D& v3V)                       // incrementation
  { x += v3V.x; y += v3V.y; z += v3V.z; return *this; }

inline CVect3D& CVect3D::operator -= (const CVect3D& v3V)                       // decrementation
  { x -= v3V.x; y -= v3V.y; z -= v3V.z; return *this; }

inline CVect3D& CVect3D::operator *= (const float fK)                           // multiplication by a constant
  { x *= fK;    y *= fK;    z *= fK;    return *this; }

inline CVect3D& CVect3D::operator *= (const CVect3D& v3V)                       // term by term *
  { x *= v3V.x; y *= v3V.y; z *= v3V.z; return *this; }

inline CVect3D& CVect3D::operator /= (const float fK)                           // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    x *= fKinv; y *= fKinv; z *= fKinv; return *this; }

inline CVect3D& CVect3D::operator /= (const CVect3D& v3V)                       // term by term /
  { MY_ASSERT(v3V.x != 0.f,"division by zero !");
    MY_ASSERT(v3V.y != 0.f,"division by zero !");
    MY_ASSERT(v3V.z != 0.f,"division by zero !");
    x /= v3V.x; y /= v3V.y; z /= v3V.z; return *this; }

inline float&   CVect3D::operator [] (const ui uiIndex)                         // indexing
  {
  MY_ASSERT(uiIndex <= Z,"invalid index");
  return V [uiIndex]; 
  }

inline const float CVect3D::operator [] (const ui uiIndex) const                // indexing
  {
  MY_ASSERT(uiIndex <= Z,"invalid index");
  return V [uiIndex]; 
  }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CVect3D  operator  - (const CVect3D& v3V)                                // -V
  { return CVect3D(-v3V.x,-v3V.y,-v3V.z); }

inline CVect3D  operator  * (const float fK,const CVect3D& v3V)                 // k*V
  { return CVect3D(fK*v3V.x,fK*v3V.y,fK*v3V.z); }

inline CVect3D  operator  * (const CVect3D& v3V,const float fK)                 // V*k
  { return CVect3D(fK*v3V.x,fK*v3V.y,fK*v3V.z); }

inline CVect3D  operator  / (const CVect3D& v3V,const float fK)                 // V/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CVect3D(fKinv*v3V.x,fKinv*v3V.y,fKinv*v3V.z); }

inline CVect3D  operator  - (const CVect3D& v3V1,const CVect3D& v3V2)           // V1-V2
  { return CVect3D(v3V1.x-v3V2.x,v3V1.y-v3V2.y,v3V1.z-v3V2.z); }

inline CVect3D  operator  + (const CVect3D& v3V1,const CVect3D& v3V2)           // V1+V2
  { return CVect3D(v3V1.x+v3V2.x,v3V1.y+v3V2.y,v3V1.z+v3V2.z); }

inline float    operator  * (const CVect3D& v3V1,const CVect3D& v3V2)           // V1*V2 (dot product)
  { return(v3V1.x*v3V2.x + v3V1.y*v3V2.y + v3V1.z*v3V2.z); }

inline CVect3D  operator  ^ (const CVect3D& v3V1,const CVect3D& v3V2)           // V1^V2 (cross product)
  { return CVect3D(v3V1.y*v3V2.z - v3V1.z*v3V2.y,
                   v3V1.z*v3V2.x - v3V1.x*v3V2.z,
                   v3V1.x*v3V2.y - v3V1.y*v3V2.x); }

inline bool     operator == (const CVect3D& v3V1,const CVect3D& v3V2)           // V1==V2
  { return((v3V1.x == v3V2.x) && (v3V1.y == v3V2.y) && (v3V1.z == v3V2.z)); }

inline bool     operator != (const CVect3D& v3V1,const CVect3D& v3V2)           // V1!=V2
  { return((v3V1.x != v3V2.x) || (v3V1.y != v3V2.y) || (v3V1.z != v3V2.z)); }

inline CVect3D  operator  * (const CMat3x3& m3M, const CVect3D& v3V)            // M3*V3
  { return CVect3D(m3M[0][0]*v3V[0] + m3M[1][0]*v3V[1] + m3M[2][0]*v3V[2],
                   m3M[0][1]*v3V[0] + m3M[1][1]*v3V[1] + m3M[2][1]*v3V[2],
                   m3M[0][2]*v3V[0] + m3M[1][2]*v3V[1] + m3M[2][2]*v3V[2]); }

inline CVect3D  operator  * (const CVect3D& v3V, const CMat3x3& m3M)            // V3*M3
  { return CVect3D(m3M[0][0]*v3V[0] + m3M[0][1]*v3V[1] + m3M[0][2]*v3V[2],
                   m3M[1][0]*v3V[0] + m3M[1][1]*v3V[1] + m3M[1][2]*v3V[2],
                   m3M[2][0]*v3V[0] + m3M[2][1]*v3V[1] + m3M[2][2]*v3V[2]); }

inline CVect4D  operator  * (const CMat4x4& m4M, const CVect3D& v3V)            // M4*V3 (extended with 1.f)
  { return CVect4D(m4M[0][0]*v3V[0] + m4M[1][0]*v3V[1] + m4M[2][0]*v3V[2] + m4M[3][0],
                   m4M[0][1]*v3V[0] + m4M[1][1]*v3V[1] + m4M[2][1]*v3V[2] + m4M[3][1],
                   m4M[0][2]*v3V[0] + m4M[1][2]*v3V[1] + m4M[2][2]*v3V[2] + m4M[3][2],
                   m4M[0][3]*v3V[0] + m4M[1][3]*v3V[1] + m4M[2][3]*v3V[2] + m4M[3][3]); }


inline CVect4D  operator  * (const CVect3D& v3V, const CMat4x4& m4M)            // V3*M4
  { return CVect4D(m4M[0][0]*v3V[0] + m4M[0][1]*v3V[1] + m4M[0][2]*v3V[2] + m4M[0][3],
                   m4M[1][0]*v3V[0] + m4M[1][1]*v3V[1] + m4M[1][2]*v3V[2] + m4M[1][3],
                   m4M[2][0]*v3V[0] + m4M[2][1]*v3V[1] + m4M[2][2]*v3V[2] + m4M[2][3],
                   m4M[3][0]*v3V[0] + m4M[3][1]*v3V[1] + m4M[3][2]*v3V[2] + m4M[3][3]); }

inline float    operator  * (const CVect4D& v4V, const CVect3D& v3V)            // V4*V3
  { return(v3V.x*v4V.x + v3V.y*v4V.y + v3V.z*v4V.z + v4V.w); }

inline float    operator  * (const CVect3D& v3V, const CVect4D& v4V)            // V3*V4
  { return(v3V.x*v4V.x + v3V.y*v4V.y + v3V.z*v4V.z + v4V.w); }

inline ostream& operator << (ostream& s,CVect3D& v3V)                           // output
  { return s << '(' << v3V.x << ';' << v3V.y << ';' << v3V.z << ')'; }

inline ostream& operator << (ostream& s,const CVect3D& v3V)                     // output
  { CVect3D v3Tmp(v3V);
    return s << '(' << v3Tmp.x << ';' << v3Tmp.y << ';' << v3Tmp.z << ')'; }

//----------------------------------------------- friend functions ---------------------------------------------------//

inline void Swap(CVect3D& v3V1,CVect3D& v3V2)                                   // swap
  { CVect3D v3Tmp(v3V1); v3V1 = v3V2; v3V2 = v3Tmp; }

inline CVect3D Smallest(const CVect3D& v3V1,const CVect3D& v3V2)                // mini(V1,V2) (smallest vector)
  { 
  float f1 = v3V1.Length2(); float f2 = v3V2.Length2();
  if(f1 < f2) return v3V1; else return v3V2; 
  }

inline CVect3D Longest (const CVect3D& v3V1,const CVect3D& v3V2)                // maxi(V1,V2) (longest  vector) 
  { 
  float f1 = v3V1.Length2(); float f2 = v3V2.Length2();
  if(f1 > f2) return v3V1; else return v3V2; 
  }

inline CVect3D Prod    (const CVect3D& v3V1,const CVect3D& v3V2)                // term by term *
  { return CVect3D(v3V1.x*v3V2.x,v3V1.y*v3V2.y,v3V1.z*v3V2.z); }

inline CVect3D Div     (const CVect3D& v3V1,const CVect3D& v3V2)                // term by term /
  { MY_ASSERT(v3V2.x != 0.f,"division by zero !");
    MY_ASSERT(v3V2.y != 0.f,"division by zero !");
    MY_ASSERT(v3V2.z != 0.f,"division by zero !");
    return CVect3D(v3V1.x/v3V2.x,v3V1.y/v3V2.y,v3V1.z/v3V2.z); }

inline float   Dot     (const CVect3D& v3V1,const CVect3D& v3V2)                // dot product
  { return(v3V1.x*v3V2.x + v3V1.y*v3V2.y + v3V1.z*v3V2.z); }

inline CVect3D Cross   (const CVect3D& v3V1,const CVect3D& v3V2)                // cross product
  { return CVect3D(v3V1.y*v3V2.z - v3V1.z*v3V2.y,
                   v3V1.z*v3V2.x - v3V1.x*v3V2.z,
                   v3V1.x*v3V2.y - v3V1.y*v3V2.x); }

inline CVect3D SwapAxis(CVect3D& v3V,const si siX,const si siY,const si siZ)    // exchange some axis (3D vector)
  { 
  float fX,fY,fZ;                                                     
  if(siX >= 0) fX = +v3V[siX]; else fX = -v3V[-siX];
  if(siY >= 0) fY = +v3V[siY]; else fY = -v3V[-siY];
  if(siZ >= 0) fZ = +v3V[siZ]; else fZ = -v3V[-siZ];
  return CVect3D(fX,fY,fZ); 
  }
  
//----------------------------------------------- special functions --------------------------------------------------//

inline void  CVect3D::Zero()                                                    // set to (0;0;0)
  { x = y = z = 0.f; }

inline float CVect3D::Length () const                                           // vector length d
  { return(float)sqrt(Length2()); }

inline float CVect3D::Length2() const                                           // vector squared length d^2
  { return(x*x + y*y + z*z); }

inline CVect3D CVect3D::Normalized() const                                      // return normalized copy
  {
  CVect3D v3Tmp(*this);
  v3Tmp.Normalize1();
  return v3Tmp;
  }

inline bool CVect3D::Equals      (const CVect3D& v3V,const float fDelta) const  // == with a tolerance
  { return((fabs(x-v3V.x) <= fDelta) && (fabs(y-v3V.y) <= fDelta) && (fabs(z-v3V.z) <= fDelta)); }

inline bool CVect3D::IsParallel  (const CVect3D& v3V,const float fDelta) const  // 'almost parallel' test
  { float fProd = Normalized()*v3V.Normalized();
    return(fabs(fProd) > 1.f-fDelta); }

inline bool CVect3D::IsOrthogonal(const CVect3D& v3V,const float fDelta) const  // 'almost orthogonal' test
  { float fProd = Normalized()*v3V.Normalized();
    return(fabs(fProd) < fDelta); }

inline void CVect3D::RotateX(const float fAngle)                                // rotate by an angle around X
  {
  float fSin = (float)sin(fAngle);
  float fCos = (float)cos(fAngle);
  float fY   = y*fCos-z*fSin;
  z = y*fSin+z*fCos;
  y = fY;
  }

inline void CVect3D::RotateY(const float fAngle)                                // rotate by an angle around Y
  {
  float fSin = (float)sin(fAngle);
  float fCos = (float)cos(fAngle);
  float fX   = x*fCos+z*fSin;
  z = -x*fSin+z*fCos;
  x = fX;
  }

inline void CVect3D::RotateZ(const float fAngle)                                // rotate by an angle around Z
  {
  float fSin = (float)sin(fAngle);
  float fCos = (float)cos(fAngle);
  float fX   = x*fCos-y*fSin;
  y = x*fSin+y*fCos;
  x = fX;
  }

//----------------------------------------------- set ----------------------------------------------------------------//

inline void CVect3D::Set(const float fD)
  { x = y = z = fD; }

inline void CVect3D::Set(const float fX,const float fY,const float fZ)
  { x = fX; y = fY; z = fZ; }

inline void CVect3D::Set(const CVect3D& v3V)
  { x = v3V.x; y = v3V.y; z = v3V.z; }

inline void CVect3D::Set(const CVect4D& v4V,const ui uiDropAxis)
  { 
  switch (uiDropAxis)
    { 
    case X  : x = v4V.y; y = v4V.z; z = v4V.w; break;
    case Y  : x = v4V.x; y = v4V.z; z = v4V.w; break;
    case Z  : x = v4V.x; y = v4V.y; z = v4V.w; break;
    default : x = v4V.x; y = v4V.y; z = v4V.z; break; 
    } 
  }

inline void CVect3D::Set(const CVect2D& v2V,const float fZ)
  { x = v2V.x; y = v2V.y; z = fZ; }

//--------------------------------------------------------------------------------------------------------------------//
//                                                4D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

inline CVect4D::CVect4D()                                                       // do nothing
  {}

inline CVect4D::CVect4D(const float fD)                                         // x=y=z=w=fD
  { x = y = z = w = fD; }

inline CVect4D::CVect4D(const float fX,const float fY,const float fZ,const float fW)
  { x = fX; y = fY; z = fZ; w = fW; }                                           // x=fX; y=fY; z=fZ; w=fW

inline CVect4D::CVect4D(const CVect4D& v4V)                                     // copy
  { x = v4V.x; y = v4V.y; z = v4V.z; w = v4V.w; }

inline CVect4D::CVect4D(const CVect3D& v3V,const float fW)                      // cast v3 to v4; w=fW
  { x = v3V.x; y = v3V.y; z = v3V.z; w = fW; }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CVect4D& CVect4D::operator  = (const CVect4D& v4V)                       // assignment
  { x  = v4V.x; y  = v4V.y; z  = v4V.z; w  = v4V.w; return *this; }

inline CVect4D& CVect4D::operator += (const CVect4D& v4V)                       // incrementation
  { x += v4V.x; y += v4V.y; z += v4V.z; w += v4V.w; return *this; }

inline CVect4D& CVect4D::operator -= (const CVect4D& v4V)                       // decrementation
  { x -= v4V.x; y -= v4V.y; z -= v4V.z; w -= v4V.w; return *this; }

inline CVect4D& CVect4D::operator *= (const float fK)                           // multiplication by a constant
  { x *= fK;    y *= fK;    z *= fK;    w *= fK;    return *this; }

inline CVect4D& CVect4D::operator *= (const CVect4D& v4V)                       // term by term *
  { x *= v4V.x; y *= v4V.y; z *= v4V.z; w *= v4V.w; return *this; }

inline CVect4D& CVect4D::operator /= (const float fK)                           // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    x *= fKinv; y *= fKinv; z *= fKinv; w *= fKinv; return *this; }

inline CVect4D& CVect4D::operator /= (const CVect4D& v4V)                       // term by term /
  { MY_ASSERT(v4V.x != 0.f,"division by zero !");
    MY_ASSERT(v4V.y != 0.f,"division by zero !");
    MY_ASSERT(v4V.z != 0.f,"division by zero !");
    MY_ASSERT(v4V.w != 0.f,"division by zero !");
    x /= v4V.x; y /= v4V.y; z /= v4V.z; w /= v4V.w; return *this; }

inline float&   CVect4D::operator [] (const ui uiIndex)                         // indexing
  {
  MY_ASSERT(uiIndex <= W,"invalid index");
  return V [uiIndex]; 
  }

inline const float CVect4D::operator [] (const ui uiIndex) const                // indexing
  {
  MY_ASSERT(uiIndex <= W,"invalid index");
  return V [uiIndex]; 
  }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CVect4D  operator  - (const CVect4D& v4V)                                // -V
  { return CVect4D(-v4V.x,-v4V.y,-v4V.z,-v4V.w); }

inline CVect4D  operator  * (const float fK,const CVect4D& v4V)                 // k*V
  { return CVect4D(fK*v4V.x,fK*v4V.y,fK*v4V.z,fK*v4V.w); }

inline CVect4D  operator  * (const CVect4D& v4V,const float fK)                 // V*k
  { return CVect4D(fK*v4V.x,fK*v4V.y,fK*v4V.z,fK*v4V.w); }

inline CVect4D  operator  / (const CVect4D& v4V,const float fK)                 // V/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CVect4D(fKinv*v4V.x,fKinv*v4V.y,fKinv*v4V.z,fKinv*v4V.w); }

inline CVect4D  operator  - (const CVect4D& v4V1,const CVect4D& v4V2)           // V1-V2
  { return CVect4D(v4V1.x-v4V2.x,v4V1.y-v4V2.y,v4V1.z-v4V2.z,v4V1.w-v4V2.w); }

inline CVect4D  operator  + (const CVect4D& v4V1,const CVect4D& v4V2)           // V1+V2
  { return CVect4D(v4V1.x+v4V2.x,v4V1.y+v4V2.y,v4V1.z+v4V2.z,v4V1.w+v4V2.w); }

inline float    operator  * (const CVect4D& v4V1,const CVect4D& v4V2)           // V1*V2 (dot product)
  { return(v4V1.x*v4V2.x + v4V1.y*v4V2.y + v4V1.z*v4V2.z + v4V1.w*v4V2.w); }

inline bool     operator == (const CVect4D& v4V1,const CVect4D& v4V2)           // V1==V2
  { return((v4V1.x == v4V2.x) && (v4V1.y == v4V2.y) && (v4V1.z == v4V2.z) && (v4V1.w == v4V2.w)); }

inline bool     operator != (const CVect4D& v4V1,const CVect4D& v4V2)           // V1!=V2
  { return((v4V1.x != v4V2.x) || (v4V1.y != v4V2.y) || (v4V1.z != v4V2.z) || (v4V1.w != v4V2.w)); }

inline CVect4D  operator  * (const CMat4x4& m4M, const CVect4D& v4V)            // M4*V4
  { return CVect4D(m4M[0][0]*v4V[0] + m4M[1][0]*v4V[1] + m4M[2][0]*v4V[2] + m4M[3][0]*v4V[3],
                   m4M[0][1]*v4V[0] + m4M[1][1]*v4V[1] + m4M[2][1]*v4V[2] + m4M[3][1]*v4V[3],
                   m4M[0][2]*v4V[0] + m4M[1][2]*v4V[1] + m4M[2][2]*v4V[2] + m4M[3][2]*v4V[3],
                   m4M[0][3]*v4V[0] + m4M[1][3]*v4V[1] + m4M[2][3]*v4V[2] + m4M[3][3]*v4V[3]); }

inline CVect4D  operator  * (const CVect4D& v4V, const CMat4x4& m4M)            // V4*M4
  { return CVect4D(m4M[0][0]*v4V[0] + m4M[0][1]*v4V[1] + m4M[0][2]*v4V[2] + m4M[0][3]*v4V[3],
                   m4M[1][0]*v4V[0] + m4M[1][1]*v4V[1] + m4M[1][2]*v4V[2] + m4M[1][3]*v4V[3],
                   m4M[2][0]*v4V[0] + m4M[2][1]*v4V[1] + m4M[2][2]*v4V[2] + m4M[2][3]*v4V[3],
                   m4M[3][0]*v4V[0] + m4M[3][1]*v4V[1] + m4M[3][2]*v4V[2] + m4M[3][3]*v4V[3]); }

inline ostream& operator << (ostream& s,CVect4D& v4V)                           // output
  { return s << '(' << v4V.x << ';' << v4V.y << ';' << v4V.z << ';' << v4V.w << ')'; }

inline ostream& operator << (ostream& s,const CVect4D& v4V)                     // output
  { CVect4D v4Tmp(v4V);
    return s << '(' << v4Tmp.x << ';' << v4Tmp.y << ';' << v4Tmp.z << ';' << v4Tmp.w << ')'; }

//----------------------------------------------- friend functions ---------------------------------------------------//

inline void Swap(CVect4D& v4V1,CVect4D& v4V2)                                   // swap
  { CVect4D v4Tmp(v4V1); v4V1 = v4V2; v4V2 = v4Tmp; }

inline CVect4D Smallest(const CVect4D& v4V1,const CVect4D& v4V2)                // mini(V1,V2) (smallest vector) 
  { 
  float f1 = v4V1.Length2(); float f2 = v4V2.Length2();
  if(f1 < f2) return v4V1; else return v4V2; 
  }

inline CVect4D Longest (const CVect4D& v4V1,const CVect4D& v4V2)                // maxi(V1,V2) (longest  vector) 
  { 
  float f1 = v4V1.Length2(); float f2 = v4V2.Length2();
  if(f1 > f2) return v4V1; else return v4V2; 
  }

inline CVect4D Prod    (const CVect4D& v4V1,const CVect4D& v4V2)                // term by term *
  { return CVect4D(v4V1.x*v4V2.x,v4V1.y*v4V2.y,v4V1.z*v4V2.z,v4V1.w*v4V2.w); }

inline CVect4D Div     (const CVect4D& v4V1,const CVect4D& v4V2)                // term by term /
  { MY_ASSERT(v4V2.x != 0.f,"division by zero !");
    MY_ASSERT(v4V2.y != 0.f,"division by zero !");
    MY_ASSERT(v4V2.z != 0.f,"division by zero !");
    MY_ASSERT(v4V2.w != 0.f,"division by zero !");
    return CVect4D(v4V1.x/v4V2.x,v4V1.y/v4V2.y,v4V1.z/v4V2.z,v4V1.w/v4V2.w); }

inline float   Dot     (const CVect4D& v4V1,const CVect4D& v4V2)                // dot product
  { return(v4V1.x*v4V2.x + v4V1.y*v4V2.y + v4V1.z*v4V2.z + v4V1.w*v4V2.w); }

inline CVect4D SwapAxis(CVect4D& v4V,const si siX,const si siY,const si siZ,const si siW)
  {                                                                             // exchange some axis (3D vector)
  float fX,fY,fZ,fW;                                                     
  if(siX >= 0) fX = +v4V[siX]; else fX = -v4V[-siX];
  if(siY >= 0) fY = +v4V[siY]; else fY = -v4V[-siY];
  if(siZ >= 0) fZ = +v4V[siZ]; else fZ = -v4V[-siZ];
  if(siW >= 0) fW = +v4V[siW]; else fW = -v4V[-siW];
  return CVect4D(fX,fY,fZ,fW); 
  }

//----------------------------------------------- special functions --------------------------------------------------//

inline void  CVect4D::Zero()                                                    // set to (0;0;0;0)
  { x = y = z = w = 0.f; }

inline float CVect4D::Length () const                                           // vector length d
  { return(float)sqrt(Length2()); }

inline float CVect4D::Length2() const                                           // vector squared length d^2
  { return(x*x + y*y + z*z + w*w); }

inline CVect4D CVect4D::Normalized() const                                      // return normalized copy
  {
  CVect4D v4Tmp(*this);
  v4Tmp.Normalize1();
  return v4Tmp;
  }

inline bool CVect4D::Equals      (const CVect4D& v4V,const float fDelta) const  // == with a tolerance
  { return((fabs(x-v4V.x) <= fDelta) && (fabs(y-v4V.y) <= fDelta) && (fabs(z-v4V.z) <= fDelta) && (fabs(w-v4V.w) <= fDelta)); }

inline bool CVect4D::IsParallel  (const CVect4D& v4V,const float fDelta) const  // 'almost parallel' test
  { float fProd = Normalized()*v4V.Normalized();
    return(fabs(fProd) > 1.f-fDelta); }

inline bool CVect4D::IsOrthogonal(const CVect4D& v4V,const float fDelta) const  // 'almost orthogonal' test
  { float fProd = Normalized()*v4V.Normalized();
    return(fabs(fProd) < fDelta); }

//----------------------------------------------- set ----------------------------------------------------------------//

inline void CVect4D::Set(const float fD)
  { x = y = z = w = fD; }

inline void CVect4D::Set(const float fX,const float fY,const float fZ,const float fW)
  { x = fX; y = fY; z = fZ; w = fW; }

inline void CVect4D::Set(const CVect4D& v4V)
  { x = v4V.x; y = v4V.y; z = v4V.z; w = v4V.w; }

inline void CVect4D::Set(const CVect3D& v3V,const float fW)
  { x = v3V.x; y = v3V.y; z = v3V.z; w = fW; }

//--------------------------------------------------------------------------------------------------------------------//
//                                                3*3 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

inline CMat3x3::CMat3x3()                                                       // do nothing
  {}

inline CMat3x3::CMat3x3(const float fD)                                         // all values=fD
  { Col[0] = Col[1] = Col[2] = CVect3D(fD); }

inline CMat3x3::CMat3x3(const CVect3D& m_v3V0,const CVect3D& m_v3V1,const CVect3D& m_v3V2)
  { Col[0] = m_v3V0; Col[1] = m_v3V1; Col[2] = m_v3V2; }                        // 3 columns of the matrix

inline CMat3x3::CMat3x3(const CMat3x3& m3M)                                     // copy
  { memcpy(Col,m3M.Col,sizeof(Col)); }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CMat3x3& CMat3x3::operator  = (const CMat3x3& m3M)                       // assignment
  { memcpy(Col,m3M.Col,sizeof(Col)); return *this; }

inline CMat3x3& CMat3x3::operator += (const CMat3x3& m3M)                       // incrementation
  { Col[0] += m3M.Col[0]; Col[1]  += m3M.Col[1];
    Col[2] += m3M.Col[2]; return *this; }

inline CMat3x3& CMat3x3::operator -= (const CMat3x3& m3M)                       // decrementation
  { Col[0] -= m3M.Col[0]; Col[1]  -= m3M.Col[1];
    Col[2] -= m3M.Col[2]; return *this; }

inline CMat3x3& CMat3x3::operator *= (const float fK)                           // multiplication by a constant
  { Col[0] *= fK;         Col[1]  *= fK;
    Col[2] *= fK;         return *this; }

inline CMat3x3& CMat3x3::operator *= (const CMat3x3& m3M)                       // multiplication
  { *this = (*this)*m3M;  return *this; }

inline CMat3x3& CMat3x3::operator /= (const float fK)                           // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    Col[0] *= fKinv;      Col[1]  *= fKinv;
    Col[2] *= fKinv;      return *this; }

inline CMat3x3& CMat3x3::operator /= (const CMat3x3& m3M)                       // multiplication by inverse
  { *this = (*this)*m3M.Inverted(); return *this; }

inline CVect3D& CMat3x3::operator [] (const ui uiIndex)                         // indexing column
  {
  MY_ASSERT( uiIndex <= Z,"invalid index");
  return Col[uiIndex]; 
  }

inline const CVect3D CMat3x3::operator [] (const ui uiIndex) const              // indexing column
  {
  MY_ASSERT( uiIndex <= Z,"invalid index");
  return Col[uiIndex]; 
  }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CMat3x3  operator  - (const CMat3x3& m3M)                                // -M
  { return CMat3x3(-m3M.Col[0],-m3M.Col[1],-m3M.Col[2]); }

inline CMat3x3  operator  * (const float fK,const CMat3x3& m3M)                 // k*M
  { return CMat3x3(fK*m3M.Col[0],fK*m3M.Col[1],fK*m3M.Col[2]); }

inline CMat3x3  operator  * (const CMat3x3& m3M,const float fK)                 // M*k
  { return CMat3x3(fK*m3M.Col[0],fK*m3M.Col[1],fK*m3M.Col[2]); }

inline CMat3x3  operator  / (const CMat3x3& m3M,const float fK)                 // M/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CMat3x3(fKinv*m3M.Col[0],fKinv*m3M.Col[1],fKinv*m3M.Col[2]); }

inline CMat3x3  operator  - (const CMat3x3& m3M1,const CMat3x3& m3M2)           // M1-M2
  { return CMat3x3(m3M1.Col[0]-m3M2.Col[0],m3M1.Col[1]-m3M2.Col[1],m3M1.Col[2]-m3M2.Col[2]); }

inline CMat3x3  operator  + (const CMat3x3& m3M1,const CMat3x3& m3M2)           // M1+M2
  { return CMat3x3(m3M1.Col[0]+m3M2.Col[0],m3M1.Col[1]+m3M2.Col[1],m3M1.Col[2]+m3M2.Col[2]); }

inline CMat3x3  operator  * (const CMat3x3& m3M1,const CMat3x3& m3M2)           // M1*M2
  { return CMat3x3(m3M1*m3M2.Col[0],m3M1*m3M2.Col[1],m3M1*m3M2.Col[2]); }

inline bool     operator == (const CMat3x3& m3M1,const CMat3x3& m3M2)           // M1==M2
  { return ((m3M1.Col[0] == m3M2.Col[0]) && (m3M1.Col[1] == m3M2.Col[1]) && 
            (m3M1.Col[2] == m3M2.Col[2])); }

inline bool     operator != (const CMat3x3& m3M1,const CMat3x3& m3M2)           // M1!=M2
  { return ((m3M1.Col[0] != m3M2.Col[0]) || (m3M1.Col[1] != m3M2.Col[1]) || 
            (m3M1.Col[2] != m3M2.Col[2])); }

inline ostream& operator << (ostream& s,CMat3x3& m3M)                           // output
  { CMat3x3 m3Tmp = m3M.Transposed();
    return s << '\n' << m3Tmp[0] << '\n' << m3Tmp[1] << '\n' << m3Tmp[2] << '\n'; }

inline ostream& operator << (ostream& s,const CMat3x3& m3M)                     // output
  { CMat3x3 m3Tmp = m3M.Transposed();
    return s << '\n' << m3Tmp[0] << '\n' << m3Tmp[1] << '\n' << m3Tmp[2] << '\n'; }

//----------------------------------------------- friend functions ---------------------------------------------------//

inline void Swap(CMat3x3& m3M1,CMat3x3& m3M2)                                   // swap
  { CMat3x3 m3Tmp(m3M1); m3M1 = m3M2; m3M2 = m3Tmp; }

//----------------------------------------------- special functions --------------------------------------------------//

inline void CMat3x3::Zero()                                                     // zeros everywhere
  { Col[0].Zero(); Col[1].Zero(); Col[2].Zero(); }

inline void CMat3x3::SetIdentity()                                              // set to identity
  { *this = Identity; }

inline CMat3x3 CMat3x3::Transposed() const                                      // return transposed copy
  { CMat3x3 m3Tmp(*this);
    m3Tmp.Transpose();
    return m3Tmp; }

inline void CMat3x3::Invert()                                                   // invert
  { *this = Inverted(); }

inline void CMat3x3::Translation(const CVect2D& v2V)                            // translation
  { Col[0].Set(1.f,  0.f,  0.f);
    Col[1].Set(0.f,  1.f,  0.f);
    Col[2].Set(v2V.x,v2V.y,1.f); }

inline void CMat3x3::Scale(const CVect3D& v3Scale)                              // scale
  { Col[0].Set(v3Scale.x,0.f,0.f);
    Col[1].Set(0.f,v3Scale.y,0.f);
    Col[2].Set(0.f,0.f,v3Scale.z); }

//----------------------------------------------- set ----------------------------------------------------------------//

inline void CMat3x3::Set(const float fD)
  { Col[0] = Col[1] = Col[2] = CVect3D(fD); }

inline void CMat3x3::Set(const CVect3D& v3V0,const CVect3D& v3V1,const CVect3D& v3V2)
  { Col[0] = v3V0; Col[1] = v3V1; Col[2] = v3V2; }

inline void CMat3x3::Set(const CMat3x3& m3M)
  { memcpy(Col,m3M.Col,sizeof(Col)); }

//--------------------------------------------------------------------------------------------------------------------//
//                                                4*4 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

inline CMat4x4::CMat4x4()                                                       // do nothing
  {}

inline CMat4x4::CMat4x4(const float fD)                                         // all values=fD
  { Col[0] = Col[1] = Col[2] = Col[3] = CVect4D(fD); }

inline CMat4x4::CMat4x4(const CVect4D& v4V0,const CVect4D& v4V1,const CVect4D& v4V2,const CVect4D& v4V3)
  { Col[0] = v4V0; Col[1] = v4V1; Col[2] = v4V2; Col[3] = v4V3; }               // 4 columns of the matrix

inline CMat4x4::CMat4x4(const CMat4x4& m4M)                                     // copy
  { memcpy(Col,m4M.Col,sizeof(Col)); }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CMat4x4& CMat4x4::operator  = (const CMat4x4& m4M)                       // assignment
  { memcpy(Col,m4M.Col,sizeof(Col)); return *this; }

inline CMat4x4& CMat4x4::operator += (const CMat4x4& m4M)                       // incrementation
  { Col[0] += m4M.Col[0]; Col[1]  += m4M.Col[1];
    Col[2] += m4M.Col[2]; Col[3]  += m4M.Col[3]; return *this; }

inline CMat4x4& CMat4x4::operator -= (const CMat4x4& m4M)                       // decrementation
  { Col[0] -= m4M.Col[0]; Col[1]  -= m4M.Col[1];
    Col[2] -= m4M.Col[2]; Col[3]  -= m4M.Col[3]; return *this; }

inline CMat4x4& CMat4x4::operator *= (const float fK)                           // multiplication by a constant
  { Col[0] *= fK;         Col[1]  *= fK;
    Col[2] *= fK;         Col[2]  *= fK; return *this; }

inline CMat4x4& CMat4x4::operator *= (const CMat4x4& m4M)                       // multiplication
  { *this = (*this)*m4M;  return *this; }

inline CMat4x4& CMat4x4::operator /= (const float fK)                           // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    Col[0] *= fKinv;      Col[1]  *= fKinv;
    Col[2] *= fKinv;      Col[2]  *= fKinv; return *this; }

inline CMat4x4& CMat4x4::operator /= (const CMat4x4& m4M)                       // multiplication by inverse
  { *this = (*this)*m4M.Inverted(); return *this; }

inline CVect4D& CMat4x4::operator [] (const ui uiIndex)                         // indexing columns
  {
  MY_ASSERT( uiIndex <= W,"invalid index");
  return Col[uiIndex]; 
  }

inline const CVect4D CMat4x4::operator [] (const ui uiIndex) const              // indexing column
  {
  MY_ASSERT( uiIndex <= W,"invalid index");
  return Col[uiIndex]; 
  }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CMat4x4 operator  - (const CMat4x4& m4M)                                 // -M
  { return CMat4x4(-m4M.Col[0],-m4M.Col[1],-m4M.Col[2],-m4M.Col[3]); }

inline CMat4x4 operator  * (const float fK,const CMat4x4& m4M)                  // k*M
  { return CMat4x4(fK*m4M.Col[0],fK*m4M.Col[1],fK*m4M.Col[2],fK*m4M.Col[3]); }

inline CMat4x4 operator  * (const CMat4x4& m4M,const float fK)                  // M*k
  { return CMat4x4(fK*m4M.Col[0],fK*m4M.Col[1],fK*m4M.Col[2],fK*m4M.Col[3]); }

inline CMat4x4 operator  / (const CMat4x4& m4M,const float fK)                  // M/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CMat4x4(fKinv*m4M.Col[0],fKinv*m4M.Col[1],fKinv*m4M.Col[2],fKinv*m4M.Col[3]); }

inline CMat4x4 operator  - (const CMat4x4& m4M1,const CMat4x4& m4M2)            // M1-M2
  { return CMat4x4(m4M1.Col[0]-m4M2.Col[0],m4M1.Col[1]-m4M2.Col[1],m4M1.Col[2]-m4M2.Col[2],m4M1.Col[3]-m4M2.Col[3]); }

inline CMat4x4 operator  + (const CMat4x4& m4M1,const CMat4x4& m4M2)            // M1+M2
  { return CMat4x4(m4M1.Col[0]+m4M2.Col[0],m4M1.Col[1]+m4M2.Col[1],m4M1.Col[2]+m4M2.Col[2],m4M1.Col[3]+m4M2.Col[3]); }

inline CMat4x4 operator  * (const CMat4x4& m4M1,const CMat4x4& m4M2)            // M1*M2
  { return CMat4x4(m4M1*m4M2.Col[0],m4M1*m4M2.Col[1],m4M1*m4M2.Col[2],m4M1*m4M2.Col[3]); }

inline bool     operator == (const CMat4x4& m4M1,const CMat4x4& m4M2)           // M1==M2
  { return ((m4M1.Col[0] == m4M2.Col[0]) && (m4M1.Col[1] == m4M2.Col[1]) && 
            (m4M1.Col[2] == m4M2.Col[2]) && (m4M1.Col[3] == m4M2.Col[3])); }

inline bool     operator != (const CMat4x4& m4M1,const CMat4x4& m4M2)           // M1!=M2
  { return ((m4M1.Col[0] != m4M2.Col[0]) || (m4M1.Col[1] != m4M2.Col[1]) || 
            (m4M1.Col[2] != m4M2.Col[2]) || (m4M1.Col[3] != m4M2.Col[3])); }

inline ostream& operator << (ostream& s,CMat4x4& m4M)                           // output
  { CMat4x4 m4Tmp = m4M.Transposed();
    return s << '\n' << m4Tmp[0] << '\n' << m4Tmp[1] << '\n' << m4Tmp[2] << '\n' << m4Tmp[3] << '\n'; }

inline ostream& operator << (ostream& s,const CMat4x4& m4M)                     // output
  { CMat4x4 m4Tmp = m4M.Transposed();
    return s << '\n' << m4Tmp[0] << '\n' << m4Tmp[1] << '\n' << m4Tmp[2] << '\n' << m4Tmp[3] << '\n'; }

//----------------------------------------------- friend functions ---------------------------------------------------//

inline void Swap(CMat4x4& m4M1,CMat4x4& m4M2)                                   // swap
  { CMat4x4 m4Tmp(m4M1); m4M1 = m4M2; m4M2 = m4Tmp; }

//----------------------------------------------- special functions --------------------------------------------------//

inline void CMat4x4::Zero()                                                     // zeros everywhere
  { Col[0].Zero(); Col[1].Zero(); Col[2].Zero(); Col[3].Zero(); }

inline void CMat4x4::SetIdentity()                                              // set to identity
  { *this = Identity; }

inline CMat4x4 CMat4x4::Transposed() const                                      // return transposed copy
  { CMat4x4 m4Tmp(*this);
    m4Tmp.Transpose();
    return m4Tmp; }

inline void CMat4x4::Invert()                                                   // invert
  { *this = Inverted(); }

inline void CMat4x4::Translation(const CVect3D& v3V)                            // translation
  { Col[0].Set(1.f,  0.f,  0.f,  0.f);
    Col[1].Set(0.f,  1.f,  0.f,  0.f);
    Col[2].Set(0.f,  0.f,  1.f,  0.f);
    Col[3].Set(v3V.x,v3V.y,v3V.z,1.f); }

inline void CMat4x4::Scale(const CVect3D& v3Scale)                              // scale
  { Col[0].Set(v3Scale.x,0.f,0.f,0.f);
    Col[1].Set(0.f,v3Scale.y,0.f,0.f);
    Col[2].Set(0.f,0.f,v3Scale.z,0.f); 
    Col[3].Set(0.f,0.f,      0.f,1.f); }

inline CVect3D CMat4x4::RotateVector(const CVect3D& v3V)                        // rotate 3D vector (without casting it nor the result to v4)
  { return CVect3D(Col[0][0]*v3V.x + Col[1][0]*v3V.y + Col[2][0]*v3V.z,
                   Col[0][1]*v3V.x + Col[1][1]*v3V.y + Col[2][1]*v3V.z,
                   Col[0][2]*v3V.x + Col[1][2]*v3V.y + Col[2][2]*v3V.z); }

//----------------------------------------------- set ----------------------------------------------------------------//

inline void CMat4x4::Set(const float fD)
  { Col[0] = Col[1] = Col[2] = Col[3] = CVect4D(fD); }

inline void CMat4x4::Set(const CVect4D& v4V0,const CVect4D& v4V1,const CVect4D& v4V2,const CVect4D& v4V3)
  { Col[0] = v4V0; Col[1] = v4V1; Col[2] = v4V2; Col[3] = v4V3; }

inline void CMat4x4::Set(const CMat4x4& m4M)
  { memcpy(Col,m4M.Col,sizeof(Col)); }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_VECTORMATRIX_INL_
