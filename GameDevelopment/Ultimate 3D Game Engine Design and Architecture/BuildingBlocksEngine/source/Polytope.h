/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_POLYTOPE_H_
#define _BB_POLYTOPE_H_


#include<NameSpace.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class Polytope
{
   public:
      Polytope()
      {
         m_totalFaces = 0;
         m_totalEdges = 0;
      }

      virtual ~Polytope()
      {

      }


      virtual void ProjectionInterval(Vector3D &axis, float &center,
                                      float &ext, float &min,
                                      float &max) = 0;

      virtual unsigned int GetSupport(Vector3D &axis,
                                      Vector3D *contacts) = 0;


      virtual Vector3D GetFaceNormal(int index) = 0;
      virtual Vector3D GetEdgeDirection(int index) = 0;
      virtual Vector3D GetCenterPos() = 0;

      int GetTotalFaces()
      {
         return m_totalFaces;
      }

      int GetTotalEdges()
      {
         return m_totalEdges;
      }


   protected:
      int m_totalFaces;
      int m_totalEdges;
};

END_ENGINE_NAMESPACE

#endif