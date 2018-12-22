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
 *  AtiFly.cpp -- Automated fly through code
 ******************************************************************************
 *  Written by Alex Vlachos (Alex@Vlachos.com)
 ******************************************************************************/

// INCLUDES ===================================================================
#include "AtiFly.h"

// DEFINES ====================================================================
#define ATI_FLY_CUBE_RADIUS 2.5f
#define ATI_FLY_SPLINE_TENSION (-0.5f)
#define ATI_FLY_S ((1.0f-ATI_FLY_SPLINE_TENSION)*0.5f)
#define ATI_STRING_LENGTH 64

// MACROS =====================================================================
#define AtiFreeArray(array) \
   if (array != NULL)       \
      delete [] array;      \
   array = NULL;

#define AtiMallocArray(array,num,type)                                   \
   array = NULL;                                                         \
   if (num > 0)                                                          \
   {                                                                     \
      array = new type [num];                                            \
      if (array == NULL)                                                 \
      {                                                                  \
         return FALSE;                                                   \
      }                                                                  \
      memset (array, 0, sizeof(type) * num);                             \
   }

// GLOBALS ====================================================================
AtiMatrix sgCardinalMatrix = { -ATI_FLY_S,     2.0f*ATI_FLY_S,      -ATI_FLY_S, 0.0f,
                               2.0f-ATI_FLY_S, ATI_FLY_S-3.0f,      0.0f,       1.0f,
                               ATI_FLY_S-2.0f, 3.0f-2.0f*ATI_FLY_S, ATI_FLY_S,  0.0f,
                               ATI_FLY_S,      -ATI_FLY_S,          0.0f,       0.0f };

static char8 sgFileBase[ATI_STRING_LENGTH] = "";

//=============================================================================
AtiFlyPath::AtiFlyPath()
{
   mFileName[0] = '\0';

   mInFlight = FALSE;
   mStartTime = 0.0;
   mPausedTime = 0.0;
   mPaused = FALSE;

   mNumFlyPoints = 0;
   mCurrentEditPoint = -1;
   mCurrentEditPointTime = 0.0;
   mSampleInterval = 1.0;
   mPathTime = 0.0;
   mNumSplineIntervals = 0;

   mLocation = NULL;
   mLocationIntervals = NULL;

   mOrientation = NULL;
   mOrientationIntervals = NULL;

   mPreviousPath = this;
   mNextPath = this;
}

//=============================================================================
AtiFlyPath::~AtiFlyPath()
{
   InitPath();
}

//=============================================================================
void AtiFlyPath::SetBasePath (char8 *aPath)
{
   strcpy(sgFileBase, aPath);
}

//=============================================================================
void AtiFlyPath::InitPath (void)
{
   mInFlight = FALSE;
   mStartTime = 0.0;
   mPausedTime = 0.0;
   mPaused = FALSE;

   mNumFlyPoints = 0;
   mCurrentEditPoint = -1;
   mCurrentEditPointTime = 0.0;
   mSampleInterval = 1.0;
   mPathTime = 0.0;
   mNumSplineIntervals = 0;

   AtiFreeArray(mLocation);
   AtiFreeArray(mLocationIntervals);

   AtiFreeArray(mOrientation);
   AtiFreeArray(mOrientationIntervals);
}

//=============================================================================
bool8 AtiFlyPath::ReadFile (char8 *aFileName)
{
   int32 i;
   FILE *fp;
   float32 tmpf;
   char8 buff[ATI_STRING_LENGTH*2];

   /*************************/
   /* Init Member Variables */
   /*************************/
   InitPath();

   /*****************************/
   /* Copy File Name To Current */
   /*****************************/
   strncpy(mFileName, aFileName, 31);
   mFileName[31] = '\0';

   /*************/
   /* Open File */
   /*************/
   sprintf(buff, "%s%s", sgFileBase, aFileName);
   fp = fopen(buff, "r");
   if (fp == NULL)
      return FALSE;

   /***********************/
   /* Get Sample Interval */
   /***********************/
   fscanf(fp, "%f", &tmpf);
   mSampleInterval = tmpf;

   /************************/
   /* Get number of points */
   /************************/
   fscanf(fp, "%d", &mNumFlyPoints);
   if (mNumFlyPoints <= 0)
   {
      mNumFlyPoints = 0;
      mCurrentEditPoint = -1;
      return TRUE;
   }
   mCurrentEditPoint = 0;

   /*******************/
   /* Allocate Memory */
   /*******************/
   AtiMallocArray(mLocation, mNumFlyPoints+1, AtiVertex4);
   AtiMallocArray(mOrientation, mNumFlyPoints+1, AtiQuaternion);

   /************/
   /* Get data */
   /************/
   for (i=0; i<mNumFlyPoints; i++)
   {
      fscanf(fp, "%f %f %f %f", &(mLocation[i].x), &(mLocation[i].y), &(mLocation[i].z), &(mLocation[i].w));
      fscanf(fp, "%f %f %f %f", &(mOrientation[i].x), &(mOrientation[i].y), &(mOrientation[i].z), &(mOrientation[i].w));

      //Read in the hex versions of the floats
      //fscanf(fp, "%x %x %x %x", &(mLocation[i].x), &(mLocation[i].y), &(mLocation[i].z), &(mLocation[i].w));
      //fscanf(fp, "%x %x %x %x", &(mOrientation[i].x), &(mOrientation[i].y), &(mOrientation[i].z), &(mOrientation[i].w));
   }

   /*********************************************/
   /* Don't allow first point to be a cut point */
   /*********************************************/
   mLocation[0].w = 0.0f;

   /**************/
   /* Close File */
   /**************/
   fclose(fp);

   /********************/
   /* Compile Fly Path */
   /********************/
   if (CompilePositionsAndOrientations() == FALSE)
      return FALSE;

   /***************************/
   /* Finished With No Errors */
   /***************************/
   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::ReadCurrentFile (void)
{
   return ReadFile(mFileName);
}

//=============================================================================
bool8 AtiFlyPath::WriteFile (char8 *aFileName)
{
   int32 i;
   FILE *fp;
   char8 buff[ATI_STRING_LENGTH*2];

   /***********************************/
   /* Make sure we have enough points */
   /***********************************/
   if (mNumFlyPoints < 5)
      return FALSE;

   /*************/
   /* Open File */
   /*************/
   sprintf(buff, "%s%s", sgFileBase, aFileName);
   fp = fopen(buff, "w");
   if (fp == NULL)
      return FALSE;

   /*************************/
   /* Write Sample Interval */
   /*************************/
   fprintf(fp, "%f\n", mSampleInterval);

   /**************************/
   /* Write number of points */
   /**************************/
   fprintf(fp, "%d\n", mNumFlyPoints);

   /************************************/
   /* Write positions and orientations */
   /************************************/
   for (i=0; i<mNumFlyPoints; i++)
   {
      fprintf(fp, "%f %f %f %f\n", mLocation[i].x, mLocation[i].y, mLocation[i].z, mLocation[i].w);
      fprintf(fp, "%f %f %f %f\n", mOrientation[i].x, mOrientation[i].y, mOrientation[i].z, mOrientation[i].w);

      //Write out the hex versions of the floats
      //fprintf(fp, "%x %x %x %x\n", *(int*)&(mLocation[i].x), *(int*)&(mLocation[i].y), *(int*)&(mLocation[i].z), *(int*)&(mLocation[i].w));
      //fprintf(fp, "%x %x %x %x\n", *(int*)&(mOrientation[i].x), *(int*)&(mOrientation[i].y), *(int*)&(mOrientation[i].z), *(int*)&(mOrientation[i].w));
   }

   /**************/
   /* Close File */
   /**************/
   fclose(fp);

   /***************************/
   /* Finished With No Errors */
   /***************************/
   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::WriteCurrentFile (void)
{
   return WriteFile(mFileName);
}

//=============================================================================
bool8 AtiFlyPath::StartFlight (float64 aTime)
{
   /***********************************/
   /* Make sure we have enough points */
   /***********************************/
   if (mNumFlyPoints < 5)
      return FALSE;

   /*****************/
   /* Set variables */
   /*****************/
   mInFlight = TRUE;
   mPaused = FALSE;
   mStartTime = aTime;

   /***********************************************/
   /* Engine specific: This will start you at the */
   /* selected position when in editing mode      */
   /***********************************************/
   //if (gEditingPath == TRUE)
   //   mStartTime -= mCurrentEditPointTime;

   return TRUE;
}

//=============================================================================
void AtiFlyPath::PauseFlight (float64 aTime)
{
   mInFlight = FALSE;
   mPaused = TRUE;
   mPausedTime = aTime;
}

//=============================================================================
bool8 AtiFlyPath::ResumeFlight (float64 aTime)
{
   if ((mPaused == FALSE) && (mInFlight == FALSE))
   {
      return StartFlight(aTime);
   }

   if (mPaused == FALSE)
      return FALSE;

   mStartTime += aTime - mPausedTime;
   mPaused = FALSE;
   mInFlight = TRUE;

   return TRUE;
}

//=============================================================================
void AtiFlyPath::EndFlight (void)
{
   mInFlight = FALSE;
   mPaused = FALSE;
}

//=============================================================================
float64 AtiFlyPath::GetTotalPathTime (void)
{
   return (mPathTime);
}

//=============================================================================
float64 AtiFlyPath::GetTimeRelativeToStartFlight (float64 aTime)
{
   if (mPaused == TRUE)
      return (mPausedTime - mStartTime);
   else
      return (aTime - mStartTime);
}

//=============================================================================
bool8 AtiFlyPath::AddFlyPointAfterCurrent (AtiVertex3 aPosition, AtiQuaternion aOrientation)
{
   AtiVertex4 *tmpLocation = NULL;
   AtiQuaternion *tmpOrientation = NULL;

   /******************************/
   /* Make sure we're not flying */
   /******************************/
   if (mInFlight == TRUE)
      return FALSE;

   /*********************************/
   /* Allocate memory for new point */
   /*********************************/
   tmpLocation = mLocation;
   AtiMallocArray(mLocation, mNumFlyPoints+1, AtiVertex4);

   tmpOrientation = mOrientation;
   AtiMallocArray(mOrientation, mNumFlyPoints+1, AtiQuaternion);

   /*********************************************/
   /* Coyp all points before current edit point */
   /*********************************************/
   if ((mNumFlyPoints > 0) && (mCurrentEditPoint >= 0))
   {
      memcpy(mLocation, tmpLocation, sizeof(AtiVertex4)*(mCurrentEditPoint+1));
      memcpy(mOrientation, tmpOrientation, sizeof(AtiQuaternion)*(mCurrentEditPoint+1));
   }

   /********************************************/
   /* Copy all points after current edit point */
   /********************************************/
   if ((mCurrentEditPoint+1) < mNumFlyPoints) //If last point is not current
   {
      memcpy(&(mLocation[mCurrentEditPoint+2]), &(tmpLocation[mCurrentEditPoint+1]), sizeof(AtiVertex4)*(mNumFlyPoints-mCurrentEditPoint-1));
      memcpy(&(mOrientation[mCurrentEditPoint+2]), &(tmpOrientation[mCurrentEditPoint+1]), sizeof(AtiQuaternion)*(mNumFlyPoints-mCurrentEditPoint-1));
   }

   /*********************/
   /* Now add new point */
   /*********************/
   mLocation[mCurrentEditPoint+1].x = aPosition.x;
   mLocation[mCurrentEditPoint+1].y = aPosition.y;
   mLocation[mCurrentEditPoint+1].z = aPosition.z;
   mLocation[mCurrentEditPoint+1].w = 0.0f;

   mOrientation[mCurrentEditPoint+1].x = aOrientation.x;
   mOrientation[mCurrentEditPoint+1].y = aOrientation.y;
   mOrientation[mCurrentEditPoint+1].z = aOrientation.z;
   mOrientation[mCurrentEditPoint+1].w = aOrientation.w;

   /*******************/
   /* Update counters */
   /*******************/
   mNumFlyPoints++;
   mCurrentEditPoint++;

   /*******************/
   /* Free tmp memory */
   /*******************/
   AtiFreeArray(tmpLocation);
   AtiFreeArray(tmpOrientation);

   /********************/
   /* Compile Fly Path */
   /********************/
   if (CompilePositionsAndOrientations() == FALSE)
      return FALSE;

   return TRUE;
}

//=============================================================================
void AtiFlyPath::ChangeCurrentFlyPoint (AtiVertex3 aPosition, AtiQuaternion aOrientation)
{
   /******************************/
   /* Make sure we're not flying */
   /******************************/
   if (mInFlight == TRUE)
      return;

   /*************************************/
   /* Make sure we have a point to move */
   /*************************************/
   if (mCurrentEditPoint < 0)
      return;

   /**********/
   /* Update */
   /**********/
   mLocation[mCurrentEditPoint].x = aPosition.x;
   mLocation[mCurrentEditPoint].y = aPosition.y;
   mLocation[mCurrentEditPoint].z = aPosition.z;

   mOrientation[mCurrentEditPoint].x = aOrientation.x;
   mOrientation[mCurrentEditPoint].y = aOrientation.y;
   mOrientation[mCurrentEditPoint].z = aOrientation.z;
   mOrientation[mCurrentEditPoint].w = aOrientation.w;

   /********************/
   /* Compile Fly Path */
   /********************/
   if (CompilePositionsAndOrientations() == FALSE)
      return;
}

//=============================================================================
void AtiFlyPath::DeleteCurrentFlyPoint (bool8 aSimulateBackspaceKey)
{
   /******************************/
   /* Make sure we're not flying */
   /******************************/
   if (mInFlight == TRUE)
      return;

   /***************************************/
   /* Make sure we have a point to delete */
   /***************************************/
   if ((mCurrentEditPoint < 0) || (mNumFlyPoints == 0))
      return;

   /*****************************************************/
   /* Copy all points after current edit point back one */
   /*****************************************************/
   if ((mCurrentEditPoint+1) < mNumFlyPoints) //If last point is not current
   {
      memcpy(&(mLocation[mCurrentEditPoint]), &(mLocation[mCurrentEditPoint+1]), sizeof(AtiVertex4)*(mNumFlyPoints-mCurrentEditPoint-1));
      memcpy(&(mOrientation[mCurrentEditPoint]), &(mOrientation[mCurrentEditPoint+1]), sizeof(AtiQuaternion)*(mNumFlyPoints-mCurrentEditPoint-1));
   }

   /*******************/
   /* Update counters */
   /*******************/
   mNumFlyPoints--;
   if (aSimulateBackspaceKey == TRUE)
   {
      if ((mCurrentEditPoint != 0) || (mNumFlyPoints == 0))
         mCurrentEditPoint--;
   }
   else //Like delete key
   {
      if (mCurrentEditPoint == mNumFlyPoints)
         mCurrentEditPoint--;
   }

   /********************/
   /* Compile Fly Path */
   /********************/
   if (CompilePositionsAndOrientations() == FALSE)
      return;
}

//=============================================================================
void AtiFlyPath::ToggleCutForCurrentFlyPoint (void)
{
   /******************************/
   /* Make sure we're not flying */
   /******************************/
   if (mInFlight == TRUE)
      return;

   /**********/
   /* Toggle */
   /**********/
   if (mCurrentEditPoint > 0) //Any point except first point
   {
      if (mLocation[mCurrentEditPoint].w == 0.0)
         mLocation[mCurrentEditPoint].w = 1.0;
      else
         mLocation[mCurrentEditPoint].w = 0.0;
   }

   /********************/
   /* Compile Fly Path */
   /********************/
   if (CompilePositionsAndOrientations() == FALSE)
      return;
}

//=============================================================================
// NOTE: This isn't meant to be very optimal since end-users won't ever see this
//=============================================================================
bool8 AtiFlyPath::DrawFlyPath (void)
{
   int32 i;
   int32 j;
   int32 numVertices = 0;
   AtiVertexColor1 *vertexArray;
   int32 numIndices = 0;
   uint16 *indexArray;

   bool8 inFlight = mInFlight;
   float64 startTime = mStartTime;
   float64 pausedTime = mPausedTime;
   bool8 paused = mPaused;

   if (mNumFlyPoints <= 0)
      return TRUE;

   /*******************/
   /* Allocate memory */
   /*******************/
   AtiMallocArray(vertexArray, mNumFlyPoints*46, AtiVertexColor1);
   AtiMallocArray(indexArray, mNumFlyPoints*46, uint16);

   /******************/
   /* Reset counters */
   /******************/
   numVertices = 0;
   numIndices = 0;

   if (mNumFlyPoints >= 5)
   {
      /**********************************/
      /* Build draw array for cut lines */
      /**********************************/
      for (i=0; i<mNumFlyPoints-1; i++)
      {
         if (mLocation[i].w != 0.0f)
         {
            //This point
            vertexArray[numVertices].vertex.x = mLocation[i].x;
            vertexArray[numVertices].vertex.y = mLocation[i].y;
            vertexArray[numVertices].vertex.z = mLocation[i].z;
            vertexArray[numVertices].color1.r = 0;
            vertexArray[numVertices].color1.g = 0;
            vertexArray[numVertices].color1.b = 255;
            numVertices++;

            //Next point
            vertexArray[numVertices].vertex.x = mLocation[i+1].x;
            vertexArray[numVertices].vertex.y = mLocation[i+1].y;
            vertexArray[numVertices].vertex.z = mLocation[i+1].z;
            vertexArray[numVertices].color1.r = 0;
            vertexArray[numVertices].color1.g = 0;
            vertexArray[numVertices].color1.b = 255;
            numVertices++;
         }
      }

      if (mNumFlyPoints > 0)
      {
         if ((mLocation[i].w != 0.0f) && (mNextPath->mNumFlyPoints > 0))
         {
            //This point
            vertexArray[numVertices].vertex.x = mLocation[i].x;
            vertexArray[numVertices].vertex.y = mLocation[i].y;
            vertexArray[numVertices].vertex.z = mLocation[i].z;
            vertexArray[numVertices].color1.r = 0;
            vertexArray[numVertices].color1.g = 0;
            vertexArray[numVertices].color1.b = 255;
            numVertices++;

            //Next point
            vertexArray[numVertices].vertex.x = mNextPath->mLocation[0].x;
            vertexArray[numVertices].vertex.y = mNextPath->mLocation[0].y;
            vertexArray[numVertices].vertex.z = mNextPath->mLocation[0].z;
            vertexArray[numVertices].color1.r = 0;
            vertexArray[numVertices].color1.g = 0;
            vertexArray[numVertices].color1.b = 255;
            numVertices++;
         }
      }

      /*******************************/
      /* Build draw array for spline */
      /*******************************/
      StartFlight(0.0);
      for (i=0; i<mNumSplineIntervals; i++)
      {
         for (j=0; j<7; j++)
         {
            float64 x1 = 0.0;
            float64 x2 = 0.0;

            AtiMatrix matrix;
            AtiQuaternion quat;
            AtiVertex3 pos;

            x1 = (i*7 + (j+0)) * (mSampleInterval/7.0);
            x2 = (i*7 + (j+1)) * (mSampleInterval/7.0);
            if (j == 6)
               x2 -= 0.001f;

            if (GetPathPosition(&pos, x1) == TRUE)
            {
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 0;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;
            }

            if (GetPathPosition(&pos, x2) == TRUE)
            {
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 0;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;
            }

            if (GetPathOrientation(&quat, x2) == TRUE)
            {
               AtiQuatToMatrix(&quat, matrix);

               //This point
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;

               //Up Vector
               vertexArray[numVertices].vertex.x = pos.x + matrix[1]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].vertex.y = pos.y + matrix[5]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].vertex.z = pos.z + matrix[9]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;

               //This point
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 0;
               numVertices++;

               //View Vector
               vertexArray[numVertices].vertex.x = pos.x - matrix[2]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].vertex.y = pos.y - matrix[6]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].vertex.z = pos.z - matrix[10]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 0;
               numVertices++;
            }

            if ((j == 0) && (GetPathOrientation(&quat, x1) == TRUE))
            {
               AtiQuatToMatrix(&quat, matrix);
               GetPathPosition(&pos, x1);

               //This point
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;

               //Up Vector
               vertexArray[numVertices].vertex.x = pos.x + matrix[1]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].vertex.y = pos.y + matrix[5]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].vertex.z = pos.z + matrix[9]*ATI_FLY_CUBE_RADIUS*2.0f;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 255;
               numVertices++;

               //This point
               vertexArray[numVertices].vertex.x = pos.x;
               vertexArray[numVertices].vertex.y = pos.y;
               vertexArray[numVertices].vertex.z = pos.z;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 0;
               numVertices++;

               //View Vector
               vertexArray[numVertices].vertex.x = pos.x - matrix[2]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].vertex.y = pos.y - matrix[6]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].vertex.z = pos.z - matrix[10]*ATI_FLY_CUBE_RADIUS*4.0f;
               vertexArray[numVertices].color1.r = 255;
               vertexArray[numVertices].color1.g = 255;
               vertexArray[numVertices].color1.b = 0;
               numVertices++;
            }
         }
      }
      EndFlight();

      for (i=0; i<numVertices; i++)
      {
         indexArray[numIndices] = i;
         numIndices++;
      }

      /**********************************************/
      /* Draw: An example draw call from our engine */
      /**********************************************/
      //AwDraw (AWPT_LINE_LIST, sizeof(AtiVertexColor1),
      //        0, (uint8 *)vertexArray, 0, numVertices,
      //        0, indexArray,  0, numIndices);
   }

   /******************/
   /* Reset counters */
   /******************/
   numIndices = 0;
   numVertices = 0;

   /**********************************/
   /* Build draw array for traingles */
   /**********************************/
   if (mNumFlyPoints > 0)
   {
      for (i=0; i<mNumFlyPoints; i++)
      {
         AtiMatrix matrix;
         AtiQuatToMatrix(&(mOrientation[i]), matrix);
         AtiColorDWordRGBA color;

         if (i == mCurrentEditPoint)
         {
            color.r = 255;
            color.g = 255;
            color.b = 0;
            color.a = 0;
         }
         else if (i == 0)
         {
            color.r = 0;
            color.g = 255;
            color.b = 0;
            color.a = 0;
         }
         else if (i == mNumFlyPoints-1)
         {
            color.r = 255;
            color.g = 0;
            color.b = 0;
            color.a = 0;
         }
         else
         {
            color.r = 0;
            color.g = 0;
            color.b = 255;
            color.a = 0;
         }


         vertexArray[numVertices].vertex.x = mLocation[i].x + ((matrix[0] + matrix[1] + matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((matrix[4] + matrix[5] + matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((matrix[8] + matrix[9] + matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((matrix[0] - matrix[1] + matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((matrix[4] - matrix[5] + matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((matrix[8] - matrix[9] + matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((-matrix[0] - matrix[1] + matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((-matrix[4] - matrix[5] + matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((-matrix[8] - matrix[9] + matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((-matrix[0] + matrix[1] + matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((-matrix[4] + matrix[5] + matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((-matrix[8] + matrix[9] + matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((matrix[0] + matrix[1] - matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((matrix[4] + matrix[5] - matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((matrix[8] + matrix[9] - matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((matrix[0] - matrix[1] - matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((matrix[4] - matrix[5] - matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((matrix[8] - matrix[9] - matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((-matrix[0] - matrix[1] - matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((-matrix[4] - matrix[5] - matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((-matrix[8] - matrix[9] - matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         vertexArray[numVertices].vertex.x = mLocation[i].x + ((-matrix[0] + matrix[1] - matrix[2] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.y = mLocation[i].y + ((-matrix[4] + matrix[5] - matrix[6] )*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].vertex.z = mLocation[i].z + ((-matrix[8] + matrix[9] - matrix[10])*ATI_FLY_CUBE_RADIUS);
         vertexArray[numVertices].color1.color = color.color;
         numVertices++;

         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-6;
         indexArray[numIndices++] = numVertices-7;
         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-5;
         indexArray[numIndices++] = numVertices-6;

         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-3;
         indexArray[numIndices++] = numVertices-4;
         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-7;
         indexArray[numIndices++] = numVertices-3;

         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-4;
         indexArray[numIndices++] = numVertices-1;
         indexArray[numIndices++] = numVertices-8;
         indexArray[numIndices++] = numVertices-1;
         indexArray[numIndices++] = numVertices-5;

         indexArray[numIndices++] = numVertices-7;
         indexArray[numIndices++] = numVertices-2;
         indexArray[numIndices++] = numVertices-3;
         indexArray[numIndices++] = numVertices-7;
         indexArray[numIndices++] = numVertices-6;
         indexArray[numIndices++] = numVertices-2;

         indexArray[numIndices++] = numVertices-5;
         indexArray[numIndices++] = numVertices-2;
         indexArray[numIndices++] = numVertices-6;
         indexArray[numIndices++] = numVertices-5;
         indexArray[numIndices++] = numVertices-1;
         indexArray[numIndices++] = numVertices-2;

         indexArray[numIndices++] = numVertices-4;
         indexArray[numIndices++] = numVertices-2;
         indexArray[numIndices++] = numVertices-1;
         indexArray[numIndices++] = numVertices-4;
         indexArray[numIndices++] = numVertices-3;
         indexArray[numIndices++] = numVertices-2;
      }

      /**********************************************/
      /* Draw: An example draw call from our engine */
      /**********************************************/
      //AwDraw (AWPT_TRIANGLE_LIST, sizeof(AtiVertexColor1),
      //        0, (uint8 *)vertexArray, 0, numVertices,
      //        0, indexArray,  0, numIndices);
   }

   /***************/
   /* Free memory */
   /***************/
   AtiFreeArray(vertexArray);
   AtiFreeArray(indexArray);

   mInFlight    = inFlight;
   mStartTime   = startTime;
   mPausedTime  = pausedTime;
   mPaused      = paused;

   return TRUE;
}

//=============================================================================
void AtiFlyPath::SelectNearestFlyPoint (AtiVertex3 aPos)
{
   int32 i;
   float32 tmpf;
   float32 dist = 999999999999.0f;

   /**********************/
   /* Find closest point */
   /**********************/
   mCurrentEditPoint = -1;
   for (i=0; i<mNumFlyPoints; i++)
   {
      tmpf = ((mLocation[i].x - aPos.x) * (mLocation[i].x - aPos.x)) +
             ((mLocation[i].y - aPos.y) * (mLocation[i].y - aPos.y)) +
             ((mLocation[i].z - aPos.z) * (mLocation[i].z - aPos.z));

      if ((tmpf < dist) || (i == 0))
      {
         dist = tmpf;
         mCurrentEditPoint = i;
      }
   }

   if (mPaused == TRUE)
   {
      int32 point;
      float32 time;
      
      GetCurrentFlyPointInfo(&point, &time);
      mPausedTime = mStartTime + time;
   }
}

//=============================================================================
bool8 AtiFlyPath::GetCurrentFlyPoint (AtiVertex3 *aPosition, AtiQuaternion *aOrientation)
{
   if (mCurrentEditPoint < 0)
      return FALSE;

   aPosition->x = mLocation[mCurrentEditPoint].x;
   aPosition->y = mLocation[mCurrentEditPoint].y;
   aPosition->z = mLocation[mCurrentEditPoint].z;

   aOrientation->x = mOrientation[mCurrentEditPoint].x;
   aOrientation->y = mOrientation[mCurrentEditPoint].y;
   aOrientation->z = mOrientation[mCurrentEditPoint].z;
   aOrientation->w = mOrientation[mCurrentEditPoint].w;

   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::GetCurrentFlyPointInfo(int32 *aPointNumber, float32 *aPointTime)
{
   int32 i;

   if (mCurrentEditPoint < 0)
      return FALSE;

   *aPointNumber = mCurrentEditPoint;
   *aPointTime = 0.0f;
   for (i=0; i<mCurrentEditPoint; i++)
   {
      if (mLocation[i].w == 0.0)
         *aPointTime += (float32)mSampleInterval;
   }

   return TRUE;
}

//=============================================================================
void AtiFlyPath::ClearFlyPointSelection (void)
{
   if (mNumFlyPoints == 0)
      mCurrentEditPoint = -1;
   else
      mCurrentEditPoint = 0;
}

//=============================================================================
void AtiFlyPath::SetNextPath (AtiFlyPath *aPath)
{
   if (aPath != NULL)
      mNextPath = aPath;
   else
      mNextPath = this;
}

//=============================================================================
void AtiFlyPath::SetPreviousPath (AtiFlyPath *aPath)
{
   if (aPath != NULL)
      mPreviousPath = aPath;
   else
      mPreviousPath = this;
}

//=============================================================================
AtiFlyPath* AtiFlyPath::GetNextPath (void)
{
   return mNextPath;
}

//=============================================================================
AtiFlyPath* AtiFlyPath::GetPreviousPath (void)
{
   return mPreviousPath;
}

//=============================================================================
bool8 AtiFlyPath::CompilePositionsAndOrientations (void)
{
   if (CompilePositions() == FALSE)
      return FALSE;

   if (CompileOrientations() == FALSE)
      return FALSE;

   return TRUE;
}

//=============================================================================
float32 AtiDistance (AtiVertex4 *v1, AtiVertex4 *v2)
{
   AtiVertex4 tmpv;

   tmpv.x = v1->x - v2->x;
   tmpv.y = v1->y - v2->y;
   tmpv.z = v1->z - v2->z;

   return (float32)(sqrt((tmpv.x*tmpv.x) + (tmpv.y*tmpv.y) + (tmpv.z*tmpv.z)));
}

//=============================================================================
void AtiVertex4Lerp (AtiVertex4 *ret, AtiVertex4 *v1, AtiVertex4 *v2, float32 lerpValue)
{
   ret->x = v1->x + lerpValue*(v2->x - v1->x);
   ret->y = v1->y + lerpValue*(v2->y - v1->y);
   ret->z = v1->z + lerpValue*(v2->z - v1->z);
}

//=============================================================================
void AtiVertex4Copy (AtiVertex4 *v1, AtiVertex4 *v2)
{
   v1->x = v2->x;
   v1->y = v2->y;
   v1->z = v2->z;
}

//=============================================================================
bool8 AtiFlyPath::CompilePositions (void)
{
   int32 i;
   int32 j;
   int32 k;
   float32 tmpf;
   float32 tmpf2;
   int32 numPoints = 0;
   AtiVertex4 *pointArray = NULL;
   AtiVertex4 *tmpD2 = NULL;
   AtiVertex4 *tmpIntD2 = NULL;

   /***********************************/
   /* Make sure we have enough points */
   /***********************************/
   if (mNumFlyPoints < 5)
      return FALSE;

   /********************************************/
   /* Allocate new memory for spline intervals */
   /********************************************/
   AtiFreeArray(mLocationIntervals);
   AtiMallocArray(mLocationIntervals, mNumFlyPoints, AtiSplineInterval);

   /*****************************************/
   /* Allocate memory for temporary storage */
   /*****************************************/
   AtiMallocArray(pointArray, mNumFlyPoints+9, AtiVertex4);
   AtiMallocArray(tmpD2,      mNumFlyPoints+9, AtiVertex4);
   AtiMallocArray(tmpIntD2,   mNumFlyPoints+9, AtiVertex4);

   /***************************************************************************/
   /* Build each array of vertices padding by 1 point on each end and process */
   /***************************************************************************/
   mPathTime = 0.0;
   mNumSplineIntervals = 0;
   for (i=0; i<mNumFlyPoints; i++)
   {
      /*******************/
      /* Skip cut points */
      /*******************/
      if (mLocation[i].w != 0.0)
         continue;

      /****************/
      /* Init counter */
      /****************/
      numPoints = 0;

      /**************************/
      /* Pad spline by 4 points */
      /**************************/
      if ((i == 0) && (mPreviousPath->mNumFlyPoints >= 5) && (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-1].w == 0.0))
      {
         AtiVertex4Copy (&(pointArray[numPoints+3]), &(mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-1]));
         if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-2].w == 0.0)
         {
            AtiVertex4Copy (&(pointArray[numPoints+2]), &(mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-2]));
            if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-3].w == 0.0)
            {
               AtiVertex4Copy (&(pointArray[numPoints+1]), &(mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-3]));
               if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-4].w == 0.0)
               {
                  AtiVertex4Copy (&(pointArray[numPoints+0]), &(mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-4]));
               }
               else
               {
                  AtiVertex4Lerp (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
               }
            }
            else
            {
               AtiVertex4Lerp (&(pointArray[numPoints+1]), &(pointArray[numPoints+2]), &(pointArray[numPoints+3]), -1.0f);
               AtiVertex4Lerp (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
            }
         }
         else
         {
            AtiVertex4Lerp (&(pointArray[numPoints+2]), &(pointArray[numPoints+3]), &(mLocation[i]), -1.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+1]), &(pointArray[numPoints+2]), &(pointArray[numPoints+3]), -1.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
         }
      }
      else //Not first point on spline, just create phantom points
      {
         if (i < mNumFlyPoints-1) //If not last point on spline
         {
            AtiVertex4Lerp (&(pointArray[numPoints+3]), &(mLocation[i]), &(mLocation[i+1]), -1.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+2]), &(mLocation[i]), &(mLocation[i+1]), -2.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+1]), &(mLocation[i]), &(mLocation[i+1]), -3.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+0]), &(mLocation[i]), &(mLocation[i+1]), -4.0f);
         }
         else
         {
            if (mNextPath->mNumFlyPoints >= 5)
            {
               AtiVertex4Lerp (&(pointArray[numPoints+3]), &(mLocation[i]), &(mNextPath->mLocation[0]), -1.0f);
               AtiVertex4Lerp (&(pointArray[numPoints+2]), &(mLocation[i]), &(mNextPath->mLocation[0]), -2.0f);
               AtiVertex4Lerp (&(pointArray[numPoints+1]), &(mLocation[i]), &(mNextPath->mLocation[0]), -3.0f);
               AtiVertex4Lerp (&(pointArray[numPoints+0]), &(mLocation[i]), &(mNextPath->mLocation[0]), -4.0f);
            }
            else //No next point...something's wrong
            {
               AtiVertex4Copy (&(pointArray[numPoints+3]), &(mLocation[i]));
               AtiVertex4Copy (&(pointArray[numPoints+2]), &(mLocation[i]));
               AtiVertex4Copy (&(pointArray[numPoints+1]), &(mLocation[i]));
               AtiVertex4Copy (&(pointArray[numPoints+0]), &(mLocation[i]));
            }
         }
      }
      numPoints += 4;

      /*****************************************/
      /* Add all points that aren't cut points */
      /*****************************************/
      for (; i<mNumFlyPoints; i++)
      {
         /******************/
         /* Add this point */
         /******************/
         AtiVertex4Copy (&(pointArray[numPoints]), &(mLocation[i]));
         numPoints++;

         /**********************/
         /* Break if cut point */
         /**********************/
         if (mLocation[i].w != 0.0)
            break;
      }

      /*************************************************/
      /* Add last 4 pad points by either getting them  */
      /* from the next path or creating phantom points */
      /*************************************************/
      if (((i < mNumFlyPoints) && (mLocation[i].w != 0.0)) || ((i >= mNumFlyPoints) && (mNextPath->mNumFlyPoints < 5))) //If cut point OR next path isn't valid
      {
         AtiVertex4Lerp (&(pointArray[numPoints+0]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 2.0f);
         AtiVertex4Lerp (&(pointArray[numPoints+1]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 3.0f);
         AtiVertex4Lerp (&(pointArray[numPoints+2]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 4.0f);
         AtiVertex4Lerp (&(pointArray[numPoints+3]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 5.0f);
         numPoints += 4;
      }
      else //Look at next path
      {
         /********************************************************************/
         /* This is the last point and smoothly feeding into the next spline */
         /********************************************************************/
         //Add first point to close the spline
         AtiVertex4Copy (&(pointArray[numPoints]), &(mNextPath->mLocation[0]));
         numPoints++;

         //Pad with next point
         AtiVertex4Copy (&(pointArray[numPoints+0]), &(mNextPath->mLocation[1]));
         if (mNextPath->mLocation[1].w == 0.0f)
         {
            AtiVertex4Copy (&(pointArray[numPoints+1]), &(mNextPath->mLocation[2]));
            if (mNextPath->mLocation[2].w == 0.0f)
            {
               AtiVertex4Copy (&(pointArray[numPoints+2]), &(mNextPath->mLocation[3]));
               if (mNextPath->mLocation[3].w == 0.0f)
               {
                  AtiVertex4Copy (&(pointArray[numPoints+3]), &(mNextPath->mLocation[4]));
               }
               else
               {
                  AtiVertex4Lerp (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
               }
            }
            else
            {
               AtiVertex4Lerp (&(pointArray[numPoints+2]), &(pointArray[numPoints+0]), &(pointArray[numPoints+1]), 2.0f);
               AtiVertex4Lerp (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
            }
         }
         else
         {
            AtiVertex4Lerp (&(pointArray[numPoints+1]), &(pointArray[numPoints-1]), &(pointArray[numPoints+0]), 2.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+2]), &(pointArray[numPoints+0]), &(pointArray[numPoints+1]), 2.0f);
            AtiVertex4Lerp (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
         }
         numPoints += 4;
      }

      /********************************************************/
      /* Create spline interval variables based on pointArray */
      /********************************************************/
      tmpD2[0].x = 0.0f;
      tmpD2[0].y = 0.0f;
      tmpD2[0].z = 0.0f;
      tmpD2[0].w = 0.0f;
      tmpIntD2[0].x = 0.0f;
      tmpIntD2[0].y = 0.0f;
      tmpIntD2[0].z = 0.0f;
      tmpIntD2[0].w = 0.0f;

      //Calculate second derivative for all points except first and last
      for (j=1; j<numPoints-1; j++) //For each point
      {
         for (k=0; k<4; k++) //For x, y, and z
         {
            float32 time  = 0.5f;
            float32 time2 = 1.0f;

            tmpf = (time * tmpD2[j-1].v[k]) + 2.0f;
            tmpD2[j].v[k] = (time - 1.0f) / tmpf;

            tmpf2 = ((pointArray[j+1].v[k] - pointArray[j].v[k]) / (time2)) - 
                    ((pointArray[j].v[k] - pointArray[j-1].v[k]) / (time2));

            tmpIntD2[j].v[k] = ((6.0f * tmpf2) / (time2+time2)) - (time * tmpIntD2[j-1].v[k]);
            tmpIntD2[j].v[k] /= tmpf;
         }
      }

      tmpD2[numPoints-1].x = 0.0f;
      tmpD2[numPoints-1].y = 0.0f;
      tmpD2[numPoints-1].z = 0.0f;
      tmpD2[numPoints-1].w = 0.0f;
      tmpIntD2[numPoints-1].x = 0.0f;
      tmpIntD2[numPoints-1].y = 0.0f;
      tmpIntD2[numPoints-1].z = 0.0f;
      tmpIntD2[numPoints-1].w = 0.0f;

      for (j=numPoints-2; j>=1; j--)
      {
         tmpD2[j].x = (tmpD2[j].x * tmpD2[j+1].x) + tmpIntD2[j].x;
         tmpD2[j].y = (tmpD2[j].y * tmpD2[j+1].y) + tmpIntD2[j].y;
         tmpD2[j].z = (tmpD2[j].z * tmpD2[j+1].z) + tmpIntD2[j].z;
         tmpD2[j].w = (tmpD2[j].w * tmpD2[j+1].w) + tmpIntD2[j].w;
      }
      
      for (j=4; j<numPoints-5; j++)
      {
         mLocationIntervals[mNumSplineIntervals].lo.x = pointArray[j].x;
         mLocationIntervals[mNumSplineIntervals].lo.y = pointArray[j].y;
         mLocationIntervals[mNumSplineIntervals].lo.z = pointArray[j].z;
         mLocationIntervals[mNumSplineIntervals].lo.w = pointArray[j].w;

         mLocationIntervals[mNumSplineIntervals].hi.x = pointArray[j+1].x;
         mLocationIntervals[mNumSplineIntervals].hi.y = pointArray[j+1].y;
         mLocationIntervals[mNumSplineIntervals].hi.z = pointArray[j+1].z;
         mLocationIntervals[mNumSplineIntervals].hi.w = pointArray[j+1].w;

         mLocationIntervals[mNumSplineIntervals].lo2.x = tmpD2[j].x;
         mLocationIntervals[mNumSplineIntervals].lo2.y = tmpD2[j].y;
         mLocationIntervals[mNumSplineIntervals].lo2.z = tmpD2[j].z;
         mLocationIntervals[mNumSplineIntervals].lo2.w = tmpD2[j].w;

         mLocationIntervals[mNumSplineIntervals].hi2.x = tmpD2[j+1].x;
         mLocationIntervals[mNumSplineIntervals].hi2.y = tmpD2[j+1].y;
         mLocationIntervals[mNumSplineIntervals].hi2.z = tmpD2[j+1].z;
         mLocationIntervals[mNumSplineIntervals].hi2.w = tmpD2[j+1].w;

         /********************/
         /* Update variables */
         /********************/
         mNumSplineIntervals++;
         mPathTime += mSampleInterval;
      }
   }

   /*********************************/
   /* Free temporary storage memory */
   /*********************************/
   AtiFreeArray(pointArray);
   AtiFreeArray(tmpD2);
   AtiFreeArray(tmpIntD2);

   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::GetPathPosition (AtiVertex3 *aPosition, float64 aTime)
{
   int32 tmpi;
   int32 interval;
   float32 intervalOffset;
   float64 tmpf64;
   float32 a;
   float32 b;

   if (mInFlight == FALSE)
      return FALSE;

   /******************************/
   /* Take care of loop wrapping */
   /******************************/
   tmpf64 = aTime - mStartTime;
   if (tmpf64 >= mPathTime)
   {
      tmpi = (int32)(tmpf64/mPathTime);
      tmpf64 -= mPathTime * (float64)(tmpi);
   }

   /**********************************************************************************/
   /* Figure out which interval this time falls into and calculate normalized offset */
   /**********************************************************************************/
   interval = (int32)(tmpf64/mSampleInterval);
   intervalOffset = (float32)((tmpf64 - (mSampleInterval*(float64)(interval))) / mSampleInterval);

   a = 1.0f - intervalOffset;
   b = intervalOffset;

   aPosition->x = a*mLocationIntervals[interval].lo.x + b*mLocationIntervals[interval].hi.x + ((a*a*a - a)*mLocationIntervals[interval].lo2.x + (b*b*b - b)*mLocationIntervals[interval].hi2.x) / 6.0f;// * (float32)(mSampleInterval*mSampleInterval) / 6.0f;
   aPosition->y = a*mLocationIntervals[interval].lo.y + b*mLocationIntervals[interval].hi.y + ((a*a*a - a)*mLocationIntervals[interval].lo2.y + (b*b*b - b)*mLocationIntervals[interval].hi2.y) / 6.0f;// * (float32)(mSampleInterval*mSampleInterval) / 6.0f;
   aPosition->z = a*mLocationIntervals[interval].lo.z + b*mLocationIntervals[interval].hi.z + ((a*a*a - a)*mLocationIntervals[interval].lo2.z + (b*b*b - b)*mLocationIntervals[interval].hi2.z) / 6.0f;// * (float32)(mSampleInterval*mSampleInterval) / 6.0f;

   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::CompileOrientations (void)
{
   int32 i;
   int32 j;
   int32 k;
   float32 tmpf;
   float32 tmpf2;
   int32 numPoints = 0;
   AtiQuaternion *pointArray = NULL;
   AtiQuaternion *tmpD2 = NULL;
   AtiQuaternion *tmpIntD2 = NULL;

   /***********************************/
   /* Make sure we have enough points */
   /***********************************/
   if (mNumFlyPoints < 5)
      return FALSE;

   /********************************************/
   /* Allocate new memory for spline intervals */
   /********************************************/
   AtiFreeArray(mOrientationIntervals);
   AtiMallocArray(mOrientationIntervals, mNumFlyPoints, AtiSplineInterval);

   /*****************************************/
   /* Allocate memory for temporary storage */
   /*****************************************/
   AtiMallocArray(pointArray, mNumFlyPoints+9, AtiQuaternion);
   AtiMallocArray(tmpD2,      mNumFlyPoints+9, AtiQuaternion);
   AtiMallocArray(tmpIntD2,   mNumFlyPoints+9, AtiQuaternion);

   /***************************************************************************/
   /* Build each array of vertices padding by 1 point on each end and process */
   /***************************************************************************/
   mPathTime = 0.0;
   mNumSplineIntervals = 0;
   for (i=0; i<mNumFlyPoints; i++)
   {
      /*******************/
      /* Skip cut points */
      /*******************/
      if (mLocation[i].w != 0.0)
         continue;

      /****************/
      /* Init counter */
      /****************/
      numPoints = 0;

      /**************************/
      /* Pad spline by 4 points */
      /**************************/
      if ((i == 0) && (mPreviousPath->mNumFlyPoints >= 5) && (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-1].w == 0.0))
      {
         AtiQuatCopy (&(pointArray[numPoints+3]), &(mPreviousPath->mOrientation[mPreviousPath->mNumFlyPoints-1]));
         if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-2].w == 0.0)
         {
            AtiQuatCopy (&(pointArray[numPoints+2]), &(mPreviousPath->mOrientation[mPreviousPath->mNumFlyPoints-2]));
            if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-3].w == 0.0)
            {
               AtiQuatCopy (&(pointArray[numPoints+1]), &(mPreviousPath->mOrientation[mPreviousPath->mNumFlyPoints-3]));
               if (mPreviousPath->mLocation[mPreviousPath->mNumFlyPoints-4].w == 0.0)
               {
                  AtiQuatCopy (&(pointArray[numPoints+0]), &(mPreviousPath->mOrientation[mPreviousPath->mNumFlyPoints-4]));
               }
               else
               {
                  AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
               }
            }
            else
            {
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(pointArray[numPoints+2]), &(pointArray[numPoints+3]), -1.0f);
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
            }
         }
         else
         {
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(pointArray[numPoints+3]), &(mOrientation[i]), -1.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(pointArray[numPoints+2]), &(pointArray[numPoints+3]), -1.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), -1.0f);
         }
      }
      else //Not first point on spline, just create phantom points
      {
         if (i < mNumFlyPoints-1) //If not last point on spline
         {
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(mOrientation[i]), &(mOrientation[i+1]), -1.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(mOrientation[i]), &(mOrientation[i+1]), -2.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(mOrientation[i]), &(mOrientation[i+1]), -3.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(mOrientation[i]), &(mOrientation[i+1]), -4.0f);
         }
         else
         {
            if (mNextPath->mNumFlyPoints >= 5)
            {
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(mOrientation[i]), &(mNextPath->mOrientation[0]), -1.0f);
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(mOrientation[i]), &(mNextPath->mOrientation[0]), -2.0f);
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(mOrientation[i]), &(mNextPath->mOrientation[0]), -3.0f);
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(mOrientation[i]), &(mNextPath->mOrientation[0]), -4.0f);
            }
            else //No next point...something's wrong
            {
               AtiQuatCopy (&(pointArray[numPoints+3]), &(mOrientation[i]));
               AtiQuatCopy (&(pointArray[numPoints+2]), &(mOrientation[i]));
               AtiQuatCopy (&(pointArray[numPoints+1]), &(mOrientation[i]));
               AtiQuatCopy (&(pointArray[numPoints+0]), &(mOrientation[i]));
            }
         }
      }
      numPoints += 4;

      /*****************************************/
      /* Add all points that aren't cut points */
      /*****************************************/
      for (; i<mNumFlyPoints; i++)
      {
         /******************/
         /* Add this point */
         /******************/
         AtiQuatCopy (&(pointArray[numPoints]), &(mOrientation[i]));
         numPoints++;

         /**********************/
         /* Break if cut point */
         /**********************/
         if (mLocation[i].w != 0.0)
            break;
      }

      /*************************************************/
      /* Add last 4 pad points by either getting them  */
      /* from the next path or creating phantom points */
      /*************************************************/
      if (((i < mNumFlyPoints) && (mLocation[i].w != 0.0)) || ((i >= mNumFlyPoints) && (mNextPath->mNumFlyPoints < 5))) //If cut point OR next path isn't valid
      {
         AtiQuatSlerpShortestPath (&(pointArray[numPoints+0]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 2.0f);
         AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 3.0f);
         AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 4.0f);
         AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(pointArray[numPoints-2]), &(pointArray[numPoints-1]), 5.0f);
         numPoints += 4;
      }
      else //Look at next path
      {
         /********************************************************************/
         /* This is the last point and smoothly feeding into the next spline */
         /********************************************************************/
         //Add first point to close the spline
         AtiQuatCopy (&(pointArray[numPoints]), &(mNextPath->mOrientation[0]));
         numPoints++;

         //Pad with next point
         AtiQuatCopy (&(pointArray[numPoints+0]), &(mNextPath->mOrientation[1]));
         if (mNextPath->mLocation[1].w == 0.0f)
         {
            AtiQuatCopy (&(pointArray[numPoints+1]), &(mNextPath->mOrientation[2]));
            if (mNextPath->mLocation[2].w == 0.0f)
            {
               AtiQuatCopy (&(pointArray[numPoints+2]), &(mNextPath->mOrientation[3]));
               if (mNextPath->mLocation[3].w == 0.0f)
               {
                  AtiQuatCopy (&(pointArray[numPoints+3]), &(mNextPath->mOrientation[4]));
               }
               else
               {
                  AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
               }
            }
            else
            {
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(pointArray[numPoints+0]), &(pointArray[numPoints+1]), 2.0f);
               AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
            }
         }
         else
         {
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+1]), &(pointArray[numPoints-1]), &(pointArray[numPoints+0]), 2.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+2]), &(pointArray[numPoints+0]), &(pointArray[numPoints+1]), 2.0f);
            AtiQuatSlerpShortestPath (&(pointArray[numPoints+3]), &(pointArray[numPoints+1]), &(pointArray[numPoints+2]), 2.0f);
         }
         numPoints += 4;
      }

      /*************************************************/
      /* Order quarternions for shortest path rotation */
      /*************************************************/
      for (j=0; j<(numPoints-1); j++)
      {
         if (AtiQuatDot(&(pointArray[j]), &(pointArray[j+1])) < 0.0f)
         {
            pointArray[j+1].x = -pointArray[j+1].x;
            pointArray[j+1].y = -pointArray[j+1].y;
            pointArray[j+1].z = -pointArray[j+1].z;
            pointArray[j+1].w = -pointArray[j+1].w;
         }
      }

      /************************************************/
      /* Move quaternions from S3 space into R4 space */
      /************************************************/
      for (j=0; j<numPoints; j++)
      {
         tmpf = 1.0f / (float32)sqrt(2.0*(1.0 - pointArray[j].w));
         pointArray[j].x *= tmpf;
         pointArray[j].y *= tmpf;
         pointArray[j].z *= tmpf;
         pointArray[j].w = (1.0f - pointArray[j].w) * tmpf;
      }

      /********************************************************/
      /* Create spline interval variables based on pointArray */
      /********************************************************/
      tmpD2[0].x = 0.0f;
      tmpD2[0].y = 0.0f;
      tmpD2[0].z = 0.0f;
      tmpD2[0].w = 0.0f;
      tmpIntD2[0].x = 0.0f;
      tmpIntD2[0].y = 0.0f;
      tmpIntD2[0].z = 0.0f;
      tmpIntD2[0].w = 0.0f;

      //Calculate second derivative for all points except first and last
      for (j=1; j<numPoints-1; j++) //For each point
      {
         for (k=0; k<4; k++) //For x, y, and z
         {
            float32 time  = 0.5f;
            float32 time2 = 1.0f;

            tmpf = (time * tmpD2[j-1].v[k]) + 2.0f;
            tmpD2[j].v[k] = (time - 1.0f) / tmpf;

            tmpf2 = ((pointArray[j+1].v[k] - pointArray[j].v[k]) / (time2)) - 
                    ((pointArray[j].v[k] - pointArray[j-1].v[k]) / (time2));

            tmpIntD2[j].v[k] = ((6.0f * tmpf2) / (time2+time2)) - (time * tmpIntD2[j-1].v[k]);
            tmpIntD2[j].v[k] /= tmpf;
         }
      }

      tmpD2[numPoints-1].x = 0.0f;
      tmpD2[numPoints-1].y = 0.0f;
      tmpD2[numPoints-1].z = 0.0f;
      tmpD2[numPoints-1].w = 0.0f;
      tmpIntD2[numPoints-1].x = 0.0f;
      tmpIntD2[numPoints-1].y = 0.0f;
      tmpIntD2[numPoints-1].z = 0.0f;
      tmpIntD2[numPoints-1].w = 0.0f;

      for (j=numPoints-2; j>=1; j--)
      {
         tmpD2[j].x = (tmpD2[j].x * tmpD2[j+1].x) + tmpIntD2[j].x;
         tmpD2[j].y = (tmpD2[j].y * tmpD2[j+1].y) + tmpIntD2[j].y;
         tmpD2[j].z = (tmpD2[j].z * tmpD2[j+1].z) + tmpIntD2[j].z;
         tmpD2[j].w = (tmpD2[j].w * tmpD2[j+1].w) + tmpIntD2[j].w;
      }

      for (j=4; j<numPoints-5; j++)
      {
         mOrientationIntervals[mNumSplineIntervals].lo.x = pointArray[j].x;
         mOrientationIntervals[mNumSplineIntervals].lo.y = pointArray[j].y;
         mOrientationIntervals[mNumSplineIntervals].lo.z = pointArray[j].z;
         mOrientationIntervals[mNumSplineIntervals].lo.w = pointArray[j].w;

         mOrientationIntervals[mNumSplineIntervals].hi.x = pointArray[j+1].x;
         mOrientationIntervals[mNumSplineIntervals].hi.y = pointArray[j+1].y;
         mOrientationIntervals[mNumSplineIntervals].hi.z = pointArray[j+1].z;
         mOrientationIntervals[mNumSplineIntervals].hi.w = pointArray[j+1].w;

         mOrientationIntervals[mNumSplineIntervals].lo2.x = tmpD2[j].x;
         mOrientationIntervals[mNumSplineIntervals].lo2.y = tmpD2[j].y;
         mOrientationIntervals[mNumSplineIntervals].lo2.z = tmpD2[j].z;
         mOrientationIntervals[mNumSplineIntervals].lo2.w = tmpD2[j].w;

         mOrientationIntervals[mNumSplineIntervals].hi2.x = tmpD2[j+1].x;
         mOrientationIntervals[mNumSplineIntervals].hi2.y = tmpD2[j+1].y;
         mOrientationIntervals[mNumSplineIntervals].hi2.z = tmpD2[j+1].z;
         mOrientationIntervals[mNumSplineIntervals].hi2.w = tmpD2[j+1].w;

         /********************/
         /* Update variables */
         /********************/
         mNumSplineIntervals++;
         mPathTime += mSampleInterval;
      }
   }

   /*********************************/
   /* Free temporary storage memory */
   /*********************************/
   AtiFreeArray(pointArray);
   AtiFreeArray(tmpD2);
   AtiFreeArray(tmpIntD2);

   return TRUE;
}

//=============================================================================
bool8 AtiFlyPath::GetPathOrientation (AtiQuaternion *aOrientation, float64 aTime)
{
   int32 tmpi;
   int32 interval;
   float32 intervalOffset;
   float64 tmpf64;
   float32 a;
   float32 b;
   float32 tmpf;
   AtiQuaternion tmpQuat;

   if (mInFlight == FALSE)
      return FALSE;

   /******************************/
   /* Take care of loop wrapping */
   /******************************/
   tmpf64 = aTime - mStartTime;
   if (tmpf64 >= mPathTime)
   {
      tmpi = (int32)(tmpf64/mPathTime);
      tmpf64 -= mPathTime * (float64)(tmpi);
   }

   /**********************************************************************************/
   /* Figure out which interval this time falls into and calculate normalized offset */
   /**********************************************************************************/
   interval = (int32)(tmpf64/mSampleInterval);
   intervalOffset = (float32)((tmpf64 - (mSampleInterval*(float64)(interval))) / mSampleInterval);

   a = 1.0f - intervalOffset;
   b = intervalOffset;

   tmpQuat.x = a*mOrientationIntervals[interval].lo.x + b*mOrientationIntervals[interval].hi.x + ((a*a*a - a)*mOrientationIntervals[interval].lo2.x + (b*b*b - b)*mOrientationIntervals[interval].hi2.x) / 6.0f;
   tmpQuat.y = a*mOrientationIntervals[interval].lo.y + b*mOrientationIntervals[interval].hi.y + ((a*a*a - a)*mOrientationIntervals[interval].lo2.y + (b*b*b - b)*mOrientationIntervals[interval].hi2.y) / 6.0f;
   tmpQuat.z = a*mOrientationIntervals[interval].lo.z + b*mOrientationIntervals[interval].hi.z + ((a*a*a - a)*mOrientationIntervals[interval].lo2.z + (b*b*b - b)*mOrientationIntervals[interval].hi2.z) / 6.0f;
   tmpQuat.w = a*mOrientationIntervals[interval].lo.w + b*mOrientationIntervals[interval].hi.w + ((a*a*a - a)*mOrientationIntervals[interval].lo2.w + (b*b*b - b)*mOrientationIntervals[interval].hi2.w) / 6.0f;

   //R4 space -> S3 space
   tmpf = 1.0f / ((tmpQuat.x*tmpQuat.x) + (tmpQuat.y*tmpQuat.y) + (tmpQuat.z*tmpQuat.z) + (tmpQuat.w*tmpQuat.w));
   aOrientation->x = (2.0f*tmpQuat.x*tmpQuat.w) * tmpf;
   aOrientation->y = (2.0f*tmpQuat.y*tmpQuat.w) * tmpf;
   aOrientation->z = (2.0f*tmpQuat.z*tmpQuat.w) * tmpf;
   aOrientation->w = ((tmpQuat.x*tmpQuat.x) + (tmpQuat.y*tmpQuat.y) + (tmpQuat.z*tmpQuat.z) - (tmpQuat.w*tmpQuat.w)) * tmpf;

   AtiQuatNormalize(aOrientation);

   return TRUE;
}
