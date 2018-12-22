#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

class cFrustum
{
  private:
    D3DXPLANE m_Planes[6]; // The frustum planes

  public:
    // Construct the six planes from current view and
    // projection. Can override the default depth value.
    BOOL Construct(cGraphics *Graphics, float ZDistance = 0.0f);

    // The following functions check a single point, cube,
    // rectangle, and sphere if contained in the frustum.
    // A return value of TRUE means visible, FALSE not visible.
    // When checking cubes or rectangles, you can
    // supply a BOOL variable that determines if all the
    // points are in the frustum.
    BOOL CheckPoint(float XPos, float YPos, float ZPos);

    BOOL CheckCube(float XCenter, float YCenter,               \
                   float ZCenter, float Size,                  \
                   BOOL *CompletelyContained = NULL);
    
    BOOL CheckRectangle(float XCenter, float YCenter,          \
                        float ZCenter, float XSize,            \
                        float YSize, float ZSize,              \
                        BOOL *CompletelyContained = NULL);
    
    BOOL CheckSphere(float XCenter, float YCenter,             \
                     float ZCenter, float Radius);
};

#endif
