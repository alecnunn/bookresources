/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
// FrameTimer.h - A simple frame-rate utility
//  Evan Hart - ATI Research - January 6,1999
//
//  This set of functions averages the frame
// times to keep a smoothed frame-rate.
//
///////////////////////////////////////////////

#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include <windows.h>

extern double ElapsedTime;

//number of frames used in smoothing
#define FRAME_HISTORY 10

//start keeping the counter
void StartFrameCounter();

//update the couter for this frame
void PingFrameCounter();

//get the current smoothed FPS
float GetFPS();

#endif

