// FILE: CLevel.h 

#ifndef CLEVEL_H
#define CLEVEL_H

#include <windows.h>
#include "CSelectionBuffer.h"
#include "ComboBoxHelper.h"


// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * A level consists of sector objects which organize geometry in the
 * level. Additionally a level maintains the list of portals that
 * connect the sectors.
 */
class CLevel {
   public:
      CLevel(void);
      ~CLevel(void);

      // all misc stuff
      void      ClearAll(ZFXSkinManager*);
      void      SetName(const char *ch);
      void      GetName(char *ch);
      bool      HasName(void);

      bool      SaveLevel(const char*, ZFXRenderDevice*);
      bool      LoadLevel(const char*, HWND, HWND, ZFXRenderDevice*);
      void      SaveSkin(ZFXSkinManager*, FILE*, UINT);
      void      LoadSkin(HWND, ZFXSkinManager*, FILE*);

      void      RemovePortalConnections(UINT id);
      void      NewPortalInTown( CPortal* );
      void      NewDoorInTown( CEntity* );

      void      RecalcVertexNormals(void);

      UINT      CreatePortalID(void) { m_PID++; return m_PID; }
      UINT      CreateSkinID(ZFXRenderDevice*,const char*,float);

      // all render stuff
      void      RenderActive(ZFXRenderDevice*,DWORD,bool);
      void      RenderActiveLit(ZFXRenderDevice*,bool);
      void      RenderInActive(ZFXRenderDevice*,DWORD,bool);

      // all sector stuff
      HRESULT   AddSector(const char *ch);
      HRESULT   RemoveSector(const char *ch);
      bool      SectorExists(const char *ch);
      bool      SelectSector(const char *ch);
      CSector*  GetSelectedSector(void) { return m_ppSector[m_nActive]; }

      UINT      GetNumSectors(void) { return m_NumSectors; }
      void      GetNumLobs(UINT *pVerts, UINT *pIndis, 
                           UINT *pPolys, UINT *pMesh,  
                           UINT *pPorts, UINT *pLights,
                           UINT *pEntys);


   private:
      char      m_chName[MAX_PATH];    // level name and path
      LPSECTOR *m_ppSector;            // list of sectors
      UINT      m_NumSectors;          // number of sectors
      UINT      m_nActive;             // active sector
      UINT      m_PID;                 // portal ID counter

      void      Init(void);
      void      Release(void);

   }; // class
typedef class CLevel *LPLEVEL;
/*----------------------------------------------------------------*/

#endif