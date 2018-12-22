//--------------------------------------------------------------------------------------------------------------------//
//                                                VECTORS & MATRICES CLASSES                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "VectorMatrix.h"

#include  "MyAssert.h"
#include  "Quaternion.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

const CMat3x3 CMat3x3::Identity(CVect3D(1.f,0.f,0.f),    
                                CVect3D(0.f,1.f,0.f),    
                                CVect3D(0.f,0.f,1.f));

const CMat4x4 CMat4x4::Identity(CVect4D(1.f,0.f,0.f,0.f),
                                CVect4D(0.f,1.f,0.f,0.f),
                                CVect4D(0.f,0.f,1.f,0.f),
                                CVect4D(0.f,0.f,0.f,1.f));

//--------------------------------------------------------------------------------------------------------------------//
//                                                2D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- special functions --------------------------------------------------//

bool CVect2D::Normalize(const float fNorm/*=1.0f*/)                             // in:desired norm ; out:-
  {
  if(fNorm == 1.0f) return(Normalize1());

  float fL = Length();
  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }
  *this *= fNorm/fL;
  return(true); 
  }

//

bool CVect2D::Normalize1()                                                      // norm to 1.0f (dedicated version)
  {
  float fL = Length2();
  if((fL >= 1.0f-EPS_NORMALIZE) && (fL <= 1.0f+EPS_NORMALIZE))
    return(true);                                                               // already normalized

  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }

  fL = (float)sqrt(fL);
  *this /= fL;
  return(true); 
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                3D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- special functions --------------------------------------------------//

bool CVect3D::Normalize(const float fNorm/*=1.0f*/)                             // in:desired norm ; out:-
  {
  if(fNorm == 1.0f) return(Normalize1());

  float fL = Length();
  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }
  *this *= fNorm/fL;
  return(true); 
  }

//

bool CVect3D::Normalize1()                                                      // norm to 1.0f (dedicated version)
  {
  float fL = Length2();
  if((fL >= 1.0f-EPS_NORMALIZE) && (fL <= 1.0f+EPS_NORMALIZE))
    return(true);                                                               // already normalized

  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }

  fL = (float)sqrt(fL);
  *this /= fL;
  return(true); 
  }

//

ui CVect3D::MainAxis() const                                                    // main axis index
  {
  float fX = (float)fabs(x);
  float fY = (float)fabs(y);
  float fZ = (float)fabs(z);
  if(fX >= fY)
    {
    if(fX >= fZ) return X;
    }
  else
    {
    if(fY >= fZ) return Y;
    }
  return Z;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                4D VECTOR                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- special functions --------------------------------------------------//

bool CVect4D::Normalize(const float fNorm/*=1.0f*/)                             // in:desired norm ; out:-
  {
  if(fNorm == 1.0f) return(Normalize1());

  float fL = Length();
  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }
  *this *= fNorm/fL;
  return(true); 
  }

//

bool CVect4D::Normalize1()                                                      // norm to 1.0f (dedicated version)
  {
  float fL = Length2();
  if((fL >= 1.0f-EPS_NORMALIZE) && (fL <= 1.0f+EPS_NORMALIZE))
    return(true);                                                               // already normalized

  if(fL == 0.f) 
    {
    MY_ASSERT(false,"0-length vector; can't be normalized");
    return(false);
    }

  fL = (float)sqrt(fL);
  *this /= fL;
  return(true); 
  }

//

ui CVect4D::MainAxis() const                                                    // main axis index
  {
  float fX = (float)fabs(x);
  float fY = (float)fabs(y);
  float fZ = (float)fabs(z);
  float fW = (float)fabs(w);
  if(fX >= fY)
    {
    if(fX >= fZ)
      {
      if(fX >= fW) return X;
      else         return W;
      }
    else
      {
      if(fZ >= fW) return Z;
      else         return W;
      }
    }
  else
    {
    if(fY >= fZ)
      {
      if(fY >= fW) return Y;
      else         return W;
      }
    else
      {
      if(fZ >= fW) return Z;
      else         return W;
      }
    }
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                3*3 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

CMat3x3::CMat3x3(const CQuaternion& qRot)
  {
  qRot.ToRotation(this);
  }

//----------------------------------------------- special functions --------------------------------------------------//

void CMat3x3::Transpose()                                                       // transpose
  {
  float fTmp;
  fTmp = Col[0][1]; Col[0][1] = Col[1][0]; Col[1][0] = fTmp;
  fTmp = Col[0][2]; Col[0][2] = Col[2][0]; Col[2][0] = fTmp;
  fTmp = Col[1][2]; Col[1][2] = Col[2][1]; Col[2][1] = fTmp;
  }

//

CMat3x3 CMat3x3::Inverted() const                                               // return inverted copy
  {                                                                             // cofactors transpose
  CMat3x3 m3B;

  m3B[0][0] = Col[1][1]*Col[2][2] - Col[2][1]*Col[1][2];
  m3B[0][1] = Col[0][2]*Col[2][1] - Col[0][1]*Col[2][2];
  m3B[0][2] = Col[0][1]*Col[1][2] - Col[1][1]*Col[0][2];

  m3B[1][0] = Col[2][0]*Col[1][2] - Col[1][0]*Col[2][2];
  m3B[1][1] = Col[0][0]*Col[2][2] - Col[0][2]*Col[2][0];
  m3B[1][2] = Col[1][0]*Col[0][2] - Col[0][0]*Col[1][2];

  m3B[2][0] = Col[1][0]*Col[2][1] - Col[1][1]*Col[2][0];
  m3B[2][1] = Col[0][1]*Col[2][0] - Col[0][0]*Col[2][1];
  m3B[2][2] = Col[0][0]*Col[1][1] - Col[0][1]*Col[1][0];

  float fDet = Col[0][0]*m3B[0][0] + Col[1][0]*m3B[0][1] + Col[2][0]*m3B[0][2];
  if(fDet < 1e-6) return Identity;

  m3B /= fDet;
  return m3B;
  }

//

void CMat3x3::Rotation(const CVect2D& v2Center,const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);
  float fT   = 1.f-fCos;

  Col[0].Set(fCos,                         fSin,                         0.f);
  Col[1].Set(-fSin,                        fCos,                         0.f);
  Col[2].Set(v2Center.x*fT+v2Center.y*fSin,v2Center.y*fT-v2Center.x*fSin,1.f);
  }

//

void CMat3x3::RotationX(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set(1.f, 0.f,  0.f);
  Col[1].Set(0.f,fCos,-fSin);
  Col[2].Set(0.f,fSin, fCos);
  }

//

void CMat3x3::RotationY(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set( fCos,0.f,fSin);
  Col[1].Set(  0.f,1.f, 0.f);
  Col[2].Set(-fSin,0.f,fCos);
  }

//

void CMat3x3::RotationZ(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set(fCos,-fSin,0.f);
  Col[1].Set(fSin, fCos,0.f);
  Col[2].Set( 0.f,  0.f,1.f);
  }

//

void CMat3x3::RotationXYZ(const CVect3D& v3Angles)
  {
  RotationZ(v3Angles.z);
  CMat3x3 m3Y,m3X;
  m3Y.RotationY(v3Angles.y);
  m3X.RotationX(v3Angles.x);
  *this *= m3Y*m3X;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                4*4 MATRIX                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- constructors -------------------------------------------------------//

CMat4x4::CMat4x4(const CQuaternion& qRot)
  {
  qRot.ToRotation(this);
  }

//----------------------------------------------- special functions --------------------------------------------------//

void CMat4x4::Transpose()                                                       // transpose
  {
  float fTmp;
  fTmp = Col[0][1]; Col[0][1] = Col[1][0]; Col[1][0] = fTmp;
  fTmp = Col[0][2]; Col[0][2] = Col[2][0]; Col[2][0] = fTmp;
  fTmp = Col[0][3]; Col[0][3] = Col[3][0]; Col[3][0] = fTmp;
  fTmp = Col[1][2]; Col[1][2] = Col[2][1]; Col[2][1] = fTmp;
  fTmp = Col[1][3]; Col[1][3] = Col[3][1]; Col[3][1] = fTmp;
  fTmp = Col[2][3]; Col[2][3] = Col[3][2]; Col[3][2] = fTmp;
  }

//

CMat4x4 CMat4x4::Inverted() const                                               // return inverted copy
  {                                                                             // Gauss-Jordan elimination with partial pivoting
  CMat4x4 m4A(*this);                                                           // as A evolves from the original  matrix to identity
  CMat4x4 m4B(Identity);                                                        // ...B evolves from identity to inverse(A)
  ui      I,J,I1;

  // loop over rows of A from top to bottom, eliminating left & right diagonals

  for(J = 0; J < 4; J++)
    {                                                                           // find largest pivot in row j among columns j..3
    I1 = J;                                                                     // column with largest pivot candidate
    for(I = J+1; I < 4; I++) if(fabs(m4A[I][J]) > fabs(m4A[I1][J])) I1 = I;

    // swap columns i1 & j in A & B to put pivot on diagonal

    if(I1 != J)
      {
      Swap(m4A[I1],m4A[J]);
      Swap(m4B[I1],m4B[J]);
      }

    // scale column j to have a unit diagonal

    float fK = m4A[J][J];
    if(fK != 1.f)
      {
      MY_ASSERT(fK != 0.f,"division by zero !")
      float fKinv  = 1.0f/fK;

      m4A[J] *= fKinv;
      m4B[J] *= fKinv;
      }

    // eliminate off-diagonal elements in row j of A, doing identical ops to B

    for(I = 0; I < 4; I++) if(I != J)
      {
      m4B[I] -= m4A[I][J]*m4B[J];
      m4A[I] -= m4A[I][J]*m4A[J];
      }
    }

  return m4B;
  }

//

CMat4x4 CMat4x4::AffineInverted() const                                         // should be faster
  {                                                                             // if the matrix only contains affine trfs
  CMat4x4 m4B;

  // invert 3x3 top-left part

  m4B[0][0] = Col[1][1]*Col[2][2] - Col[2][1]*Col[1][2];
  m4B[0][1] = Col[0][2]*Col[2][1] - Col[0][1]*Col[2][2];
  m4B[0][2] = Col[0][1]*Col[1][2] - Col[1][1]*Col[0][2];

  m4B[1][0] = Col[2][0]*Col[1][2] - Col[1][0]*Col[2][2];
  m4B[1][1] = Col[0][0]*Col[2][2] - Col[0][2]*Col[2][0];
  m4B[1][2] = Col[1][0]*Col[0][2] - Col[0][0]*Col[1][2];

  m4B[2][0] = Col[1][0]*Col[2][1] - Col[1][1]*Col[2][0];
  m4B[2][1] = Col[0][1]*Col[2][0] - Col[0][0]*Col[2][1];
  m4B[2][2] = Col[0][0]*Col[1][1] - Col[0][1]*Col[1][0];

  float fDet = Col[0][0]*m4B[0][0] + Col[1][0]*m4B[0][1] + Col[2][0]*m4B[0][2];
  if(fDet < 1e-6) return Identity;

  float fInvDet = 1.f/fDet;
  m4B[0][0] *= fInvDet; m4B[0][1] *= fInvDet; m4B[0][2] *= fInvDet;
  m4B[1][0] *= fInvDet; m4B[1][1] *= fInvDet; m4B[1][2] *= fInvDet;
  m4B[2][0] *= fInvDet; m4B[2][1] *= fInvDet; m4B[2][2] *= fInvDet;

  // translation: "rot then trans" becomes "(-trans) then inv rot"

  CVect3D v3Trans(-Col[POS],W);

  m4B[3][0] = m4B[0][0]*v3Trans.x + m4B[1][0]*v3Trans.y + m4B[2][0]*v3Trans.z;
  m4B[3][1] = m4B[0][1]*v3Trans.x + m4B[1][1]*v3Trans.y + m4B[2][1]*v3Trans.z;
  m4B[3][2] = m4B[0][2]*v3Trans.x + m4B[1][2]*v3Trans.y + m4B[2][2]*v3Trans.z;

  //

  m4B[0][3] = m4B[1][3] = m4B[2][3] = 0.f;
  m4B[3][3] = 1.f;
  return m4B;
  }

//

CMat4x4 CMat4x4::RotTransInverted() const                                       // even faster
  {                                                                             // if the matrix contains a rot + 1 trans
  CMat4x4 m4B;

  // transpose rotation

  m4B[0][0] = Col[0][0]; m4B[0][1] = Col[1][0]; m4B[0][2] = Col[2][0];
  m4B[1][0] = Col[0][1]; m4B[1][1] = Col[1][1]; m4B[1][2] = Col[2][1];
  m4B[2][0] = Col[0][2]; m4B[2][1] = Col[1][2]; m4B[2][2] = Col[2][2];

  // translation: "rot then trans" becomes "(-trans) then inv rot"

  CVect3D v3Trans(-Col[POS],W);

  m4B[3][0] = m4B[0][0]*v3Trans.x + m4B[1][0]*v3Trans.y + m4B[2][0]*v3Trans.z;
  m4B[3][1] = m4B[0][1]*v3Trans.x + m4B[1][1]*v3Trans.y + m4B[2][1]*v3Trans.z;
  m4B[3][2] = m4B[0][2]*v3Trans.x + m4B[1][2]*v3Trans.y + m4B[2][2]*v3Trans.z;

  //

  m4B[0][3] = m4B[1][3] = m4B[2][3] = 0.f;
  m4B[3][3] = 1.f;  
  return m4B;
  }

//

void CMat4x4::Rotation(const CVect3D& v3Axis,const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);
  float fT   = 1.f-fCos;
  float fSX  = v3Axis.x*fSin, fSY = v3Axis.y*fSin, fSZ = v3Axis.z*fSin;
  float fTX  = v3Axis.x*fT,   fTY = v3Axis.y*fT,   fTZ = v3Axis.z*fT;

  CVect3D v3Ax = v3Axis.Normalized();
  Col[0].Set(fTX*v3Ax.x+fCos,fTX*v3Ax.y+fSZ, fTX*v3Ax.z-fSY, 0.f);
  Col[1].Set(fTX*v3Ax.y-fSZ, fTY*v3Ax.y+fCos,fTY*v3Ax.z+fSX, 0.f);
  Col[2].Set(fTX*v3Ax.z+fSY, fTY*v3Ax.z-fSX, fTZ*v3Ax.z+fCos,0.f);
  Col[3].Set(    0.f,            0.f,             0.f,       1.f);
  }

//

void CMat4x4::RotationX(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set(1.f, 0.f,  0.f,0.f);
  Col[1].Set(0.f,fCos,-fSin,0.f);
  Col[2].Set(0.f,fSin, fCos,0.f);
  Col[3].Set(0.f, 0.f,  0.f,1.f);
  }

//

void CMat4x4::RotationY(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set( fCos,0.f,fSin,0.f);
  Col[1].Set(  0.f,1.f, 0.f,0.f);
  Col[2].Set(-fSin,0.f,fCos,0.f);
  Col[3].Set(0.f, 0.f,  0.f,1.f);
  }

//

void CMat4x4::RotationZ(const float fAngle)
  {
  float fCos = (float)cos(fAngle);
  float fSin = (float)sin(fAngle);

  Col[0].Set(fCos,-fSin,0.f,0.f);
  Col[1].Set(fSin, fCos,0.f,0.f);
  Col[2].Set( 0.f, 0.f, 1.f,0.f);
  Col[3].Set( 0.f, 0.f, 0.f,1.f);
  }

//

void CMat4x4::RotationXYZ(const CVect3D& v3Angles)
  {
  RotationZ(v3Angles.z);
  CMat4x4 m4Y,m4X;
  m4Y.RotationY(v3Angles.y);
  m4X.RotationX(v3Angles.x);
  *this *= m4Y*m4X;
  }

//

void CMat4x4::MulHomo(const CMat4x4& m4M)                                       // homogeneous multiplication
  {
  CMat4x4 m4M0(*this);
  (*this)[0][0] = m4M0[0][0]*m4M[0][0] + m4M0[1][0]*m4M[0][1] + m4M0[2][0]*m4M[0][2];
  (*this)[0][1] = m4M0[0][1]*m4M[0][0] + m4M0[1][1]*m4M[0][1] + m4M0[2][1]*m4M[0][2];
  (*this)[0][2] = m4M0[0][2]*m4M[0][0] + m4M0[1][2]*m4M[0][1] + m4M0[2][2]*m4M[0][2];
  (*this)[0][3] = m4M0[0][3]*m4M[0][0] + m4M0[1][3]*m4M[0][1] + m4M0[2][3]*m4M[0][2];

  (*this)[1][0] = m4M0[0][0]*m4M[1][0] + m4M0[1][0]*m4M[1][1] + m4M0[2][0]*m4M[1][2];
  (*this)[1][1] = m4M0[0][1]*m4M[1][0] + m4M0[1][1]*m4M[1][1] + m4M0[2][1]*m4M[1][2];
  (*this)[1][2] = m4M0[0][2]*m4M[1][0] + m4M0[1][2]*m4M[1][1] + m4M0[2][2]*m4M[1][2];
  (*this)[1][3] = m4M0[0][3]*m4M[1][0] + m4M0[1][3]*m4M[1][1] + m4M0[2][3]*m4M[1][2];

  (*this)[2][0] = m4M0[0][0]*m4M[2][0] + m4M0[1][0]*m4M[2][1] + m4M0[2][0]*m4M[2][2];
  (*this)[2][1] = m4M0[0][1]*m4M[2][0] + m4M0[1][1]*m4M[2][1] + m4M0[2][1]*m4M[2][2];
  (*this)[2][2] = m4M0[0][2]*m4M[2][0] + m4M0[1][2]*m4M[2][1] + m4M0[2][2]*m4M[2][2];
  (*this)[2][3] = m4M0[0][3]*m4M[2][0] + m4M0[1][3]*m4M[2][1] + m4M0[2][3]*m4M[2][2];

  (*this)[3][0] = m4M0[0][0]*m4M[3][0] + m4M0[1][0]*m4M[3][1] + m4M0[2][0]*m4M[3][2] + m4M0[3][0];
  (*this)[3][1] = m4M0[0][1]*m4M[3][0] + m4M0[1][1]*m4M[3][1] + m4M0[2][1]*m4M[3][2] + m4M0[3][1];
  (*this)[3][2] = m4M0[0][2]*m4M[3][0] + m4M0[1][2]*m4M[3][1] + m4M0[2][2]*m4M[3][2] + m4M0[3][2];
  (*this)[3][3] = m4M0[0][3]*m4M[3][0] + m4M0[1][3]*m4M[3][1] + m4M0[2][3]*m4M[3][2] + m4M0[3][3];
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
