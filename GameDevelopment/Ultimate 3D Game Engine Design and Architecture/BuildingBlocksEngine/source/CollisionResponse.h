/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_COLLISION_RESPONSE_H_
#define _BB_COLLISION_RESPONSE_H_


#include<NameSpace.h>
#include<PointMass.h>
#include<Polytope.h>
#include<Frustum.h>
#include<Plane.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class CollisionResponse
{
   public:
      CollisionResponse() {}
      ~CollisionResponse() {}

      // Point Mass Collision as Spheres.
      bool ResponseToCollision(PointMass *p1, float radius1,
                               PointMass *p2, float radius2,
                               float e);

      bool ResponseToCollision(PointMass *p1, float radius1,
                               Frustum &frustum, float e);

      bool ResponseToCollision(PointMass *p1, float radius1,
                               Plane &plane, float e);
};


END_ENGINE_NAMESPACE

#endif