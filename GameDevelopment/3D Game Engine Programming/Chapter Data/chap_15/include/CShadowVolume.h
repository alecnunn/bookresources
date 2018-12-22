// FILE: CShadowVolume.h

#ifndef CSHADOWVOLUME_H
#define CSHADOWVOLUME_H


#include <windows.h>
#include "CGameLight.h"
#include "ZFXRenderDevice.h"
#include "zfx.h"


typedef struct TRIANGLE_TYPE {
   VERTEX *pV0;
   VERTEX *pV1;
   VERTEX *pV2;
   ZFXVector vcN;
   } TRIANGLE;


class CShadowVolume {
   public:
      CShadowVolume(void);
      virtual ~CShadowVolume(void);

      HRESULT Init(const void*, UINT, const WORD*,
                   UINT, UINT nVertexSize, ZFXAabb, 
                   ZFXRenderDevice*);

      HRESULT UpdateGeometry(const void*, const WORD*, ZFXAabb);

      HRESULT Render( CGameLight* );


   private:
      ZFXRenderDevice *m_pDevice;
      ZFXAabb          m_Aabb;

      // original mesh
      TRIANGLE        *m_pTris;
      BYTE            *m_pVerts;
      WORD            *m_pIndis;
      WORD            *m_pEdges;
      int             *m_pAdjacency;
      bool            *m_pIsCap;
      UINT             m_NumV;
      UINT             m_NumI;
      UINT             m_NumT;
      UINT             m_NumE;
      UINT             m_nSize;


      // during initialization
      PVERTEX         *m_pVolume;
      UINT             m_NumVV;


      void   BuildShadowVolume(ZFXVector);
      void   AddEdge(WORD v0, WORD v1);
      inline PVERTEX Extrude(const VERTEX*, const ZFXVector&);

/*
      void GenerateAdjacency();
      inline bool Adjacent(const TRIANGLE&, const TRIANGLE&);
      inline bool EqualPosition(const VERTEX*, const VERTEX*);
      inline bool HasEqualEdge(const TRIANGLE&,const VERTEX*,const VERTEX*);
      inline void AddExtrudedEdge(const VERTEX*,const VERTEX*,const ZFXVector&);
      inline bool Adjacent(const VERTEX*, const VERTEX*,
                           const VERTEX*, const VERTEX*);
*/

   }; // class
typedef class CShadowVolume *LPSHADOWVOLUME;
/*----------------------------------------------------------------*/


#endif