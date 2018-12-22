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
#include<CollisionReport.h>
#include<Frustum.h>
#include<Plane.h>
#include<Vector3D.h>
#include<RigidBox.h>


DECLARE_ENGINE_NAMESPACE


class CollisionResponse
{
   public:
      CollisionResponse();
      virtual ~CollisionResponse();

      // Point Mass Collision as Spheres.
      bool ResponseToCollision(PointMass *p1, float radius1,
                               PointMass *p2, float radius2,
                               float e);

      bool ResponseToCollision(PointMass *p1, float radius1,
                               Frustum &frustum, float e);

      bool ResponseToCollision(PointMass *p1, float radius1,
                               Plane &plane, float e);


      // Polytope Collisions.
      bool ResponseToCollision(Polytope &p1, Polytope &p2);

      bool ResponseToCollision(Polytope &p1, Polytope &p2,
                               Vector3D &p1Vel, Vector3D &p2Vel,
                               float p1Max, float p2Max,
                               CollisionReport &report);

      bool ResponseToCollision(Polytope &p1, Plane &plane,
                               Vector3D &relVel, float max,
                               CollisionReport &report);

      // Rigid Box Collisions.
      bool ResponseToCollision(RigidBox &body, bool useAverage);
      bool ResponseToCollision(OBB &obb, bool useAverage);
      bool ResponseToCollision(Frustum &frustum, bool useAverage);
      bool ResponseToCollision(Plane &plane, bool useAverage);

   private:
      bool Intersect(Polytope &p1, Polytope &p2);

      bool Intersect(Polytope &p1, Polytope &p2,
                     Vector3D &relativeVel, float max,
                     CollisionReport &report);

      bool Intersect(Polytope &p1, Plane &plane,
                     Vector3D &relativeVel, float max,
                     CollisionReport &report);

      bool OverlapOnInterval(Polytope &p1, Polytope &p2,
                           Vector3D &axis);

      bool OverlapOnInterval(Polytope &p1, Polytope &p2,
							        Vector3D &p2Vel, Vector3D &axis,
							        float max, float &last,
							        CollisionReport &report);

      void CalcContactPoints(Polytope &p1, Polytope &p2,
                             Vector3D &p1Vel, Vector3D &p2Vel,
                             CollisionReport &report);

      void CalcContactPoints(Polytope &p1, Plane &plane,
                             Vector3D &p1Vel, CollisionReport &report);

      void GetEdgeContacts(Vector3D &pA0, Vector3D &pA1,
                           Vector3D &pB0, Vector3D &pB1,
                           CollisionReport &report);

      void GetPolygonContacts(Vector3D *clipVerts, int numClip,
                              Vector3D *polys, int numPolys,
                              CollisionReport &report);

      bool ClipPolygon(Vector3D *inClipVerts, int numInClipVerts,
                       Vector3D *inVerts, int numInVerts,
                       Vector3D *outVerts, int &numOutVerts);
};


END_ENGINE_NAMESPACE

#endif