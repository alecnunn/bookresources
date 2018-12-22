// FILE: CSelectionBuffer.h 

#ifndef CSELECTIONBUFFER_H
#define CSELECTIONBUFFER_H

#include <windows.h>
#include "CPolygon.h"
#include "CPolymesh.h"
#include "CSector.h"
#include "CLevel.h"
#include "CGeometry.h"


typedef struct VERTEX_SB_TYPE {
   VERTEX    *pV;    // address of vertex
   LPPOLYGON  pPoly; // address of the poly he belongs to
   } VERTEX_SB;

class CLevel;

// P R O T O T Y P E S /////////////////////////////////////////////

class CSelectionBuffer {
   public:
      CSelectionBuffer(void);
      ~CSelectionBuffer(void);

      void Reset(void);

      void DestroyWithoutNotify(void);

      void DeleteSelectedObjects(CLevel*);
      void CopySelectedObjects(LPSECTOR);

      bool IsInside(const ZFXVector&, ZFXAXIS);

      UINT GetNumVerts(void)   { return m_NumVerts; }
      UINT GetNumLobs(void)    { return m_NumLobs;  }
      UINT GetNumObjects(void) { return m_NumLobs+
                                        m_NumVerts; }

      ZFXVector GetCenter(void) { return m_Aabb.vcCenter; }
      
      LPPOLYGON     IsPolyQuad(void);
      LPLEVELOBJECT GetHeadPointer(void);

      void Render(ZFXRenderDevice*, float, bool, const ZFXVector*, const ZFXCOLOR*);
      void Translate(const ZFXVector&);

      // add things to the buffer
      void AddLob(const LPLEVELOBJECT);
      void AddVertex(VERTEX*,const LPPOLYGON);

      // called while adding things
      void AddBoundingBox(const ZFXAabb&);

      // deform things in the buffer 
      void MergePolysToMesh(LPSECTOR,HWND);
      void MergeMeshs(LPSECTOR);
      void FragmentMeshs(LPSECTOR);
      void InsideOut(void);
      void Rotate(ZFXAXIS,float);
      void Mirror(ZFXAXIS);
      void Snap(void);

      // work on textures
      void SetSkinTo(UINT nID);
      void TextureRemap(ZFXAXIS);
      void TransTextureCoords(float ftU, float ftV, 
                              float frU, float frV);
      bool GetTextureTrans(float *pftU, float *pftV,
                           float *pfrU, float *pfrV);

      // save a copy of selected stuff to disk
      void SaveAsPrefab(const char*);


   private:
      LPLEVELOBJECT *m_ppLob;       // selected level object
      VERTEX_SB  *m_pVerts;         // selected vertices
      UINT        m_NumLobs;        // number of objects
      UINT        m_NumVerts;       // number of vertices
      ZFXAabb     m_Aabb;           // bounding box
      float       m_fD;             // bounding box oversizing delta

      void Init(void);
      void Release(void);
      void RenderBoundingBox(ZFXRenderDevice*);
      void RenderVertices(ZFXRenderDevice*, float);

   }; // class
typedef class CSelectionBuffer *LPSELECTIONBUFFER;
/*----------------------------------------------------------------*/

#endif