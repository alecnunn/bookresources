/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PLANE_H_
#define _BB_PLANE_H_

#include<NameSpace.h>
#include<Defines.h>
#include<Vector3D.h>
#include<OBB.h>


DECLARE_ENGINE_NAMESPACE


class Plane
{
   public:
      Plane();
      Plane(scalar A, scalar B, scalar C, scalar D);

      void CreatePlaneFromTri(Vector3D &t1,
                              Vector3D &t2,
                              Vector3D &t3);

      bool Intersect(const Plane &pl, Vector3D *intersectPoint);
      bool Intersect(const Vector3D &bbMin, const Vector3D &bbMax);
      bool Intersect(const Vector3D &position, float radius);
      bool Intersect(const OBB &obb);

      BB_PLANE_STATUS ClassifyPoint(const Vector3D &v);
      BB_PLANE_STATUS ClassifyPoint(const Vector3D &v, float *dist);
      BB_PLANE_STATUS ClassifyPoint(float x, float y, float z);
      BB_PLANE_STATUS ClassifyPoint(float x, float y, float z, float *dist);

      scalar GetDistance(const Vector3D &v);
      scalar GetDistance(float x, float y, float z);

      Vector3D Reflect(Vector3D vec, float e);

      bool ClipTriangle(Vector3D *inVerts, int totalInVerts,
                        Vector3D *outFrontVerts, int *totalOutFrontVerts,
                        Vector3D *outBackVerts, int *totalOutBackVerts);

      Vector3D GetNormal()
      {
         return Vector3D(a, b, c);
      }

      void SetNormal(Vector3D n)
      {
         a = n.x;
         b = n.y;
         c = n.z;
      }

      void SetPointOnPlane(Vector3D p)
      {
         m_pointOnPlane = p;
      }

      bool operator==(Plane p)
      {
         return (a == p.a && b == p.b &&
                 c == p.c && d == p.d);
      }

      void operator=(Plane p)
      {
         a = p.a;
         b = p.b;
         c = p.c;
         d = p.d;
      }


      scalar a, b, c, d;
      Vector3D m_pointOnPlane;
};


END_ENGINE_NAMESPACE


#endif