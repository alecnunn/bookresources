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
 *  AtiQuaternion.h -- Header for quaternions
 ******************************************************************************
 *  Written by Alex Vlachos (Alex@Vlachos.com)
 ******************************************************************************/

#ifndef __ATIQUATERNION__H
#define __ATIQUATERNION__H

#include "Types.h"

// DEFINES ====================================================================
#ifndef ATI_PI
 #define ATI_PI 3.1415926535897932384626433832795f
#endif
#ifndef AtiDegToRad
 #define AtiDegToRad(D) ((D) * (ATI_PI/180.0f))
#endif
#ifndef AtiRadToDeg
 #define AtiRadToDeg(R) ((R) * (180.0f/ATI_PI))
#endif

// STRUCTURE DEFINITIONS ======================================================
typedef union
{
   //Point in 3D space
   struct { float32 x, y, z, w; };
   struct { float32 q[4]; };
   struct { float32 v[4]; }; //Makes for easier porting of code
} AtiQuaternion;

typedef float32 AtiMatrix[16];

// FUNCTION DEFS ==============================================================
void AtiQuatNormalize (AtiQuaternion *quat);
void AtiQuatToMatrix (AtiQuaternion *quat, AtiMatrix matrix);
void AtiMatrixToQuat (AtiMatrix matrix, AtiQuaternion *quat);
void AtiQuaternionToGlRotation (AtiQuaternion *quat, float32 *x, float32 *y, float32 *z, float32 *degrees);
void AtiGlRotationToQuaternion (AtiQuaternion *quat, float32 x, float32 y, float32 z, float32 angle);
void AtiQuatMul (AtiQuaternion *q1, AtiQuaternion *q2, AtiQuaternion *result);
void AtiQuatCopy (AtiQuaternion *q1, AtiQuaternion *q2);
void AtiQuatLog (AtiQuaternion *q1, AtiQuaternion *q2);
float32 AtiQuatDot (AtiQuaternion *q1, AtiQuaternion *q2);
void AtiQuatInverse (AtiQuaternion *q, AtiQuaternion *result);
void AtiQuatExp (AtiQuaternion *q1, AtiQuaternion *q2);
void AtiQuatSlerpShortestPath (AtiQuaternion *result, AtiQuaternion *from, AtiQuaternion *to, float32 t);
void AtiQuatSlerp (AtiQuaternion *result, AtiQuaternion *from, AtiQuaternion *to, float32 t);
void AtiQuatSQuad (float32 t, AtiQuaternion *q0, AtiQuaternion *q1, AtiQuaternion *q2, AtiQuaternion *q3, AtiQuaternion *result);

void AtiQuatAxBxC (AtiQuaternion *q1, AtiQuaternion *q2, AtiQuaternion *q3, AtiQuaternion *result);
float32 AtiDeterminant3x3(float32 det[3][3]);

#endif // __ATIQUATERNION__H
