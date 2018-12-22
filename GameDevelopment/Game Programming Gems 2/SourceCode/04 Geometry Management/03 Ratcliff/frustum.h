/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#ifndef FRUSTUM_H

#define FRUSTUM_H

/***********************************************************************/
/** FRUSTUM.H   : Represents a clipping frustum.                       */
/**               You should replace this with your own more robust    */
/**               view frustum clipper.                                */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/

enum ViewState
{
  VS_INSIDE,   // completely inside the frustum.
  VS_PARTIAL,  // partially inside and partially outside the frustum.
  VS_OUTSIDE   // completely outside the frustum
};

#include "vector.h"

enum CohenSutherland
{
  CS_LEFT   = (1<<0),
  CS_RIGHT  = (1<<1),
  CS_TOP    = (1<<2),
  CS_BOTTOM = (1<<3)
};

class Frustum
{
public:

  void Set(int x1,int y1,int x2,int y2);

  ViewState ViewVolumeTest(const Vector3d<float> &center,float radius) const;

private:

  void ViewCode(int x,int y,int &acode,int &ocode) const;

  int mX1;
  int mX2;
  int mY1;
  int mY2;
};

#endif
