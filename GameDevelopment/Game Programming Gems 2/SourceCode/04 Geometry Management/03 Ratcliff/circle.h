/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#ifndef CIRCLE_H
#define CIRCLE_H

/***********************************************************************/
/** CIRCLE.H    : Demonstration application showing the sphere tree    */
/**               usage.  Contains a collection of circles in motion.  */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768


//======================================================================================
int DrawCircle(int locx, int locy, int radius, int color);


//======================================================================================

#include "spherepack.h"
#include "pool.h"

class CircleFactory;
class Circle;
class Frustum;

enum ViewState;

#define MAX_ATTRACTORS 16

class Attractor
{
public:
  Attractor(void);

  int GetX(void) const { return mX; };
  int GetY(void) const { return mY; };

  void AdjustXY(int x,int y,int &dx,int &dy);
  void ResetTest(void);

private:
  int mAcount;
  int mX;
  int mY;
  int mDx;
  int mDy;
};


class CircleItem; // forward reference this class

class Circle
{
public:

	Circle(int x,
	       int y,
	       int radius,
	       SpherePackFactory *factory,
	       Attractor *attractor);

 ~Circle(void);

	int											Process(SpherePackFactory *factory);
	int											Render(void);

  void SetCircleItem(CircleItem *item) { mItem = item; };
  CircleItem * GetCircleItem(void) { return mItem; };

  SpherePack * GetSpherePack(void) { return mSphere; };

  ViewState GetViewState(void) const { return mViewState; };
  void SetViewState(ViewState state) { mViewState = state; };

private:
  int                     mJumpCounter;
	int											mLocX;
	int											mLocY;
	int											mRadius;

	int											mDeltaX;
	int											mDeltaY;
  SpherePack             *mSphere;
  Attractor              *mAttractor;
  CircleItem             *mItem;
  ViewState               mViewState;
};


enum CircleState
{
  CS_SHOW_ALL,
  CS_SHOW_FRUSTUM,
  CS_SHOW_RAYTRACE,
  CS_SHOW_RANGE_TEST,
  CS_LAST
};

class CircleItem
{
public:
  CircleItem(void)
  {
    mNext = 0;
    mPrevious = 0;
  }

  void SetCircle(Circle *circle) { mCircle = circle; };
  Circle * GetCircle(void) { return mCircle; };

  CircleItem * GetNext(void) const { return mNext; };
  CircleItem * GetPrevious(void) const { return mPrevious; };

  void SetNext(CircleItem *item) { mNext = item; };
  void SetPrevious(CircleItem *item) { mPrevious = item; };

private:
  CircleItem *mNext;
  CircleItem *mPrevious;
  Circle     *mCircle;
};

//======================================================================================
class CircleFactory : public SpherePackCallback
{
public:
													CircleFactory(int circlecount);
													~CircleFactory(void);


  virtual void RayTraceCallback(const Vector3d<float> &p1,          // source pos of ray
                                const Vector3d<float> &dir,          // dest pos of ray
              								  float distance,
 								                const Vector3d<float> &sect,
                                SpherePack *sphere);


  virtual void VisibilityCallback(const Frustum &f,SpherePack *sphere,ViewState state);

  void RangeTestCallback(const Vector3d<float> &p,float distance,SpherePack *sphere,ViewState state);

  void SetState(CircleState s) { mState = s; };

	int											Process(void);
	int											Render(void);

private:

  void RayTest(int x1,int y1,int x2,int y2);
  void RangeTest(int x1,int y1,int distance);
  void FrustumTest(const Frustum &f);


  CircleState             mState;
  int                     mHitCount;

	Circle **mCircles;

  SpherePackFactory      *mFactory;
  Attractor               mAttractors[MAX_ATTRACTORS];
  Pool< CircleItem >      mVisible;
  int                     mCircleCount;
};



#endif // CIRCLE_H
