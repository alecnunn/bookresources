/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Plane.h>
#include<math.h>
#include<string.h>


DECLARE_ENGINE_NAMESPACE


Plane::Plane()
{
   a = 0;
   b = 0;
   c = 0;
   d = 0;
}


Plane::Plane(scalar A, scalar B, scalar C, scalar D)
{
   a = A;
   b = B;
   c = C;
   d = D;
}


void Plane::CreatePlaneFromTri(Vector3D &t1, Vector3D &t2,
                              Vector3D &t3)
{
   Vector3D e1, e2, n;

   e1 = t2 - t1;
   e2 = t3 - t1;

   n = e1.CrossProduct(e2);
   n.Normalize();

   a = n.x;
   b = n.y;
   c = n.z;
   d = - (a * t1.x + b * t1.y + c * t1.z);

   m_pointOnPlane = t1;
}


bool Plane::Intersect(const Plane &pl, Vector3D *intersectPoint)
{
   Vector3D cross, normal(a, b, c), plNormal(pl.a, pl.b, pl.c);
   float length = 0;

   cross = normal.CrossProduct(plNormal);
   length = cross.Dot3(cross);

   if(length < 1e-08f)
      return false;
   
   if(intersectPoint)
      {
         float l0 = normal.Dot3(normal);
         float l1 = normal.Dot3(plNormal);
         float l2 = plNormal.Dot3(plNormal);

         float det = l0 * l2 - l1 * l1;
         float invDet = 0;
         
         if(fabs(det) < 1e-08f)
            return false;
         
         invDet = 1 / det;

         float d0 = (l2 * d - l1 * pl.d) * invDet;
         float d1 = (l0 * pl.d - l1 * d) * invDet;
         
         (*intersectPoint) = normal * d0 + plNormal * d1;
      }

   return true;
}


bool Plane::Intersect(const Vector3D &bbMin, const Vector3D &bbMax)
{
   Vector3D min, max;
   Vector3D normal(a, b, c);
   
   if(normal.x >= 0.0f)
      {
         min.x = bbMin.x;
         max.x = bbMax.x;
      }
   else
      {
         min.x = bbMax.x;
         max.x = bbMin.x;
      }
   
   if(normal.y >= 0.0f)
      {
         min.y = bbMin.y;
         max.y = bbMax.y;
      }
   else
      {
         min.y = bbMax.y;
         max.y = bbMin.y;
      }
   
   if(normal.z >= 0.0f)
      {
         min.z = bbMin.z;
         max.z = bbMax.z;
      }
   else
      {
         min.z = bbMax.z;
         max.z = bbMin.z;
      }
   
   if((normal.Dot3(min) + d) > 0.0f)
      return false;

   if((normal.Dot3(max) + d) >= 0.0f)
      return true;
   
   return false;
}


bool Plane::Intersect(const Vector3D &position, float radius)
{
	float dp = fabs(GetDistance(position));

	if(dp <= radius)
      return true;

   return false;
}


bool Plane::Intersect(const OBB &obb)
{
   Vector3D n(a, b, c);

   float r = fabs(obb.m_halfAxis1 * (n.Dot3(obb.m_axis1))) +
             fabs(obb.m_halfAxis2 * (n.Dot3(obb.m_axis2))) +
             fabs(obb.m_halfAxis3 * (n.Dot3(obb.m_axis3)));

   return(GetDistance(obb.m_center) <= r);
}


BB_PLANE_STATUS Plane::ClassifyPoint(const Vector3D &v)
{
   return ClassifyPoint(v.x, v.y, v.z, 0);
}


BB_PLANE_STATUS Plane::ClassifyPoint(const Vector3D &v, float *dist)
{
   return ClassifyPoint(v.x, v.y, v.z, dist);
}


BB_PLANE_STATUS Plane::ClassifyPoint(float x, float y, float z)
{
   return ClassifyPoint(x, y, z, 0);
}


BB_PLANE_STATUS Plane::ClassifyPoint(float x, float y, float z, float *dist)
{
   float distance = a * x + b * y + c * z + d;

   if(dist != 0)
      *dist = distance;

   if(distance > 0.001)
      return BB_PLANE_FRONT;

   if(distance < -0.001)
      return BB_PLANE_BACK;

   return BB_PLANE_ON_PLANE;
}


scalar Plane::GetDistance(const Vector3D &v)
{
   return a * v.x + b * v.y + c * v.z + d;
}


scalar Plane::GetDistance(float x, float y, float z)
{
   return a * x + b * y + c * z + d;
}


Vector3D Plane::Reflect(Vector3D vec, float e)
{
	Vector3D n(a, b, c);

	if((vec.Dot3(n)) > 0.0f)
      n = n * -1.0f;

	n = n * (n.Dot3(vec));

	Vector3D dir = vec - n;
	Vector3D reflect = dir - (n * e);

   return reflect;
}


bool Plane::ClipTriangle(Vector3D *inVerts, int totalInVerts,
                         Vector3D *outFrontVerts, int *totalOutFrontVerts,
                         Vector3D *outBackVerts, int *totalOutBackVerts)
{
   if(inVerts == 0 || totalInVerts <= 0 || totalInVerts != 3)
      return false;

   float dist[3] = { 0 };
   int side[3] = { 0 };
   int numFront = 0, numBack = 0;
   int frontIndex = 0, backIndex = 0;

   BB_PLANE_STATUS result = BB_PLANE_FRONT;

   Vector3D tempFront[4];
   Vector3D tempBack[4];

   for(int i = 0; i < 3; i++)
      {
         result = ClassifyPoint(inVerts[i], &dist[i]);

         if(result == BB_PLANE_FRONT)
            {
               side[i] = BB_PLANE_FRONT;
               numFront++;
            }
         if(result == BB_PLANE_BACK)
            {
               side[i] = BB_PLANE_BACK;
               numBack++;
            }
         else
            {
               side[i] = BB_PLANE_FRONT;
               numFront++;
            }
      }

   if(!numBack)
      {
         outFrontVerts = new Vector3D[totalInVerts];
         memcpy(outFrontVerts, inVerts, sizeof(Vector3D) * totalInVerts);

         if(totalOutFrontVerts != NULL)
            *totalOutFrontVerts = totalInVerts;
      }
   else if(!numFront)
      {
         outBackVerts = new Vector3D[totalInVerts];
         memcpy(outBackVerts, inVerts, sizeof(Vector3D) * totalInVerts);

         if(totalOutBackVerts != NULL)
            *totalOutBackVerts = totalInVerts;
      }
   else
      {
         for(int i = 0; i < 3; i++)
            {
               if(side[i] == BB_PLANE_FRONT)
                  {
                     tempFront[frontIndex++] = inVerts[i];
                  }
               else if(side[i] == BB_PLANE_FRONT)
                  {
                     tempBack[backIndex++] = inVerts[i];
                  }

               int j = (i + 1) % 3;

               if(side[i] != side[j])
                  {
                     float t = dist[i] / (dist[i] - dist[j]);
                     Vector3D v = inVerts[i] * (1.0f - t) + inVerts[j] * t;
                     
                     tempFront[frontIndex++] = v;
                     tempBack[backIndex++] = v;
                  }
            }

         if(frontIndex < 4)
            {
               outFrontVerts = new Vector3D[frontIndex];
               memcpy(outFrontVerts, tempFront, sizeof(Vector3D) * frontIndex);

               if(totalOutFrontVerts != NULL)
                  *totalOutFrontVerts = frontIndex;
            }
         else
            {
               outFrontVerts = new Vector3D[6];

               outFrontVerts[0] = tempFront[0];
               outFrontVerts[1] = tempFront[1];
               outFrontVerts[2] = tempFront[2];

               outFrontVerts[3] = tempFront[2];
               outFrontVerts[4] = tempFront[3];
               outFrontVerts[5] = tempFront[0];

               if(totalOutFrontVerts != NULL)
                  *totalOutFrontVerts = 6;
            }

         if(backIndex < 4)
            {
               outBackVerts = new Vector3D[backIndex];
               memcpy(outBackVerts, tempBack, sizeof(Vector3D) * backIndex);

               if(totalOutBackVerts != NULL)
                  *totalOutBackVerts = backIndex;
            }
         else
            {
               outBackVerts = new Vector3D[6];

               outBackVerts[0] = tempBack[0];
               outBackVerts[1] = tempBack[1];
               outBackVerts[2] = tempBack[2];

               outBackVerts[3] = tempBack[2];
               outBackVerts[4] = tempBack[3];
               outBackVerts[5] = tempBack[0];

               if(totalOutBackVerts != NULL)
                  *totalOutBackVerts = 6;
            }
      }

   return true;
}


END_ENGINE_NAMESPACE