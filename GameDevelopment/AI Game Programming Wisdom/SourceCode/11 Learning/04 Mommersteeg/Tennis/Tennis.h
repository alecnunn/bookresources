//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Tennis.h
//----------------------------------------------------------------------------------------------

#ifndef __TENNIS_H
#define __TENNIS_H

//----------------------------------------------------------------------------------------------
// Global definitions
//----------------------------------------------------------------------------------------------

#define NAME			"Tennis Demo"
#define TITLE			"Tennis Demo"
#define SCREEN_WIDTH    800  // size of screen
#define SCREEN_HEIGHT   600
#define FIELD_WIDTH		400
#define FIELD_HEIGHT	480
#define CROSS_WIDTH		32
#define CROSS_HEIGHT	32
#define SCREEN_BPP      16   // bits per pixel
#define OUTBOX_WIDTH	45
#define DEFAULT_SPEED	85

extern DWORD			g_FrameRate;
extern DWORD			g_TargetSpeed;

#define NormSpeed(v)	(g_FrameRate>1?(((float)v*(float)g_TargetSpeed)/(float)g_FrameRate):v)

//----------------------------------------------------------------------------------------------
#endif // __TENNIS_H
