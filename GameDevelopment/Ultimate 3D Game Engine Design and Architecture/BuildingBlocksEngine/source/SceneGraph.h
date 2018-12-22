/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SCENE_GRAPH_H_
#define _BB_SCENE_GRAPH_H_

#include<node.h>


DECLARE_ENGINE_NAMESPACE


class SceneGraph
{
   public:
      SceneGraph()
      {
         m_root = NULL;
      }

      ~SceneGraph()
      {
         Release();
      }


      void Release()
      {
         if(m_root != NULL)
            {
               delete m_root;
               m_root = NULL;
            }
      }


      void AddNode(Node *node)
      {
         if(m_root == NULL)
            m_root = new Node;

         m_root->AddChild(node);
      }


      void Process()
      {
         if(m_root != NULL)
            m_root->Process();
      }


   private:
      Node *m_root;
};


END_ENGINE_NAMESPACE


#endif