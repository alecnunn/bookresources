// FILE: CSpawnPoint.h 

#ifndef CSPAWNPOINT_H
#define CSPAWNPOINT_H

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
class CSpawnPoint : public CLevelObject {
   public:
      CSpawnPoint(void);
      virtual ~CSpawnPoint(void);

      void      CopyOf(CLevelObject*);
      
      void      Render(ZFXVertexCacheManager*,bool);
      void      Translate(const ZFXVector&);

      bool      Picked(const ZFXRay &Ray, float *pfD)
                     { return m_Mesh.Picked(Ray, pfD); }

      bool      Intersects(const ZFXAabb &b, ZFXAXIS a)
                     { return m_Mesh.Intersects(b, a); }

      ZFXVector GetPosition(void) { return m_vcPos;  }
      void      SetPosition(const ZFXVector&);

      void      Save(FILE*);
      bool      Load(FILE*);

   private:
      CPolymesh m_Mesh;
      ZFXVector m_vcPos;

      void      Init(void);
      void      Release(void);

   }; // class
typedef class CSpawnPoint *LPSPAWNPOINT;
/*----------------------------------------------------------------*/


#endif
