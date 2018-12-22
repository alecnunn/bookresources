/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "frustum.h"

void Frustum::Set(int x1,int y1,int x2,int y2)
{
  mX1 = x1;
  mY1 = y1;
  mX2 = x2;
  mY2 = y2;
}


ViewState Frustum::ViewVolumeTest(const Vector3d<float> &center,float radius) const
{
  int acode = 0xFFFFFF;
  int ocode = 0;

  int x = int(center.x);
  int y = int(center.y);
  int r = int(radius);

  ViewCode(x-r,y-r,acode,ocode);
  ViewCode(x+r,y-r,acode,ocode);
  ViewCode(x+r,y+r,acode,ocode);
  ViewCode(x-r,y+r,acode,ocode);

  if ( acode ) return VS_OUTSIDE;
  if ( ocode ) return VS_PARTIAL;

  return VS_INSIDE;
}


void Frustum::ViewCode(int x,int y,int &acode,int &ocode) const
{
  int code = 0;
  if ( x < mX1 ) code|=CS_LEFT;
  if ( x > mX2 ) code|=CS_RIGHT;
  if ( y < mY1 ) code|=CS_TOP;
  if ( y > mY2 ) code|=CS_BOTTOM;
  ocode|=code;
  acode&=code;
}
