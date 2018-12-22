// FILE: CGameLight.h

#ifndef CGAMELIGHT_H
#define CGAMELIGHT_H


#include <windows.h>
#include <stdio.h>
#include "zfx.h"
#include "ZFXRenderer.h"
#include "ZFXRenderDevice.h"


#define SAFE_DELETE(p)   { if(p!=NULL) { delete (p);    (p)=NULL; } }
#define SAFE_DELETE_A(p) { if(p!=NULL) { delete [] (p); (p)=NULL; } }
#define SAFE_FREE(p)     { if(p!=NULL) { free(p);       (p)=NULL; } }


typedef struct LIGHTCACHE_TYPE {
   WORD *pIndis;
   UINT  nSkin;
   UINT  NumIndis;
   UINT  ID;
   } LIGHTCACHE;
/*----------------------------------------------------------------*/


class CGameLight {
   public:
      CGameLight(void);
      virtual ~CGameLight(void);

      bool        Load(FILE*,UINT);

      bool        InfluencesTri(const ZFXVector*);
      bool        InfluencesSkin(UINT nSkin, UINT *pIBID);
      bool        InfluencesAabb(const ZFXAabb&);
      HRESULT     AddTriIndices(WORD,WORD,WORD,const ZFXVector*,UINT);
      HRESULT     CloseCaches(ZFXRenderDevice*);
      ZFXVector   GetPosition(void)     { return m_vcPos;    }
      ZFXCOLOR    GetColor(void)        { return m_Clr;      }
      float       GetRadius(void)       { return m_fR;       }
      float       GetSqrRadius(void)    { return m_fR2;      } 
      ZFXAabb     GetAabb(void)         { return m_Aabb;     }
      bool        IsFlickering(void)    { return m_bFlicker; }
      bool        IsShadowCasting(void) { return m_bShadow;  }

   private:
      ZFXAabb     m_Aabb;       // light aabb
      ZFXVector   m_vcPos;      // light position
      ZFXCOLOR    m_Clr;        // light color
      LIGHTCACHE *m_pCache;     // light cache
      float       m_fR;         // light radius
      float       m_fR2;        // squared radius
      bool        m_bFlicker;   // flickering light?
      bool        m_bShadow;    // casting shadow no light?
      UINT        m_NumSkins;   // max skins influenced

      bool      LoadAndSkipMeshData(FILE*);
   }; // class
typedef class CGameLight *LPGAMELIGHT;
/*----------------------------------------------------------------*/


#endif