// File: ZFXMCAircraft.h
#ifndef ZFXMCAIRCRAFT_H
#define ZFXMCAIRCRAFT_H


const float rho = 0.0023769f; // desity of air at sea level, slugs/ft^3

const float fRadToDeg  = 180.f/3.141592654f; // radian to degree
const float fDegToRad  = 3.141592654f/180.f; // degree to radian
const float fFtToMtr   = 0.3048006f;         // feet to meter
const float fMtrToFt   = 1.0f / 0.3048006f;  // meter to feet
const float fLbsToNwt  = 4.448f;             // pounds to newton
const float fSlugToLbs = 32.17f;             // slug to pounds
const float fSlugToKg  = 14.59390f;          // slug to kg
const float fLbsToSlug = 1.0f / 32.17f;      // pounds to slug
const float fKgToSlug  = 1.0f / 14.59390f;   // kilogram to slugs


// maximum deflections in degrees
const int MAX_AILERON  = 20;
const int MAX_RUDDER   = 30;
const int MAX_ELEVATOR = 25;


// deflection of control surfaces in degrees
typedef struct ZFXCTRLSRFC_TYPE {
   int nAilrL;
   int nAilrR;
   int nElvrL;
   int nElvrR;
   int nRddrL;
   int nRddrR;
   int nFlaps;
   } ZFXCTRLSRFC;
/*----------------------------------------------------------------*/

// tables holding all the nasty data
typedef struct ZFXMCATABLE_TYPE {
   float D[12][9];      // damping data
   float T[6][6];       // thrust
   float Cx[12][5];     // direction aerodyn. force
   float Cz[12];        // downwards aerodyn. force
   float Cm[12][5];     // pitching momentum
   float Cl[12][7];     // rolling momentum
   float Cn[12][7];     // yawing momentum
   float ClA[12][7];    // roll due to aileron
   float ClR[12][7];    // roll due to rudder
   float CnA[12][7];    // yaw due to aileron
   float CnR[12][7];    // yaw due to rudder
   } ZFXMCATABLE;

/**
 * Base class for all types of controllers to inherit from. This already
 * implements a very basic camera that is unaware of its surrounding
 * but able to move totally free like in zero-g space. 
 */
class ZFXMCAircraft : public ZFXMovementController  {
   public:
   	ZFXMCAircraft(void);
   	virtual ~ZFXMCAircraft(void);

      virtual void SetStartParams(ZFXVector vcPos,   // position
                                  float fThrust)=0;  // thrust

      void SetInertia(float Ixx, float Iyy,
                      float Izz, float Ixz);

      void SetAircraftParams(float fSpan,     // span in feet
                             float fChord,    // mean aerodyn.chord
                             float fS,        // reference area
                             float fXCG,      // x from CG
                             float fXCGR,     // relative to chord
                             float fMass);    // in lbs

      virtual void Update(float fElapsedTime);

      float GetSpeedKts(void)  { return (m_vcV.GetLength()/1.688f); }
      float GetThrustLbs(void) { return m_fThrust; }
      float GetRollDeg(void)   { return (m_fRoll*fRadToDeg); }
      float GetYawDeg(void)    { return (m_fYaw*fRadToDeg); }
      float GetPitchDeg(void)  { return (-m_fPitch*fRadToDeg); }
      float GetAOA(void)       { return m_fAOA*fRadToDeg; }

      ZFXVector GetVelocity(void) { return m_vcV; }

      void  IncThrust(void);
      void  DecThrust(void);  
//      void   LeftRudder(void);
//      void   RightRudder(void);
      void  RollLeft(float fIntensity);
      void  RollRight(float fIntensity);
      void  PitchUp(float fIntensity);
      void  PitchDown(float fIntensity);
      void  FlapsDown(int nDegrees);
      void  ZeroAilerons(void);
      void  ZeroElevators(void);
      void  ZeroFlaps(void);
      void  ZeroRudder(void);
      void  ZeroCtrlSurfs(void);

   protected:
      ZFXCTRLSRFC    m_CSrfc;             // surface settings
      ZFXMCATABLE    m_tbl;               // tables featuring data
      ZFXVector      m_vcPosFt;           // position in local system
      ZFXVector      m_vcVFt;             // velocity in feet/sec
      float          m_fS;                // reference area
      float          m_fMass;             // total aircraft mass
      float          m_fVT;               // velocity in ft/s
      float          m_fAOA;              // angle of attack
      float          m_fBeta;             // beta angle
      float          m_fAlt;              // alt in ft
      float          m_fPower;            // engine power 0-1
      float          m_fRoll;             // roll angle in rad
      float          m_fPitch;            // pitch angle in rad
      float          m_fYaw;              // yaw angle in rad
      float          m_fG;                // G force acting on plane
      float          m_p, m_q, m_r;       // roll, pitch, yaw rate (rad)
      float          m_fSpan;             // wing span in feet
      float          m_fcbar;             // mean aerodynamic chord
      float          m_fC[10];            // coeffs c1-c9 (c0 unused)
      float          m_fXCG, m_fXCGR;     // center of gravity (rel.to chord)

      // initalize and release (for con-/destructor)
      virtual void Init(void);
      virtual void Release(void);

      void  CalcLoads(float fET, int nAilr, int nElvr, int nRddr);
      void  CalcAtmosphere(float *fMach, float *fQBar);
      void  CalcDamp(float fAOA, float *pdamp);
      float CalcThrust(float fPower, float fMach, float fAlt);

      float CalcCx(float fAOA,  int nElevator);
      float CalcCy(float fBeta, int nAileron, int nRudder);
      float CalcCz(float fAOA,  float fBeta, int nElevator);
      float CalcCl(float fAOA,  float fBeta);
      float CalcCm(float fAOA,  int nElevator);
      float CalcCn(float fAOA,  float fBeta);
      
      float CalcClDueToAileron(float fAOA, float fBeta);
      float CalcClDueToRudder (float fAOA, float fBeta);
      float CalcCnDueToAileron(float fAOA, float fBeta);
      float CalcCnDueToRudder (float fAOA, float fBeta);

      // virtual stuff, will be called by ZFXMCAicraft::Init
      virtual void SetAllTables(void)=0;

      // interpolate the coefficient tables
      float Interpolate_12x5(float fTbl[][5],  float fAOA, int nElvr);
      float Interpolate_12x7(float fTbl[][7], float fAOA,  float fBeta);

      float RudderLiftCoefficient(float fAOA);
      float RudderDragCoefficient(float fAOA);
      float LiftCoefficient(float fAOA, int nFlaps);
      float DragCoefficient(float fAOA, int nFlaps);
   };
/*----------------------------------------------------------------*/

#endif
