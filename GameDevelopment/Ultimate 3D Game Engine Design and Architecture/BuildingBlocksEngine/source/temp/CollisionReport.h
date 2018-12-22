/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_COLLISION_REPORT_H_
#define _BB_COLLISION_REPORT_H_

#include<NameSpace.h>
#include<Vector3D.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE


class CollisionReport
{
   public:
      CollisionReport()
      {
         ResetReport();
      }

      virtual ~CollisionReport()
      {
      }

      void ResetReport()
      {
         m_numContactA = m_numContactB =
         m_numContactPoints = 0;

         m_minimalTransDist = Vector3D(0, 0, 0);
         m_collisionNormal = Vector3D(0, 0, 0);
         m_average = Vector3D(0, 0, 0);
         m_tempTotalVec = Vector3D(0, 0, 0);

         m_time = 0;

         m_isCollision = m_isOverlap = false;
      }


      void AddContactPoint(Vector3D &point)
      {
         if(m_numContactPoints < BB_MAX_CONTACT_POINTS)
            {
               m_contactPoints[m_numContactPoints++] = point;
               m_tempTotalVec += point;

               if(m_numContactPoints > 0)
                  m_average = m_tempTotalVec / (scalar)m_numContactPoints;
            }
      }

      Vector3D GetContactPoint(int index)
      {
         if(index >= 0 && index < BB_MAX_CONTACT_POINTS)
            return m_contactPoints[index];

         return Vector3D(0, 0, 0);
      }

      int GetNumCollisionPoints()
      {
         return m_numContactPoints;
      }

      void AddContactPointA(Vector3D &point)
      {
         if(m_numContactA < BB_MAX_CONTACT_POINTS)
            {
               m_contactSetA[m_numContactA++] = point;
            }
      }

      Vector3D GetContactPointA(int index)
      {
         if(index >= 0 && index < BB_MAX_CONTACT_POINTS)
            return m_contactSetA[index];

         return Vector3D(0, 0, 0);
      }

      int GetNumContactA()
      {
         return m_numContactA;
      }

      void AddContactPointB(Vector3D &point)
      {
         if(m_numContactB < BB_MAX_CONTACT_POINTS)
            {
               m_contactSetB[m_numContactB++] = point;
            }
      }
      
      Vector3D GetContactPointB(int index)
      {
         if(index >= 0 && index < BB_MAX_CONTACT_POINTS)
            return m_contactSetB[index];

         return Vector3D(0, 0, 0);
      }

      int GetNumContactB()
      {
         return m_numContactB;
      }

      Vector3D GetMTD()
      {
         return m_minimalTransDist;
      }

      void SetMTD(Vector3D mtd)
      {
         m_minimalTransDist = mtd;
      }

      Vector3D GetCollisionNormal()
      {
         return m_collisionNormal;
      }

      void SetCollisionNormal(Vector3D n)
      {
         m_collisionNormal = n;
      }

      Vector3D GetAverageVector()
      {
         return m_average;
      }

      scalar GetTime()
      {
         return m_time;
      }

      void SetTime(scalar t)
      {
         m_time = t;
      }

      bool GetIsCollision()
      {
         return m_isCollision;
      }

      void SetIsCollision(bool val)
      {
         m_isCollision = val;
      }

      bool GetIsOverlap()
      {
         return m_isOverlap;
      }

      void SetIsOverlap(bool val)
      {
         m_isOverlap = val;
      }


   private:
      Vector3D m_contactPoints[BB_MAX_CONTACT_POINTS];
      Vector3D m_contactSetA[BB_MAX_CONTACT_POINTS];
      Vector3D m_contactSetB[BB_MAX_CONTACT_POINTS];

      int m_numContactA, m_numContactB,
          m_numContactPoints;

      Vector3D m_minimalTransDist;
      Vector3D m_collisionNormal;
      Vector3D m_average, m_tempTotalVec;

      scalar m_time;

      bool m_isCollision;
      bool m_isOverlap;
};


END_ENGINE_NAMESPACE

#endif