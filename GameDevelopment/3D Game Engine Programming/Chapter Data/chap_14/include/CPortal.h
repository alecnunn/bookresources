// FILE: CPortal.h 

#ifndef CPORTAL_H
#define CPORTAL_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderer.h"
#include "CLevelObject.h"


class CSector;


// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * A Portal is a 4 sided polygon which is not visible in the game.
 * Here we need an ID to keep track of the polygons after saving
 * as they are connecting to each other and those connections
 * need to be maintained properly.
 */
class CPortal : public CLevelObject {
   public:
      CPortal(void);
      virtual ~CPortal(void);

      void      CopyOf(CLevelObject*);
      void      SetGeometry(const VERTEX*, const WORD*,
                            const ZFXAabb&);

      void      Render(ZFXVertexCacheManager*,bool);
      bool      Picked(const ZFXRay&, float*);
      bool      Intersects(const ZFXAabb&, ZFXAXIS);
      void      Translate(const ZFXVector&) { /* no action */; }

      // connectivity stuff
      UINT      GetID(void)               { return m_ID;       }
      UINT      GetItsTargetID(void)      { return m_BID;      }
      CSector*  GetItsSector(void)        { return m_pDad;     }
      CPortal*  GetItsTarget(void)        { return m_pBrother; }
      void      SetID(UINT ID)            { m_ID = ID;         }
      void      SetItsSector(CSector *pS) { m_pDad = pS;       }
      void      SetItsTarget(CPortal *pP);

      void      DeconnectFrom(UINT id)    { if(m_BID==id)m_BID=0; }

      void      Save(FILE*);
      bool      Load(FILE*);

   private:
      CSector*  m_pDad;           // sector which it belongs to
      CPortal*  m_pBrother;       // portal which it connects to
      UINT      m_ID;             // portal ID
      UINT      m_BID;            // brother ID
      VERTEX   *m_pVerts;         // array of vertices
      WORD     *m_pIndis;         // array of indices
      WORD     *m_pLineIndis;     // array of polyline indices

      bool      m_bVisible;       // is other side visible

      void      Init(void);
      void      Release(void);
   }; // class
typedef class CPortal *LPPORTAL;
/*----------------------------------------------------------------*/


#endif
