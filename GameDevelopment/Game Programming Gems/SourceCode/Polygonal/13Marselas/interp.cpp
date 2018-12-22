/* Copyright (C) Herbert Marselas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herbert Marselas, 2000"
 */
#include "mtxlib.h"

void calculateFramePercentage(long dwTotalAnimFrames, float fTotalAnimTime,
                              float fDesiredTime, 
                              long &dwFirstFrame, long &dwSecondFrame, 
                              float &fPercentage)
{
   // determine which frames are involved

   float fTimePerFrame = fTotalAnimTime / (float) dwTotalAnimFrames;

   dwFirstFrame = 0;

   if (fDesiredTime > fTotalAnimTime)
      fDesiredTime -= fTotalAnimTime;

   float f;
   for (f = 0.0f; f <= fDesiredTime; f += fTimePerFrame)
      dwFirstFrame++;

   // set first frame

   if (f > fDesiredTime)
      dwFirstFrame--;

   if (dwFirstFrame < 0)
      dwFirstFrame = dwTotalAnimFrames - 1;
   else
      if (dwFirstFrame >= dwTotalAnimFrames)
         dwFirstFrame = 0;

   // set second frame

   dwSecondFrame = dwFirstFrame + 1;

   if (dwSecondFrame >= dwTotalAnimFrames)
      dwSecondFrame = 0;

   // calc the percentage

   fPercentage = (fDesiredTime - ((float) dwFirstFrame * fTimePerFrame)) *
                  fTimePerFrame;

} // calculateFramePercentage


void combineVertices(long dwVertexCount, float fPercentage,
                     vector3 *pFirstFrameVertices, vector3 *pSecondFrameVertices, 
                     vector3 *pCombinedVertices)
{
   for (long i = 0; 
        i < dwVertexCount; 
        i++, pFirstFrameVertices++, pSecondFrameVertices++, pCombinedVertices++)
   {
      *pCombinedVertices = *pFirstFrameVertices + 
                           fPercentage * (*pSecondFrameVertices - *pFirstFrameVertices);
   }
}


void calculateFramePercentageSpline(long dwTotalAnimFrames, float fTotalAnimTime,
                              float fDesiredTime, 
                              long &dwFirstFrame, long &dwSecondFrame, 
                              long &dwThirdFrame, long &dwFourthFrame,
                              float &fPercentage)
{
   // determine which frames are involved

   float fTimePerFrame = fTotalAnimTime / (float) dwTotalAnimFrames;

   dwSecondFrame = 0;

   if (fDesiredTime > fTotalAnimTime)
      fDesiredTime -= fTotalAnimTime;

   float f;
   for (f = 0.0f; f <= fDesiredTime; f += fTimePerFrame)
      dwSecondFrame++;

   // set second frame

   if (f > fDesiredTime)
     dwSecondFrame --;

   if (dwSecondFrame < 0)
      dwSecondFrame = dwTotalAnimFrames - 1;
   else
      if (dwSecondFrame >= dwTotalAnimFrames)
         dwSecondFrame = 0;

   // set frame before second frame

   dwFirstFrame = dwSecondFrame - 1;

   if (dwFirstFrame < 0)
      dwFirstFrame = dwTotalAnimFrames - 1;

   // set upper frame

   dwThirdFrame = dwSecondFrame + 1;

   if (dwThirdFrame >= dwTotalAnimFrames)
      dwThirdFrame = 0;

   // set frame after the third frame

   dwFourthFrame = dwThirdFrame + 1;

   if (dwFourthFrame >= dwTotalAnimFrames)
      dwFourthFrame = 0;

   // get the upper percent

   fPercentage = (fDesiredTime - ((float) dwSecondFrame * fTimePerFrame)) *
                  fTimePerFrame;

} // calculateFramePercentage


void combineVerticesSpline(long dwVertexCount, float fPercentage,
                     vector3 *pFirstFrameVertices,
                     vector3 *pSecondFrameVertices,
                     vector3 *pThirdFrameVertices,
                     vector3 *pFourthFrameVertices,
                     vector3 *pCombinedVertices)
{
   float t = fPercentage;
   float t2 = t * t;
   float t3 = t2 * t;

   vector3 m0, m1;

   const float alpha = 0.0f;

   for (long i = 0; 
        i < dwVertexCount; 
        i++, pFirstFrameVertices++, pSecondFrameVertices++, 
        pThirdFrameVertices++, pFourthFrameVertices++)
   {

      m0 = ((1 - alpha) / 2.0f) * 
           ((*pSecondFrameVertices - *pFirstFrameVertices) + 
             *pThirdFrameVertices - *pSecondFrameVertices);

      m1 = ((1 - alpha) / 2.0f) * 
           ((*pThirdFrameVertices - *pSecondFrameVertices) + 
             *pFourthFrameVertices - *pThirdFrameVertices);

      *pCombinedVertices = (((2 * t3) - (3 * t2) + 1) * *pSecondFrameVertices) +
                     ((t3 - (2 * t2) + t) * m0) +
                     ((t3 - t2) * m1) +
                     (((-2 * t3) + (3 * t2)) * *pThirdFrameVertices);
   }
}

