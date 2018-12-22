// File: ZFXMovementController.cpp

#include "ZFXGeneral.h"
#pragma comment(lib, "zfx3d.lib")
#pragma comment(lib, "winmm.lib")


FILE* pLogg=NULL;


/*----------------------------------------------------------------*/
/* ZFXMovementController class implementation                     *
/*----------------------------------------------------------------*/

// Just initialize some values.
ZFXMovementController::ZFXMovementController(void) {
   pLogg = fopen("mclog.txt", "w");
   Init();
   } // constructor
/*----------------------------------------------------------------*/


// Here we have nothing to do at all.
ZFXMovementController::~ZFXMovementController(void) {
   if (pLogg) fclose(pLogg);
   } // destuctor
/*----------------------------------------------------------------*/


// Just initialize some values.
void ZFXMovementController::Init(void) {
   m_vcPos.Set(0.0f, 0.0f, 0.0f);
   m_vcRight.Set(1.0f, 0.0f, 0.0f);
   m_vcUp.Set(0.0f, 1.0f, 0.0f);
   m_vcDir.Set(0.0f, 0.0f, 1.0f);
   m_vcV.Set(0.0f, 0.0f, 0.0f);
   m_fRotX = m_fRotY = m_fRotZ = m_fThrust = 0.0f;
   m_fRollSpd = m_fPitchSpd = m_fYawSpd = 0.0f;
   m_Quat.x = m_Quat.y = m_Quat.z = 0.0f;
   m_Quat.w = 1.0f;
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Recalculate the orientation of the local axes according to the
 * current rotation values around the axes.
 */
void ZFXMovementController::RecalcAxes(void) {
   ZFXQuat    qFrame;
   ZFXMatrix  mat;

   static float f2PI = 6.283185f;

   // keep in range of 360 degree
   if (m_fRotX > f2PI) m_fRotX -= f2PI;
   else if (m_fRotX < -f2PI) m_fRotX += f2PI;
   
   if (m_fRotY > f2PI) m_fRotY -= f2PI;
   else if (m_fRotY < -f2PI) m_fRotY += f2PI;
   
   if (m_fRotZ > f2PI) m_fRotZ -= f2PI;
   else if (m_fRotZ < -f2PI) m_fRotZ += f2PI;

   // build new quaternion for this rotation
   qFrame.MakeFromEuler(m_fRotX, m_fRotY, m_fRotZ);

   // add to existing rotation
   m_Quat *= qFrame;

   // extract the local axes
   m_Quat.GetMatrix(&mat);

   m_vcRight.x = mat._11;
   m_vcRight.y = mat._21;
   m_vcRight.z = mat._31;

   m_vcUp.x    = mat._12;
   m_vcUp.y    = mat._22;
   m_vcUp.z    = mat._32;

   m_vcDir.x   = mat._13;
   m_vcDir.y   = mat._23;
   m_vcDir.z   = mat._33;
   } // RecalcAxes
/*----------------------------------------------------------------*/


