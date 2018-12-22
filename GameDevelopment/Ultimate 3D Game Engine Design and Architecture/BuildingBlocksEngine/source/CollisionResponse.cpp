/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<CollisionResponse.h>
#include<BoundingSphere.h>
#include<Defines.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


// Point Mass Collision as Spheres.


bool CollisionResponse::ResponseToCollision(PointMass *p1, float radius1,
                                            PointMass *p2, float radius2,
                                            float e)
{
   if(p1 == 0 || p2 == 0)
      return false;

   Vector3D pos1 = p1->GetPosition();
   Vector3D pos2 = p2->GetPosition();
   Vector3D vel1 = p1->GetVelocity();
   Vector3D vel2 = p2->GetVelocity();

   float mass1 = p1->GetMass();
   float mass2 = p2->GetMass();

   BoundingSphere s1, s2;

   s1.m_center = pos1;
   s1.m_radius = (double)radius1;

   s2.m_center = pos2;
   s2.m_radius = (double)radius2;

   Vector3D massDir;
   float massDirLength = 0;
   float massTotal = 0;
   float radiusTotal = 0;

   if(s2.CollisionCheck(s1) == true)
      {
         massDir = pos2 - pos1;
         massDirLength = massDir.Magnitude();
         massTotal = mass1 + mass2;
         radiusTotal = radius1 + radius2;

         massDir.Normalize();

         if(massDirLength < radiusTotal)
            {
               p2->SetPosition(pos1 + massDir * (radiusTotal + e));
            }

         float v1 = massDir.Dot3(vel1);
         float v2 = massDir.Dot3(vel2);
         float vp1 = ((mass1 - (e * mass2)) * v1 + (1 + e) * mass2 * v2) / massTotal;
         float vp2 = ((mass2 - (e * mass1)) * v2 + (1 + e) * mass1 * v1) / massTotal;

         p1->SetVelocity(vel2 + (massDir * (vp2 - v2)));
         p2->SetVelocity(vel1 + (massDir * (vp1 - v1)));

         return true;
      }

   return false;
}


bool CollisionResponse::ResponseToCollision(PointMass *p1, float radius1,
                                            Frustum &frustum, float e)
{
   bool result = false;
   Plane plane;

   for(int i = 0; i < frustum.GetTotalPlanes(); i++)
      {
         frustum.GetPlane(i, &plane);

         if(ResponseToCollision(p1, radius1, plane, e) == true)
            result = true;
      }

   return result;
}


bool CollisionResponse::ResponseToCollision(PointMass *p1, float radius1,
                                            Plane &plane, float e)
{
   if(p1 == 0)
      return false;

   if(plane.Intersect(p1->GetPosition(), radius1) == false)
      return false;

   p1->SetVelocity(plane.Reflect(p1->GetVelocity(), e));

   return true;
}


END_ENGINE_NAMESPACE