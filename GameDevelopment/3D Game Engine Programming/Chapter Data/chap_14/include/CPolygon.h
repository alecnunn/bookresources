// FILE: CPolygon.h 

#ifndef CPOLYGON_H
#define CPOLYGON_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderer.h"
#include "CLevelObject.h"
#include "CPortal.h"


typedef enum SELECTMODE_TYPE {
   SM_POLYGON,
   SM_MESH,
   SM_VERTEX,
   SM_PORTAL,
   SM_ENTITY,
   SM_LIGHT,
   SM_SPAWN
   } SELECTMODE;
/*----------------------------------------------------------------*/


class CSelectionBuffer;
class CPolymesh;


// P R O T O T Y P E S /////////////////////////////////////////////

int SortBySkin(const void*, const void*);

/**
 * The polygon class is the atom of our editor. It can consist of
 * any number of vertices that form a closed region which can be
 * rendered as a list of triangles. 
 */
class CPolygon : public CLevelObject {
   public:
      CPolygon(void);
      virtual ~CPolygon(void);

      void    CopyOf(CLevelObject*);
      bool    CreatePortal(CPortal*);

      void    SetVertices(const VERTEX*, UINT);
      void    SetIndices(const WORD*, UINT);

      void    SetSkinID(UINT ID) {   m_SkinID = ID;   }
      UINT    GetSkinID(void)    { return m_SkinID;   }
      
      void    GetTexOff(float *pU, float *pV) {
                  if (pU) *pU = m_fTexOff[0];
                  if (pV) *pV = m_fTexOff[1]; }
      void    GetTexRep(float *pU, float *pV) {
                  if (pU) *pU = m_fTexRep[0];
                  if (pV) *pV = m_fTexRep[1]; }

      void    Render(ZFXVertexCacheManager*,bool);

      void    Translate(const ZFXVector&);
      void    Rotate(ZFXAXIS,  const ZFXVector&, float);
      void    Mirror(ZFXAXIS,  const ZFXVector&);
      bool    Picked(const ZFXRay&, float*);
      bool    Intersects(const ZFXAabb&, ZFXAXIS);
      void    GetIntersectingVerts(const ZFXAabb&,CSelectionBuffer*,ZFXAXIS);
      void    InsideOut(void);

      bool    IsPartOfMesh(void) { return m_bPartOfMesh; }
      void    SetAsPartOfMesh(CPolymesh *p);
      CPolymesh* GetParent(void) { return m_pDad; }

      void    Triangulate(void);
      void    CalcBoundingBox(void);
      void    CalcNormals(void);
      void    CalcTextureCoords(ZFXAXIS, const ZFXAabb*);
      void    TransTextureCoords(float ftU, float ftV, 
                                 float frU, float frV);
      void    GetTextureTrans(float*,float*,float*,float*);

      void    Save(FILE*);
      bool    Load(FILE*);

   private:
      VERTEX     *m_pVerts;         // array of vertices
      WORD       *m_pIndis;         // array of indices
      WORD       *m_pLineIndis;     // array of polyline indices
      CPolymesh*  m_pDad;           // mesh if part of mesh
      UINT        m_SkinID;         // zfxskin id
      bool        m_bPartOfMesh;    // is this one part of a mesh
      float       m_fTexOff[2];     // texture offset [u,v]
      float       m_fTexRep[2];     // texture repeat [u,v]

      void        Init(void);
      void        Release(void);
   }; // class
typedef class CPolygon *LPPOLYGON;
/*----------------------------------------------------------------*/


#endif
