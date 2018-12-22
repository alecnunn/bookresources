// FILE: CGameLevel.h

#ifndef CGAMELEVEL_H
#define CGAMELEVEL_H


#include <windows.h>
#include "CGameSector.h"

class CGameSector;
class CGamePortal;

class CGameLevel {
   public:
      CGameLevel(ZFXRenderDevice*);
      virtual ~CGameLevel(void);
      
      void    Reset(void);
      bool    Load(const char*);
      bool    FindBrother(CGamePortal*);
      void    BroadcastShaders(UINT,UINT,UINT,UINT,UINT,UINT);
      void    Update(float);
      void    TouchAndUse(const ZFXVector&);

      HRESULT Render(UINT Sector, UINT *Tris, UINT *Lights, 
                     const ZFXVector &vcPos);

      HRESULT RenderShadows(UINT Sector, const ZFXVector &vcPos);

      void    RenderOctree(bool,bool);
      void    RenderPortals(const ZFXVector&, bool);

      bool    GetSpawnPoint(UINT*, ZFXVector*);

      bool    GetFloor(UINT nSectorID, 
                       const ZFXVector &EyePos,
                       float *fDistToFloor, 
                       ZFXPlane *FloorPlane);

      bool    TestCollision(UINT *nSectorID, 
                            const ZFXAabb &Objekt,
                            ZFXPlane *pP,
                            bool *pbIsDoor);

      bool    TestCollision(UINT *nSectorID, 
                            const ZFXRay &Ray, 
                            float fRayLength, 
                            float *pfHitDistance,
                            bool *pbIsDoor);

      CGamePortal* GetPortal(UINT);

      // debug only
      bool AllPortalsConnectedOk(void);

   private:
      LPZFXRENDERDEVICE  m_pDevice;
      CGameSector      **m_pSectors;
      UINT               m_NumSectors;
      UINT               m_PSH;        // shader for depth only
      UINT               m_VSH;        // shader for depth only
      UINT               m_NumSpawns;  // spawnpoints

      void Init(ZFXRenderDevice*);
      void Release();

      void LoadSkin(ZFXSkinManager*,FILE*);
      void BuildAllPortalRelationships(void);
   }; // class
/*----------------------------------------------------------------*/


#endif