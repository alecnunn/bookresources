// File: ZFXMCEgo.cpp

#include "ZFXGeneral.h"

/*----------------------------------------------------------------*/
/* ZFXMCEgo class implementation                                  *
/*----------------------------------------------------------------*/

// Just initialize some values.
ZFXMCEgo::ZFXMCEgo(void) {
   ZFXMovementController::Init();
   m_fSpeed = 0;
   m_fSlide = 0;
   } // constructor
/*----------------------------------------------------------------*/

// Just some destruction work
ZFXMCEgo::~ZFXMCEgo(void) {

   } // destructor
/*----------------------------------------------------------------*/


// Set rotation values and recalculate local vectors.
void ZFXMCEgo::SetRotation(float rx, float ry, float rz) {
   m_fRotX = rx;
   m_fRotY = ry;
   m_fRotZ = rz;

   RecalcAxes();
   } // SetRotation
/*----------------------------------------------------------------*/

/**
 * Calculate movement based on behavior model and attribute values.
 * Input parameter is elapsed time since last frame.
 */
void ZFXMCEgo::Update(float fET) {
   ZFXVector vcS;
   // add rotation speed
   m_fRotX += (m_fPitchSpd * fET);
   m_fRotY += (m_fYawSpd   * fET);
   m_fRotZ += (m_fRollSpd  * fET);

   // recalculate axes
   RecalcAxes();

   // calculate velocity vectors
   m_vcV = m_vcDir * m_fSpeed * fET;
   vcS   = m_vcRight * m_fSlide * fET;

   // move position according to velocity vector
   m_vcPos += m_vcV + vcS;
   } // Update
/*----------------------------------------------------------------*/


/**
 *
 */
void ZFXMCEgo::RecalcAxes(void) {
   ZFXMatrix  mat;

   static float f2PI = 6.283185f;

   // keep in range of 360 degree
   if (m_fRotY > f2PI) m_fRotY -= f2PI;
   else if (m_fRotY < -f2PI) m_fRotY += f2PI;
   
   // dont look up/down more than 80 degrees
   if (m_fRotX > 1.4f) m_fRotX = 1.4f;
   else if (m_fRotX < -1.4f) m_fRotX = -1.4f;


   // initialize axes
   m_vcRight = ZFXVector(1.0f, 0.0f, 0.0f);
   m_vcUp    = ZFXVector(0.0f, 1.0f, 0.0f);
   m_vcDir   = ZFXVector(0.0f, 0.0f, 1.0f);

   // rotate around Y axis
   mat.RotaArbi(m_vcUp, m_fRotY);
   m_vcRight = m_vcRight * mat;
   m_vcDir   = m_vcDir * mat;
 
   // rotate around X axis
   mat.RotaArbi(m_vcRight, m_fRotX);
   m_vcUp  = m_vcUp * mat;
   m_vcDir = m_vcDir * mat;

   // fight rounding errors
   m_vcDir.Normalize();
   m_vcRight.Cross(m_vcUp, m_vcDir);
   m_vcRight.Normalize();
   m_vcUp.Cross(m_vcDir, m_vcRight);
   m_vcUp.Normalize();
   } // RecalcAxes
/*----------------------------------------------------------------*/


/**
 *
 */
void ZFXMCEgo::GetRotation(float *pfX, float *pfY, float *pfZ) {
   if (pfX) *pfX = m_fRotX;
   if (pfY) *pfY = m_fRotY;
   if (pfZ) *pfZ = m_fRotZ;
   } // GetRotation
/*----------------------------------------------------------------*/


/**
 *
 */
ZFXVector ZFXMCEgo::GetRotation(void) {
   return ZFXVector( m_fRotX,
                     m_fRotY,
                     m_fRotZ );
   } // GetRotation
/*----------------------------------------------------------------*/