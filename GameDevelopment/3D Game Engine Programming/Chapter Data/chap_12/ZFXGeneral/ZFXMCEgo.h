// File: ZFXMCEgo.h
#ifndef ZFXMCEGO_H
#define ZFXMCEGO_H


#include "ZFXGeneral.h"

class ZFXMCEgo : public ZFXMovementController {
   public:
   	ZFXMCEgo(void);
   	virtual ~ZFXMCEgo(void);

      virtual void Update(float fElapsedTime);

      void      GetRotation(float *fX, float *fY, float *fZ);
      ZFXVector GetRotation(void);

      void SetRotationSpeedX(float f) { m_fPitchSpd = f; }
      void SetRotationSpeedY(float f) { m_fYawSpd   = f; }
      void SetSpeed(float a)          { m_fSpeed    = a; }
      void SetSlideSpeed(float a)     { m_fSlide    = a; }

      // set attributes directly (avoid this except for initialization)
      void SetRotation(float rx, float ry, float rz);
      void SetPos(ZFXVector &vc)   { memcpy(&m_vcPos, &vc, sizeof(ZFXVector));   }
      void SetRight(ZFXVector &vc) { memcpy(&m_vcRight, &vc, sizeof(ZFXVector)); }
      void SetUp(ZFXVector &vc)    { memcpy(&m_vcUp,  &vc, sizeof(ZFXVector));   }
      void SetDir(ZFXVector &vc)   { memcpy(&m_vcDir, &vc, sizeof(ZFXVector));   }

   private:
      float m_fSpeed;
      float m_fSlide;

      void RecalcAxes(void);
   };
/*----------------------------------------------------------------*/

#endif
