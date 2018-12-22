/*
   Cloth Simulation Demo - Chapter 6
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _CLOTH_H_
#define _CLOTH_H_


#include<Spring.h>
#include<Vector3D.h>
#include<VertexDescriptor.h>


struct stClothVertex
{
   bbe::Vector3D pos, normal;
};


class Cloth
{
   public:
      Cloth()
      {
         m_balls = 0;
         m_constant = 0;
         m_totalBalls = 0;
         m_dampFactor = 0;
         m_totalSprings = 0;
         m_springs = 0;
         m_vertices = 0;
         m_totalVertices = 0;

         m_descriptor.AddElement(BB_ELEMENT_TYPE_VERTEX_3F);
         m_descriptor.AddElement(BB_ELEMENT_TYPE_NORMAL_3F);
      }

      ~Cloth()
      {

      }

      bool Initialize(int gridSize, float mass, float springConst,
                      float springLen, float damp);

      void RecalculateMesh();
      void Shutdown();

      void UpdateSprings()
      {
         for(int j = 0; j < m_totalSprings; ++j)
            {
               m_springs[j].Update();
            }
      }

      stClothVertex *GetVertices()
      {
         return m_vertices;
      }

      int GetVertexSizeInBytes()
      {
         return GetVertexStride() * m_totalVertices;
      }

      int GetVertexStride()
      {
         return sizeof(stClothVertex);
      }

      bbe::VertexDescriptor GetDescriptor()
      {
         return m_descriptor;
      }

      int GetGridSize()
      {
         return m_gridSize;
      }

      bbe::SpringMass *GetMasses()
      {
         return m_balls;
      }

      int GetTotalMasses()
      {
         return m_totalBalls;
      }

      bbe::Spring *GetSprings()
      {
         return m_springs;
      }

      int GetTotalSprings()
      {
         return m_totalSprings;
      }

   private:
      int m_gridSize;

      stClothVertex *m_vertices;
      int m_totalVertices;

      bbe::VertexDescriptor m_descriptor;

      bbe::SpringMass *m_balls;
      int m_totalBalls;

      bbe::Spring *m_springs;
      int m_totalSprings;

      float m_constant;
      float m_dampFactor;
};

#endif