//--------------------------------------------------------------------------------------------------------------------//
//                                                QUATERNION CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Quaternion.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                FRIEND FUNCTIONS                                                    //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Slerp --------------------------------------------------------------//
// spherical linear interpolation [AART]
// in : quaternions to interpolate,parameter
// out: intermediate quaternion
//--------------------------------------------------------------------------------------------------------------------//

CQuaternion Slerp(const CQuaternion& qQuat1,const CQuaternion& qQuat2,const float fT)
  {
  float fOmega,fCosom,fSinom,fSclp,fSclq;
  fCosom = qQuat1.m_v3Vector*qQuat2.m_v3Vector+qQuat1.m_fScalar*qQuat2.m_fScalar;

  if((1.f+fCosom) > 1e-5)
    {
    if((1.f-fCosom) > 1e-5)
      {
      fOmega = (float)acos(fCosom);
      fSinom = (float)sin (fOmega);
      fSclp  = (float)sin((1.f-fT)*fOmega)/fSinom;
      fSclq  = (float)sin(     fT *fOmega)/fSinom;
      }

    else
      {
      fSclp = 1.f-fT;
      fSclq = fT;
      }

    return(fSclp*qQuat1+fSclq*qQuat2);
    }

  else
    {
    CQuaternion qR(-qQuat1.m_v3Vector.y,+qQuat1.m_v3Vector.x,-qQuat1.m_fScalar,+qQuat1.m_v3Vector.z);
    fSclp = (float)sin((1.f-fT)*PI*0.5f);
    fSclq = (float)sin(     fT *PI*0.5f);
    
    qR.m_v3Vector = fSclp*qQuat1.m_v3Vector + fSclq*qR.m_v3Vector;
    return qR;
    }
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONVERSIONS                                                         //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- FromAxisAngle ------------------------------------------------------//
// axis+angle -> quaternion
// in : axis,angle
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::FromAxisAngle(const CVect3D& v3Axis,const float fAngle)
  {
  m_v3Vector = v3Axis*(float)sin(fAngle*0.5f);
  m_fScalar  =        (float)cos(fAngle*0.5f);
  }

//----------------------------------------------- ToAxisAngle --------------------------------------------------------//
// quaternion -> axis+angle [Eberly]
// in : axis ptr,angle ptr
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::ToAxisAngle(CVect3D* pv3Axis,float* pfAngle) const
  {
  float fLength2 = m_v3Vector*m_v3Vector;

  if(fLength2 < 1e-6)
    {
    pv3Axis->Set(0.f);
    *pfAngle = 0.f;
    }

  else
    {
    *pfAngle = 2.f*(float)acos(m_fScalar);
    float fInvLength2 = 1.f/fLength2;
    pv3Axis->Set(m_v3Vector*fInvLength2);
    }
  }

//----------------------------------------------- FromRotation -------------------------------------------------------//
// m3x3 -> quaternion [AART]
// in : 3*3 matrix
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::FromRotation(const CMat3x3& m3M)
  {
  float fTr = m3M[0][0]+m3M[1][1]+m3M[2][2];
  float fS;

  if(fTr > 0.f)
    {
    fS = (float)sqrt(fTr+1.f);
    m_fScalar = fS*0.5f;
    fS = 0.5f/fS;
    m_v3Vector.Set((m3M[2][1]-m3M[1][2])*fS,(m3M[0][2]-m3M[2][0])*fS,(m3M[1][0]-m3M[0][1])*fS);
    }

  else
    {
    u32 Next[3] = { Y,Z,X };
    u32 I = X;
    if(m3M[Y][Y] > m3M[X][X]) I = Y;
    if(m3M[Z][Z] > m3M[I][I]) I = Z;
    u32 J = Next[I];
    u32 K = Next[J];

    fS = (float)sqrt((m3M[I][I] - (m3M[J][J]+m3M[K][K])) + 1.f);
    m_v3Vector[I] = fS*0.5f;
    fS = 0.5f/fS;
    m_fScalar     = (m3M[K][J]-m3M[J][K])*fS;
    m_v3Vector[J] = (m3M[J][I]+m3M[I][J])*fS;
    m_v3Vector[K] = (m3M[K][I]+m3M[I][K])*fS;
    }
  }

//----------------------------------------------- FromRotation -------------------------------------------------------//
// m4x4 -> quaternion [AART]
// in : 4*4 matrix
// out:
// rem: same as above (m3M->m4M)
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::FromRotation(const CMat4x4& m4M)
  {
  float fTr = m4M[0][0]+m4M[1][1]+m4M[2][2];
  float fS;

  if(fTr > 0.f)
    {
    fS = (float)sqrt(fTr+1.f);
    m_fScalar = fS*0.5f;
    fS = 0.5f/fS;
    m_v3Vector.Set((m4M[2][1]-m4M[1][2])*fS,(m4M[0][2]-m4M[2][0])*fS,(m4M[1][0]-m4M[0][1])*fS);
    }

  else
    {
    u32 Next[3] = { Y,Z,X };
    u32 I = X;
    if(m4M[Y][Y] > m4M[X][X]) I = Y;
    if(m4M[Z][Z] > m4M[I][I]) I = Z;
    u32 J = Next[I];
    u32 K = Next[J];

    fS = (float)sqrt((m4M[I][I] - (m4M[J][J]+m4M[K][K])) + 1.f);
    m_v3Vector[I] = fS*0.5f;
    fS = 0.5f/fS;
    m_fScalar     = (m4M[K][J]-m4M[J][K])*fS;
    m_v3Vector[J] = (m4M[J][I]+m4M[I][J])*fS;
    m_v3Vector[K] = (m4M[K][I]+m4M[I][K])*fS;
    }
  }

//----------------------------------------------- ToRotation ---------------------------------------------------------//
// quaternion -> m3x3
// in : 3*3 matrix ptr
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::ToRotation(CMat3x3* pm3M) const
  {
  float s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;
  s = 2.f/Length2();                                        // NB: unit quaternion => s=2.f;

  xs = m_v3Vector[X]*s;  ys = m_v3Vector[Y]*s;  zs = m_v3Vector[Z]*s;
  wx = m_fScalar    *xs; wy = m_fScalar    *ys; wz = m_fScalar    *zs;
  xx = m_v3Vector[X]*xs; xy = m_v3Vector[X]*ys; xz = m_v3Vector[X]*zs;
  yy = m_v3Vector[Y]*ys; yz = m_v3Vector[Y]*zs; zz = m_v3Vector[Z]*zs;

  (*pm3M)[0].Set(1.f-(yy+zz), xy-wz,       xz+wy);          // col 0
  (*pm3M)[1].Set(xy+wz,       1.f-(xx+zz), yz-wx);          // col 1
  (*pm3M)[2].Set(xz-wy,       yz+wx,       1.f-(xx+yy));    // col 2
  }

//----------------------------------------------- ToRotation ---------------------------------------------------------//
// quaternion -> m4x4
// in : 4*4 matrix ptr
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CQuaternion::ToRotation(CMat4x4* pm4M) const
  {
  float s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;
  s = 2.f/Length2();                                        // NB: unit quaternion => s=2.f;

  xs = m_v3Vector[X]*s;  ys = m_v3Vector[Y]*s;  zs = m_v3Vector[Z]*s;
  wx = m_fScalar    *xs; wy = m_fScalar    *ys; wz = m_fScalar    *zs;
  xx = m_v3Vector[X]*xs; xy = m_v3Vector[X]*ys; xz = m_v3Vector[X]*zs;
  yy = m_v3Vector[Y]*ys; yz = m_v3Vector[Y]*zs; zz = m_v3Vector[Z]*zs;

  (*pm4M)[0].Set(1.f-(yy+zz),xy-wz,      xz+wy,      0.f);  // col 0
  (*pm4M)[1].Set(xy+wz,      1.f-(xx+zz),yz-wx,      0.f);  // col 1
  (*pm4M)[2].Set(xz-wy,      yz+wx,      1.f-(xx+yy),0.f);  // col 2
  (*pm4M)[3].Set(0.f,        0.f,        0.f,        1.f);  // col 3
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
