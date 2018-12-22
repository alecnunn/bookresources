/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#include <stdlib.h>

#include "circle.h"
#include "spherepack.h"
#include "frustum.h"

#define JUMP_TIME 128

#define FIXED 16
#define SWID (SCREEN_WIDTH*FIXED)
#define SHIT (SCREEN_HEIGHT*FIXED)

static int gCenterX=SWID/2;
static int gCenterY=SHIT/2;

int PrintText(int x, int y, int color, char* output, ...);
int DrawLine(int x1, int y1, int x2, int y2, int color);
int DrawCircle(int locx, int locy, int radius, int color);

Attractor::Attractor(void)
{
  mX = rand()%SWID;
  mY = rand()%SHIT;

  mDx = (rand()&7)-4;
  mDy = (rand()&7)-4;


  mAcount = (rand()%127)+16;
}

void Attractor::ResetTest(void)
{
  mAcount--;

  if ( mAcount < 0 )
  {
    mX = rand()%SWID;
    mY = rand()%SHIT;
    mDx = (rand()&7)-4;
    mDy = (rand()&7)-4;
    mAcount = (rand()%127)+16;
  }

  mX+=mDx;
  mY+=mDy;

	if ( mX > SWID)
	{
		mDx*= -1;
	}

	if ( mX < 0)
	{
		mDx *= -1;
	}

	if ( mY > SHIT)
	{
		mDy *= -1;
	}

	if (mY < 0)
	{
		mDy *= -1;
	}


}

int Cap(int v)
{
  if ( v > 0 )
  {
    if ( v > 32 ) v = 32;
  }
  else
  {
    if ( v < -32 ) v = -32;
  }
  return v;
}

void Attractor::AdjustXY(int x,int y,int &dx,int &dy)
{
  if ( mX < x ) dx--;
  if ( mX > x ) dx++;
  if ( mY < y ) dy--;
  if ( mY > x ) dy++;

  dx = Cap(dx);
  dy = Cap(dy);
}

//======================================================================================
//======================================================================================
Circle::Circle(int x,int y,int radius,SpherePackFactory *factory,Attractor *attractor)
{
  mItem = 0;
	mLocX			= x;
	mLocY			= y;
	mRadius		= radius;
  mAttractor = attractor;

  mJumpCounter = (rand()%JUMP_TIME)+JUMP_TIME/4;

  if ( mAttractor )
  {
    mDeltaX = (rand()&7)-3;
    mDeltaY = (rand()&7)-3;
  }
  else
  {
    mDeltaX = 0;
    mDeltaY = 0;
  }


  Vector3d<float> pos(x,y,0);

  pos.x = float(mLocX)*(1.0f/FIXED);
  pos.y = float(mLocY)*(1.0f/FIXED);
  pos.z = 0;


  mSphere   = factory->AddSphere(pos,mRadius,this);
}


//======================================================================================
Circle::~Circle(void)
{
}


//======================================================================================
int Circle::Process(SpherePackFactory *factory)
{
  if ( mAttractor )
  {

    mAttractor->AdjustXY(mLocX,mLocY,mDeltaX,mDeltaY);

 	  if ((mLocX + mDeltaX) > SWID)
    {
  		mDeltaX *= -1;
    }
	  if ((mLocX + mDeltaX) < 0)
    {
  		mDeltaX *= -1;
    }
	  if ((mLocY + mDeltaY) > SHIT)
    {
  		mDeltaY *= -1;
    }
	  if ((mLocY + mDeltaY) < 0)
    {
  		mDeltaY *= -1;
    }


    mLocX+=mDeltaX;
    mLocY+=mDeltaY;


    Vector3d<float> pos;

    pos.x = float(mLocX)*(1.0f/FIXED);
    pos.y = float(mLocY)*(1.0f/FIXED);
    pos.z = 0;

    mSphere->NewPos(pos);

  }


  mJumpCounter--;

  if ( mJumpCounter <= 0 )
  {
    Vector3d<float> pos;

    if ( !mAttractor )
    {
      int wx = SWID/4;
      int wy = SHIT/4;
      mLocX = (rand()%wx)-(wx/2)+gCenterX;
      mLocY = (rand()%wy)-(wy/2)+gCenterY;

      if ( mLocX < 0 ) mLocX = 0;
      if ( mLocX > SWID ) mLocX = SWID;
      if ( mLocY < 0 ) mLocY = 0;
      if ( mLocY > SHIT ) mLocY = SHIT;

    }

    pos.x = float(mLocX)*(1.0f/FIXED);
    pos.y = float(mLocY)*(1.0f/FIXED);
    pos.z = 0;

    mJumpCounter = rand()%JUMP_TIME;

    mDeltaX = (rand()&31)-16;
    mDeltaY = (rand()&31)-16;
    mSphere->NewPos(pos);
  }

	return 0;
}


//======================================================================================
int Circle::Render(void)
{
	DrawCircle(mLocX, mLocY, mRadius, 0x00ffffff);

	return 0;
}




//======================================================================================
//======================================================================================
CircleFactory::CircleFactory(int circlecount)
{
  mState = CS_SHOW_ALL;
  mCircleCount = circlecount;
  mCircles = new Circle *[mCircleCount];

  mVisible.Set(mCircleCount); // visiblelist

  mFactory = new SpherePackFactory(mCircleCount,256,64,8);

	for (int i = 0; i != mCircleCount; i++)
	{

    int a = rand()%MAX_ATTRACTORS;

    Attractor *at = &mAttractors[a];
    if ( (i&3) == 0 ) at = 0; // 1 of 4 are non moving

		mCircles[i] = new Circle(rand()%SWID,
		                         rand()%SHIT,
		                         (rand()%4) + 1,
		                         mFactory,
                             at);
	}
}


//======================================================================================
CircleFactory::~CircleFactory(void)
{
	for (int i = 0; i != mCircleCount; i++)
	{
		delete(mCircles[i]);
	}
  delete mFactory;
}


//======================================================================================
int CircleFactory::Process(void)
{
  if ( 1 )
  {
    static gCount = 0;
    gCount++;
    if ( gCount == 128 )
    {
      gCount = 0;
      gCenterX = rand()%SWID;
      gCenterY = rand()%SHIT;
    }
  }

  if ( 1 )
  {
    for (int i=0; i<MAX_ATTRACTORS; i++) mAttractors[i].ResetTest();
  }

  // Perform 'physics' on all circles.
  for (int i = 0; i != mCircleCount; i++)
  {
 	  mCircles[i]->Process(mFactory);
  }

  CircleState gLastState = CS_SHOW_ALL;

  if ( mState != gLastState )
  {
    // Frustum culling in this example presumes frame to frame coherency.
    // If we change modes we need to reset the visibility status of the
    // sphere tree to a null state.
    gLastState = mState;
    mFactory->Reset(); // reset visibility state
  }

  mFactory->Process(); // controls how much CPU to give up to reintegration and recomputation fifos

	return 0;
}


//======================================================================================
int CircleFactory::Render(void)
{
  static int fcount=0;

  fcount++;

  unsigned int color1=0xFFFFFF;
  unsigned int color2=0xFFFFFF;
  unsigned int color3=0xFFFFFF;
  unsigned int color4=0xFFFFFF;

  switch ( mState )
  {
    case CS_SHOW_ALL:
      mFactory->Render();
      color1 = 0x00FFFF;
      break;
    case CS_SHOW_FRUSTUM:
      color2 = 0x00FFFF;
      if ( 1 )
      {
        PrintText(0,12,0xFFFFFF,"Show Spheres Intersecting A Frustum");

        static int x1 = 300;
        static int y1 = 250;
        static int x2 = 500;
        static int y2 = 400;


        if ( (fcount&255) == 0 )
        {
          x1 = rand()%(SCREEN_WIDTH-200);
          y1 = rand()%(SCREEN_HEIGHT-200);

          int wid = rand()%((SCREEN_WIDTH)/2-x1)+32;
          int hit = rand()%((SCREEN_HEIGHT)/2-y1)+32;

          x2 = x1+wid;
          y2 = y1+hit;
          if ( x2 > (SCREEN_WIDTH-20) ) x2 = SCREEN_WIDTH-20;
          if ( y2 > (SCREEN_HEIGHT-20) ) y2 = SCREEN_HEIGHT-20;

        }

        Frustum f;

        f.Set(x1,y1,x2,y2);

        DrawLine(x1,y1,x2,y1,0xFFFFFF);
        DrawLine(x1,y2,x2,y2,0xFFFFFF);
        DrawLine(x1,y1,x1,y2,0xFFFFFF);
        DrawLine(x2,y1,x2,y2,0xFFFFFF);

        FrustumTest(f);

        // now render everybody in the visible list.
        int count = mVisible.Begin();

        for (int i=0; i<count; i++)
        {
          CircleItem *item = mVisible.GetNext();
          if ( !item ) break;
          Circle *circle = item->GetCircle();
          SpherePack *sphere = circle->GetSpherePack();
          Vector3d<float> pos = sphere->GetCenter();
          int color = 0x00FF00;
          if ( circle->GetViewState() == VS_PARTIAL ) color = 0x00FFFF;
          DrawCircle( int(pos.x), int(pos.y), int( sphere->GetRadius() ), color);
        }

        PrintText(0,24,0xFFFFFF,"%d Spheres Intersected Frustum.",count);

      }
      break;
    case CS_SHOW_RAYTRACE:
      color3 = 0x00FFFF;
      if ( 1 )
      {
        PrintText(0,12,0xFFFFFF,"Show Spheres Intersecting A Line Segment");

        static int x1 = 300;
        static int y1 = 250;
        static int x2 = 500;
        static int y2 = 400;

        if ( (fcount&255) == 0 )
        {
          x1 = rand()%SCREEN_WIDTH;
          y1 = rand()%SCREEN_HEIGHT;
          x2 = rand()%SCREEN_WIDTH;
          y2 = rand()%SCREEN_HEIGHT;
        }

        RayTest(x1,y1,x2,y2);
      }
      break;
    case CS_SHOW_RANGE_TEST:
      color4 = 0x00FFFF;
      if ( 1 )
      {
        static int dx = 0;
        static int dy = 0;
        static int radius = 128;

        if ( (fcount&255) == 0 )
        {
          dx = (rand()&255)-128;
          dy = (rand()&255)-128;
          radius = (rand()&127)+32;
        }

        PrintText(0,12,0xFFFFFF,"Show Spheres Within A Certain Range");

        RangeTest(SCREEN_WIDTH/2+dx,SCREEN_HEIGHT/2+dy,radius);

      }
      break;
  }


  PrintText(900, 0,color1,"(A) Show All");
  PrintText(900,20,color2,"(F) Frustum Culling");
  PrintText(900,40,color3,"(T) Ray Tracing");
  PrintText(900,60,color4,"(R) Range Testing");
  PrintText(900,80,0xFFFFFF,"(SPACE) Pause");
  PrintText(900,100,0xFFFFFF, "(ENTER) UnPause");


	return 0;
}


void CircleFactory::RayTest(int x1,int y1,int x2,int y2)
{
  Vector3d<float> p1(x1,y1,0);
  Vector3d<float> p2(x2,y2,0);

  DrawLine(x1,y1,x2,y2,0xFFFFFF);

  mHitCount = 0;

  mFactory->RayTrace(p1,p2,this);

  PrintText(0,24,0xFFFFFF,"%d Spheres Intersected Ray.",mHitCount);

}


void CircleFactory::RayTraceCallback(const Vector3d<float> &p1,          // source pos of ray
                                     const Vector3d<float> &dir,          // direction of ray (normalized)
									 float distance,                     //length of ray
									 const Vector3d<float> &sect,
                                     SpherePack *sphere)
{
  const Vector3d<float> pos = sphere->GetPos();

  float radius = sphere->GetRadius();

  DrawCircle( int(pos.x), int(pos.y), int(radius), 0x00FF00);

  int sx = int(sect.x);
  int sy = int(sect.y);

  DrawLine(sx-10,sy,sx+10,sy,0x0000FF);
  DrawLine(sx,sy-10,sx,sy+10,0x0000FF);

  mHitCount++;

}


void CircleFactory::RangeTest(int x1,int y1,int distance)
{
  DrawCircle(x1,y1,distance,  0x00FF);
  DrawCircle(x1,y1,distance+2,0x00FF);
  DrawCircle(x1,y1,distance+4,0x00FF);
  DrawCircle(x1,y1,distance+6,0x00FF);

  mHitCount = 0;

  Vector3d<float> p(x1,y1,0);

  mFactory->RangeTest(p,distance,this);

  PrintText(0,24,0xFFFFFF,"%d Spheres Inside Range.",mHitCount);

}

void CircleFactory::RangeTestCallback(const Vector3d<float> &p,float distance,SpherePack *sphere,ViewState state)
{
  const Vector3d<float> pos = sphere->GetPos();

  float radius = sphere->GetRadius();

  DrawCircle( int(pos.x), int(pos.y), int(radius), 0x00FF00);


  mHitCount++;
}


void CircleFactory::FrustumTest(const Frustum &f)
{
  mFactory->FrustumTest(f,this);
}


void CircleFactory::VisibilityCallback(const Frustum &f,SpherePack *sphere,ViewState state)
{
  Circle *circle = (Circle *) sphere->GetUserData();

  if ( state == VS_OUTSIDE ) // it is not visible!
  {
    CircleItem *item = circle->GetCircleItem();

    if ( item )
    {
      circle->SetCircleItem(0);
      mVisible.Release(item);
    }
  }
  else
  {
    circle->SetViewState(state);

    CircleItem *item = circle->GetCircleItem();

    if ( !item )
    {

      item = mVisible.GetFreeLink();

      if ( item )
      {
        circle->SetCircleItem(item);
        item->SetCircle(circle);
      }

    }
  }
}

