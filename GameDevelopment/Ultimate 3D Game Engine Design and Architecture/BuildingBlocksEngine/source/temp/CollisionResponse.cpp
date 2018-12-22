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


CollisionResponse::CollisionResponse()
{
}


CollisionResponse::~CollisionResponse()
{
}


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


// Polytope Collisions.


bool CollisionResponse::ResponseToCollision(Polytope &p1, Polytope &p2)
{
   return Intersect(p1, p2);
}


bool CollisionResponse::ResponseToCollision(Polytope &p1, Polytope &p2,
                                            Vector3D &p1Vel, Vector3D &p2Vel,
                                            float p1Max, float p2Max,
                                            CollisionReport &report)
{
   Vector3D vel = p2Vel - p1Vel;

   report.SetMTD(Vector3D(BB_NULL_FLOAT, BB_NULL_FLOAT,
                           BB_NULL_FLOAT));

   report.SetCollisionNormal(Vector3D(0, 0, 0));
   report.SetTime(0);

   if(!Intersect(p1, p2, vel, BB_MIN(p1Max, p2Max), report))
      return false;

   Vector3D n = report.GetCollisionNormal();

   if(n.x == 0 && n.y == 0 && n.z == 0)
      {
         report.SetCollisionNormal(report.GetMTD());
         report.SetIsOverlap(true);
      }
   else
      {
         report.SetIsCollision(true);
      }

   n.Normalize();
   report.SetCollisionNormal(n);

   if(n.Dot3(p1.GetCenterPos() - p2.GetCenterPos()) < 0)
      {
         report.SetCollisionNormal(n * -1);

         if(report.GetIsOverlap() == true)
            report.SetMTD(report.GetMTD() * -1);
      }

   CalcContactPoints(p1, p2, p1Vel, p2Vel, report);

   return true;
}


bool CollisionResponse::ResponseToCollision(Polytope &p1, Plane &plane,
                                            Vector3D &relVel, float max,
                                            CollisionReport &report)
{
   Vector3D offsetVel, n(0, 0, 0);

   report.SetMTD(Vector3D(BB_NULL_FLOAT, BB_NULL_FLOAT,
                           BB_NULL_FLOAT));
   report.SetCollisionNormal(n);
   report.SetTime(0);

   offsetVel = relVel * max;

   if(Intersect(p1, plane, offsetVel, max, report) == false)
      return false;

   n = report.GetCollisionNormal();
   n.Normalize();
   report.SetCollisionNormal(n);

   CalcContactPoints(p1, plane, relVel, report);

   return true;
}


// Rigid Box collisions.


bool CollisionResponse::ResponseToCollision(RigidBox &body, bool useAverage)
{
   return false;
}


bool CollisionResponse::ResponseToCollision(OBB &obb, bool useAverage)
{
   return false;
}


bool CollisionResponse::ResponseToCollision(Frustum &frustum, bool useAverage)
{
   return false;
}


bool CollisionResponse::ResponseToCollision(Plane &plane, bool useAverage)
{
   return false;
}


bool CollisionResponse::Intersect(Polytope &p1, Polytope &p2)
{
   int i = 0, j = 0;
   Vector3D axis, dir;

   for(i = 0; i < p1.GetTotalFaces(); i++)
      {
         if(OverlapOnInterval(p1, p2, p1.GetFaceNormal(i)) == false)
            return false;
      }

   for(i = 0; i < p2.GetTotalFaces(); i++)
      {
         if(OverlapOnInterval(p1, p2, p2.GetFaceNormal(i)) == false)
            return false;
      }

   for(i = 0; i < p1.GetTotalEdges(); i++)
      {
         for(j = 0; j < p2.GetTotalEdges(); j++)
            {
               dir = p1.GetEdgeDirection(i);
               axis = dir.CrossProduct(p2.GetEdgeDirection(j));

               if(axis.Magnitude() < 1.0E-6f)
                  continue;

               if(OverlapOnInterval(p1, p2, axis) == false)
                  return false;
            }
      }

   return true;
}


bool CollisionResponse::Intersect(Polytope &p1, Polytope &p2,
                                  Vector3D &relativeVel, float max,
                                  CollisionReport &report)
{
   float lastTime = BB_NULL_FLOAT;
   Vector3D axis, dir;

   for(int i = 0; i < p1.GetTotalFaces(); i++)
      {
         if(OverlapOnInterval(p1, p2, relativeVel,
            p1.GetFaceNormal(i), max, lastTime, report) == false)
            {
               return false;
            }
      }

   for(i = 0; i < p2.GetTotalFaces(); i++)
      {
         if(OverlapOnInterval(p2, p1, relativeVel,
            p2.GetFaceNormal(i), max, lastTime, report) == false)
            {
               return false;
            }
      }

   for(i = 0; i < p1.GetTotalEdges(); i++)
      {
         for(int j = 0; j < p2.GetTotalEdges(); j++)
            {
               dir = p1.GetEdgeDirection(i);
               axis = dir.CrossProduct(p2.GetEdgeDirection(j));

               if(axis.Magnitude() < 1.0E-6f)
                  continue;

               if(OverlapOnInterval(p1, p2, relativeVel,
                  axis, max, lastTime, report) == false)
                  {
                     return false;
                  }
            }
      }

   if(report.GetTime() > max || lastTime < 0)
      return false;

   return true;
}


bool CollisionResponse::Intersect(Polytope &p1, Plane &plane,
                                  Vector3D &relVel, float max,
                                  CollisionReport &report)
{
   float minA = 0, maxA = 0, center = 0, extent = 0;
   Vector3D n(plane.GetNormal());

   if(plane.ClassifyPoint(p1.GetCenterPos()) == BB_PLANE_BACK)
      n = n * -1;

   p1.ProjectionInterval(n, center, extent, minA, maxA);

   float dot = n.Dot3(relVel);
   float dist = plane.GetDistance(p1.GetCenterPos());

   if(dist <= extent)
      {
         if(dot == 0)
            center = 1;
         else
            center = (extent - dist) / -dot;

         report.SetTime(center * max);
         report.SetCollisionNormal(n);
         report.SetMTD(relVel * (center * max));
         report.SetIsOverlap(true);

         return true;
      }
   else if(dot < 0)
      {
         extent = extent - dot;

         if(dist <= extent)
            {
               report.SetCollisionNormal(n);
               center = (extent - dist) / -dot;

               report.SetTime(center * max);
               report.SetIsCollision(true);

               return true;
            }
      }

   return false;
}


bool CollisionResponse::OverlapOnInterval(Polytope &p1, Polytope &p2, Vector3D &axis)
{
   float minA = 0, maxA = 0, minB = 0, maxB = 0, center = 0, extent = 0;

   p1.ProjectionInterval(axis, center, extent, minA, maxA);
   p2.ProjectionInterval(axis, center, extent, minB, maxB);

   if(minA > maxB || minB > maxA)
      return false;

   return true;
}


bool CollisionResponse::OverlapOnInterval(Polytope &p1, Polytope &p2, Vector3D &p2Vel,
                                          Vector3D &axis, float max, float &lastTime,
                                          CollisionReport &report)
{
   float minA = 0, maxA = 0, minB = 0, maxB = 0;
   float centerA = 0, extentA = 0, centerB = 0, extentB = 0;
   float side = 0, firstTime = 0, vDot = 0, temp = 0;

   firstTime = report.GetTime();

   p1.ProjectionInterval(axis, centerA, extentA, minA, maxA);
   p2.ProjectionInterval(axis, centerB, extentB, minB, maxB);

   vDot = p2Vel.Dot3(axis);

   if(maxB < minA)
      {
         if(vDot <= 0)
            {
               return false;
            }
         else
            {
               temp = (minA - maxB) / vDot;

               if(temp > firstTime)
                  {
                     firstTime = temp;
                     side = -1;
                  }

               if(firstTime > max)
                  return false;

               temp = (maxA - minB) / vDot;

               if(temp < lastTime)
                  lastTime = temp;

               if(firstTime > lastTime)
                  return false;
            }
      }
   else if(maxA < minB)
      {
         if(vDot >= 0)
            {
               return false;
            }
         else
            {
               temp = (maxA - minB) / vDot;

               if(temp > firstTime)
                  {
                     firstTime = temp;
                     side = 1;
                  }

               if(firstTime > max)
                  return false;

               temp = (minA - maxB) / vDot;

               if(temp < lastTime)
                  lastTime = temp;

               if(firstTime > lastTime)
                  return false;
            }
      }
   else
      {
         if(vDot > 0)
            {
               temp = (maxA - minB) / vDot;

               if(temp < lastTime)
                  lastTime = temp;

               if(firstTime > lastTime)
                  return false;
            }
         else if(vDot < 0)
            {
               temp = (minA - maxB) / vDot;

               if(temp < lastTime)
                  lastTime = temp;

               if(firstTime > lastTime)
                  return false;
            }

         float diffA = maxB - minA;
         float diffB = maxA - minB;
         temp = diffA;

         if(fabs(diffB) < fabs(temp))
            temp = diffB;

         Vector3D mtd = report.GetMTD();

         if(mtd.Magnitude() > (temp * temp))
            report.SetMTD(axis * temp);
      }

   if(side != 0)
      {
         report.SetTime(firstTime);
         report.SetCollisionNormal(axis * -side);
      }

   return true;
}


void CollisionResponse::CalcContactPoints(Polytope &p1, Polytope &p2, Vector3D &p1Vel,
                                          Vector3D &p2Vel, CollisionReport &report)
{
   Vector3D contactA[4], contactB[4], n;
   int i = 0;

   n = report.GetCollisionNormal();

   int totalA = p1.GetSupport(n, contactA);
   int totalB = p2.GetSupport(n * -1, contactB);

   for(i = 0; i < totalA; i++)
      { 
         contactA[i] += p1Vel * report.GetTime();
         report.AddContactPointA(contactA[i]);
      }
   for(i = 0; i < totalB; i++) 
      {
         contactB[i] += p2Vel * report.GetTime();
         report.AddContactPointB(contactB[i]);
      }

   if(totalA == 1)
      {
         report.AddContactPoint(contactA[0]);
      }
   else if(totalA == 2)
      {
         if(totalB == 1)
            {
               report.AddContactPoint(contactB[0]);
            }
         else if(totalB == 2)
            {
               //GetEdgeEdgeContacts(contactA[0], contactA[1],
                                   //contactB[0], contactB[1],
                                   //report);
            }
         else
            {
               GetPolygonContacts(contactB, totalB,
                                  contactA, totalA,
                                  report);
            }
      }
   else
      {
         if(totalB == 1)
            {
               report.AddContactPoint(contactB[0]);
            }
         else
            {
               GetPolygonContacts(contactA, totalA,
                                  contactB, totalB,
                                  report);
            }
      }
}


void CollisionResponse::CalcContactPoints(Polytope &p1, Plane &plane, Vector3D &p1Vel,
                                          CollisionReport &report)
{
   Vector3D contactA[4];

   int total = p1.GetSupport(report.GetCollisionNormal() * 1.0f,
                             contactA);

   for(int i = 0; i < total; i++) 
      {
         contactA[i] += p1Vel * report.GetTime();

         report.AddContactPoint(contactA[i]);
         report.AddContactPointA(contactA[i]);
      }
}


/*void CollisionResponse::GetEdgeEdgeContacts(Vector3D &pA0, Vector3D &pA1,
                                            Vector3D &pB0, Vector3D &pB1,
                                            CollisionReport &report)
{
   Vector3D diffA = pA1 - pA0;
   Vector3D diffB = pB1 - pB0;

   if(fabs(diffA.Dot3(diffB)) < 0.001f)
      {
         Vector3D centerA = pA0 + (diffA * 0.5f);
         Vector3D centerB = pB0 + (diffB * 0.5f);

         diffA = centerA + (centerB - centerA) * 0.5f;
         report.AddContactPoint(diffA);
      }
   else
      {
         Vector3D diff = pA0 - pB0;
         float a = diffA.Dot3(diffA);
         float e = diffB.Dot3(diffB);
         float f = diffB.Dot3(diff);

         if((a > 0.0001f) && (e > 0.0001f))
            {
               float b = diffA.Dot3(diffB), s = 0.0f;
               float c = diffA.Dot3(diff);
               float temp = a * e - b * b;

               if(temp != 0.0f)
                  {
                     s = (b * f - c * e) / temp;

                     if(s < 0.0f)
                        s = 0.0f;
                     else if(s > 1.0f)
                        s = 1.0f;
                  }

               float t = (b * s + f) / e;

               if(t < 0.0f)
                  {
                     t = 0.0f;
                     s = -c / a;

                     if(s < 0.0f)
                        s = 0.0f;
                     else if(s > 1.0f)
                        s = 1.0f;
                  }
               else if(t > 1.0f)
                  {
                     t = 1.0f;
                     s = (b - c) / a;

                     if(s < 0.0f)
                        s = 0.0f;
                     else if(s > 1.0f)
                        s = 1.0f;
                  }

               report.AddContactPoint(pB0 + diffB * t);
            }
      }
}*/


void CollisionResponse::GetPolygonContacts(Vector3D *clipVerts, int numClip,
                                           Vector3D *polys, int numPolys,
                                           CollisionReport &report)
{
   int totalVerts = 0;
   Vector3D clipped[64];

   if(ClipPolygon(clipVerts, numClip, polys,
                  numPolys, clipped, totalVerts))
      {
         for(int i = 0; i < totalVerts; i++)
            {
               report.AddContactPoint(clipped[i]);
            }
      }
}


bool CollisionResponse::ClipPolygon(Vector3D *inClipVerts, int numInClipVerts,
                                    Vector3D *inVerts, int numInVerts,
                                    Vector3D *outVerts, int &numOutVerts)
{
   numOutVerts = 0;

	if(numInClipVerts <= 2)
	   return false;

	Vector3D clipNormal;

	clipNormal.Normalize(inClipVerts[0],
	                     inClipVerts[1],
	                     inClipVerts[2]);

	Vector3D tempList[BB_MAX_CLIPPING_VERTS];
	int tempTotal = numInVerts;

	memcpy(tempList, inVerts, sizeof(Vector3D) * numInVerts);

	int i = numInClipVerts - 1;

	for(int ip1 = 0; ip1 < numInClipVerts; i = ip1, ip1++)
	   {
		   Vector3D diff(inClipVerts[ip1] - inClipVerts[i]);
		   Vector3D normal = diff.CrossProduct(clipNormal);

		   normal.Normalize();

		   Plane plane;
		   plane.SetNormal(normal);
		   plane.SetPointOnPlane(inClipVerts[i]);
		   plane.d = -normal.Dot3(inClipVerts[i]);

		   if(plane.ClipTriangle(tempList, tempTotal,
		                         NULL, NULL, outVerts,
		                         &numOutVerts))
		      {
			      tempTotal = numOutVerts;
			      memcpy(tempList, outVerts, sizeof(Vector3D) * numOutVerts);
		      }
		   else
		      {
		         return false;
		      }
	   }

   return true;
}


END_ENGINE_NAMESPACE