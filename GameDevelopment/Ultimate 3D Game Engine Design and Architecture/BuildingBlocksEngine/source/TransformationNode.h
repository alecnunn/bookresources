/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_TRANSFORMATION_NODE_H_
#define _BB_TRANSFORMATION_NODE_H_

#include<node.h>
#include<Vector3D.h>
#include<Quaternion.h>
#include<RenderInterface.h>


DECLARE_ENGINE_NAMESPACE


class TransformationNode : public Node
{
   public:
      TransformationNode(const Vector3D &position, const Quaternion &q,
                         RenderInterface *renderer)
      {
         m_renderer = renderer;
         m_position = position;
         m_quaternion = q;
      }

      ~TransformationNode()
      {

      }

      void Process()
      {
         if(m_renderer != NULL)
            {
               m_renderer->PushMatrix();

               float matrix[16] = { 0 };

               m_quaternion.CreateMatrix(matrix);
               matrix[12] = m_position.x;
               matrix[13] = m_position.y;
               matrix[14] = m_position.z;

               m_renderer->MultiplyToCurrentMatrix(matrix);
            }

         if(m_child != NULL)
            m_child->Process();

         if(m_renderer != NULL)
            m_renderer->PopMatrix();

         if(m_next != NULL)
            m_next->Process();
      }

   protected:
      Vector3D m_position;
      Quaternion m_quaternion;

      RenderInterface *m_renderer;
};


END_ENGINE_NAMESPACE


#endif