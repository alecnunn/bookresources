/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_OCTREE_H_
#define _BB_OCTREE_H_


#include<NameSpace.h>
#include<Defines.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class OctreeNode
{
   public:
      OctreeNode();
      ~OctreeNode();

      void CreateNode(Vector3D center, float bbSize,
                      int currentLevel, int maxLevel);

      bool isLeaf() { return m_isLeaf; }
      Vector3D GetCenter() { return m_center; }
      float GetSize() { return m_bbSize; }
      OctreeNode **GetSubNodes() { return m_childNodes; }

   private:
      bool m_isLeaf;
      Vector3D m_center;
      float m_bbSize;

      OctreeNode *m_childNodes[8];
};


END_ENGINE_NAMESPACE


#endif