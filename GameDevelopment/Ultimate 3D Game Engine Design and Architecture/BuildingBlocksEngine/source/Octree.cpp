/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Octree.h>


DECLARE_ENGINE_NAMESPACE


OctreeNode::OctreeNode()
{
   m_isLeaf = 0;
   m_bbSize = 0;

   for(int i = 0; i < 8; i++)
      m_childNodes[i] = 0;
}


OctreeNode::~OctreeNode()
{
   for(int i = 0; i < 8; i++)
      {
         if(m_childNodes[i] != 0)
            {
               delete m_childNodes[i];
               m_childNodes[i] = 0;
            }
      }
}


void OctreeNode::CreateNode(Vector3D center, float bbSize, int currentLevel, int maxLevel)
{
   m_bbSize = bbSize;
   m_center = center;

   if(currentLevel >= maxLevel)
      {
         m_isLeaf = true;
         return;
      }

   float newSize = bbSize / 4;
   m_isLeaf = false;

   Vector3D topFrontLeft (-newSize, newSize,  newSize);
   Vector3D topFrontRight( newSize, newSize,  newSize);
   Vector3D topBackLeft  (-newSize, newSize, -newSize);
   Vector3D topBackRight (newSize, newSize,  -newSize);
   Vector3D bottomFrontLeft (-newSize, -newSize,  newSize);
   Vector3D bottomFrontRight( newSize, -newSize,  newSize);
   Vector3D bottomBackLeft  (-newSize, -newSize, -newSize);
   Vector3D bottomBackRight ( newSize, -newSize, -newSize);

   m_childNodes[TOP_FRONT_LEFT] = new OctreeNode();
   m_childNodes[TOP_FRONT_RIGHT] = new OctreeNode();
   m_childNodes[TOP_BACK_LEFT] = new OctreeNode();
   m_childNodes[TOP_BACK_RIGHT] = new OctreeNode();
   m_childNodes[BOTTOM_FRONT_LEFT] = new OctreeNode();
   m_childNodes[BOTTOM_FRONT_RIGHT] = new OctreeNode();
   m_childNodes[BOTTOM_BACK_LEFT] = new OctreeNode();
   m_childNodes[BOTTOM_BACK_RIGHT] = new OctreeNode();

   currentLevel++;
   m_childNodes[TOP_FRONT_LEFT]->CreateNode((m_center +
      topFrontLeft), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[TOP_FRONT_RIGHT]->CreateNode((m_center +
      topFrontRight), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[TOP_BACK_LEFT]->CreateNode((m_center +
      topBackLeft), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[TOP_BACK_RIGHT]->CreateNode((m_center +
      topBackRight), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[BOTTOM_FRONT_LEFT]->CreateNode((m_center +
      bottomFrontLeft), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[BOTTOM_FRONT_RIGHT]->CreateNode((m_center +
      bottomFrontRight), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[BOTTOM_BACK_LEFT]->CreateNode((m_center +
      bottomBackLeft), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;

   currentLevel++;
   m_childNodes[BOTTOM_BACK_RIGHT]->CreateNode((m_center +
      bottomBackRight), bbSize / 2, currentLevel, maxLevel);
   currentLevel--;
}


END_ENGINE_NAMESPACE