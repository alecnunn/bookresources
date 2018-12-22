/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXVector.cpp                                             *
 * part of static math lib implementing basic 3D math objects      *
 *******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include "zfx3d.h"      // class definitions and stuff


// G L O B A L S ///////////////////////////////////////////////////

extern bool g_bSSE;


// F U N C T I O N S ///////////////////////////////////////////////

float _fabs(float f) { if (f<0.0f) return -f; return f; }

inline void ZFXVector::Set(float _x, float _y, float _z, float _w) {
   x=_x; y=_y; z=_z; w=_w;
   } 
/*----------------------------------------------------------------*/

inline float ZFXVector::GetLength(void) {
   float f=0;

   if (!g_bSSE) {
      f = (float)sqrt(x*x + y*y + z*z); 
      }
   else {
      w = 0.0f;
      __asm {
         mov    esi,   this            ; vector u
         movups xmm0,  [esi]           ; first vector in xmm0
         mulps  xmm0,  xmm0            ; mul with 2nd vector
         movaps xmm1,  xmm0            ; copy result
         shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
         addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
         movaps xmm1,  xmm0            ; copy results
         shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
         addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
         sqrtss xmm0,  xmm0            ; sqroot from least bit value
         movss  f, xmm0            ; move result from xmm0 to edi
         }
      w = 1.0f;
      }

   return f;
   }
/*----------------------------------------------------------------*/

inline float ZFXVector::GetSqrLength(void) const {
   return (x*x + y*y + z*z); 
   }
/*----------------------------------------------------------------*/

inline void ZFXVector::Negate(void) {
   x = -x;  y = -y;  z = -z;
   }
/*----------------------------------------------------------------*/

inline float ZFXVector::AngleWith(ZFXVector &v) {
   return (float)acos( ((*this) * v) / (this->GetLength()*v.GetLength()) );
   }
/*----------------------------------------------------------------*/

inline void ZFXVector::Normalize(void) {
   if (!g_bSSE) {
      float f = (float)sqrt(x*x + y*y + z*z);

      if (f != 0.0f) {
         x/=f; y/=f; z/=f;
         }
      }
   else {
      w = 0.0f;
      __asm {
         mov    esi,   this            ; vector u
         movups xmm0,  [esi]           ; first vector in xmm0
         movaps xmm2,  xmm0            ; copy original vector
         mulps  xmm0,  xmm0            ; mul with 2nd vector
         movaps xmm1,  xmm0            ; copy result
         shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
         addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
         movaps xmm1,  xmm0            ; copy results
         shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
         addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
         
         rsqrtps xmm0,  xmm0           ; recip. sqrt (faster than ss + shufps)
         mulps   xmm2,  xmm0           ; mul by reciprocal
         movups  [esi], xmm2           ; bring back result
         }
      w = 1.0f;
      }
   }
/*----------------------------------------------------------------*/

/**
 * Get vector from v1 to v2. 
 */
inline void ZFXVector::Difference(const ZFXVector &v1, const ZFXVector &v2) {
   x = v2.x - v1.x;
   y = v2.y - v1.y;
   z = v2.z - v1.z;
   w = 1.0f;
   }
/*----------------------------------------------------------------*/

/**
 * Build cross product of two vectors, use SSE if available. Note
 * that none of the parameters values is changed.
 */
inline void ZFXVector::Cross(const ZFXVector &v1, const ZFXVector &v2) {
   if (!g_bSSE) {
      x = v1.y * v2.z - v1.z * v2.y;
      y = v1.z * v2.x - v1.x * v2.z;
      z = v1.x * v2.y - v1.y * v2.x;
      w = 1.0f;
      }
   else {
      __asm {
         mov esi, v1
         mov edi, v2

         movups xmm0, [esi]
         movups xmm1, [edi]
         movaps xmm2, xmm0
         movaps xmm3, xmm1

         shufps xmm0, xmm0, 0xC9
         shufps xmm1, xmm1, 0xD2
         mulps  xmm0, xmm1

         shufps xmm2, xmm2, 0xD2
         shufps xmm3, xmm3, 0xC9
         mulps  xmm2, xmm3

         subps  xmm0, xmm2

         mov    esi, this
         movups [esi], xmm0
         }
      w = 1.0f;
      }
   }
/*----------------------------------------------------------------*/


/**
 * rotate the vector with the given matrix, ignoring translation
 * information in matrix if they are present
 */
inline void ZFXVector::RotateWith(const ZFXMatrix &m) {
   // applying rotational part of matrix only
   float _x = x * m._11 + y * m._21 + z * m._31;
   float _y = x * m._12 + y * m._22 + z * m._32;
   float _z = x * m._13 + y * m._23 + z * m._33;
   x = _x;   y = _y;   z = _z;
   } // RotateWidth
/*----------------------------------------------------------------*/


/**
 * rotate the vector with the inverse rotation part of the given
 * matrix, ignoring translation of the matrix if present.
 */
inline void ZFXVector::InvRotateWith(const ZFXMatrix &m) {
   // using transposed matrix
   float _x = x * m._11 + y * m._12 + z * m._13;
   float _y = x * m._21 + y * m._22 + z * m._23;
   float _z = x * m._31 + y * m._32 + z * m._33;
   x = _x;   y = _y;   z = _z;
   } // InvRotateWidth
/*----------------------------------------------------------------*/


/**
 * Add two vectors together. Note this is faster than '+' due to lack
 * of additional constructor and return.
 */
void ZFXVector::operator += (const ZFXVector &v) {
   x += v.x;   y += v.y;   z += v.z;
   }
/*----------------------------------------------------------------*/

/**
 * Add two vectors together. Note this is slower than '+=' due to 
 * additional constructor and return.
 */
ZFXVector ZFXVector::operator + (const ZFXVector &v) const {
   return ZFXVector(x+v.x, y+v.y, z+v.z);
   }
/*----------------------------------------------------------------*/

/**
 * Subtract two vectors together, use SSE if available. Note this is
 * faster than '-' due to lack of additional constructor and return.
 */
void ZFXVector::operator -= (const ZFXVector &v) {
   x -= v.x;   y -= v.y;   z -= v.z;
   }
/*----------------------------------------------------------------*/

/**
 * Subtract two vectors together, use SSE if available. Note this is
 * slower than '-=' due to additional constructor and return.
 */
ZFXVector ZFXVector::operator - (const ZFXVector &v) const {
   return ZFXVector(x-v.x, y-v.y, z-v.z);
   }
/*----------------------------------------------------------------*/

void ZFXVector::operator *= (float f) {
   x *= f;   y *= f;   z *= f;
   }
/*----------------------------------------------------------------*/

void ZFXVector::operator /= (float f) {
   x /= f;   y /= f;   z /= f;
   }
/*----------------------------------------------------------------*/

ZFXVector ZFXVector::operator * (float f) const {
   return ZFXVector(x*f, y*f, z*f);
   }
/*----------------------------------------------------------------*/

ZFXVector ZFXVector::operator / (float f) const {
   return ZFXVector(x/f, y/f, z/f);
   }
/*----------------------------------------------------------------*/

void ZFXVector::operator += (float f) {
   x += f;   y += f;   z += f;
   }
/*----------------------------------------------------------------*/

void ZFXVector::operator -= (float f) {
   x -= f;   y -= f;   z -= f;
   }
/*----------------------------------------------------------------*/

ZFXVector ZFXVector::operator + (float f) const {
   return ZFXVector(x+f, y+f, z+f);
   }
/*----------------------------------------------------------------*/

ZFXVector ZFXVector::operator - (float f) const {
   return ZFXVector(x-f, y-f, z-f);
   }
/*----------------------------------------------------------------*/

/**
 * Multiply two vectors (dot product), use of SSE would be slower
 * due to overhead of data movement.
 */
float ZFXVector::operator * (const ZFXVector &v) const {
   return (v.x*x + v.y*y + v.z*z);
   }
/*----------------------------------------------------------------*/

/**
 * Subtract two vectors together, use SSE if available. Note this is
 * slower than '-=' due to additional constructor and return.
 */
ZFXQuat ZFXVector::operator * (const ZFXQuat &q) const {
   return ZFXQuat(  q.w*x + q.z*y - q.y*z,
                    q.w*y + q.x*z - q.z*x,
                    q.w*z + q.y*x - q.x*y,
                  -(q.x*x + q.y*y + q.z*z) );
   }
/*----------------------------------------------------------------*/

/**
 * Multiply a vector and a matrix, use SSE if available.
 */
ZFXVector ZFXVector::operator * (const ZFXMatrix &m) const {
   ZFXVector vcResult;

//   if (!g_bSSE) {
      vcResult.x = x*m._11 + y*m._21 + z*m._31 + m._41;
      vcResult.y = x*m._12 + y*m._22 + z*m._32 + m._42;
      vcResult.z = x*m._13 + y*m._23 + z*m._33 + m._43;
      vcResult.w = x*m._14 + y*m._24 + z*m._34 + m._44;
   
      vcResult.x = vcResult.x/vcResult.w;
      vcResult.y = vcResult.y/vcResult.w;
      vcResult.z = vcResult.z/vcResult.w;
      vcResult.w = 1.0f;
//      }
/*   else {
      float *ptrRet = (float*)&vcResult;

      ZFXVector s; s.Set(m._11, m._21, m._31, m._41);
      ZFXVector t; t.Set(m._12, m._22, m._32, m._42);
      ZFXVector u; u.Set(m._13, m._23, m._33, m._43);
      ZFXVector v; v.Set(m._14, m._24, m._34, m._44);

      __asm {

         mov    esi, this
         mov    edi, m

         movups xmm0, [esi]
         movaps xmm1, xmm0
         movaps xmm2, xmm0
         movaps xmm3, xmm0

         shufps xmm0, xmm2, 0x00
         shufps xmm1, xmm2, 0x55
         shufps xmm2, xmm2, 0xAA
         shufps xmm3, xmm3, 0xFF

         mov    edx,  s
         movups xmm4, edx
         mov    edx,  t
         movups xmm5, edx
         mov    edx,  u
         movups xmm6, edx
         mov    edx,  v
         movups xmm7, edx

         mulps xmm0, xmm4
         mulps xmm1, xmm5
         mulps xmm2, xmm6
         mulps xmm3, xmm7

         addps xmm0, xmm1
         addps xmm0, xmm2
         addps xmm0, xmm3

         movups vcResult, xmm0
         
/*
         mov    ecx,   this      ; vector
         mov    edx,   m         ; matrix

         movss  xmm0,  [ecx]
         mov    eax,   ptrRet    ; result vector

         shufps xmm0,  xmm0, 0
         movss  xmm1,  [ecx+4]

         movups xmm4, [edx]
         mulps  xmm0,  xmm4
//         mulps  xmm0,  [edx]

         shufps xmm1,  xmm1, 0
         movss  xmm2,  [ecx+8]

         movups xmm5, [edx+16]
         mulps  xmm1,  xmm5
//         mulps  xmm1,  [edx+16]

         shufps xmm2,  xmm2, 0
         movss  xmm3,  [ecx+12]

         movups xmm6, [edx+32]
         mulps  xmm2,  xmm6
//         mulps  xmm2,  [edx+32]

         shufps xmm3,  xmm3, 0
         addps  xmm0,  xmm1

         movups xmm7, [edx+48]
         mulps  xmm3,  xmm7
//         mulps  xmm3,  [edx+48]

         addps  xmm2,  xmm3
         addps  xmm0,  xmm2

         movups [eax], xmm0     ; save as result

         }
      }
*/         
   return vcResult;
   }
/*----------------------------------------------------------------*/



