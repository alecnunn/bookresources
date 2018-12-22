#include "Core_Global.h"
#include "Frustum.h"

BOOL cFrustum::Construct(cGraphics *Graphics, float ZDistance)
{
  D3DXMATRIX Matrix, matView, matProj;
  float      ZMin, Q;

  // Error checking
  if(Graphics == NULL || Graphics->GetDeviceCOM() == NULL)
    return FALSE;

  // Calculate FOV data
  Graphics->GetDeviceCOM()->GetTransform(D3DTS_PROJECTION, &matProj);
  if(ZDistance != 0.0f) {
    // Calculate new projection matrix based on distance provided
    ZMin = -matProj._43 / matProj._33;
    Q = ZDistance / (ZDistance - ZMin);
    matProj._33 = Q;
    matProj._43 = -Q * ZMin;
  }
  Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
  D3DXMatrixMultiply(&Matrix, &matView, &matProj);

  // Calculate the planes
  m_Planes[0].a = Matrix._14 + Matrix._13; // Near plane
  m_Planes[0].b = Matrix._24 + Matrix._23;
  m_Planes[0].c = Matrix._34 + Matrix._33;
  m_Planes[0].d = Matrix._44 + Matrix._43;
  D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

  m_Planes[1].a = Matrix._14 - Matrix._13; // Far plane
  m_Planes[1].b = Matrix._24 - Matrix._23;
  m_Planes[1].c = Matrix._34 - Matrix._33;
  m_Planes[1].d = Matrix._44 - Matrix._43;
  D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

  m_Planes[2].a = Matrix._14 + Matrix._11; // Left plane
  m_Planes[2].b = Matrix._24 + Matrix._21;
  m_Planes[2].c = Matrix._34 + Matrix._31;
  m_Planes[2].d = Matrix._44 + Matrix._41;
  D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);

  m_Planes[3].a = Matrix._14 - Matrix._11; // Right plane
  m_Planes[3].b = Matrix._24 - Matrix._21;
  m_Planes[3].c = Matrix._34 - Matrix._31;
  m_Planes[3].d = Matrix._44 - Matrix._41;
  D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

  m_Planes[4].a = Matrix._14 - Matrix._12; // Top plane
  m_Planes[4].b = Matrix._24 - Matrix._22;
  m_Planes[4].c = Matrix._34 - Matrix._32;
  m_Planes[4].d = Matrix._44 - Matrix._42;
  D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

  m_Planes[5].a = Matrix._14 + Matrix._12; // Bottom plane
  m_Planes[5].b = Matrix._24 + Matrix._22;
  m_Planes[5].c = Matrix._34 + Matrix._32;
  m_Planes[5].d = Matrix._44 + Matrix._42;
  D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);

  return TRUE;
}

BOOL cFrustum::CheckPoint(float XPos, float YPos, float ZPos)
{
  short i;

  // Make sure point is in frustum
  for(i=0;i<6;i++) {
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XPos, YPos, ZPos)) < 0.0f)
      return FALSE;
  }

  return TRUE;
}

BOOL cFrustum::CheckCube(float XCenter, float YCenter, float ZCenter, float Size)
{
  short i;

  // Make sure at least one point is completely in frustum
  for(i=0;i<6;i++) {
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter-Size, ZCenter-Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter-Size, ZCenter-Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter+Size, ZCenter-Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter+Size, ZCenter-Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter-Size, ZCenter+Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter-Size, ZCenter+Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter+Size, ZCenter+Size)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter+Size, ZCenter+Size)) >= 0.0f)
      continue;

    return FALSE;
  }

  return TRUE;
}

BOOL cFrustum::CheckRectangle(float XCenter, float YCenter, float ZCenter, float XSize, float YSize, float ZSize)
{
  short i;

  // Make sure at least one point is in frustum
  for(i=0;i<6;i++) {
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter-YSize, ZCenter-ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter-YSize, ZCenter-ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter+YSize, ZCenter-ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter+YSize, ZCenter-ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter-YSize, ZCenter+ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter-YSize, ZCenter+ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter+YSize, ZCenter+ZSize)) >= 0.0f)
      continue;
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter+YSize, ZCenter+ZSize)) >= 0.0f)
      continue;

    return FALSE;
  }

  return TRUE;
}

BOOL cFrustum::CheckSphere(float XCenter, float YCenter, float ZCenter, float Radius)
{
  short i;

  // Make sure radius is in frustum
  for(i=0;i<6;i++) {
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter, YCenter, ZCenter)) < -Radius)
      return FALSE;
  }
  return TRUE;
}
