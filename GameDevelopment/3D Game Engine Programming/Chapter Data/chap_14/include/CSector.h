// FILE: CSector.h 

#ifndef CSECTOR_H
#define CSECTOR_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderDevice.h"
#include "CPolygon.h"
#include "CPolymesh.h"
#include "CLight.h"
#include "CEntity.h"
#include "CSpawnPoint.h"


// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * A sector is a concav region of a level which can contain polygons,
 * polymeshs, lights, entities and stuff. But no other sectors. The
 * portals connect two sectors but are stored within the level class.
 */
class CSector : public CLevelObject {
   public:
      CSector(const char *chName);
      ~CSector(void);

      bool NameEqual(const char *chName);
      void SetName(const char *chName);
      void GetName(char *buffer);

      void CopyOf(CLevelObject*)       { /* no action */; }
      void Translate(const ZFXVector&) { /* no action */; }

      UINT GetNumVertsM(void);    // vertices in meshs
      UINT GetNumIndisM(void);    // indices in meshs
      UINT GetNumMeshs(void)      { return m_NumMeshs;  }
      UINT GetNumPorts(void)      { return m_NumPorts;  }
      UINT GetNumLights(void)     { return m_NumLights; }
      UINT GetNumEntities(void)   { return m_NumEntys;  }
      UINT GetNumPolysTotal(void) { return (m_NumPolys + 
                                            m_NumPsInMeshs); }

      void Render(ZFXVertexCacheManager*,DWORD,bool bHide);
      void RenderLit(ZFXRenderDevice*,bool bHide);
      void Render(ZFXVertexCacheManager *pVCM, bool b) 
                  { Render(pVCM, LOB_ALL, b); }

      ZFXCOLOR GetAmbient(void) { return m_Ambient; }
      void     SetAmbient(float fR, float fG, float fB) {
                          m_Ambient.fR = fR;
                          m_Ambient.fG = fG;
                          m_Ambient.fB = fB; }

      void SelectAll(SELECTMODE, CSelectionBuffer*);
      void SelectInvert(SELECTMODE, CSelectionBuffer*);
      void SelectIntersecting(SELECTMODE, CSelectionBuffer*, 
                              const ZFXAabb&, ZFXAXIS);

      void RecalcVertexNormals(void);

      void HideNone(void);
      void HideSelected(SELECTMODE);
      void HideUnselected(void);
      void HideInvert(void);

      void Pick(SELECTMODE, const ZFXRay&, CSelectionBuffer*);
      bool Picked(const ZFXRay&, float*) { return false; }
      bool Intersects(const ZFXAabb&, ZFXAXIS) { return false; }

      LPLEVELOBJECT AddObject(const CLevelObject&);
      void RemoveObject(LPLEVELOBJECT);

      void RemovePortalConnections(UINT id);
      void NewPortalInTown( CPortal* );
      void NewDoorInTown( CEntity* );

      void SetID(UINT id) { m_ID = id;   }
      UINT GetID(void)    { return m_ID; }

      void Save(FILE *pFile);
      bool Load(FILE *pFile);
      void LoadPrefab(const char*,ZFXRenderDevice*,CSelectionBuffer*);

   private:
      char           m_chName[256];    // sector name
      LPPOLYGON     *m_ppPoly;         // sector polygons
      LPPOLYMESH    *m_ppMesh;         // sector meshes
      LPPORTAL      *m_ppPortal;       // sector portals
      LPLIGHT       *m_ppLight;        // sector lights
      LPENTITY      *m_ppEntity;       // sector entities
      LPSPAWNPOINT  *m_ppSpawn;        // sectors spawns
      UINT           m_NumMeshs;       // number of meshes
      UINT           m_NumPsInMeshs;   // number of polys in meshes
      UINT           m_NumPorts;       // number of portals
      UINT           m_NumLights;      // numbers of lights
      UINT           m_NumEntys;       // number of entities
      UINT           m_NumSpawns;      // number of spawn points
      UINT           m_ID;             // sector id (not used yet)
      ZFXCOLOR       m_Ambient;        // ambient light level

      // temporary pointers
      LPLEVELOBJECT *m_ppLob;
      UINT          *m_pC;

      void Init(void);
      void Release(void);
      void CalcBoundingBox(void);

      bool SetPointers(SELECTMODE);
      bool SetPointers(UINT);
   }; // class
typedef class CSector *LPSECTOR;
/*----------------------------------------------------------------*/

#endif
