/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Ray.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


Ray::Ray()
{

}


Ray::Ray(Vector3D &origin, Vector3D &dir)
{
   m_origin = origin;
   m_direction = dir;
}


bool Ray::Intersect(Vector3D &pos, float radius, float *dist)
{
   Vector3D RayToSphereDir;
   float RayToSphereLength = 0.0f;
   float IntersectPoint = 0.0f;
   float SquaredPoint = 0.0f;

   RayToSphereDir = pos - m_origin;
   RayToSphereLength = RayToSphereDir.Dot3(RayToSphereDir);

   IntersectPoint = RayToSphereDir.Dot3(m_direction);

   if(IntersectPoint < 0 )
      return false;

   SquaredPoint = (radius * radius) - RayToSphereLength +
                  (IntersectPoint * IntersectPoint);

   if(SquaredPoint < 0)
      return false;

   if(dist)
      *dist = IntersectPoint - (float)sqrt(SquaredPoint);

   return true;
}


bool Ray::Intersect(Vector3D &p1, Vector3D &p2,
                    Vector3D &p3, bool cull, float *dist)
{
   Vector3D vecAB = p2 - p1;
   Vector3D vecAC = p3 - p1;

   Vector3D cross;
   cross = m_direction.CrossProduct(vecAC);
   
   float det = vecAB.Dot3(cross);

   if(cull && det < 0.0001f)
      {
         return false;
      }
   else if(det < 0.0001f && det > -0.0001f)
      {
         return false;
      }
      
   Vector3D rayPointVec = m_origin - p1;
   float test1 = rayPointVec.Dot3(cross);

   if(test1 < 0.0f || test1 > det)
      return false;
   
   Vector3D cross2;
   cross2 = rayPointVec.CrossProduct(vecAB);
   float test2 = m_direction.Dot3(cross2);

   if(test2 < 0.0f || test1 + test2 > det)
      return false;
   
   float inverseDet = 1.0f / det;
   
   if(dist)
      {
         *dist = vecAC.Dot3(cross2);
         *dist *= inverseDet;
      }

   return true;
}


bool Ray::Intersect(Plane &pl, bool cull, Vector3D *intersectPoint, float *dist)
{
   Vector3D normal(pl.a, pl.b, pl.c);

   float rayD = normal.Dot3(m_direction);

   if(fabs(rayD) < 0.00001f)
      return false;

   if(cull && rayD > 0.0f)
      return false;

   float originD = -(normal.Dot3(m_origin) + pl.d);
   float intersectDist = originD / rayD;

   if(intersectDist < 0.001f)
      return false;
   
   if(dist)
      *dist = intersectDist;

   if(intersectPoint)
      *intersectPoint = m_origin + (m_direction * intersectDist);
   
   return true;
}


bool Ray::Intersect(Vector3D &bbMin, Vector3D &bbMax,
                     Vector3D *intersectPoint)
{
   Vector3D maxVal(-1, -1, -1);
   bool rayInside = false;

   if(m_origin.x < bbMin.x)
      {
         if(intersectPoint)
            (*intersectPoint).x = bbMin.x;

         rayInside = false;

         if(m_direction.x != 0)
            maxVal.x = (bbMin.x - m_origin.x) / m_direction.x;
      }
   else if(m_origin.x > bbMax.x)
      {
         if(intersectPoint)
            (*intersectPoint).x = bbMax.x;

         rayInside = false;

         if(m_direction.x != 0)
            maxVal.x = (bbMax.x - m_origin.x) / m_direction.x;
      }
   
   if(m_origin.y < bbMin.y)
      {
         if(intersectPoint)
            (*intersectPoint).y = bbMin.y;

         rayInside = false;

         if(m_direction.y != 0)
            maxVal.y = (bbMin.y - m_origin.y) / m_direction.y;
      }
   else if(m_origin.y > bbMax.y)
      {
         if(intersectPoint)
            (*intersectPoint).y = bbMax.y;

         rayInside = false;

         if(m_direction.y != 0)
            maxVal.y = (bbMax.y - m_origin.y) / m_direction.y;
      }
      
   if(m_origin.z < bbMin.z)
      {
         if(intersectPoint)
            (*intersectPoint).z = bbMin.z;

         rayInside = false;

         if(m_direction.z != 0)
            maxVal.z = (bbMin.z - m_origin.z) / m_direction.z;
      }
   else if(m_origin.z > bbMax.z)
      {
         if(intersectPoint)
            (*intersectPoint).z = bbMax.z;

         rayInside = false;

         if(m_direction.z != 0)
            maxVal.z = (bbMax.z - m_origin.z) / m_direction.z;
      }
   
   if(rayInside)
      {
         (*intersectPoint) = m_origin;

         return true;
      }

   int index = 0;
   float temp[3];
   temp[0] = maxVal.x; temp[1] = maxVal.y; temp[2] = maxVal.z;

   if(maxVal.y > temp[index])
      index = 1;

   if(maxVal.z > temp[index])
      index = 2;

   if(temp[index] < 0)
      return false;
   
   if(index != 0)
      {
         (*intersectPoint).x = m_origin.x + maxVal.x * m_direction.x;

         if((*intersectPoint).x < bbMin.x - 0.00001f ||
            (*intersectPoint).x < bbMax.x + 0.00001f)
            {
               return false;
            }
      }
   if(index != 1)
      {
         (*intersectPoint).y = m_origin.y + maxVal.y * m_direction.y;

         if((*intersectPoint).y < bbMin.y - 0.00001f ||
            (*intersectPoint).y < bbMax.y + 0.00001f)
            {
               return false;
            }
      }
   if(index != 2)
      {
         (*intersectPoint).z = m_origin.z + maxVal.z * m_direction.z;

         if((*intersectPoint).z < bbMin.z - 0.00001f ||
            (*intersectPoint).z < bbMax.z + 0.00001f)
            {
               return false;
            }
      }

   return true;
}


END_ENGINE_NAMESPACE