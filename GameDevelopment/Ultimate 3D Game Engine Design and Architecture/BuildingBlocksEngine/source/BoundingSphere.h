/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_BOUNDING_SPHERE_H_
#define _BB_BOUNDING_SPHERE_H_

#include<NameSpace.h>
#include<Vector3D.h>
#include<BoundingBox.h>


DECLARE_ENGINE_NAMESPACE


class BoundingSphere
{
   public:
      BoundingSphere();
      BoundingSphere(BoundingSphere &bs);

      void Calculate(Vector3D *v, int numPoints);

      void Translate(Vector3D v);

      bool CollisionCheck(BoundingSphere &bs);
      bool CollisionCheck(Vector3D &v);
      bool CollisionCheck(Vector3D &p1, Vector3D &p2,
                          Vector3D &p3);
      bool CollisionCheck(BoundingBox &aabb);

      Vector3D m_center;
      double m_radius;
};


END_ENGINE_NAMESPACE


#endif