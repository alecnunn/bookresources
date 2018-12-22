//--------------------------------------------------------------------------------------------------------------------//
//                                                QUATERNION CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_QUATERNION_H_
#define   _GPG4_QUATERNION_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                QUATERNION                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CQuaternion
  {
  // public methods

  public:

    // constructors

    inline          CQuaternion         (void);
    inline          CQuaternion         (const CVect3D&     v3Vector,const float fScalar);
    inline          CQuaternion         (const CVect4D&     v4Quat);                      // x/y/z=vector, w=scalar
    inline          CQuaternion         (const float        fX,const float fY,const float fZ,const float fW);
    inline          CQuaternion         (const CQuaternion& qQuat);
    inline          CQuaternion         (const CMat3x3&     m3Rot);
    inline          CQuaternion         (const CMat4x4&     m4Rot);

    // assignment operators

    CQuaternion&           operator   = (const CQuaternion& qQuat);                       // assignment
    CQuaternion&           operator  += (const CQuaternion& qQuat);                       // incrementation
    CQuaternion&           operator  -= (const CQuaternion& qQuat);                       // decrementation
    CQuaternion&           operator  *= (const CQuaternion& qQuat);                       // multiplication
    CQuaternion&           operator  *= (const float        fK);                          // multiplication by a constant
    CQuaternion&           operator  /= (const float        fK);                          // division       by a constant

    // friends operators

    friend CQuaternion inline operator -(const CQuaternion& qQuat);                                 // -Q
    friend CQuaternion inline operator *(const float        fK,    const CQuaternion& qQuat);       // k*Q 
    friend CQuaternion inline operator *(const CQuaternion& qQuat, const float        fK);          // Q*k
    friend CQuaternion inline operator /(const CQuaternion& qQuat, const float        fK);          // Q/k
    friend CQuaternion inline operator +(const CQuaternion& qQuat1,const CQuaternion& qQuat2);      // Q1+Q2
    friend CQuaternion inline operator -(const CQuaternion& qQuat1,const CQuaternion& qQuat2);      // Q1-Q2
    friend CQuaternion inline operator *(const CQuaternion& qQuat1,const CQuaternion& qQuat2);      // Q1*Q2
    friend bool        inline operator==(const CQuaternion& qQuat1,const CQuaternion& qQuat2);      // Q1==Q2
    friend bool        inline operator!=(const CQuaternion& qQuat1,const CQuaternion& qQuat2);      // Q1!=Q2

    friend ostream&        operator  << (ostream& s,                     CQuaternion& qQuat);       // output
    friend ostream&        operator  << (ostream& s,               const CQuaternion& qQuat);       // output

    // friend functions

    friend CQuaternion        Slerp     (const CQuaternion& qQuat1,const CQuaternion& qQuat2,const float fT);

    // special functions

    CQuaternion     inline    Conjugate (void) const;
    float           inline    Length    (void) const;
    float           inline    Length2   (void) const;
    void            inline    Identity  (void);
    void            inline    Normalize (void);

    // conversions

    void            FromAxisAngle       (const CVect3D&  v3Axis,const float   fAngle);
    void            ToAxisAngle         (      CVect3D* pv3Axis,      float* pfAngle) const;

    void            FromRotation        (const CMat3x3&  m3M);
    void            FromRotation        (const CMat4x4&  m4M);
    void            ToRotation          (      CMat3x3* pm3M) const;
    void            ToRotation          (      CMat4x4* pm4M) const;

    // get/set

    inline float    GetScalar           (void) const;
    inline CVect3D& GetVector           (void);

    inline void     SetScalar           (const float        fScalar);
    inline void     SetVector           (const CVect3D&     v3Vector);
    inline void     SetVector           (const float        fX,const float fY,const float fZ);
    inline void     Set                 (const float        fX,const float fY,const float fZ,const float fW);
    inline void     Set                 (const CVect4D&     v4Quat);
    inline void     Set                 (const CQuaternion& qQuat);

  // public data

  public:
                                                                                          // ! MUST be the same size as a CVect4D
    CVect3D         m_v3Vector;                                                           // imaginary part
    float           m_fScalar;                                                            // real      part
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "Quaternion.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_QUATERNION_H_
