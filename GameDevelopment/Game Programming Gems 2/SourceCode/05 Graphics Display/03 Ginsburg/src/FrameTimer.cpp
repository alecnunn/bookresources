/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */

#include "FrameTimer.h"

//Global Data
LARGE_INTEGER last;
double TimerResolution;
double frameTimes[FRAME_HISTORY];
int frameLocation;
unsigned int framesProcessed;
double ElapsedTime;

//Performance Counter based timing function
void StartTimer(){
  LARGE_INTEGER num;
  ElapsedTime=0.0;

  QueryPerformanceFrequency(&num);
  TimerResolution=(double)num.HighPart*4294967296.0;
  TimerResolution+=(double)num.LowPart;
  TimerResolution=1.0/TimerResolution;
  QueryPerformanceCounter(&last);
}

//Performance Counter based timing function
double PingTimer(){
  LARGE_INTEGER t;
  double elapsed;

  QueryPerformanceCounter(&t);

  elapsed=((double)t.HighPart-(double)last.HighPart)*4294967296.0;
  elapsed+=(double)t.LowPart-(double)last.LowPart;
  elapsed*=TimerResolution;

  last.HighPart=t.HighPart;
  last.LowPart=t.LowPart;

  return elapsed;
}

void StartFrameCounter(){
  StartTimer();
  frameLocation=0;
  framesProcessed=0;
}

void PingFrameCounter(){
  frameTimes[frameLocation]=PingTimer();
  ElapsedTime+=frameTimes[frameLocation];
  frameLocation=(frameLocation+1)%FRAME_HISTORY;
  framesProcessed++;
}

float GetFPS(){
  int ii,top;
  double total=0.0f;

  top=(FRAME_HISTORY>framesProcessed)?framesProcessed:FRAME_HISTORY;
  for(ii=0;ii<top;ii++)
    total+=frameTimes[ii];

  total/=(float)FRAME_HISTORY;

  total=1.0f/total;
  return (float)total;
}
