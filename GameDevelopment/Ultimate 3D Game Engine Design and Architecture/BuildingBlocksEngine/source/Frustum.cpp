/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Frustum.h>
#include<Defines.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


Frustum::Frustum()
{

}


void Frustum::CalculateFrustum(float angle, float ratio, float near, float far,
                               Vector3D &camPos, Vector3D &lookAt, Vector3D &up)
{
   Vector3D xVec, yVec, zVec;
	Vector3D vecN, vecF;
	Vector3D nearTopLeft, nearTopRight,
	         nearBottomLeft, nearBottomRight;
   Vector3D farTopLeft, farTopRight,
            farBottomLeft, farBottomRight;

	float radians = (float)tan((DEG_TO_RAD(angle)) * 0.5);
	float nearH = near  * radians;
	float nearW = nearH * ratio;
	float farH = far    * radians;
	float farW = farH   * ratio;

	zVec = camPos - lookAt;
	zVec.Normalize();

	xVec = up.CrossProduct(zVec);
	xVec.Normalize();

	yVec = zVec.CrossProduct(xVec);

	vecN = camPos - zVec * near;
	vecF = camPos - zVec * far;

	nearTopLeft     = vecN + yVec * nearH - xVec * nearW;
	nearTopRight    = vecN + yVec * nearH + xVec * nearW;
	nearBottomLeft  = vecN - yVec * nearH - xVec * nearW;
	nearBottomRight = vecN - yVec * nearH + xVec * nearW;

	farTopLeft      = vecF + yVec * farH - xVec * farW;
	farTopRight     = vecF + yVec * farH + xVec * farW;
	farBottomLeft   = vecF - yVec * farH - xVec * farW;
	farBottomRight  = vecF - yVec * farH + xVec * farW;

   m_frustum.clear();

   Plane plane;

   plane.CreatePlaneFromTri(nearTopRight, nearTopLeft,
                            farTopLeft);
   AddPlane(plane);

   plane.CreatePlaneFromTri(nearBottomLeft, nearBottomRight,
                            farBottomRight);
   AddPlane(plane);

   plane.CreatePlaneFromTri(nearTopLeft, nearBottomLeft,
                            farBottomLeft);
   AddPlane(plane);

   plane.CreatePlaneFromTri(nearBottomRight, nearTopRight,
                            farBottomRight);
   AddPlane(plane);

   plane.CreatePlaneFromTri(nearTopLeft, nearTopRight,
                            nearBottomRight);
   AddPlane(plane);

   plane.CreatePlaneFromTri(farTopRight, farTopLeft,
                            farBottomLeft);
   AddPlane(plane);
}


void Frustum::AddPlane(Plane &pl)
{
   m_frustum.push_back(pl);
}


bool Frustum::GetPlane(int index, Plane *out)
{
   if(out == 0 || index >= (int)m_frustum.size() || index < 0)
      return false;

   *out = m_frustum[index];

   return true;
}


bool Frustum::isPointVisible(float x, float y, float z)
{
   for(int i = 0; i < (int)m_frustum.size(); i++)
      {
         if(m_frustum[i].GetDistance(x, y, z) < 0)
            return false;
      }

   return true;
}


bool Frustum::isSphereVisible(float x, float y, float z, float radius)
{
   float distance = 0;

   for(int i = 0; i < (int)m_frustum.size(); i++)
      {
         distance = m_frustum[i].GetDistance(x, y, z);

         if(distance < -radius)
            return false;
      }

   return true;
}


bool Frustum::isCubeVisible(float x, float y, float z, float size)
{
   float minX, maxX;
   float minY, maxY;
   float minZ, maxZ;

   minX = x - size; maxX = x + size;
   minY = y - size; maxY = y + size;
   minZ = z - size; maxZ = z + size;

   return isBoxVisible(Vector3D(minX, minY, minZ),
                       Vector3D(maxX, maxY, maxZ));
}


bool Frustum::isBoxVisible(Vector3D min, Vector3D max)
{
   if(isPointVisible(min.x, min.y, min.z))
      return true;

   if(isPointVisible(max.x, min.y, min.z))
      return true;

   if(isPointVisible(min.x, max.y, min.z))
      return true;

   if(isPointVisible(max.x, max.y, min.z))
      return true;

   if(isPointVisible(min.x, min.y, max.z))
      return true;

   if(isPointVisible(max.x, min.y, max.z))
      return true;

   if(isPointVisible(min.x, max.y, max.z))
      return true;

   if(isPointVisible(max.x, max.y, max.z))
      return true;

   return false;
}


bool Frustum::isOBBVisible(OBB &obb)
{
   Vector3D n;
   float radius = 0.0f;

   for(int i = 0; i < (int)m_frustum.size(); i++)
      {
         n = Vector3D(m_frustum[i].a,
                       m_frustum[i].b,
                       m_frustum[i].c);
         n = n * -1;

         float r = fabs(obb.m_halfAxis1 * (n.Dot3(obb.m_axis1))) +
                   fabs(obb.m_halfAxis2 * (n.Dot3(obb.m_axis2))) +
                   fabs(obb.m_halfAxis3 * (n.Dot3(obb.m_axis3)));

         if(n.Dot3((obb.m_center - m_frustum[i].d)) < -r)
            return false;
      }

   return true;
}


END_ENGINE_NAMESPACE