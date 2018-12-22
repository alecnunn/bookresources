/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _SPLINE_H
#define _SPLINE_H
#include "object.h"
#include <cstring>
#include <cstdlib>

#define MAX_CONTROL_PNT 100
#define MAX_CURVE_PNT 1000

#define B_SPLINE(u, u_2, u_3, cntrl0, cntrl1, cntrl2, cntrl3) \
	( \
		( \
			(-1*u_3 + 3*u_2 - 3*u + 1) * (cntrl0) + \
			( 3*u_3 - 6*u_2 + 0*u + 4) * (cntrl1) + \
			(-3*u_3 + 3*u_2 + 3*u + 1) * (cntrl2) + \
			( 1*u_3 + 0*u_2 + 0*u + 0) * (cntrl3)   \
		) / 6 \
	) 

#define B_TANGENT(u, u_2, cntrl0, cntrl1, cntrl2, cntrl3) \
	( \
		( \
			(-1*u_2 + 2*u - 1) * (cntrl0) + \
			( 3*u_2 - 4*u + 0) * (cntrl1) + \
			(-3*u_2 + 2*u + 1) * (cntrl2) + \
			( 1*u_2 + 0*u + 0) * (cntrl3)   \
		) / 2 \
	) 

#define CATMULL_ROM_SPLINE(u, u_2, u_3, cntrl0, cntrl1, cntrl2, cntrl3) \
	( \
		( \
			(-1*u_3 + 2*u_2 - 1*u + 0) * (cntrl0) + \
			( 3*u_3 - 5*u_2 + 0*u + 2) * (cntrl1) + \
			(-3*u_3 + 4*u_2 + 1*u + 0) * (cntrl2) + \
			( 1*u_3 - 1*u_2 + 0*u + 0) * (cntrl3)   \
		) / 2 \
	) 

#define CATMULL_ROM_TANGENT(u, u_2, cntrl0, cntrl1, cntrl2, cntrl3) \
	( \
		( \
			(-3*u_2 +  4*u - 1) * (cntrl0) + \
			( 9*u_2 - 10*u + 0) * (cntrl1) + \
			(-9*u_2 +  8*u + 1) * (cntrl2) + \
			( 3*u_2 -  2*u + 0) * (cntrl3)   \
		) / 2 \
	) 


#define CONTROL_NORMAL_ROT 1
#define CONTROL_NORMAL_GIVEN 2
#define CONTROL_PHYSICS_NO_GRAVITY 1
#define CONTROL_PHYSICS_BREAK 2
#define DISPLAY_MODE_OFF 0
#define DISPLAY_MODE_POINTS 1
#define DISPLAY_MODE_LINES 2

typedef struct SplineControlData_Str {
	vector3 pos;
	vector3 nrm;
	int physicsFlag;
	float physicsValue;
	int nrmFlag;
	float nrmValue;
} SplineControlData;

typedef struct SplineCurveData_Str {
	vector3 pos;
	vector3 tan;
	vector3 nrm;
	vector3 sid;
	float distance;
} SplineCurveData;

//---------------------------------------------------------------------------
// The Spline Curve Class
//---------------------------------------------------------------------------
class Spline : public Object {
public:
	Spline(char * fileName);
	Spline(SplineControlData * data, int controlCntIn, int curveSubD);
	~Spline() {free(controlData); free(curveData);}

public:
	//-------------------------------------------------
	// Control Points
	SplineControlData * controlData;

	// Number of control points
	int controlCnt;

	// Current Control Pnt
	int controlIndx;

	// Defualt Normal Vector
	vector3 defualtNrm;

	//-------------------------------------------------
	// The Curve Points
	SplineCurveData * curveData;

	// Subdivision between each control point 
	int curveSubD;

	// The number of Points on the final curve
	int curveCnt;

	// The Current Control Pnt
	int curveIndx;

	//-------------------------------------------------
	// Curve Parameters
	int curveType;
	float shortestDistance;

	//-------------------------------------------------
	// Display Parameters
	int displayControlMode;
	int displayCurveMode;
	int displayCurveModulo;
	int displayOrientMode;
	int displayOrientModulo;
	float displayOrientLen;
	COLOR controlClr;
	COLOR curveClr;
	COLOR tanClr;
	COLOR nrmClr;
	COLOR sidClr;

public:
	void ParseDefinitionFile(char * fileName);
	void DrawPrimitives();
	void DrawLines();
	int GetIndexAtDistanceFromIndex(float distance, int startIndex);
	int GetIndexAtDistance(float distance);
	void Build();
	void Build(vector3 defualtNrm);
};

#endif