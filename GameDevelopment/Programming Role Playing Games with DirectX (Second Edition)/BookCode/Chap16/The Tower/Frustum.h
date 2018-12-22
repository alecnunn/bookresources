#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

class cFrustum
{
  private:
    D3DXPLANE m_Planes[6];

  public:
    BOOL Construct(cGraphics *Graphics, float ZDistance = 0.0f);
    BOOL CheckPoint(float XPos, float YPos, float ZPos);
    BOOL CheckCube(float XCenter, float YCenter, float ZCenter, float Size, BOOL *CompletelyContained = NULL);
    BOOL CheckRectangle(float XCenter, float YCenter, float ZCenter, float XSize, float YSize, float ZSize, BOOL *CompletelyContained = NULL);
    BOOL CheckSphere(float XCenter, float YCenter, float ZCenter, float Radius);
};

#endif
