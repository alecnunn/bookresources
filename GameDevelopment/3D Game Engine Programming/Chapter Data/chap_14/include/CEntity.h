// FILE: CEntity.h 

#ifndef CENTITY_H
#define CENTITY_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderer.h"
#include "CLevelObject.h"
#include "CPolyMesh.h"




// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * 
 */
class CEntity : public CLevelObject {
   public:
      CEntity(void);
      virtual ~CEntity(void);

      void       CopyOf(CLevelObject*);

      void       Render(ZFXVertexCacheManager*,bool);
      void       Translate(const ZFXVector&);

      bool       Picked(const ZFXRay&, float*);
      bool       Intersects(const ZFXAabb&, ZFXAXIS);

      void       Save(FILE*);
      bool       Load(FILE*);

      ZFXENTITY  GetSubType(void) { return m_SubType; }
      void       SetSubType(ZFXENTITY);
      void       SetGeometry(const CPolymesh *pMesh);
      void       SetData(const ENTITYDOOR&);
      void       SetData(const ENTITYPLAYER&);
      void*      GetData(void) { return m_pData; }

      void       RemovePortalConnections(UINT);
      bool       ConnectTo( CPortal* );

   private:
      ZFXENTITY  m_SubType;   // door, player, ...
      CPolymesh  m_Mesh;      // polymesh object
      void      *m_pData;     // info struct
      UINT       m_nSize;     // size of pData

      void       Init(void);
      void       Release(void);
   }; // class
typedef class CEntity *LPENTITY;
/*----------------------------------------------------------------*/


#endif
