/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_BOUNDING_BOX_H_
#define _BB_BOUNDING_BOX_H_


#include<NameSpace.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class BoundingBox
{
   public:
      BoundingBox();
      BoundingBox(const BoundingBox &aabb);

      void Calculate(Vector3D *v, int numPoints);
      void Expand(float amt);

      void Translate(Vector3D v);

      bool CollisionCheck(const BoundingBox &aabb);
      bool CollisionCheck(const Vector3D &v);

      void operator=(BoundingBox b)
      {
         m_min = b.m_min;
         m_max = b.m_max;
         m_center = b.m_center;
      }

      Vector3D m_min, m_max, m_center;
};


END_ENGINE_NAMESPACE


#endif