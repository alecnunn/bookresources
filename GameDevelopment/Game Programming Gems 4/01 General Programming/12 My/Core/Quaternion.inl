//--------------------------------------------------------------------------------------------------------------------//
//                                                QUATERNION CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_QUATERNION_INL_
#define   _GPG4_QUATERNION_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <math.h>
#include  "MyAssert.h"

//--------------------------------------------------------------------------------------------------------------------//
//                                                QUATERNION                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{

//----------------------------------------------- constructors -------------------------------------------------------//

inline CQuaternion::CQuaternion()                                               // do nothing
  {}

inline CQuaternion::CQuaternion(const CVect3D& v3Vector,const float fScalar)    // vector+scalar
  { m_v3Vector = v3Vector; m_fScalar = fScalar; }

inline CQuaternion::CQuaternion(const CVect4D& v4Quat)                          // x/y/z=vector, w=scalar
  { m_v3Vector.x = v4Quat.x; m_v3Vector.y = v4Quat.y;
    m_v3Vector.z = v4Quat.z; m_fScalar    = v4Quat.w; }

inline CQuaternion::CQuaternion(const float fX,const float fY,const float fZ,const float fW)
  { m_v3Vector.x = fX; m_v3Vector.y = fY; m_v3Vector.z = fZ; m_fScalar = fW; }

inline CQuaternion::CQuaternion(const CQuaternion& qQuat)                       // copy
  { m_v3Vector = qQuat.m_v3Vector; m_fScalar = qQuat.m_fScalar; }

inline CQuaternion::CQuaternion(const CMat3x3& m3Rot)
  { FromRotation(m3Rot); }

inline CQuaternion::CQuaternion(const CMat4x4& m4Rot)
  { FromRotation(m4Rot); }

//----------------------------------------------- assignment operators -----------------------------------------------//

inline CQuaternion& CQuaternion::operator  = (const CQuaternion& qQuat)         // assignment
  { m_v3Vector  = qQuat.m_v3Vector; m_fScalar  = qQuat.m_fScalar; return *this; }

inline CQuaternion& CQuaternion::operator += (const CQuaternion& qQuat)         // incrementation
  { m_v3Vector += qQuat.m_v3Vector; m_fScalar += qQuat.m_fScalar; return *this; }

inline CQuaternion& CQuaternion::operator -= (const CQuaternion& qQuat)         // decrementation
  { m_v3Vector -= qQuat.m_v3Vector; m_fScalar -= qQuat.m_fScalar; return *this; }

inline CQuaternion& CQuaternion::operator *= (const CQuaternion& qQuat)         // multiplication
  {
  CVect3D v3Tmp((m_v3Vector^qQuat.m_v3Vector) + m_v3Vector*qQuat.m_fScalar + m_fScalar*qQuat.m_v3Vector);
  m_fScalar  =   m_fScalar *qQuat.m_fScalar   - m_v3Vector*qQuat.m_v3Vector;
  m_v3Vector =   v3Tmp;
  return *this;
  }

inline CQuaternion& CQuaternion::operator *= (const float fK)                   // multiplication by a constant
  { m_v3Vector *= fK; m_fScalar *= fK; return *this; }

inline CQuaternion& CQuaternion::operator /= (const float fK)                   // division by a constant
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    m_v3Vector *= fKinv; m_fScalar *= fKinv; return *this; }

//----------------------------------------------- friend operators ---------------------------------------------------//

inline CQuaternion  operator  - (const CQuaternion& qQuat)                      // -Q
  { return CQuaternion(-qQuat.m_v3Vector,-qQuat.m_fScalar); }

inline CQuaternion  operator  * (const float fK,const CQuaternion& qQuat)       // k*Q
  { return CQuaternion(fK*qQuat.m_v3Vector,fK*qQuat.m_fScalar); }

inline CQuaternion  operator  * (const CQuaternion& qQuat,const float fK)       // Q*k
  { return CQuaternion(fK*qQuat.m_v3Vector,fK*qQuat.m_fScalar); }

inline CQuaternion  operator  / (const CQuaternion& qQuat,const float fK)       // Q/k
  { MY_ASSERT(fK != 0.f,"division by zero !");
    float fKinv   = 1.f/fK;
    return CQuaternion(fKinv*qQuat.m_v3Vector,fKinv*qQuat.m_fScalar); }

inline CQuaternion  operator  + (const CQuaternion& qQuat1,const CQuaternion& qQuat2)     // Q1+Q2
  { return CQuaternion(qQuat1.m_v3Vector+qQuat2.m_v3Vector,qQuat1.m_fScalar+qQuat2.m_fScalar); }

inline CQuaternion  operator  - (const CQuaternion& qQuat1,const CQuaternion& qQuat2)     // Q1-Q2
  { return CQuaternion(qQuat1.m_v3Vector-qQuat2.m_v3Vector,qQuat1.m_fScalar-qQuat2.m_fScalar); }

inline CQuaternion  operator  * (const CQuaternion& qQuat1,const CQuaternion& qQuat2)     // Q1*Q2
  { return CQuaternion((qQuat1.m_v3Vector^qQuat2.m_v3Vector)+qQuat1.m_v3Vector*qQuat2.m_fScalar+qQuat1.m_fScalar*qQuat2.m_v3Vector,
                        qQuat1.m_fScalar *qQuat2.m_fScalar  -qQuat1.m_v3Vector*qQuat2.m_v3Vector); }

inline bool         operator == (const CQuaternion& qQuat1,const CQuaternion& qQuat2)     // Q1==Q2
  { return ((qQuat1.m_fScalar == qQuat2.m_fScalar) && (qQuat1.m_v3Vector == qQuat2.m_v3Vector)); }

inline bool         operator != (const CQuaternion& qQuat1,const CQuaternion& qQuat2)     // Q1!=Q2
  { return ((qQuat1.m_fScalar != qQuat2.m_fScalar) || (qQuat1.m_v3Vector != qQuat2.m_v3Vector)); }

inline ostream& operator << (ostream& s,CQuaternion& qQuat)                     // output
  { return s << qQuat.m_v3Vector << ';' << qQuat.m_fScalar; }

inline ostream& operator << (ostream& s,const CQuaternion& qQuat)               // output
  { 
  CQuaternion qTmp(qQuat);
  return s << qTmp.m_v3Vector << ';' << qTmp.m_fScalar;
  }

//----------------------------------------------- special functions --------------------------------------------------//

inline CQuaternion CQuaternion::Conjugate() const
  { return CQuaternion(-m_v3Vector,m_fScalar); }

inline float CQuaternion::Length () const
  { return(float)sqrt(Length2()); }

inline float CQuaternion::Length2() const
  { return(m_fScalar*m_fScalar + m_v3Vector*m_v3Vector); }

inline void CQuaternion::Identity()
  { m_v3Vector.Set(0.f); m_fScalar = 1.f; }

inline void CQuaternion::Normalize()
  { float fL = 1.f/Length(); m_v3Vector *= fL; m_fScalar *= fL; }

//----------------------------------------------- get/set ------------------------------------------------------------//

inline float CQuaternion::GetScalar() const
  { return m_fScalar; }

inline CVect3D& CQuaternion::GetVector()
  { return m_v3Vector; }

inline void CQuaternion::SetScalar(const float fScalar)
  { m_fScalar = fScalar; }

inline void CQuaternion::SetVector(const CVect3D& v3Vector)
  { m_v3Vector = v3Vector; }

inline void CQuaternion::SetVector(const float fX,const float fY,const float fZ)
  { m_v3Vector.Set(fX,fY,fZ); }

inline void CQuaternion::Set(const float fX,const float fY,const float fZ,const float fW)
  { m_v3Vector.Set(fX,fY,fZ); m_fScalar = fW; }

inline void CQuaternion::Set(const CVect4D& v4Quat)
  { m_v3Vector.Set(v4Quat.x,v4Quat.y,v4Quat.z); m_fScalar = v4Quat.w; }

inline void CQuaternion::Set(const CQuaternion& qQuat)
  { m_v3Vector.Set(qQuat.m_v3Vector); m_fScalar = qQuat.m_fScalar; }


//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_QUATERNION_INL_
