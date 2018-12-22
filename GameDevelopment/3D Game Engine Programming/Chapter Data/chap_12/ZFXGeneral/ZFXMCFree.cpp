// File: ZFXMCFree.cpp

#include "ZFXGeneral.h"

/*----------------------------------------------------------------*/
/* ZFXMCFree class implementation                                 *
/*----------------------------------------------------------------*/

// Just initialize some values.
ZFXMCFree::ZFXMCFree(void) {
   ZFXMovementController::Init();
   } // constructor
/*----------------------------------------------------------------*/

// Just some destruction work
ZFXMCFree::~ZFXMCFree(void) {

   } // destructor
/*----------------------------------------------------------------*/


// Set rotation values and recalculate local vectors.
void ZFXMCFree::SetRotation(float rx, float ry, float rz) {
   m_fRotX = rx;
   m_fRotY = ry;
   m_fRotZ = rz;

   RecalcAxes();
   } // SetRotation
/*----------------------------------------------------------------*/


// add speed to the rotation around the axes
void ZFXMCFree::AddRotationSpeed(float sx, float sy, float sz) { 
   m_fPitchSpd += sx; 
   m_fYawSpd   += sy;
   m_fRollSpd  += sz;
   } // AddRotationSpeed
/*----------------------------------------------------------------*/


// set the rotation speed around the axes to this values
void ZFXMCFree::SetRotationSpeed(float sx, float sy, float sz) { 
   m_fPitchSpd = sx; 
   m_fYawSpd   = sy;
   m_fRollSpd  = sz;
   } // SetRotationSpeed
/*----------------------------------------------------------------*/
 

/**
 * Calculate movement based on behavior model and attribute values.
 * Input parameter is elapsed time since last frame.
 */
void ZFXMCFree::Update(float fET) {
   // add rotation speed
   m_fRotX = (m_fPitchSpd * fET);
   m_fRotY = (m_fYawSpd   * fET);
   m_fRotZ = (m_fRollSpd  * fET);

   // calculate velocity vector
   m_vcV = m_vcDir * m_fThrust * fET;

   // move position according to velocity vector
   m_vcPos += m_vcV;

   // recalculate axes
   RecalcAxes();
   } // Update
/*----------------------------------------------------------------*/

