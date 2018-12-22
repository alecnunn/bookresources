// FILE: CPolymesh.h 

#ifndef CPOLYMESH_H
#define CPOLYMESH_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderDevice.h"
#include "CPolygon.h"




// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * A polymesh is a collection of an arbitrary number of polygons 
 * that form an arbitrary mesh without restrictions. In our case
 * we limit the number of polygons to 256 for ease of usage.
 */
class CPolymesh : public CLevelObject {
   public:
      CPolymesh(void);
      virtual ~CPolymesh(void);

      void       CopyOf(CLevelObject*);
      void       Render(ZFXVertexCacheManager*, bool bHide);

      void       Translate(const ZFXVector&);
      void       Rotate(ZFXAXIS, const ZFXVector&, float a) { ; }
      void       Mirror(ZFXAXIS, const ZFXVector&) { ; }
      void       InsideOut(void);
      void       SetSkinID(UINT);
      void       CountNonRedundantSkins(UINT*Array, UINT*Num);

      CPolygon*  PickPoly(const ZFXRay&, float, float*);
      bool       Picked(const ZFXRay&, float*);

      bool       Intersects(const ZFXAabb&, ZFXAXIS);
      void       GetIntersectingPolys(const ZFXAabb&,CSelectionBuffer*,ZFXAXIS);
      void       GetIntersectingVerts(const ZFXAabb&,CSelectionBuffer*,ZFXAXIS);

      void       CreateFromPolygons(const CPolygon *, UCHAR);
      HRESULT    RemovePolygon(const LPPOLYGON);
      HRESULT    AddPolygon(const CPolygon &);

      void       Save(FILE*);
      bool       Load(FILE*);
      void       SavePurePolys(FILE*);

      void       SelectAllPolys(CSelectionBuffer*);
      void       SelectInvert(CSelectionBuffer*);
      
      void       HideNoPolys(void);
      void       HideInvertPolys(void);
      void       HideSelectedPolys(void);
      void       HideUnselectedPolys(void);

      LPPOLYGON  GetPolygon(UINT i) { return m_ppPoly[i]; }

      void       CalcBoundingBox(void);
      void       CalcNormals(void);
      void       CalcTextureCoords(ZFXAXIS, const ZFXAabb*);

      void       TransTextureCoords(float ftU, float ftV, 
                                    float frU, float frV);
      bool       GetTextureTrans(float *pftU, float *pftV,
                                 float *pfrU, float *pfrV);
      
   private:
      LPPOLYGON *m_ppPoly;
      bool       m_bSupprBoxCalc;

      void       Init(void);
      void       Release(void);
   }; // class
typedef class CPolymesh *LPPOLYMESH;
/*----------------------------------------------------------------*/

#endif
