/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<OBB.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


OBB::OBB()
{
   m_halfAxis1 = m_halfAxis2 = m_halfAxis3 = 0;

   m_axis1 = Vector3D(1, 0, 0);
   m_axis2 = Vector3D(0, 1, 0);
   m_axis3 = Vector3D(0, 0, 1);

   m_totalFaces = 3;
   m_totalEdges = 3;
}


OBB::OBB(OBB &obb)
{
   m_halfAxis1 = obb.m_halfAxis1;
   m_halfAxis2 = obb.m_halfAxis2;
   m_halfAxis3 = obb.m_halfAxis3;

   m_axis1 = obb.m_axis1;
   m_axis2 = obb.m_axis2;
   m_axis3 = obb.m_axis3;

   m_totalFaces = obb.m_totalFaces;
   m_totalEdges = obb.m_totalEdges;

   m_center = obb.m_center;
}


OBB::OBB(BoundingBox &aabb)
{
   Calculate(aabb);
}


void OBB::Calculate(Vector3D *vertices, int numVerts)
{
   BoundingBox aabb;

   aabb.Calculate(vertices, numVerts);
   Calculate(aabb);
}


void OBB::Calculate(BoundingBox &aabb)
{
   m_axis1 = Vector3D(1, 0, 0);
   m_axis2 = Vector3D(0, 1, 0);
   m_axis3 = Vector3D(0, 0, 1);

   m_center = aabb.m_center;

   m_halfAxis1 = (aabb.m_max.x - aabb.m_min.x) * 0.5f;
   m_halfAxis2 = (aabb.m_max.y - aabb.m_min.y) * 0.5f;
   m_halfAxis3 = (aabb.m_max.z - aabb.m_min.z) * 0.5f;
}


void OBB::Transform(OBB &obb, Matrix4x4 &mat)
{
   Matrix4x4 tempMat = mat;

   m_center = tempMat.VectorMatrixMultiply(obb.GetCenterPos());

   tempMat.matrix[12] = 0;
   tempMat.matrix[13] = 0;
   tempMat.matrix[14] = 0;

   m_axis1 = tempMat.VectorMatrixMultiply(obb.GetAxis1());
   m_axis2 = tempMat.VectorMatrixMultiply(obb.GetAxis2());
   m_axis3 = tempMat.VectorMatrixMultiply(obb.GetAxis3());

   m_axis1.Normalize();
   m_axis2.Normalize();
   m_axis3.Normalize();

   m_halfAxis1 = obb.GetHalfAxis1() * m_axis1.Magnitude();
   m_halfAxis2 = obb.GetHalfAxis2() * m_axis2.Magnitude();
   m_halfAxis3 = obb.GetHalfAxis3() * m_axis3.Magnitude();
}


void OBB::ObjectTransform(OBB &obb, Matrix4x4 &mat)
{
   Matrix4x4 tempMat;
	Vector3D trans;

	tempMat.inverseMatrix(mat);

	trans = Vector3D(tempMat.matrix[12],
	                  tempMat.matrix[13],
	                  tempMat.matrix[14]);

	tempMat.matrix[12] = 0;
   tempMat.matrix[13] = 0;
   tempMat.matrix[14] = 0;

	m_center = tempMat.VectorMatrixMultiply(obb.GetCenterPos());
	m_axis1 = tempMat.VectorMatrixMultiply(obb.GetAxis1());
   m_axis2 = tempMat.VectorMatrixMultiply(obb.GetAxis2());
   m_axis3 = tempMat.VectorMatrixMultiply(obb.GetAxis3());

	m_center += trans;

	m_halfAxis1 = obb.GetHalfAxis1();
   m_halfAxis2 = obb.GetHalfAxis2();
   m_halfAxis3 = obb.GetHalfAxis3();
}


void OBB::ProjectionInterval(Vector3D &axis, float &center,
                             float &ext, float &min,
                             float &max)
{
   center = m_center.Dot3(axis);

   ext = fabs(m_axis1.Dot3(axis)) * m_halfAxis1 +
         fabs(m_axis2.Dot3(axis)) * m_halfAxis2 +
         fabs(m_axis3.Dot3(axis)) * m_halfAxis3;

   min = center - ext;
   max = center + ext;
}


unsigned int OBB::GetSupport(Vector3D &axis, Vector3D *contacts)
{
   Vector3D vecs[8] = 
   { 
      Vector3D(-1, -1, -1), Vector3D( 1, -1, -1),
      Vector3D( 1, -1,  1), Vector3D(-1, -1,  1),
      Vector3D(-1,  1, -1), Vector3D( 1,  1, -1),
      Vector3D( 1,  1,  1), Vector3D(-1,  1,  1)
   };

   int faces[6][4] = 
   {
      {4, 0, 3, 7}, {1, 5, 6, 2},
      {0, 1, 2, 3}, {7, 6, 5, 4},
      {5, 1, 0, 4}, {6, 7, 3, 2},
   };

   float d[3];

   float face_threshold = 1.0f - 1.0E-4f;

   for(int i = 0; i < 3; i++)
      {
         if(i == 0)
            d[i] = axis.Dot3(m_axis1);
         else if(i == 1)
            d[i] = axis.Dot3(m_axis2);
         else
            d[i] = axis.Dot3(m_axis3);

         if(fabs(d[i]) > face_threshold)
            {
               int f = (SIGN(d[i]) == -1.0f)? (i * 2) : (i * 2) + 1;

               for(int j = 0; j < 4; j++)
                  {
                     Vector3D P = vecs[faces[f][j]];

                     contacts[j] = m_center + 
                        m_axis1 * (P.x * -m_halfAxis1) +
                        m_axis2 * (P.y * -m_halfAxis2) +
                        m_axis3 * (P.z * -m_halfAxis2);
                  }

               return 4;
            }
      }

   const float edge_threshold = 2.6E-3f;

   for(int i = 0; i < 3; i++)
      {
         if(fabs(d[i]) < edge_threshold)
            {
               int j = (i + 1) % 3;
               int k = (i + 2) % 3;

               Vector3D newCenter = m_center +
                  GetAxis(j) * (SIGN(d[j]) * -GetHalfAxis(j)) +
                  GetAxis(k) * (SIGN(d[k]) * -GetHalfAxis(k));

               contacts[0] = newCenter + GetAxis(i) * -GetHalfAxis(i);
               contacts[1] = newCenter - GetAxis(i) * -GetHalfAxis(i);

               return 2;
            }
      }

   Vector3D newCenter = m_center + 
      GetAxis(0) * (SIGN(d[0]) * -GetHalfAxis(0)) +
      GetAxis(1) * (SIGN(d[1]) * -GetHalfAxis(1)) +
      GetAxis(2) * (SIGN(d[2]) * -GetHalfAxis(2));

   contacts[0] = newCenter;

   return 1;
}


END_ENGINE_NAMESPACE