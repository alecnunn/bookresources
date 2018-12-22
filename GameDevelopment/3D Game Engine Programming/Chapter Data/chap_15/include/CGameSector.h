// FILE: CGameSector.h

#ifndef CGAMESECTOR_H
#define CGAMESECTOR_H


#include <windows.h>
#include <stdio.h>
#include "zfx3d.h"
#include "zfx.h"
#include "CGameLight.h"
#include "CGamePortal.h"
#include "CGameEntity.h"
#include "CShadowVolume.h"
#include "CGameCharacter.h"

class CGameLevel;
class CGameEntity;
class CGamePortal;


// makes life easier
typedef struct SECBUF_TYPE {
   UINT SkinID;
   UINT NumTris;
   UINT IBID;
   } SECBUF;

typedef struct POLYGON_TYPE {
   VERTEX *pVerts;
   WORD   *pIndis;
   UINT    NumV;
   UINT    NumI;
   UINT    SkinID;
   } POLYGON;
/*----------------------------------------------------------------*/


class CGameSector {
   public:
      CGameSector(ZFXRenderDevice*, UINT, CGameLevel*);
      virtual ~CGameSector(void);
      
      bool    Load(FILE*);
      void    Update(float);
      void    TouchAndUse(const ZFXVector&);
      HRESULT RenderDepth(ZFXPlane*,const ZFXVector&);
      HRESULT RenderLit(ZFXPlane*,const ZFXVector&,UINT*,UINT*);
      HRESULT RenderShadows(ZFXPlane*,const ZFXVector&);

      HRESULT RenderAmbient(ZFXPlane*,const ZFXVector&);

      void    RenderPortals(const ZFXVector&, bool);
      void    RenderOctree(bool,bool);
      void    ReceiveShaders(UINT,UINT,UINT,UINT);

      UINT    GetIndex(void)     { return m_Index;     }
      UINT    GetNumSpawns(void) { return m_NumSpawns; }

      ZFXAabb GetAabb(void)
                  { return m_pOctree->GetAabb(); }

      bool    GetSpawnPoint(ZFXVector*);
      bool    GetFloor(const ZFXVector&, float*, ZFXPlane*);
      bool    TestCollision(UINT*, const ZFXAabb&, ZFXPlane*, bool*);
      bool    TestCollision(UINT*, const ZFXRay&, float, float*, bool*);

      CGamePortal* GetPortal(UINT);
      bool         FindBrother(CGamePortal*);
      void         BuildAllPortalRelationships(void);
      bool         AllPortalsConnectedOk(void);

   private:
      LPZFXRENDERDEVICE m_pDevice;
      CGameLevel     *m_pLevel;       // level which this sector belongs to
      CGameLight     *m_pLights;      // array of all lights in sector
      CGamePortal    *m_pPorts;       // array of all portals in this sector
      CShadowVolume  *m_pMeshShadows; // array of all shadow volumes from meshs
      CGameEntity   **m_ppEntities;   // array of all entities in this sector
      ZFXVector      *m_pSpawns;      // array of spawnpoints

      SECBUF     *m_pBuffer;     // vertexbuffer handles
      ZFXOctree  *m_pOctree;     // octree of this sector

      POLYGON    *m_pPolys;      // temporary during creation
      ZFXPolygon *m_pZFXPolys;   // temporary during creation
      VERTEX     *m_pVerts;      // temporary during creation
      WORD       *m_pIndis;      // temporary during creation
      UINT        m_Count;       // temporary during creation

      ZFXAabb     m_Aabb;        // sectors bounding box (?)
      ZFXCOLOR    m_Ambient;     // ambient light, what else
      UINT        m_BufID;       // id of renderbuffer
      UINT        m_NumSkins;    // number of skins in this sector
      UINT        m_NumVerts;    // number of vertices
      UINT        m_NumIndis;    // number of indices
      UINT        m_NumPorts;    // number of portals
      UINT        m_NumMeshs;    // number of meshs
      UINT        m_NumEntys;    // number of entities
      UINT        m_NumLights;   // number of lights
      UINT        m_NumSpawns;   // number of spawnpoints
      UINT        m_NumZFXPolys; // number of polygons
      UINT        m_NumPolys;    // number of polygons
      UINT        m_Index;       // index in level's sector array

      UINT        m_BaseVSH;     // shaders, what else? :)
      UINT        m_BasePSH;
      UINT        m_OmniPSH;
      UINT        m_OmniVSH;

      bool        m_bDepthRendered;   // already rendered to depth this frame?
      bool        m_bLitRendered;     // already rendered this frame?
      bool        m_bShadowsRendered; // same for shadows


      bool      LoadSinglePolygon(FILE*);
      bool      LoadMeshs(FILE*);
      bool      LoadPortals(FILE*);
      bool      LoadLights(FILE*);
      bool      LoadEntities(FILE*);
      bool      LoadSpawns(FILE*);
      bool      LoadAndSkipMeshData(FILE*);
      HRESULT   CompilePolygonList(void);
      HRESULT   CreateOctree(void);
      ZFXMatrix CalcTransAttenNoRot(const ZFXVector&,float);
      void      RenderLightPass(const ZFXPlane*,UINT,bool);
      void      RenderDoors(const ZFXPlane*,UINT,UINT,UINT,UINT,bool);

   }; // class
typedef class CGameSector *LPGAMESECTOR;
/*----------------------------------------------------------------*/


#endif