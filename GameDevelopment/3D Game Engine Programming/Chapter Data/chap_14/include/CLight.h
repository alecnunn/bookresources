// FILE: CLight.h 

#ifndef CLIGHT_H
#define CLIGHT_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderer.h"
#include "CLevelObject.h" 
#include "CGeometry.h"
#include "CPolymesh.h" 



// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * 
 */
class CLight : public CLevelObject {
   public:
      CLight(void);
      virtual ~CLight(void);

      void      CopyOf(CLevelObject*);
      
      void      Render(ZFXVertexCacheManager*,bool);
      void      Translate(const ZFXVector&);

      bool      Picked(const ZFXRay &Ray, float *pfD)
                     { return m_Mesh.Picked(Ray, pfD); }

      bool      Intersects(const ZFXAabb &b, ZFXAXIS a)
                     { return m_Mesh.Intersects(b, a); }

      ZFXVector GetPosition(void) { return m_vcPos;  }
      void      SetPosition(const ZFXVector&);

      void      SetColor(const ZFXCOLOR &clr);
      void      SetColor(float fR, float fG, float fB);
      ZFXCOLOR  GetColor(void) { return m_Clr; }

      void      SetRadius(float fR) { m_fR = fR; }
      float     GetRadius(void)     { return m_fR; }

      void      SetFlickering(bool b) { m_bFlicker = b; }
      bool      IsFlickering(void)    { return m_bFlicker; }

      void      SetShadowCasting(bool b);
      bool      IsShadowCasting(void) { return m_bShadow; }

      void      Save(FILE*);
      bool      Load(FILE*);

   private:
      CPolymesh m_Mesh;
      ZFXVector m_vcPos;
      ZFXCOLOR  m_Clr;
      float     m_fR;
      bool      m_bFlicker;
      bool      m_bShadow;

      void      Init(void);
      void      Release(void);

   }; // class
typedef class CLight *LPLIGHT;
/*----------------------------------------------------------------*/


#endif
