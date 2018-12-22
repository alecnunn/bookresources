//--------------------------------------------------------------------------------------------------------------------//
//                                                VECTORS & MATRICES CLASSES                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VECTORMATRIX_H_
#define   _GPG4_VECTORMATRIX_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <strstrea.h>                                      // ostrstream
#include  "BasicTypes.h"

//----------------------------------------------- DEFINES ------------------------------------------------------------//

enum      { X,Y,Z,W };                                      // 4D axis
enum      { R,G,B,A };                                      // 4D color
enum      { U,V };                                          // 2D tex coord
enum      { S,T,P,Q };                                      // 4D tex coord

enum      { XN,UP,DIR,POS };                                // matrix columns meanings

#define   PI                            3.141592654f
#define   EPS_NORMALIZE                 0.001f

//----------------------------------------------- MACROS -------------------------------------------------------------//

#define   RAD2DEG(fAngleRad)            (fAngleRad*180.f/PI)
#define   DEG2RAD(fAngleDeg)            (fAngleDeg*PI/180.f)

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class   CVect2D;                                          // 2D vector
  class   CVect3D;                                          // 3D vector
  class   CVect4D;                                          // 4D vector
  class   CMat3x3;                                          // 3*3 matrix
  class   CMat4x4;                                          // 4*4 matrix
  class   CQuaternion;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                2D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CVect2D
  {
  // public methods

  public:

    // constructors

    inline          CVect2D             (void);
    inline          CVect2D             (const float    fD);                              // x=y=fD
    inline          CVect2D             (const float    fX, const float fY);              // x=fX; y=fY
    inline          CVect2D             (const CVect2D& v2V);                             // copy 
    inline          CVect2D             (const CVect3D& v3V,const ui uiDropAxis);         // cast v3 to v2 (drop an axis)

    // assignment operators

    CVect2D&               operator   = (const CVect2D& v2V);                             // assignment
    CVect2D&               operator  += (const CVect2D& v2V);                             // incrementation
    CVect2D&               operator  -= (const CVect2D& v2V);                             // decrementation
    CVect2D&               operator  *= (const float    fK);                              // multiplication by a constant
    CVect2D&               operator  *= (const CVect2D& v2V);                             // term by term *
    CVect2D&               operator  /= (const float    fK);                              // division by a constant
    CVect2D&               operator  /= (const CVect2D& v2V);                             // term by term /
    float&                 operator  [] (const ui       uiIndex);                         // indexing
    const   float          operator  [] (const ui       uiIndex) const;                   // indexing                            

    // friend operators

    friend  CVect2D inline operator   - (const CVect2D& v2V);                             // -V
    friend  CVect2D inline operator   * (const float    fK,  const CVect2D& v2V);         // k*V 
    friend  CVect2D inline operator   * (const CVect2D& v2V, const float    fK);          // V*k
    friend  CVect2D inline operator   / (const CVect2D& v2V, const float    fK);          // V/k
    friend  CVect2D inline operator   - (const CVect2D& v2V1,const CVect2D& v2V2);        // V1-V2
    friend  CVect2D inline operator   + (const CVect2D& v2V1,const CVect2D& v2V2);        // V1+V2
    friend  float   inline operator   * (const CVect2D& v2V1,const CVect2D& v2V2);        // V1*V2 (dot   product)
    friend  float   inline operator   ^ (const CVect2D& v2V1,const CVect2D& v2V2);        // V1^V2 (cross product)
    friend  bool    inline operator  == (const CVect2D& v2V1,const CVect2D& v2V2);        // V1==V2
    friend  bool    inline operator  != (const CVect2D& v2V1,const CVect2D& v2V2);        // V1!=V2

    friend  ostream&       operator  << (ostream& s,               CVect2D& v2V);         // output
    friend  ostream&       operator  << (ostream& s,         const CVect2D& v2V);         // output

    // friend functions

    friend  void    inline Swap         (      CVect2D& v2V1,      CVect2D& v2V2);        // swap
    friend  CVect2D inline Smallest     (const CVect2D& v2V1,const CVect2D& v2V2);        // mini(V1,V2) (smallest vector)
    friend  CVect2D inline Longest      (const CVect2D& v2V1,const CVect2D& v2V2);        // maxi(V1,V2) (longest  vector) 
    friend  CVect2D inline Prod         (const CVect2D& v2V1,const CVect2D& v2V2);        // term by term *
    friend  CVect2D inline Div          (const CVect2D& v2V1,const CVect2D& v2V2);        // term by term /
    friend  float   inline Dot          (const CVect2D& v2V1,const CVect2D& v2V2);        // dot   product
    friend  float   inline Cross        (const CVect2D& v2V1,const CVect2D& v2V2);        // cross product
 
    // special functions

    void    inline  Zero                (void);                                           // set to (0;0)
    float   inline  Length              (void) const;                                     // vector length d
    float   inline  Length2             (void) const;                                     // vector squared length d^2
    bool            Normalize           (const float fNorm=1.0f);                         // normalize
    bool            Normalize1          (void);                                           // normalize to 1.f - dedicated version
    CVect2D inline  Normalized          (void) const;                                     // return normalized copy

    ui      inline  MainAxis            (void) const;                                     // main axis index
    bool    inline  Equals              (const CVect2D& v2V,const float fDelta) const;    // == with a tolerance
    bool    inline  IsParallel          (const CVect2D& v2V,const float fDelta) const;    // 'almost parallel' test
    bool    inline  IsOrthogonal        (const CVect2D& v2V,const float fDelta) const;    // 'almost orthogonal' test
    void    inline  Rotate              (const float fAngle);                             // rotate by an angle
    CVect2D inline  Rotated             (const float fAngle);                             // return rotated copy

    // set

    inline  void    Set                 (const float    fD);
    inline  void    Set                 (const float    fX, const float fY);
    inline  void    Set                 (const CVect2D& v2V);
    inline  void    Set                 (const CVect3D& v3V,const ui uiDropAxis);

    // friend classes

    friend  class   CVect3D;

  // public data

  public:

    union
      {
      float         V[2];                                                                 // x/y coords
      struct 
        { 
        float       x,y;
        };
      struct
        {
        float       u,v;
        };
      struct
        {
        float       s,t;
        };
      };
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                3D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CVect3D
  {
  // public methods

  public:

    // constructors

    inline          CVect3D             (void);                                               
    inline          CVect3D             (const float    fD);                              // x=y=z=fD
    inline          CVect3D             (const float    fX, const float fY,const float fZ);// x=fX; y=fY; z=fZ
    inline          CVect3D             (const CVect3D& v3V);                             // copy 
    inline          CVect3D             (const CVect4D& v4V,const ui uiDropAxis);         // cast v4 to v3 (drop an axis)
    inline          CVect3D             (const CVect2D& v2V,const float fZ);              // cast v2 to v3; z=fZ

    // assignment operators

    CVect3D&               operator   = (const CVect3D& v3V);                             // assignment
    CVect3D&               operator  += (const CVect3D& v3V);                             // incrementation
    CVect3D&               operator  -= (const CVect3D& v3V);                             // decrementation
    CVect3D&               operator  *= (const float    fK);                              // multiplication by a constant
    CVect3D&               operator  *= (const CVect3D& v3V);                             // term by term *
    CVect3D&               operator  /= (const float    fK);                              // division by a constant
    CVect3D&               operator  /= (const CVect3D& v3V);                             // term by term /
    float&                 operator  [] (const ui       uiIndex);                         // indexing
    const   float          operator  [] (const ui       uiIndex) const;                   // indexing

    // friend operators

    friend  CVect3D inline operator   - (const CVect3D& v3V);                             // -V
    friend  CVect3D inline operator   * (const float    fK,  const CVect3D& v3V);         // k*V 
    friend  CVect3D inline operator   * (const CVect3D& v3V, const float    fK);          // V*k
    friend  CVect3D inline operator   / (const CVect3D& v3V, const float    fK);          // V/k
    friend  CVect3D inline operator   - (const CVect3D& v3V1,const CVect3D& v3V2);        // V1-V2
    friend  CVect3D inline operator   + (const CVect3D& v3V1,const CVect3D& v3V2);        // V1+V2
    friend  float   inline operator   * (const CVect3D& v3V1,const CVect3D& v3V2);        // V1*V2 (dot   product)
    friend  CVect3D inline operator   ^ (const CVect3D& v3V1,const CVect3D& v3V2);        // V1^V2 (cross product)
    friend  bool    inline operator  == (const CVect3D& v3V1,const CVect3D& v3V2);        // V1==V2
    friend  bool    inline operator  != (const CVect3D& v3V1,const CVect3D& v3V2);        // V1!=V2
    friend  CVect3D inline operator   * (const CMat3x3& m3M, const CVect3D& v3V);         // M3*V3
    friend  CVect3D inline operator   * (const CVect3D& v3V, const CMat3x3& m3M);         // V3*M3

    friend  CVect4D inline operator   * (const CMat4x4& m4M, const CVect3D& v3V);         // M4*V3
    friend  CVect4D inline operator   * (const CVect3D& v3V, const CMat4x4& m4M);         // V3*M4
    friend  float   inline operator   * (const CVect4D& v4V, const CVect3D& v3V);         // V4*V3 (dot product)
    friend  float   inline operator   * (const CVect3D& v3V, const CVect4D& v4V);         // V3*V4 (dot product)

    friend  ostream&       operator  << (ostream& s,               CVect3D& v3V);         // output
    friend  ostream&       operator  << (ostream& s,         const CVect3D& v3V);         // output

    // friend functions

    friend  void    inline Swap         (      CVect3D& v3V1,      CVect3D& v3V2);        // swap
    friend  CVect3D inline Smallest     (const CVect3D& v3V1,const CVect3D& v3V2);        // mini(V1,V2) (smallest vector) 
    friend  CVect3D inline Longest      (const CVect3D& v3V1,const CVect3D& v3V2);        // maxi(V1,V2) (longest  vector) 
    friend  CVect3D inline Prod         (const CVect3D& v3V1,const CVect3D& v3V2);        // term by term *
    friend  CVect3D inline Div          (const CVect3D& v3V1,const CVect3D& v3V2);        // term by term /
    friend  float   inline Dot          (const CVect3D& v3V1,const CVect3D& v3V2);        // dot   product
    friend  CVect3D inline Cross        (const CVect3D& v3V1,const CVect3D& v3V2);        // cross product
    friend  CVect3D inline SwapAxis     (      CVect3D& v3V, const si siX,const si siY,const si siZ);
  
    // special functions

    void    inline  Zero                (void);                                           // set to (0;0;0)
    float   inline  Length              (void) const;                                     // vector length d
    float   inline  Length2             (void) const;                                     // vector squared length d^2
    bool            Normalize           (const float fNorm = 1.0f);                       // normalize
    bool            Normalize1          (void);                                           // normalize to 1.f - dedicated version
    CVect3D inline  Normalized          (void) const;                                     // return normalized copy

    ui              MainAxis            (void) const;                                     // main axis index
    bool    inline  Equals              (const CVect3D& v3V,const float fDelta) const;    // == with a tolerance
    bool    inline  IsParallel          (const CVect3D& v3V,const float fDelta) const;    // 'almost parallel' test
    bool    inline  IsOrthogonal        (const CVect3D& v3V,const float fDelta) const;    // 'almost orthogonal' test
    void    inline  RotateX             (const float fAngle);                             // rotate by an angle around X
    void    inline  RotateY             (const float fAngle);                             // rotate by an angle around Y
    void    inline  RotateZ             (const float fAngle);                             // rotate by an angle around Z

    // set

    inline  void    Set                 (const float    fD);
    inline  void    Set                 (const float    fX, const float fY,const float fZ);
    inline  void    Set                 (const CVect3D& v3V);
    inline  void    Set                 (const CVect4D& v4V,const ui uiDropAxis);
    inline  void    Set                 (const CVect2D& v2V,const float fZ);

    // friend classes

    friend  class   CVect2D;
    friend  class   CVect4D;
    friend  class   CMat3x3;
    friend  class   CMat4x4;

    friend  CMat3x3 inline operator  *  (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1*M2 accesses V

  // public data

  public:

    union
      {
      float         V[3];                                                                 // x/y/z coords
      struct 
        { 
        float       x,y,z;
        };
      struct
        {
        float       r,g,b;
        };
      struct
        {
        float       s,t,p;
        };
      };
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                4D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CVect4D
  {
  // public methods

  public:

    // constructors

    inline          CVect4D             (void);
    inline          CVect4D             (const float    fD);                              // x=y=z=w=fD
    inline          CVect4D             (const float    fX, const float fY,const float fZ,const float fW);        
                                                                                          // x=fX; y=fY; z=fZ; w=fW
    inline          CVect4D             (const CVect4D& v4V);                             // copy 
    inline          CVect4D             (const CVect3D& v3V,const float fW);              // cast v3 to v4; w=fW

    // assignment operators

    CVect4D&               operator   = (const CVect4D& v4V);                             // assignment
    CVect4D&               operator  += (const CVect4D& v4V);                             // incrementation
    CVect4D&               operator  -= (const CVect4D& v4V);                             // decrementation
    CVect4D&               operator  *= (const float    fK);                              // multiplication by a constant
    CVect4D&               operator  *= (const CVect4D& v4V);                             // term by term *
    CVect4D&               operator  /= (const float    fK);                              // division by a constant
    CVect4D&               operator  /= (const CVect4D& v4V);                             // term by term /
    float&                 operator  [] (const ui       uiIndex);                         // indexing
    const   float          operator  [] (const ui       uiIndex) const;                   // indexing

    // friend operators

    friend  CVect4D inline operator   - (const CVect4D& v4V);                             // -V
    friend  CVect4D inline operator   * (const float    fK,  const CVect4D& v4V);         // k*V 
    friend  CVect4D inline operator   * (const CVect4D& v4V, const float    fK);          // V*k
    friend  CVect4D inline operator   / (const CVect4D& v4V, const float    fK);          // V/k
    friend  CVect4D inline operator   - (const CVect4D& v4V1,const CVect4D& v4V2);        // V1-V2
    friend  CVect4D inline operator   + (const CVect4D& v4V1,const CVect4D& v4V2);        // V1+V2
    friend  float   inline operator   * (const CVect4D& v4V1,const CVect4D& v4V2);        // V1*V2 (dot product)
    friend  bool    inline operator  == (const CVect4D& v4V1,const CVect4D& v4V2);        // V1==V2
    friend  bool    inline operator  != (const CVect4D& v4V1,const CVect4D& v4V2);        // V1!=V2
    friend  CVect4D inline operator   * (const CMat4x4& m4M, const CVect4D& v4V);         // M*V
    friend  CVect4D inline operator   * (const CVect4D& v4V, const CMat4x4& m4M);         // V*M

    friend  ostream&       operator  << (ostream& s,               CVect4D& v4V);         // output
    friend  ostream&       operator  << (ostream& s,         const CVect4D& v4V);         // output 

    // friend functions

    friend  void    inline Swap         (      CVect4D& v4V1,      CVect4D& v4V2);        // swap
    friend  CVect4D inline Smallest     (const CVect4D& v4V1,const CVect4D& v4V2);        // mini(V1,V2) (smallest vector) 
    friend  CVect4D inline Longest      (const CVect4D& v4V1,const CVect4D& v4V2);        // maxi(V1,V2) (longest  vector) 
    friend  CVect4D inline Prod         (const CVect4D& v4V1,const CVect4D& v4V2);        // term by term *
    friend  CVect4D inline Div          (const CVect4D& v4V1,const CVect4D& v4V2);        // term by term /
    friend  float   inline Dot          (const CVect4D& v4V1,const CVect4D& v4V2);        // dot   product
    friend  CVect4D inline SwapAxis     (      CVect4D& v4V, const si siX,const si siY,const si siZ,const si siW);

    // special functions

    void    inline  Zero                (void);                                           // set to (0;0;0;0)
    float   inline  Length              (void) const;                                     // vector length d
    float   inline  Length2             (void) const;                                     // vector squared length d^2
    bool            Normalize           (const float fNorm = 1.0f);                       // normalize
    bool            Normalize1          (void);                                           // normalize to 1.f - dedicated version
    CVect4D inline  Normalized          (void) const;                                     // return normalized copy

    ui              MainAxis            (void) const;                                     // main axis index
    bool    inline  Equals              (const CVect4D& v4V,const float fDelta) const;    // == with a tolerance
    bool    inline  IsParallel          (const CVect4D& v4V,const float fDelta) const;    // 'almost parallel' test
    bool    inline  IsOrthogonal        (const CVect4D& v4V,const float fDelta) const;    // 'almost orthogonal' test

    // set

    inline  void    Set                 (const float    fD);
    inline  void    Set                 (const float    fX, const float fY,const float fZ,const float fW);
    inline  void    Set                 (const CVect4D& v4V); 
    inline  void    Set                 (const CVect3D& v3V,const float fW);

    // friend classes

    friend  class   CVect3D;
    friend  class   CMat4x4;

    friend  CMat4x4 inline operator  *  (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1*M2 accesses V

    friend  float   inline operator  *  (const CVect4D& v4V, const CVect3D& v3V);         // V4*V3 accesses V
    friend  float   inline operator  *  (const CVect3D& v3V, const CVect4D& v4V);         // V3*V4 accesses V
    friend  CVect4D inline operator  *  (const CMat4x4& m4M, const CVect3D& v3V);         // M4*V3 accesses V
    friend  CVect4D inline operator  *  (const CVect3D& v3V, const CMat4x4& m4M);         // V3*M4 accesses V

  // public data

  public:

    union
      {
      float         V[4];                                                                 // x/y/z/w coords
      struct 
        { 
        float       x,y,z,w; 
        };
      struct
        {
        float       r,g,b,a;
        };
      struct
        {
        float       s,t,p,q;
        };
      };
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                3*3 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CMat3x3
  {
  // public methods

  public:

    // constructors

    inline          CMat3x3             (void);
    inline          CMat3x3             (const float    fD);                              // all values=fD
    inline          CMat3x3             (const CVect3D& v3V0,const CVect3D& v3V1,const CVect3D& v3V2);        
                                                                                          // 3 columns of the matrix
    inline          CMat3x3             (const CMat3x3&     m3M);                         // copy
                    CMat3x3             (const CQuaternion& qRot);

    // assignment operators

    CMat3x3&               operator   = (const CMat3x3& m3M);                             // assignment
    CMat3x3&               operator  += (const CMat3x3& m3M);                             // incrementation
    CMat3x3&               operator  -= (const CMat3x3& m3M);                             // decrementation
    CMat3x3&               operator  *= (const float    fK);                              // multiplication by a constant
    CMat3x3&               operator  *= (const CMat3x3& m3M);                             // multiplication
    CMat3x3&               operator  /= (const float    fK);                              // division by a constant
    CMat3x3&               operator  /= (const CMat3x3& m3M);                             // multiplication by inverse
    CVect3D&               operator  [] (const ui       uiIndex);                         // indexing column
    const   CVect3D        operator  [] (const ui       uiIndex) const;                   // indexing column

    // friend operators

    friend  CMat3x3 inline operator   - (const CMat3x3& m3M);                             // -M
    friend  CMat3x3 inline operator   * (const float    fK,  const CMat3x3& m3M);         // k*M
    friend  CMat3x3 inline operator   * (const CMat3x3& m3M, const float    fK);          // M*k
    friend  CMat3x3 inline operator   / (const CMat3x3& m3M, const float    fK);          // M/k
    friend  CMat3x3 inline operator   - (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1-M2
    friend  CMat3x3 inline operator   + (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1+M2
    friend  CMat3x3 inline operator   * (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1*M2 
    friend  bool    inline operator  == (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1==M2
    friend  bool    inline operator  != (const CMat3x3& m3M1,const CMat3x3& m3M2);        // M1!=M2

    friend  ostream&       operator  << (ostream& s,               CMat3x3& m3M);         // output
    friend  ostream&       operator  << (ostream& s,         const CMat3x3& m3M);         // output

    // friend functions

    friend  void    inline Swap         (      CMat3x3& m3M1,      CMat3x3& m3M2);        // swap

    // special functions

    void            inline Zero         (void);                                           // zeros everywhere
    void            inline SetIdentity  (void);                                           // set to identity
    void                   Transpose    (void);                                           // transpose
    CMat3x3         inline Transposed   (void) const;                                     // return transposed copy
    void            inline Invert       (void);                                           // invert
    CMat3x3                Inverted     (void) const;                                     // return inverted copy

    void            inline Translation  (const CVect2D& v2V);
    void                   Rotation     (const CVect2D& v2Center,const float fAngle);
    void                   RotationX    (const float    fAngle);
    void                   RotationY    (const float    fAngle);
    void                   RotationZ    (const float    fAngle);
    void                   RotationXYZ  (const CVect3D& v3Angles);                        // rotation from 3 angles (Bryant)
    void            inline Scale        (const CVect3D& v3Scale);

    // set

    inline  void    Set                 (const float    fD);
    inline  void    Set                 (const CVect3D& v3V0,const CVect3D& v3V1,const CVect3D& v3V2);        
    inline  void    Set                 (const CMat3x3& m3M);

    // friend classes

    friend  class   CVect3D;
    friend  class   CMat4x4;

    friend  CVect3D inline operator  *  (const CMat3x3& m3M,const CVect3D& v3V);          // M*V accesses V
    friend  CVect3D inline operator  *  (const CVect3D& v3V,const CMat3x3& m3M);          // V*M accesses V

  // public data

  public:

    static const CMat3x3      Identity;
    CVect3D                   Col[3];                                                     // 3 x/y/z columns
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                4*4 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CMat4x4
  {
  // public methods

  public:

    // constructors

    inline          CMat4x4             (void);
    inline          CMat4x4             (const float    fD);                              // all values=fD
    inline          CMat4x4             (const CVect4D& v4V0,const CVect4D& v4V1,const CVect4D& v4V2,const CVect4D& v4V3);        
                                                                                          // 4 columns of the matrix
    inline          CMat4x4             (const CMat4x4&     m4M);                         // copy 
                    CMat4x4             (const CQuaternion& qRot);

    // assignment operators

    CMat4x4&               operator   = (const CMat4x4& m4M);                             // assignment
    CMat4x4&               operator  += (const CMat4x4& m4M);                             // incrementation
    CMat4x4&               operator  -= (const CMat4x4& m4M);                             // decrementation
    CMat4x4&               operator  *= (const float    fK);                              // multiplication by a constant
    CMat4x4&               operator  *= (const CMat4x4& m4M);                             // multiplication
    CMat4x4&               operator  /= (const float    fK);                              // division by a constant
    CMat4x4&               operator  /= (const CMat4x4& m4M);                             // multiplication by inverse
    CVect4D&               operator  [] (const ui       uiIndex);                         // indexing line
    const   CVect4D        operator  [] (const ui       uiIndex) const;                   // indexing

    // friend operators

    friend  CMat4x4 inline operator   - (const CMat4x4& m4M);                             // -M
    friend  CMat4x4 inline operator   * (const float    fK,  const CMat4x4& m4M);         // k*M
    friend  CMat4x4 inline operator   * (const CMat4x4& m4M, const float    fK);          // M*k
    friend  CMat4x4 inline operator   / (const CMat4x4& m4M, const float    fK);          // M/k
    friend  CMat4x4 inline operator   - (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1-M2
    friend  CMat4x4 inline operator   + (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1+M2
    friend  CMat4x4 inline operator   * (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1*M2 
    friend  bool    inline operator  == (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1==M2
    friend  bool    inline operator  != (const CMat4x4& m4M1,const CMat4x4& m4M2);        // M1!=M2

    friend  ostream&       operator  << (ostream& s,               CMat4x4& m4M);         // output
    friend  ostream&       operator  << (ostream& s,         const CMat4x4& m4M);         // output

    // friend functions

    friend  void    inline Swap         (      CMat4x4& m4M1,      CMat4x4& m4M2);        // swap

    // special functions

    void            inline Zero         (void);                                           // zeros everywhere
    void            inline SetIdentity  (void);                                           // set to identity
    void                   Transpose    (void);                                           // transpose
    CMat4x4         inline Transposed   (void) const;                                     // return transpose
    void            inline Invert       (void);                                           // invert
    CMat4x4                Inverted     (void) const;                                     // return inverse
    CMat4x4               AffineInverted(void) const;                                     // return inverse (special case)
    CMat4x4             RotTransInverted(void) const;                                     // return inverse (special case)

    void            inline Translation  (const CVect3D& v3V);
    void                   Rotation     (const CVect3D& v3Axis,const float fAngle);
    void                   RotationX    (const float    fAngle);
    void                   RotationY    (const float    fAngle);
    void                   RotationZ    (const float    fAngle);
    void                   RotationXYZ  (const CVect3D& v3Angles);                        // rotation from 3 angles (Bryant)
    void            inline Scale        (const CVect3D& v3Scale);

    void                   MulHomo      (const CMat4x4& m4M);                             // homogeneous multiplication
    CVect3D         inline RotateVector (const CVect3D& v3V);

    //set

    inline  void    Set                 (const float    fD);
    inline  void    Set                 (const CVect4D& v4V0,const CVect4D& v4V1,const CVect4D& v4V2,const CVect4D& v4V3);        
    inline  void    Set                 (const CMat4x4& m4M);

    // friend classes

    friend  class   CVect4D;
    friend  class   CMat3x3;

    friend  CVect4D inline operator  *  (const CMat4x4& m4M,const CVect4D& v4V);          // M*V accesses V
    friend  CVect4D inline operator  *  (const CVect4D& v4V,const CMat4x4& m4M);          // V*M accesses V
    friend  CVect4D inline operator  *  (const CMat4x4& m4M,const CVect3D& v3V);          // M*V accesses V
    friend  CVect4D inline operator  *  (const CVect3D& v3V,const CMat4x4& m4M);          // V*M accesses V

  // public data

  public:

    static const CMat4x4      Identity;
    CVect4D                   Col[4];                                                     // 4 x/y/z/w columns
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "VectorMatrix.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_VECTORMATRIX_H_
