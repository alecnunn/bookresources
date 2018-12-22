/*******************************************************************
* ZFXEngine!                                                      *
* (c)2003 by Stefan Zerbst | www.zfx.info                         *
*-----------------------------------------------------------------*
* File: ZFXQuat.cpp                                               *
* part of static math lib implementing basic 3D math objects      *
*******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include "zfx3d.h"      // class definitions and stuff


// G L O B A L S ///////////////////////////////////////////////////



// F U N C T I O N S ///////////////////////////////////////////////

// get the mgnitude of a quaternion
float ZFXQuat::GetMagnitude(void) {
   return sqrtf(x*x + y*y + z*z + w*w);
   }
/*----------------------------------------------------------------*/


void ZFXQuat::Normalize(void) {
   float m=sqrtf(x*x + y*y + z*z + w*w);
   if (m!=0) 
      { x /= m; y /= m; z /= m; w /= m; }
   }
/*----------------------------------------------------------------*/


void ZFXQuat::Conjugate(ZFXQuat q) {
   x=-q.x;  y=-q.y;  z=-q.z;  w=q.w;
   }
/*----------------------------------------------------------------*/


void ZFXQuat::Rotate(const ZFXQuat &q1, const ZFXQuat &q2) { 
   ZFXQuat t = q1*q2*(~q1); 
   x=t.x; y=t.y; z=t.z; w=t.w; 
   }
/*----------------------------------------------------------------*/


ZFXVector ZFXQuat::Rotate(const ZFXVector &v) { 
   ZFXQuat t(x, y, z, w);
   ZFXQuat r = t*v*(~t);
   return ZFXVector(r.x, r.y, r.z); 
   }
/*----------------------------------------------------------------*/


// get a rotation matrix out of the quaternion
void ZFXQuat::GetMatrix(ZFXMatrix *pMat) {
   float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

   // set matrix to identity
   memset(pMat, 0, sizeof(ZFXMatrix));
   pMat->_44 = 1.0f;

   x2 = x + x; 
   y2 = y + y; 
   z2 = z + z;

   xx = x * x2;   
   xy = x * y2;  
   xz = x * z2;

   yy = y * y2;   
   yz = y * z2;   
   zz = z * z2;

   wx = w * x2;   
   wy = w * y2;   
   wz = w * z2;

   pMat->_11 = 1.0f - (yy + zz);
   pMat->_12 = xy - wz;
   pMat->_13 = xz + wy;

   pMat->_21 = xy + wz;
   pMat->_22 = 1.0f - (xx + zz);
   pMat->_23 = yz - wx;

   pMat->_31 = xz - wy;
   pMat->_32 = yz + wx;
   pMat->_33 = 1.0f - (xx + yy);
   } // GetMatrix
/*----------------------------------------------------------------*/


// construct quaternion from Euler angles
void ZFXQuat::MakeFromEuler(float fPitch, float fYaw, float fRoll) {
   float cX, cY, cZ, sX, sY, sZ, cYcZ, sYsZ, cYsZ, sYcZ;

   fPitch *= 0.5f;
   fYaw   *= 0.5f;
   fRoll  *= 0.5f;

   cX = cosf(fPitch/2);
   cY = cosf(fYaw/2);
   cZ = cosf(fRoll/2);

   sX = sinf(fPitch/2);
   sY = sinf(fYaw/2);
   sZ = sinf(fRoll/2);

   cYcZ = cY * cZ;
   sYsZ = sY * sZ;
   cYsZ = cY * sZ;
   sYcZ = sY * cZ;

   w = cX * cYcZ + sX * sYsZ;
   x = sX * cYcZ - cX * sYsZ;
   y = cX * sYcZ + sX * cYsZ;
   z = cX * cYsZ - sX * sYcZ;
   } // MakeFromEuler
/*----------------------------------------------------------------*/


// converts quaternion into euler angles
void ZFXQuat::GetEulers(float *fPitch, float *fYaw, float *fRoll) {
   double   r11, r21, r31, r32, r33, r12, r13;
   double   q00, q11, q22, q33;
   double   tmp;

   q00 = w * w;
   q11 = x * x;
   q22 = y * y;
   q33 = z * z;

   r11 = q00 + q11 - q22 - q33;
   r21 = 2 * (x*y + w*z);
   r31 = 2 * (x*z - w*y);
   r32 = 2 * (y*z + w*x);
   r33 = q00 - q11 - q22 + q33;

   tmp = fabs(r31);
   if(tmp > 0.999999)
      {
      r12 = 2 * (x*y - w*z);
      r13 = 2 * (x*z + w*y);

      *fPitch = 0.0f;
      *fYaw   = (float)-((ZFXPI/2) * r31/tmp);
      *fRoll  = (float)atan2(-r12, -r31*r13);
      }
   else {
      *fPitch = (float)atan2(r32, r33);
      *fYaw   = (float)asin(-r31);
      *fRoll  = (float)atan2(r21, r11);
      }
   } // EulerFromQuat
/*----------------------------------------------------------------*/


// dives a quaternion by a scalar value
void ZFXQuat::operator /= (float f) 
   { w /= f;  x /= f;  y /= f;  z /= f; }

ZFXQuat ZFXQuat::operator / (float f) {
   return ZFXQuat(x/f, y/f, z/f, w/f);
   }
/*----------------------------------------------------------------*/


// multiplies a quaternion by a scalar value
void ZFXQuat::operator *= (float f) 
   { w *= f;  x *= f;  y *= f;  z *= f; }

ZFXQuat ZFXQuat::operator * (float f) {
   return ZFXQuat(x*f, y*f, z*f, w*f);
   }
/*----------------------------------------------------------------*/


// adds two quaternions
void ZFXQuat::operator += (const ZFXQuat &q) 
   { w += q.w;  x += q.x;  y += q.y;  z += q.z; }

ZFXQuat ZFXQuat::operator + (const ZFXQuat &q) const {
   return ZFXQuat(x+q.x, y+q.y, z+q.z, w+q.w);
   }
/*----------------------------------------------------------------*/


// multiplies two quaternions with *= operator
void ZFXQuat::operator *= (const ZFXQuat &q) {
   float _x, _y, _z, _w;

   _w = w*q.w - x*q.x - y*q.y - z*q.z;
   _x = w*q.x + x*q.w + y*q.z - z*q.y;
   _y = w*q.y + y*q.w + z*q.x - x*q.z;
   _z = w*q.z + z*q.w + x*q.y - y*q.x;

   x = _x;
   y = _y;
   z = _z;
   w = _w;
   }
/*----------------------------------------------------------------*/


// multiplies two quaternions with * operator
ZFXQuat ZFXQuat::operator * (const ZFXQuat &q) const {
   ZFXQuat qResult;
   
   qResult.w = w*q.w - x*q.x - y*q.y - z*q.z;
   qResult.x = w*q.x + x*q.w + y*q.z - z*q.y;
   qResult.y = w*q.y + y*q.w + z*q.x - x*q.z;
   qResult.z = w*q.z + z*q.w + x*q.y - y*q.x;

   return qResult;
   }
/*----------------------------------------------------------------*/


// multiplies a vector with a quaternion
ZFXQuat ZFXQuat::operator * (const ZFXVector &v) const {
   return ZFXQuat(  w*v.x + y*v.z - z*v.y,
                    w*v.y + z*v.x - x*v.z,
                    w*v.z + x*v.y - y*v.x,
                  -(x*v.x + y*v.y + z*v.z) );
    }
/*----------------------------------------------------------------*/



