// FILE: CGamePortal.h

#ifndef CGAMEPORTAL_H
#define CGAMEPORTAL_H


#include <windows.h>
#include <stdio.h>
#include "CGameLight.h"    // SAFE_X defines
#include "ZFXRenderDevice.h"
#include "CGameSector.h"
#include "zfx.h"

class CGameSector;

class CGamePortal {
   public:
      CGamePortal(void);
      virtual ~CGamePortal(void);

      bool Load(FILE*);

      bool BuildFrustum(float,float,ZFXPlane*);

      void Render(ZFXVertexCacheManager *pVCM, const ZFXVector&);

      bool         IsBackfacingTo(const ZFXVector&);
      bool         IsOnBackside(const ZFXVector&);
      bool         IsOpen(void)          { return m_bOpen;    }

      void         SetVisibility(bool b) { m_bOpen=b;         }

      UINT         GetID(void)           { return m_ID;       }
      UINT         GetBrotherID(void)    { return m_BID;      }
      ZFXAabb      GetAabb(void)         { return m_Aabb;     }
      CGamePortal* GetBrother(void)      { return m_pBrother; }
      CGameSector* GetSector(void)       { return m_pSector;  }
      CGameSector* GetTargetSector(void);
      UINT         GetSectorIndex(void);
      UINT         GetTargetSectorIndex(void);

      void         ChopFrustum(ZFXPlane*);
      bool         IntersectsDownwardsRay(const ZFXVector&, float);

      void SetSector(CGameSector *pS) { m_pSector = pS; }
      void SetBrotherRelationship(CGamePortal*);


   private:
      ZFXPlane     m_Plane;         // portal's plane
      ZFXAabb      m_Aabb;          // bounding box
      UINT         m_ID;            // portal ID
      UINT         m_BID;           // brother ID
      CGamePortal *m_pBrother;      // pointer to brother
      CGameSector *m_pSector;       // sector this one sits in
      ZFXVector    m_vcCenter;      // portal center point

      UINT         m_NumVerts;      // number of verts
      UINT         m_NumIndis;      // number of indices
      VERTEX      *m_pVerts;        // array of vertices
      WORD        *m_pIndis;        // array of indices
      bool         m_bOpen;         // portal open
   }; // class
typedef class CGamePortal *LPGAMEPORTAL;
/*----------------------------------------------------------------*/


#endif