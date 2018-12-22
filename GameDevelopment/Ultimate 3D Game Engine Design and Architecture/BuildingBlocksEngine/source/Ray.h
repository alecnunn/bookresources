/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RAY_H_
#define _BB_RAY_H_

#include<NameSpace.h>
#include<Vector3D.h>
#include<Plane.h>


DECLARE_ENGINE_NAMESPACE


class Ray
{
   public:
      Ray();
      Ray(Vector3D &origin, Vector3D &dir);

      bool Intersect(Vector3D &pos, float radius, float *dist);

      bool Intersect(Vector3D &p1, Vector3D &p2,
                     Vector3D &p3, bool cull, float *dist);

      bool Intersect(Plane &pl, bool cull,
                     Vector3D *intersectPoint, float *dist);

      bool Intersect(Vector3D &bbMin, Vector3D &bbMax,
                     Vector3D *intersectPoint);

      Vector3D m_origin;
      Vector3D m_direction;
};


END_ENGINE_NAMESPACE


#endif