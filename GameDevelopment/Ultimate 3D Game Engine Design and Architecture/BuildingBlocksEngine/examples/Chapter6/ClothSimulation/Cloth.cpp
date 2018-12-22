/*
   Cloth Simulation Demo - Chapter 6
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"Cloth.h"


bool Cloth::Initialize(int gridSize, float mass,
                       float springConst, float springLen,
                       float damp)
{
   int springIndex = 0;

   m_constant = springConst;
   m_dampFactor = damp;
   m_gridSize = gridSize;

   m_totalBalls = m_gridSize * m_gridSize;

   m_totalSprings = (m_gridSize - 1) * m_gridSize * 2;
   m_totalSprings += (m_gridSize - 1) * (m_gridSize - 1) * 2;
   m_totalSprings += (m_gridSize - 2) * m_gridSize * 2;

   m_balls = new bbe::SpringMass[m_totalBalls];
   m_springs = new bbe::Spring[m_totalSprings];

   if(m_balls == 0 || m_springs == 0)
      return false;

   // Set the mass for all balls.
   for(int i = 0; i < m_totalBalls; i++)
      {
         m_balls[i].SetMass(mass);
         m_balls[i].SetVelocityDamper(m_dampFactor);
      }
      
   // Calculate the m_springs not on the right edge.
   for(int i = 0; i < m_gridSize; i++)
      {
         for(int j = 0; j < m_gridSize - 1; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen, &m_balls[i * m_gridSize + j],
                  &m_balls[i * m_gridSize + j + 1]);
            }
      }

   // Calculate the m_springs not on the bottom edge.
   for(int i = 0; i < m_gridSize - 1; i++)
      {
         for(int j = 0; j < m_gridSize; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen, &m_balls[i * m_gridSize + j],
                  &m_balls[(i + 1) * m_gridSize + j]);
            }
      }

   // Calculate the m_springs not on the right or bottom edge.
   for(int i = 0; i < m_gridSize - 1; i++)
      {
         for(int j = 0; j < m_gridSize - 1; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen * 1.5f, &m_balls[i * m_gridSize + j],
                  &m_balls[(i + 1) * m_gridSize + j + 1]);
            }
      }

   // Calculate the m_springs from one below and left but not
   // on the right or bottom edge.
   for(int i = 0; i < m_gridSize - 1; i++)
      {
         for(int j = 1; j < m_gridSize; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen * 1.5f, &m_balls[i * m_gridSize + j],
                  &m_balls[(i + 1) * m_gridSize + j - 1]);
            }
      }

   // Calculate the m_springs not on or next to the right edge.
   for(int i = 0; i < m_gridSize; i++)
      {
         for(int j = 0; j < m_gridSize - 2; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen * 2.0f, &m_balls[i * m_gridSize + j],
                  &m_balls[i * m_gridSize + j + 2]);
            }
      }

   // Calculate the m_springs not on or next to the bottom edge.
   for(int i = 0; i < m_gridSize - 2; i++)
      {
         for(int j = 0; j < m_gridSize; j++)
            {
               m_springs[springIndex++].SetSpring(0, m_constant,
                  springLen * 2.0f, &m_balls[i * m_gridSize + j],
                  &m_balls[(i + 2) * m_gridSize + j]);
            }
      }

   m_totalVertices = (m_gridSize - 2) * (m_gridSize - 2) * 6;
   m_vertices = new stClothVertex[m_totalVertices];

   return true;
}


void Cloth::RecalculateMesh()
{
   bbe::Vector3D *p0 = 0, *p1 = 0, *p2 = 0, *p3 = 0, *p4 = 0, *p5 = 0;
   bbe::Vector3D *n0 = 0, *n1 = 0, *n2 = 0, *n3 = 0, *n4 = 0, *n5 = 0;
   int index = 0;

   // Reset normals.
	for(int i = 0; i < m_totalBalls; i++)
		m_balls[i].SetNormal(bbe::Vector3D(0, 0, 0));

   // Recalculate all normals for the cloth mesh.
	for(int i = 0; i < m_gridSize - 1; ++i)
		{
			for(int j = 0; j < m_gridSize - 1; ++j)
			   {
				   p0 = &m_balls[i * m_gridSize + j].GetPosition();
				   p1 = &m_balls[i * m_gridSize+j + 1].GetPosition();
				   p2 = &m_balls[(i + 1) * m_gridSize + j].GetPosition();

				   p3 = &m_balls[(i + 1) * m_gridSize + j].GetPosition();
				   p4 = &m_balls[i * m_gridSize + j + 1].GetPosition();
				   p5 = &m_balls[(i + 1) * m_gridSize + j + 1].GetPosition();

				   n0 = m_balls[i * m_gridSize + j].GetNormalPtr();
				   n1 = m_balls[i * m_gridSize+j + 1].GetNormalPtr();
				   n2 = m_balls[(i + 1) * m_gridSize + j].GetNormalPtr();

				   n3 = m_balls[(i + 1) * m_gridSize + j].GetNormalPtr();
				   n4 = m_balls[i * m_gridSize + j + 1].GetNormalPtr();
				   n5 = m_balls[(i + 1) * m_gridSize + j + 1].GetNormalPtr();

               bbe::Vector3D tempN1, tempN2;
               tempN1.Normalize(*p0, *p1, *p2);
               tempN2.Normalize(*p3, *p4, *p5);
               
               *n0 += tempN1; *n1 += tempN1; *n2 += tempN1;
               *n3 += tempN2; *n4 += tempN2; *n5 += tempN2;
			   }
		   }

   for(int i = 0; i < m_totalBalls; i++)
      {
         m_balls[i].NormalizeNormal();
      }

   // Update mesh.
   for(int y = 0; y < m_gridSize - 2; y++)
      {
         for(int x = 0; x < m_gridSize - 2; x++)
            {
               // V1.
               m_vertices[index].pos =
                  m_balls[y * m_gridSize + x].GetPosition();

               m_vertices[index++].normal =
                  m_balls[y * m_gridSize + x].GetNormal();

               // V2.
               m_vertices[index].pos =
                  m_balls[y * m_gridSize + x + 1].GetPosition();

               m_vertices[index++].normal =
                  m_balls[y * m_gridSize + x + 1].GetNormal();

               // V3.
               m_vertices[index].pos =
                  m_balls[(y + 1) * m_gridSize + x].GetPosition();

               m_vertices[index++].normal =
                  m_balls[(y + 1) * m_gridSize + x].GetNormal();

               // V4.
               m_vertices[index].pos =
                  m_balls[(y + 1) * m_gridSize + x].GetPosition();

               m_vertices[index++].normal =
                  m_balls[(y + 1) * m_gridSize + x].GetNormal();

               // V5.
               m_vertices[index].pos =
                  m_balls[y * m_gridSize + x + 1].GetPosition();

               m_vertices[index++].normal =
                  m_balls[y * m_gridSize + x + 1].GetNormal();

               // V6.
               m_vertices[index].pos =
                  m_balls[(y + 1) * m_gridSize + x + 1].GetPosition();

               m_vertices[index++].normal =
                  m_balls[(y + 1) * m_gridSize + x + 1].GetNormal();
            }
      }
}


void Cloth::Shutdown()
{
   if(m_balls != 0)
      {
         delete[] m_balls;
         m_balls = 0;
      }

   if(m_springs != 0)
      {
         delete[] m_springs;
         m_springs = 0;
      }

   if(m_vertices != 0)
      {
         delete[] m_vertices;
         m_vertices = 0;
      }
}