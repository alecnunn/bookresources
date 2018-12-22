// FILE: CToolbar.h 

#ifndef CTOOLBAR_H
#define CTOOLBAR_H

#include <windows.h>
#include <commctrl.h>
#include "resources/resource.h"
#include "DlgCallbacks.h"
#include "ComboBoxHelper.h"
#include "gui.h"
#include "Globals.h"


typedef enum SHAPE_TYPE {
   SHP_NONE,
   SHP_RECT,
   SHP_CUBE,
   SHP_LIGHT
   } SHAPE;

// P R O T O T Y P E S /////////////////////////////////////////////

class CToolbar {
   public:
      CToolbar(void)  { }
      ~CToolbar(void) { }

      HRESULT Init(GLOBALS*);
      bool    MsgProc(WORD wCmd);

      void    EnableButton(UINT id, bool);

      // handle events
      void    OnFileNew(void);
      void    OnFileOpen(void);
      void    OnFileSave(void);
      void    OnFileSaveAs(void);
      void    OnPrefabOpen(void);
      void    OnPrefabSave(void);
      void    OnSectorNew(void);
      void    OnSectorDelete(void);
      void    OnMakePortal(void);
      void    OnMakeEntity(void);
      void    OnEditEntity(void);
      void    OnEditSpawn(void);

      void    OnInsertShape(bool,bool,ZFXVector,ZFXVector);

      void    SetDevice(LPZFXRENDERDEVICE pDev) { m_pDevice = pDev; }

      void    CheckForcedSave(void);

   private:
      LPZFXRENDERDEVICE  m_pDevice;
      HWND               m_hWndTB;
      GLOBALS           *m_pG;
      SHAPE              m_Shape;

      HRESULT CreateToolbar(void);
      HRESULT CreateButtons(void);

   }; // class
/*----------------------------------------------------------------*/

#endif
