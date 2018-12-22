// File: ZFXMovementController.h
#ifndef ZFXMC_H
#define ZFXMC_H

/**
 * Base class for all types of controllers to inherit from. This already
 * implements a very basic camera that is unaware of its surrounding
 * but able to move totally free like in zero-g space. 
 */
class ZFXMovementController {
   public:
      ZFXMovementController();
      virtual ~ZFXMovementController();

      virtual void Update(float fElapsedTime)=0;

      // accessor methods
      ZFXVector GetPos(void)       { return m_vcPos;   }
      ZFXVector GetRight(void)     { return m_vcRight; }
      ZFXVector GetUp(void)        { return m_vcUp;    }
      ZFXVector GetDir(void)       { return m_vcDir;   }
      ZFXVector GetVelocity(void)  { return m_vcV;     }

   protected:
      ZFXVector m_vcPos;      // position
      ZFXVector m_vcRight;    // right vector
      ZFXVector m_vcUp;       // up vector
      ZFXVector m_vcDir;      // direction vector (look direction)
      ZFXVector m_vcV;        // velocity vector (movement direction)
      ZFXQuat   m_Quat;       // quaternion for rotation

      // rotation speed around local vectors
      float     m_fRollSpd;
      float     m_fPitchSpd;
      float     m_fYawSpd;

      float     m_fRollSpdMax;
      float     m_fPitchSpdMax;
      float     m_fYawSpdMax;

      // rotation value around local vectors
      float     m_fRotX;
      float     m_fRotY;
      float     m_fRotZ;

      // other stuff
      float m_fThrust;

      // protected methods
      virtual void RecalcAxes(void);
      virtual void Init(void);
   };
/*----------------------------------------------------------------*/

#endif
