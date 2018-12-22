/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
///////////////////////////////////////////////////////////////////////////////////////////
//
//	BoxSide.cpp
//
//	Author: Dan Ginsburg
//
//	Description:
//
//		Class for computing the side of a box and testing whether two
//		box sides are neighbors.  This class is used by the "Octree" class
//		for finding neighbors
//
#include <stdio.h>
#include <stdlib.h>
#include "BoxSide.h"

///
//	Macros
//
#define PT_MIN			0
#define PT_MAX			1
#define ABS(x)			((x)<0?(-x):(x))
#define APPROX_EQ(x,v)	((ABS((x)-(v)) <= 0.001f)?TRUE:FALSE)

///
//	Globals
//

///
//	Normals for each box side
//
const float SideNormals[6][3]= 
{
	// Top
	{  0.0,  1.0,  0.0 },
	// Bottom
	{  0.0, -1.0,  0.0 },
	// Right
	{  1.0,  0.0,  0.0 },
	// Left
	{ -1.0,  0.0,  0.0 },
	// Front
	{  0.0,  0.0,  1.0 },
	// Back
	{  0.0,  0.0, -1.0 }
};

///
//	Types of points for each vert of the box 
//	relative to the Min/Max
//
const int PtTypes[8][3]=
{
	{ PT_MIN, PT_MIN, PT_MIN },
	{ PT_MAX, PT_MIN, PT_MIN },
	{ PT_MAX, PT_MIN, PT_MAX },
	{ PT_MIN, PT_MIN, PT_MAX },
	{ PT_MIN, PT_MAX, PT_MIN },
	{ PT_MAX, PT_MAX, PT_MIN },
	{ PT_MAX, PT_MAX, PT_MAX },
	{ PT_MIN, PT_MAX, PT_MAX }
};

///
//	Indices of points for each box face
//
const int PtIndices[6][4]=
{
	// Top
	{ 7, 6, 5, 4 },
	// Bottom
	{ 0, 1, 2, 3 },
	// Right
	{ 5, 6, 2, 1 },
	// Left
	{ 0, 3, 7, 4 },
	// Front
	{ 7, 6, 2, 3 },
	// Back
	{ 0, 1, 5, 4 }
};

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Constructor/Destructor
//
//

///
//	BoxSide()
//
BoxSide::BoxSide()
{
}

///
//	~BoxSide()
//
BoxSide::~BoxSide()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Protected Members
//
//

///
//	GetBoxPt()
//
vector3 BoxSide::GetBoxPt(BBOX BBox, int Index)
{
	int		i;
	vector3 Pt;

	for(i= 0; i < 3; i++)
	{
		if(PtTypes[Index][i]== PT_MIN)
			Pt[i]= BBox.Min[i];
		else
			Pt[i]= BBox.Max[i];
	}

	return(Pt);
}

///
//	CoPlanar()
//	
//		Test to see if all the points lie within TestSide
//
int BoxSide::CoPlanar(BoxSide TestSide)
{
	int i;

	///
	//	Check to see if all points satisfy plane equation
	//	for TestSide: Ax + By + Cz + D = 0
	//
	for(i = 0; i < 4; i++)
	{
		float Result;

		Result= TestSide.Normal.x * Points[i].x + 
				TestSide.Normal.y * Points[i].y + 
				TestSide.Normal.z * Points[i].z +
				TestSide.D;

		if(!APPROX_EQ(Result, 0))
			return(FALSE);
	}

	return(TRUE);
}

///
//	PointsInside()
//
//		Check to see if all the points lie inside of TestSide
//
int BoxSide::PointsInside(BoxSide TestSide)
{
	int i,
		j;

	for(i= 0; i < 4; i++)
	{
		int LessEq= FALSE,
			GreaterEq= FALSE;

		for(j= 0; j < 4 || (!LessEq && !GreaterEq); j++)
		{
			if(Points[i].x >= TestSide.Points[j].x)
				if(Points[i].y >= TestSide.Points[j].y)
					if(Points[i].z >= TestSide.Points[j].z)
						GreaterEq= TRUE;

			if(Points[i].x <= TestSide.Points[j].x)
				if(Points[i].y <= TestSide.Points[j].y)
					if(Points[i].z <= TestSide.Points[j].z)
						LessEq= TRUE;
		}

		if(!LessEq || !GreaterEq)
			return(FALSE);
	}

	return(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Public Members
//
//

///
//	SetFromBBox()
//
//      Determine all properties of the box side given a BBox and
//      index (0 - 5) of the side
//
void BoxSide::SetFromBBox(BBOX BBox, int Side)
{
	if(Side >= 0 && Side <= 5)
	{
		int i;

		Normal.x= SideNormals[Side][0];
		Normal.y= SideNormals[Side][1];
		Normal.z= SideNormals[Side][2];

		for(i= 0; i < 4; i++)
			Points[i]= GetBoxPt(BBox, PtIndices[Side][i]);

		D= -(Points[0].x * Normal.x + Points[0].y * Normal.y +
			 Points[0].z * Normal.z);
		
		Size= (BBox.Max.x - BBox.Min.x);
		CurSide= Side;
	}
}

///
//	Neighbors()
//
//      Determine if a plane neighbors this plane
//
int BoxSide::Neighbors(BoxSide TestSide)
{
	///
	//	Quick rejection: planes don't face each other
	//
	if((CurSide % 2)== (TestSide.CurSide % 2))
		return(FALSE);

	///
	//	Check to see if planes are coplanar
	//
	if(CoPlanar(TestSide))
		if(PointsInside(TestSide))
			return(TRUE);

	return(FALSE);


}

///
//	GetSize()
//
float BoxSide::GetSize()
{
	return(Size);
}