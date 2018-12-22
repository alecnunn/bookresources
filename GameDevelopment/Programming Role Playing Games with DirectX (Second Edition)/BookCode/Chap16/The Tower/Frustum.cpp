#include "Core_Global.h"
#include "Frustum.h"

BOOL cFrustum::Construct(cGraphics *Graphics, float ZDistance)
{
  D3DXMATRIX Matrix, matView, matProj;
  float      ZMin, Q;

  // Error checking
  if(Graphics == NULL)
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
  m_Planes[0].a = Matrix._14 + Matrix._13; // Near
  m_Planes[0].b = Matrix._24 + Matrix._23;
  m_Planes[0].c = Matrix._34 + Matrix._33;
  m_Planes[0].d = Matrix._44 + Matrix._43;
  D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

  m_Planes[1].a = Matrix._14 - Matrix._13; // Far
  m_Planes[1].b = Matrix._24 - Matrix._23;
  m_Planes[1].c = Matrix._34 - Matrix._33;
  m_Planes[1].d = Matrix._44 - Matrix._43;
  D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

  m_Planes[2].a = Matrix._14 + Matrix._11; // Left
  m_Planes[2].b = Matrix._24 + Matrix._21;
  m_Planes[2].c = Matrix._34 + Matrix._31;
  m_Planes[2].d = Matrix._44 + Matrix._41;
  D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);

  m_Planes[3].a = Matrix._14 - Matrix._11; // Right
  m_Planes[3].b = Matrix._24 - Matrix._21;
  m_Planes[3].c = Matrix._34 - Matrix._31;
  m_Planes[3].d = Matrix._44 - Matrix._41;
  D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

  m_Planes[4].a = Matrix._14 - Matrix._12; // Top
  m_Planes[4].b = Matrix._24 - Matrix._22;
  m_Planes[4].c = Matrix._34 - Matrix._32;
  m_Planes[4].d = Matrix._44 - Matrix._42;
  D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

  m_Planes[5].a = Matrix._14 + Matrix._12; // Bottom
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

BOOL cFrustum::CheckCube(float XCenter, float YCenter, float ZCenter, float Size, BOOL *CompletelyContained)
{
  short i;
  DWORD TotalIn = 0;

  // Count the number of points inside the frustum
  for(i=0;i<6;i++) {

    DWORD Count = 8;
    BOOL  PointIn = TRUE;
    
    // Test all eight points against plane
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter-Size, ZCenter-Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter-Size, ZCenter-Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter+Size, ZCenter-Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter+Size, ZCenter-Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter-Size, ZCenter+Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter-Size, ZCenter+Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-Size, YCenter+Size, ZCenter+Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+Size, YCenter+Size, ZCenter+Size)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    // If none contained, return FALSE
    if(Count == 0)
      return FALSE;

    // Update counter if they were all in front of plane
    TotalIn += (PointIn == TRUE) ? 1:0;
  }

  // Store BOOL flag if completely contained
  if(CompletelyContained != NULL)
    *CompletelyContained = (TotalIn == 6) ? TRUE:FALSE;

  return TRUE;
}

BOOL cFrustum::CheckRectangle(float XCenter, float YCenter, float ZCenter, float XSize, float YSize, float ZSize, BOOL *CompletelyContained)
{
  short i;

  DWORD TotalIn = 0;

  // Count the number of points inside the frustum
  for(i=0;i<6;i++) {

    DWORD Count = 8;
    BOOL  PointIn = TRUE;
    
    // Test all eight points against plane
    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter-YSize, ZCenter-ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter-YSize, ZCenter-ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter+YSize, ZCenter-ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter+YSize, ZCenter-ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter-YSize, ZCenter+ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter-YSize, ZCenter+ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter-XSize, YCenter+YSize, ZCenter+ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(XCenter+XSize, YCenter+YSize, ZCenter+ZSize)) < 0.0f) {
      PointIn = FALSE;
      Count--;
    }

    // If none contained, return FALSE
    if(Count == 0)
      return FALSE;

    // Update counter if they were all in front of plane
    TotalIn += (PointIn == TRUE) ? 1:0;
  }

  // Store BOOL flag if completely contained
  if(CompletelyContained != NULL)
    *CompletelyContained = (TotalIn == 6) ? TRUE:FALSE;

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
