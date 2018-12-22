/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
#ifndef _BOXSIDE_H_
#define _BOXSIDE_H_

///
//	Dependent Includes
//
#include "TriBox.h"

///
//	Classes
//
class BoxSide
{
	protected:
		vector3 Normal;
		vector3 Points[4];
		float   D;
		float	Size;
		int		CurSide;

		vector3 GetBoxPt(BBOX BBox, int Index);
		int     CoPlanar(BoxSide TestSide);
		int		PointsInside(BoxSide TestSide);

	public:
		BoxSide();
		~BoxSide();

		void  SetFromBBox(BBOX BBox, int Side);
		int   Neighbors(BoxSide TestSide);
		float GetSize();
		
};

#endif