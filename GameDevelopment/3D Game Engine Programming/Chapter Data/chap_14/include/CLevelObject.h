// FILE: CLevelObject.h 

#ifndef CLevelObject_H
#define CLevelObject_H

#include <windows.h>
#include "ZFX.h"
#include "ZFXRenderer.h"


#define SAFE_DELETE(p)   { if(p!=NULL) { delete (p);    (p)=NULL; } }
#define SAFE_DELETE_A(p) { if(p!=NULL) { delete [] (p); (p)=NULL; } }
#define SAFE_FREE(p)     { if(p!=NULL) { free(p);       (p)=NULL; } }


// enumerate all level objects
#define LOB_NONE         0
#define LOB_ALL          2
#define LOB_SECTOR       4
#define LOB_MESH         8
#define LOB_POLYGON     16
#define LOB_PORTAL      32
#define LOB_ENTITY      64
#define LOB_LIGHT      128
#define LOB_SPAWNPOINT 256



typedef struct ENTITYDOOR_TYPE {
   float   fDist;
   float   fPause;
   ZFXAXIS Axis;
   UINT    Portal[2];
   } ENTITYDOOR;
/*----------------------------------------------------------------*/

typedef struct ENTITYPLAYER_TYPE {

   } ENTITYPLAYER;
/*----------------------------------------------------------------*/



// P R O T O T Y P E S /////////////////////////////////////////////

/**
 * base class for all objects that a level can contain.
 */
class CLevelObject {
   public:
      CLevelObject(void) { /* nothing */ }

      virtual ~CLevelObject(void) { /* nothing */ }

      // virtual interface stuff
      virtual void Init(void)=0;
      virtual void Release(void)=0;
      virtual void Save(FILE*)=0;
      virtual bool Load(FILE*)=0;
      virtual void Render(ZFXVertexCacheManager*,bool)=0;
      virtual void Translate(const ZFXVector&)=0;
      virtual void CopyOf(CLevelObject*)=0;
      virtual bool Picked(const ZFXRay&, float*)=0;
      virtual bool Intersects(const ZFXAabb&, ZFXAXIS)=0;

      virtual void Reset(void) { Release(); Init(); }

      // all of our little helpers
      bool     IsOfType(DWORD dw) { return (m_Type==dw); }
      DWORD    GetType(void)      { return m_Type;       }
      UINT     GetNumVerts(void)  { return m_NumVerts;   }
      UINT     GetNumIndis(void)  { return m_NumIndis;   }
      UINT     GetNumPolys(void)  { return m_NumPolys;   }
      ZFXAabb  GetAabb(void)      { return m_Aabb;       }

      void     DeSelect(void)     { m_bSelected = false; }
      void     Select(void)       { m_bSelected = true;  }
      bool     IsSelected(void)   { return m_bSelected;  }

      void     Hide(void)         { m_bHidden = true; m_bSelected = false; }
      void     UnHide(void)       { m_bHidden = false;   }
      bool     IsHidden(void)     { return m_bHidden;    }


      LPVOID* InitMem(LPVOID *ppMem, UINT Slots, int Stride) {
         if (Slots==0) return NULL;
         int n = ((int)(((float)Slots)/50)+1)*50;
         ppMem = (LPVOID*)malloc(Stride*n);
         return ppMem;
         }

      LPVOID* CheckMem(LPVOID *ppMem, UINT Slots, int Stride) {
         if ( (Slots%50) == 0 )
            ppMem = (LPVOID*)realloc(ppMem,Stride*(Slots+50));
         return ppMem;
         }


   protected:
      UINT     m_NumVerts;       // number of vertices
      UINT     m_NumIndis;       // number of indices
      UINT     m_NumPolys;       // number of polygons
      bool     m_bSelected;      // is this one selected
      bool     m_bHidden;        // is this one hidden
      DWORD    m_Type;           // LOB_MESH, ...
      ZFXAabb  m_Aabb;           // bounding box
   }; // class
typedef class CLevelObject *LPLEVELOBJECT;
/*----------------------------------------------------------------*/


#endif
