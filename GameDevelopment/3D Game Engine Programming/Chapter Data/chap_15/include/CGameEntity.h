// FILE: CGameEntity.h

#ifndef CGameEntity_H
#define CGameEntity_H


#include <windows.h>
#include <stdio.h>
#include "zfx.h"
#include "CGamePortal.h"
#include "CGameLevel.h"

class CGameLevel;
class CGamePortal;

class CGameEntity {
   public:
      CGameEntity(void);
      virtual ~CGameEntity(void);

      virtual HRESULT Render(ZFXRenderDevice*)=0;
      virtual void    Update(float)=0;
      virtual bool    TouchAndUse(const ZFXVector&)=0;
      virtual bool    TestCollision(const ZFXAabb&, ZFXPlane*)=0;
      virtual bool    TestCollision(const ZFXRay&, float, float*)=0;

      virtual bool    Load(FILE*);
      virtual ZFXAabb GetAabb(void) { return m_Aabb; }
      virtual IsOfType(ZFXENTITY e) { return (e==m_Type); }

   protected:
      ZFXENTITY   m_Type;
      ZFXAabb     m_Aabb;
      VERTEX     *m_pVerts;
      WORD       *m_pIndis;
      UINT        m_NumVerts;
      UINT        m_NumIndis;
      UINT        m_nSkin;

   }; // class
typedef class CGameEntity *LPGAMEENTITY;
/*----------------------------------------------------------------*/


class CGameDoor : public CGameEntity {
   public:
      CGameDoor(void);
      virtual ~CGameDoor(void);

      virtual HRESULT Render(ZFXRenderDevice*);
      virtual void    Update(float);
      virtual bool    Load(FILE*);

      virtual bool    TouchAndUse(const ZFXVector&);
      virtual bool    TestCollision(const ZFXAabb&, ZFXPlane*);
      virtual bool    TestCollision(const ZFXRay&, float, float*);
      virtual bool    IsActive(void) { return m_bActive; }

      virtual bool    ConnectToPortals(CGameLevel*);

   private: 
      ZFXVector    m_vcT;
      ZFXAXIS      m_Axis;
      float        m_fSign;
      float        m_fTime;
      float        m_fDist;
      float        m_fPause;
      bool         m_bActive;
      bool         m_bOpening;
      bool         m_bPausing;
      UINT         m_Portal[2];
      CGamePortal* m_pPortal_A;
      CGamePortal* m_pPortal_B;

      bool        LoadMesh(FILE *pFile);

   }; // class
typedef class CGameDoor *LPGAMEDOOR;
/*----------------------------------------------------------------*/


#endif