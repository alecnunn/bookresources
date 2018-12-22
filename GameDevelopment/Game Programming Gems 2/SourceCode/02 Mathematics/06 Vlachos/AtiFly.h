/* Copyright (C) Alex Vlachos and John Isidoro, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex Vlachos and John Isidoro, 2001"
 */
/******************************************************************************
 *  AtiFly.h -- Automated fly through code
 ******************************************************************************
 *  Written by Alex Vlachos (Alex@Vlachos.com)
 ******************************************************************************/

#ifndef __AtiFly_h
#define __AtiFly_h

// INCLUDES ===================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Types.h"
#include "AtiQuaternion.h"

//=============================================================================
typedef union
{
   //Point in 3D space
   struct { float32 x, y, z; };
   struct { float32 v[3]; };
} AtiVertex3, AtiVector3;

#ifdef ATI_OPENGL_API
 //============================================================================
 typedef union
 {
   struct { uint8 r, g, b, a; };
   struct { uint32 color; };
 } AtiColorDWordRGBA;
#else //Direct3D
 //============================================================================
 typedef union
 {
   struct { uint8 b, g, r, a; };
   struct { uint32 color; };
 } AtiColorDWordRGBA;
#endif

//=============================================================================
typedef struct
{
   AtiVertex3 vertex;
   AtiColorDWordRGBA color1;
} AtiVertexColor1;

//=============================================================================
typedef union
{
   //Point in 3D space
   struct { float32 x, y, z, w; };
   struct { float32 v[4]; };
} AtiVertex4, AtiVector4;

//=============================================================================
typedef struct
{
   AtiVertex4 lo;
   AtiVertex4 hi;
   AtiVertex4 lo2;
   AtiVertex4 hi2;
} AtiSplineInterval;

//=============================================================================
class AtiFlyPath
{
   public:
      /******************************/
      /* Constructor and Destructor */
      /******************************/
      AtiFlyPath();
      ~AtiFlyPath();

      /************/
      /* File I/O */
      /************/
      void SetBasePath(char8 *aPath);
      bool8 ReadFile(char8 *aFileName);
      bool8 ReadCurrentFile(void);
      bool8 WriteFile(char8 *aFileName);
      bool8 WriteCurrentFile(void);

      /***************************/
      /* Start/Stop Fly Throughs */
      /***************************/
      void InitPath(void);
      bool8 StartFlight(float64 aTime);
      void PauseFlight(float64 aTime);
      bool8 ResumeFlight(float64 aTime);
      void EndFlight(void);

      float64 GetTotalPathTime(void);
      float64 GetTimeRelativeToStartFlight(float64 aTime);

      /*******************/
      /* Sample The Path */
      /*******************/
      bool8 GetPathPosition(AtiVertex3 *aPosition, float64 aTime);
      bool8 GetPathOrientation(AtiQuaternion *aOrientation, float64 aTime);

      /*******************/
      /* Compile Splines */
      /*******************/
      bool8 CompilePositions(void);
      bool8 CompileOrientations(void);
      bool8 CompilePositionsAndOrientations(void);

      /********************/
      /* Editing commands */
      /********************/
      bool8 AddFlyPointAfterCurrent(AtiVertex3 position, AtiQuaternion orientation);
      void ChangeCurrentFlyPoint(AtiVertex3 position, AtiQuaternion orientation);
      void DeleteCurrentFlyPoint(bool8 aSimulateBackspaceKey);
      void ToggleCutForCurrentFlyPoint(void);
      bool8 DrawFlyPath(void);
      void SelectNearestFlyPoint(AtiVertex3 aPos);
      bool8 GetCurrentFlyPoint(AtiVertex3 *position, AtiQuaternion *orientation);
      void ClearFlyPointSelection(void);
      bool8 GetCurrentFlyPointInfo(int32 *aPointNumber, float32 *aPointTime);

      /***************************/
      /* Next and Previous Paths */
      /***************************/
      void SetNextPath(AtiFlyPath *aPath);
      void SetPreviousPath(AtiFlyPath *aPath);
      AtiFlyPath* GetNextPath(void);
      AtiFlyPath* GetPreviousPath(void);

   private:
      /*********************/
      /* Private Variables */
      /*********************/
      char8 mFileName[32];
      
      bool8 mInFlight;
      float64 mStartTime;
      float64 mPausedTime;
      bool8 mPaused;
      
      int32 mNumFlyPoints;
      int32 mCurrentEditPoint;
      float64 mCurrentEditPointTime;
      float64 mSampleInterval;
      
      float64 mPathTime;
      int32 mNumSplineIntervals;

      AtiVertex4 *mLocation; //w is non-zero if this is a cut point
      AtiSplineInterval *mLocationIntervals;
      //NOTE: Always a one-to-one correspondance between mLocation and mOrientation!!!
      
      AtiQuaternion *mOrientation;
      AtiSplineInterval *mOrientationIntervals;

      //If there is no previous and next, point to self
      AtiFlyPath *mPreviousPath;
      AtiFlyPath *mNextPath;
};

#endif
