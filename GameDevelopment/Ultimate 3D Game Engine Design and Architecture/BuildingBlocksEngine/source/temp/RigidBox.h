/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RIGID_BOX_H_
#define _BB_RIGID_BOX_H_


#include<NameSpace.h>
#include<Defines.h>
#include<RigidBody.h>
#include<OBB.h>


DECLARE_ENGINE_NAMESPACE


class RigidBox : public RigidBody
{
   public:
      RigidBox();
      RigidBox(scalar width, scalar height, scalar depth);
      virtual ~RigidBox();

      scalar GetWidth() { return m_width; }
      scalar GetHeight() { return m_height; }
      scalar GeDepth() { return m_depth; }

      void SetDimensions(scalar width, scalar height, scalar depth);

      OBB GetOBB() { return m_obb; }
      OBB GetOBBWorldSpace();

      scalar GetDragArea();
      scalar GetDragCoefficient();
      scalar GetLiftCoefficient();
      scalar GetVolumeUnderHeight(scalar height);

   protected:
      void CalcInertiaTensor();

   protected:
      OBB m_obb;
      scalar m_width;
      scalar m_height;
      scalar m_depth;
};


END_ENGINE_NAMESPACE


#endif