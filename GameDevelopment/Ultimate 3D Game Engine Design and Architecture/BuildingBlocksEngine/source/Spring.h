/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SPRING_H_
#define _BB_SPRING_H_


#include<NameSpace.h>
#include<Defines.h>
#include<PointMass.h>


DECLARE_ENGINE_NAMESPACE


class SpringMass : public PointMass
{
   public:
      SpringMass()
      {
         m_fixed = false;
      }

      Vector3D GetNormal()
      {
         return m_normal;
      }

      Vector3D *GetNormalPtr()
      {
         return &m_normal;
      }

      void SetNormal(Vector3D n)
      {
         m_normal = n;
      }

      void NormalizeNormal()
      {
         m_normal.Normalize();
      }

      bool isFixed()
      {
         return m_fixed;
      }

      void SetIsFixed(bool val)
      {
         m_fixed = val;
      }


   private:
      Vector3D m_normal;
      bool m_fixed;
};


class Spring
{
   public:
      Spring()
      {
         m_ball1 = 0;
         m_ball2 = 0;
         m_tension = 0;
         m_constant = 0;
         m_length = 0;
      }

      ~Spring()
      {

      }

      void SetSpring(float t, float c, float l, SpringMass *m1, SpringMass *m2)
      {
         m_tension = t;
         m_constant = c;
         m_length = l;

         m_ball1 = m1;
         m_ball2 = m2;
      }

      void Update()
      {
         if(m_ball1 == 0 || m_ball2 == 0)
            return;

         Vector3D springVec = m_ball2->GetPosition() -
                              m_ball1->GetPosition();

         m_tension = m_constant * (springVec.Magnitude() -
                     m_length) / m_length;

			springVec.Normalize();

			m_ball1->AddForce(springVec * m_tension);
			m_ball2->AddForce((springVec * m_tension) * -1);
      }


   private:
	   SpringMass *m_ball1;
	   SpringMass *m_ball2;

	   float m_tension;
	   float m_constant;
	   float m_length;
};


END_ENGINE_NAMESPACE

#endif