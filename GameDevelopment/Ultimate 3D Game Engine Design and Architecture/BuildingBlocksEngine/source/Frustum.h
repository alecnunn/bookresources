/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_FRUSTUM_H_
#define _BB_FRUSTUM_H_

#include<NameSpace.h>
#include<Plane.h>
#include<Vector3D.h>
#include<OBB.h>
#include<vector>


DECLARE_ENGINE_NAMESPACE


class Frustum
{
   public:
      Frustum();

      void CalculateFrustum(float angle, float ratio, float near,
                            float far, Vector3D &camPos,
                            Vector3D &lookAt, Vector3D &up);

      void AddPlane(Plane &pl);
      bool GetPlane(int index, Plane *out);
      int GetTotalPlanes() { return (int)m_frustum.size(); }

      bool isPointVisible(float x, float y, float z);
      bool isSphereVisible(float x, float y, float z, float radius);
      bool isCubeVisible(float x, float y, float z, float size);
      bool isBoxVisible(Vector3D min, Vector3D max);
      bool isOBBVisible(OBB &obb);

   private:
      std::vector<Plane> m_frustum;
};


END_ENGINE_NAMESPACE


#endif