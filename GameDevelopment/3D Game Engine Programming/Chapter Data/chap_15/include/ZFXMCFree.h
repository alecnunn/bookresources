// File: ZFXMCFree.h
#ifndef ZFXMCFREE_H
#define ZFXMCFREE_H


#include "ZFXGeneral.h"

class ZFXMCFree : public ZFXMovementController {
   public:
   	ZFXMCFree(void);
   	virtual ~ZFXMCFree(void);

      virtual void Update(float fElapsedTime);

      // use this functions to influence movement
      void AddRotationSpeed(float sx, float sy, float sz);
      void SetRotationSpeed(float sx, float sy, float sz);
      void SetRotationSpeedX(float f) { m_fPitchSpd = f; }
      void SetRotationSpeedY(float f) { m_fYawSpd   = f; }
      void SetRotationSpeedZ(float f) { m_fRollSpd  = f; }
      void AddThrust(float fThrust)   { m_fThrust += fThrust; }
      void SetThrust(float fThrust)   { m_fThrust  = fThrust; }

      // set attributes directly (avoid this except for initialization)
      void SetRotation(float rx, float ry, float rz);
      void SetPos(ZFXVector &vc)   { memcpy(&m_vcPos, &vc, sizeof(ZFXVector));   }
      void SetRight(ZFXVector &vc) { memcpy(&m_vcRight, &vc, sizeof(ZFXVector)); }
      void SetUp(ZFXVector &vc)    { memcpy(&m_vcUp,  &vc, sizeof(ZFXVector));   }
      void SetDir(ZFXVector &vc)   { memcpy(&m_vcDir, &vc, sizeof(ZFXVector));   }
   };
/*----------------------------------------------------------------*/

#endif
