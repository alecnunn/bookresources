/**********************************************************************
 	MODULE: MRCexport
	
	DESCRIPTION: This module is the plugin for MAX 3 to export in MRC format
	
	Version:		1.22				Last Update:

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once

#include "std.h"

void MAXtoGL(Matrix3 &tm);

void MAXtoGL(Point3 &pnt);

void MAXtoGL(const Matrix3 &tm, float glTM[16]);

void EulerToQuat(const Point3 &euler, Quat &quat);

int TicksToFPS(TimeValue t);

#define TicksToMilliSec(ticks) (((float)(ticks)/(float)TIME_TICKSPERSEC) * 1000.f)
#define MilliSecToTicks(msec)  (((float)msec / 1000.f) * TIME_TICKSPERSEC)
