/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_ORIENTED_BOUNDING_BOX_H_
#define _BB_ORIENTED_BOUNDING_BOX_H_


#include<NameSpace.h>
#include<Vector3D.h>
#include<Polytope.h>
#include<BoundingBox.h>
#include<Matrix.h>


DECLARE_ENGINE_NAMESPACE


class OBB : public Polytope
{
   public:
      OBB();
      OBB(OBB &obb);
      OBB(BoundingBox &aabb);

      void Calculate(Vector3D *vertices, int numVerts);
      void Calculate(BoundingBox &aabb);

      void Transform(OBB &obb, Matrix4x4 &mat);
      void ObjectTransform(OBB &obb, Matrix4x4 &mat);

      void ProjectionInterval(Vector3D &axis, float &center,
                              float &ext, float &min,
                              float &max);

      unsigned int GetSupport(Vector3D &axis,
                              Vector3D *contacts);


      Vector3D GetFaceNormal(int index)
      {
         if(index == 0)
            return m_axis1;
         else if(index == 1)
            return m_axis2;

         return m_axis3;
      }

      Vector3D GetEdgeDirection(int index)
      {
         return GetFaceNormal(index);
      }

      // Accessor functions.
      Vector3D GetCenterPos() { return m_center; };
      void SetCenter(Vector3D &center) { m_center = center; }

      float GetHalfAxis1() { return m_halfAxis1; }
      float GetHalfAxis2() { return m_halfAxis2; }
      float GetHalfAxis3() { return m_halfAxis3; }

      float GetHalfAxis(int index)
      {
         if(index == 0)
            return m_halfAxis1;
         else if(index == 1)
            return m_halfAxis2;

         return m_halfAxis3;
      }

      void SetHalfAxis1(float val) { m_halfAxis1 = val; }
      void SetHalfAxis2(float val) { m_halfAxis2 = val; }
      void SetHalfAxis3(float val) { m_halfAxis3 = val; }

      Vector3D GetAxis1() { return m_axis1; }
      Vector3D GetAxis2() { return m_axis2; }
      Vector3D GetAxis3() { return m_axis3; }

      Vector3D GetAxis(int index)
      {
         if(index == 0)
            return m_axis1;
         else if(index == 1)
            return m_axis2;

         return m_axis3;
      }

      void SetAxis1(Vector3D &axis) { m_axis1 = axis; }
      void SetAxis2(Vector3D &axis) { m_axis2 = axis; }
      void SetAxis3(Vector3D &axis) { m_axis3 = axis; }

      // Members.
      Vector3D m_center;
      float m_halfAxis1, m_halfAxis2, m_halfAxis3;
      Vector3D m_axis1, m_axis2, m_axis3;
};


END_ENGINE_NAMESPACE


#endif