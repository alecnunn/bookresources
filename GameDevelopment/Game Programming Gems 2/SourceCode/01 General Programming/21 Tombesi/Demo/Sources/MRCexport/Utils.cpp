/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22				Last Update:

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "utils.h"

// ============================================================================
// ============================================================================
// methods used to change coordinate system (then in OpenGL you'll get MAX's Front View)
void MAXtoGL(Matrix3 &tm)
{
	// swap 2nd and 3rd rows
	Point3 row = tm.GetRow(1);
	tm.SetRow(1, tm.GetRow(2));
	tm.SetRow(2, -row);

	// swap 2nd and 3rd columns
	Point4 column = tm.GetColumn(1);
	tm.SetColumn(1, tm.GetColumn(2));
	tm.SetColumn(2, -column);
}

// ============================================================================
void MAXtoGL(Point3 &pnt)
{
	float tmp = pnt.y;
	pnt.y = pnt.z;
	pnt.z = -tmp;
}

// ============================================================================
void MAXtoGL(const Matrix3 &tm, float glTM[16])
{
	Matrix3 maxTM = tm;
	MAXtoGL(maxTM);

	Point3 row = maxTM.GetRow(0);
	glTM[0] = row.x;
	glTM[1] = row.y;
	glTM[2] = row.z;
	glTM[3] = 0.f;

	row = maxTM.GetRow(1);
	glTM[4] = row.x;
	glTM[5] = row.y;
	glTM[6] = row.z;
	glTM[7] = 0.f;

	row = maxTM.GetRow(2);
	glTM[8] = row.x;
	glTM[9] = row.y;
	glTM[10] = row.z;
	glTM[11] = 0.f;

	row = maxTM.GetRow(3);
	glTM[12] = row.x;
	glTM[13] = row.y;
	glTM[14] = row.z;
	glTM[15] = 1.f;
}
// ============================================================================

// ============================================================================
int TicksToFPS(TimeValue t)
{
	return (TicksToSec(t) * GetFrameRate());
}
// ============================================================================