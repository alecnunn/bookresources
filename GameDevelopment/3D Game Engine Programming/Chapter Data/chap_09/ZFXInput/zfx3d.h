/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2002 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: zfx3d.h                                                   *
 * part of static math lib implementing basic 3D math objects      *
 *******************************************************************/

// this is the main include file for the zfx3d.lib

#ifndef _ZFX3D_H_
#define _ZFX3D_H_



// I N C L U D E S /////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include <memory.h>     // memset


const double ZFXPI  =  3.14159265;
const double ZFXPI2 =  1.5707963;
const double ZFX2PI =  6.2831853;
const float  ZFXG   = -32.174f; // ft/s^2
const float  ZFXEPSILON = 0.00001f;

// D E F I N E S ///////////////////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif

#define ZFXFRONT    0
#define ZFXBACK     1
#define ZFXPLANAR   2
#define ZFXCLIPPED  3
#define ZFXCULLED   4
#define ZFXVISIBLE  5

// M A C R O S /////////////////////////////////////////////////////

float _fabs(float f);

bool ZFX3DInitCPU(void);


// S T R U C T S ///////////////////////////////////////////////////

typedef struct CPUINFO_TYP {
   bool bSSE;        // Streaming SIMD Extensions
   bool bSSE2;       // STreaming SIMD Extensions 2
   bool b3DNOW;      // 3DNow! (vendor independant)
   bool b3DNOWEX;    // 3DNow! (AMD specific extensions)
   bool bMMX;        // MMX support
   bool bMMXEX;      // MMX (AMD specific extensions)
   bool bEXT;        // extended features available
   char vendor[13];  // vendor name
   char name[48];    // cpu name
   } CPUINFO;



// F O R W A R D   D E C L A R A T I O N S /////////////////////////

class ZFXMatrix;
class ZFXObb;
class ZFXAabb;
class ZFXPlane;
class ZFXQuat;
class ZFXPolygon;


// C L A S S E S ///////////////////////////////////////////////////

// Our basic 4D vector class
class __declspec(dllexport) ZFXVector {
   public:
      float x, y, z, w;       // coordinateset

      //---------------------------------------

      ZFXVector(void) { x=0, y=0, z=0, w=1.0f; }
      ZFXVector(float _x, float _y, float _z)
         { x=_x; y=_y; z=_z; w=1.0f; }

      inline void  Set(float _x, float _y, float _z, float _w=1.0f);
      inline float GetLength(void);                   // length
      inline float GetSqrLength(void) const;          // square length
      inline void  Negate(void);                      // vector mult -1
      inline void  Normalize(void);                   // normalize
      inline float AngleWith( ZFXVector &v);          // angle in rad
      inline void  RotateWith(const ZFXMatrix&);      // apply rotation part
      inline void  InvRotateWith(const ZFXMatrix&);   // apply inverse rotation
      inline void  Difference(const ZFXVector &v1,    // from v1 to v2
                              const ZFXVector &v2);
      void operator += (const ZFXVector &v);          // operator +=
      void operator -= (const ZFXVector &v);          // operator -=
      void operator *= (float f);                     // scale vector
      void operator /= (float f);                     // scale down
      void operator += (float f);                     // add scalar
      void operator -= (float f);                     // subtract scalar
      float     operator * (const ZFXVector &v)const; // dot product
      ZFXVector operator * (float f)const;            // scale vector
      ZFXVector operator / (float f)const;            // scalar divide
      ZFXVector operator + (float f)const;            // add scalar
      ZFXVector operator - (float f)const;            // scale down
      ZFXQuat   operator * (const ZFXQuat   &q)const; // vector * quaternion
      ZFXVector operator * (const ZFXMatrix &m)const; // vector-matrix product
      ZFXVector operator + (const ZFXVector &v)const; // addition
      ZFXVector operator - (const ZFXVector &v)const; // subtraction

      inline void Cross(const ZFXVector &v1,          // cross product
                        const ZFXVector &v2); 
   };  // class
/*----------------------------------------------------------------*/


// Our basic matrix class
class __declspec(dllexport) ZFXMatrix {
   public:
      float _11, _12, _13, _14;
      float _21, _22, _23, _24;
      float _31, _32, _33, _34;
      float _41, _42, _43, _44;

      //---------------------------------------

      ZFXMatrix(void) { /* nothing to do */ ; }

      inline void Identity(void);                       // identity matrix
      inline void RotaX(float a);                       // x axis
      inline void RotaY(float a);                       // y axis
      inline void RotaZ(float a);                       // z axis
      inline void Rota(const ZFXVector &vc);            // x, y and z
      inline void Rota(float x, float y, float z);      // x, y and z
      inline void RotaArbi(const ZFXVector &vcAxis, float a);
      inline void ApplyInverseRota(ZFXVector *pvc);
      inline void Translate(float dx, float dy, float dz);
      inline void SetTranslation(ZFXVector vc, bool EraseContent=false);
	   inline ZFXVector GetTranslation(void);

      inline void Billboard(ZFXVector vcPos, ZFXVector vcDir,
                            ZFXVector vcWorldUp = ZFXVector(0,1,0));
      inline void LookAt(ZFXVector vcPos, ZFXVector vcLookAt, 
                         ZFXVector vcWorldUp = ZFXVector(0,1,0));

      inline void TransposeOf(const ZFXMatrix &m);       // transpose m, save result in this
      inline void InverseOf(const ZFXMatrix &m);         // invert m, save result in this
      
      ZFXMatrix operator * (const ZFXMatrix &m)const;    // matrix multiplication
      ZFXVector operator * (const ZFXVector &vc)const;   // matrix vector multiplication
   }; // class
/*----------------------------------------------------------------*/


// Our basic ray class
class __declspec(dllexport) ZFXRay {
   public:
      ZFXVector m_vcOrig,  // ray origin
                m_vcDir;   // ray direction

      //---------------------------------------

      ZFXRay(void) { /* nothing to do */ ; }

      inline void Set(ZFXVector vcOrig, ZFXVector vcDir);
      inline void DeTransform(const ZFXMatrix &_m); // move to matrixspace

      bool Intersects(const ZFXVector &vc0, const ZFXVector &vc1,
                      const ZFXVector &vc2, bool bCull,
                      float *t);
      bool Intersects(const ZFXVector &vc0, const ZFXVector &vc1,
                      const ZFXVector &vc2, bool bCull,
                      float fL, float *t);
      bool Intersects(const ZFXPlane &plane, bool bCull,
                      float *t, ZFXVector *vcHit);       
      bool Intersects(const ZFXPlane &plane, bool bCull,
                      float fL, float *t, ZFXVector *vcHit);
      bool Intersects(const ZFXAabb &aabb, float *t);
      bool Intersects(const ZFXAabb &aabb, float fL, float *t);
      bool Intersects(const ZFXObb &obb, float *t);
      bool Intersects(const ZFXObb &obb, float fL, float *t);
   }; // class
/*----------------------------------------------------------------*/


// Our basic plane class
class __declspec(dllexport) ZFXPlane {
   public:
      ZFXVector m_vcN,       // plane normal vector
                m_vcPoint;   // point on plane
      float     m_fD;        // distance to origin

      //---------------------------------------

      ZFXPlane(void) { /* nothing to do */ ; }

      inline void  Set(const ZFXVector &vcN, const ZFXVector &vcP);
      inline void  Set(const ZFXVector &vcN, const ZFXVector &vcP, float fD);
      inline void  Set(const ZFXVector &v0,  const ZFXVector &v1, const ZFXVector &v2);
      inline float Distance(const ZFXVector &vcPoint);
      inline int   Classify(const ZFXVector &vcPoint);
             int   Classify(const ZFXPolygon &Polygon);

      bool Intersects(const ZFXVector &vc0, const ZFXVector &vc1, 
                      const ZFXVector &vc2);
      bool Intersects(const ZFXPlane &plane, ZFXRay *pIntersection);
      bool Intersects(const ZFXAabb &aabb);
      bool Intersects(const ZFXObb &obb);

   }; // class
/*----------------------------------------------------------------*/


// Our basic orientedbox class
class __declspec(dllexport) ZFXObb {
   public:
      float     fA0,   fA1,  fA2; // half axis length
      ZFXVector vcA0, vcA1, vcA2; // box axis
      ZFXVector vcCenter;         // centerpoint

      //---------------------------------------

      ZFXObb(void) { /* nothing to do */ ; }

      inline void DeTransform(const ZFXObb &obb, 
                              const ZFXMatrix &m);

      bool Intersects(const ZFXRay &Ray, float *t);
      bool Intersects(const ZFXRay &Ray, float fL, float *t);
      bool Intersects(const ZFXObb &Obb);
      bool Intersects(const ZFXVector &v0, 
                      const ZFXVector &v1,
                      const ZFXVector &v2);

      int  Cull(const ZFXPlane *pPlanes, int nNumPlanes);      

   private:
      void ObbProj(const ZFXObb &Obb, const ZFXVector &vcV, 
                   float *pfMin, float *pfMax);
      void TriProj(const ZFXVector &v0, const ZFXVector &v1, 
                   const ZFXVector &v2, const ZFXVector &vcV, 
                   float *pfMin, float *pfMax);
   }; // class
/*----------------------------------------------------------------*/

// Our basic axisalignedbox class
class __declspec(dllexport) ZFXAabb {
   public:
      ZFXVector vcMin, vcMax; // box extreme points
      ZFXVector vcCenter;     // centerpoint

      //---------------------------------------

      ZFXAabb(void) { /* nothing to do */ ; }
      ZFXAabb(ZFXVector vcMin, ZFXVector vcMax);

      void Construct(const ZFXObb *pObb);                  // build from obb
      int  Cull(const ZFXPlane *pPlanes, int nNumPlanes);  

      // normals pointing outwards
      void GetPlanes(ZFXPlane *pPlanes);

      bool Contains(const ZFXRay &Ray, float fL);
      bool Intersects(const ZFXRay &Ray, float *t);
      bool Intersects(const ZFXRay &Ray, float fL, float *t);
      bool Intersects(const ZFXAabb &aabb);
      bool Intersects(const ZFXVector &vc0);
   }; // class
/*----------------------------------------------------------------*/


// Our basic polygon class
class __declspec(dllexport) ZFXPolygon {
   friend class ZFXPlane;        // access for easier classifying

   private:
      ZFXPlane       m_Plane;    // plane which poly lies in

      int            m_NumP;     // number of points
      int            m_NumI;     // number of indices
      ZFXAabb        m_Aabb;     // bounding box
      unsigned int   m_Flag;     // whatever you want it to be

      void CalcBoundingBox(void);

   //---------------------------------------

   public:
      ZFXPolygon(void);
      ~ZFXPolygon(void);


      ZFXVector    *m_pPoints;  // list of points
      unsigned int *m_pIndis;   // index list

      void          Set(const ZFXVector *pPoints, int nNumP,
                        const unsigned int *pIndis, int nNumI);

      void          Clip(const ZFXPlane &Plane, 
                         ZFXPolygon *pFront,
                         ZFXPolygon *pBack);
      void          Clip(const ZFXAabb &aabb);
      int           Cull(const ZFXAabb &aabb);

      void          CopyOf( const ZFXPolygon &Poly );

      void          SwapFaces(void);

      bool          Intersects(const ZFXRay &Ray, bool, float *t);
      bool          Intersects(const ZFXRay &Ray, bool, float fL, float *t);

      int           GetNumPoints(void)      { return m_NumP;    }
      int           GetNumIndis(void)       { return m_NumI;    }
      ZFXVector*    GetPoints(void)         { return m_pPoints; }
      unsigned int* GetIndices(void)        { return m_pIndis;  }
      ZFXPlane      GetPlane(void)          { return m_Plane;   }
      ZFXAabb       GetAabb(void)           { return m_Aabb;    }
      unsigned int  GetFlag(void)           { return m_Flag;    }
      void          SetFlag(unsigned int n) { m_Flag = n;       }

      // DEBUG ONLY
      void Print(FILE*);
   }; // class
/*----------------------------------------------------------------*/


// Our basic quaternion class
class __declspec(dllexport) ZFXQuat {
   public:
      float x, y, z, w;

      //---------------------------------------

      ZFXQuat(void) { x=0.0f, y=0.0f, z=0.0f, w=1.0f; }
      ZFXQuat(float _x, float _y, float _z, float _w)
         { x=_x; y=_y; z=_z; w=_w; }

      void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
      void  Normalize();
      void  Conjugate(ZFXQuat q);
      void  GetEulers(float *fPitch, float *fYaw, float *fRoll);
      void  GetMatrix(ZFXMatrix *m);
      float GetMagnitude(void);


      void    operator /= (float f);
      ZFXQuat operator /  (float f);

      void    operator *= (float f);
      ZFXQuat operator *  (float f);

      ZFXQuat operator *  (const ZFXVector &v) const;

      ZFXQuat operator *  (const ZFXQuat &q) const;
      void    operator *= (const ZFXQuat &q);

      void    operator += (const ZFXQuat &q);
      ZFXQuat operator +  (const ZFXQuat &q) const;

      ZFXQuat operator~(void) const { return ZFXQuat(-x, -y, -z, w); }

      void Rotate(const ZFXQuat &q1, const ZFXQuat &q2);

      ZFXVector Rotate(const ZFXVector &v);

   }; // class
/*----------------------------------------------------------------*/



#endif // header name define

