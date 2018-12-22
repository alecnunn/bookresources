// FILE: CApplication.h

#ifndef APPLICATIONC_H
#define APPLICATIONC_H


#include "globals.h"
#include "CToolbar.h"


typedef enum TOOL_TYPE {
   TL_SELECT,
   TL_TRANSLATE,
   TL_ROTATE,
   TL_POLYGON
   } TOOL;
/*----------------------------------------------------------------*/


#define ZFXMAX(a,b) ((a<b) ? (b) : (a))
#define ZFXMIN(a,b) ((a<b) ? (a) : (b))


// P R O T O T Y P E S /////////////////////////////////////////////

class CApplication {
   public: 
      CApplication(void) {  }
      virtual ~CApplication(void) {  }

      HRESULT Init(HWND, HINSTANCE);
      void    Update(void);
      void    RenderTick(void);
      void    Release(void);

      void    Done(void)     { m_bDone = true;   }
      bool    IsDone(void)   { return m_bDone;   }
      void    Active(bool b) { m_bActive = b;    }
      bool    IsActive(void) { return m_bActive; }

      void    InvalidateView(int);
      void    InvalidateAllViews(void);

      bool    MsgProcMenu(WORD);
      bool    MsgProcGUI(WORD);
      bool    MsgProcToolbar(WORD wCmd) { return m_Toolbar.MsgProc(wCmd); }

      LPZFXRENDERDEVICE GetRenderDevice(void) { return m_pRDevice; }

      void    OnViewPerspective(void);
      void    OnViewOrthogonal(UCHAR);
      bool    OnClose(void);
      void    OnDeleteKey(void);
      void    OnMouseMove(WPARAM, LPARAM);
      void    OnMouseClick(bool, WPARAM, LPARAM);
      void    OnSelBufChange(void);

      ZFXSkinManager* GetSkinMan(void) { return m_pRDevice->GetSkinManager(); }

      CToolbar* GetToolbar(void) { return &m_Toolbar; }
      bool      CheckForcedSave(void);
      bool      MouseInside(HWND, POINT*);

      static GLOBALS m_G;

   private:
      // general misc stuff
      DWORD             m_OldTime;           // frame counter
      DWORD             m_NewTime;           // frame counter
      float             m_dt;                // elapsed time
      LPZFXRENDERER     m_pRenderer;
      LPZFXRENDERDEVICE m_pRDevice;
      CToolbar          m_Toolbar;
      HMENU             m_hm;
      POINT             m_ptCursorNew;
      POINT             m_ptCursorOld;
      int               m_CurView;           // current viewport [0,.,3]
      UINT              m_nFontID;           // font from ZFXRenderDevice
      bool              m_bDone;             // application is ready to exit
      bool              m_bActive;           // application is active
      UINT              m_nZFXFont;          // id to ZFXRenderDevice created font

      // user settings for editor
      bool              m_bDrawLight;        // activate lighting in 3D view
      bool              m_b3DHide;           // hide hidden stuff in perspective view
      bool              m_bDrawGrid[2];      // draw grid 0=ortho, 1=perspect.
      bool              m_bDrawPoints[2];    // draw points 0=ortho, 1=perspect.
      bool              m_bDrawWired[2];     // draw wireframe 0=ortho, 1=perspect.
      bool              m_bDrawInact[2];     // draw inactive sectors
      bool              m_bDrawInactW;       // draw them in wireframe
      bool              m_bSnap;             // snap vertices to grid?
      UCHAR             m_bIsSelecting;      // while user is selecting in ortho
      UCHAR             m_bIsMovingSelection;// while user is moving the selection
      UCHAR             m_bIsCreating;       // while creating polygon

      RECT              m_rcOrthoSel;        // ortho selection rect
      ZFXVector         m_vcSel[2];          // ortho selection rect min/max
      ZFXAXIS           m_Axis[3];           // for easier axis
      ZFXVector         m_vcEndMove;         // end of moving selection
      ZFXVector         m_vcStartMove;       // start of moving selection

      SELECTMODE        m_SelectMode;        // select mode
      TOOL              m_CurrentTool;       // current tool 

      // view transformation stuff
      ZFXMatrix         m_World[4];    // world matrices of the four views
      ZFXVector         m_vcFix;       // fixpoint to look at
      float             m_fRotX;       // rota in perspective view
      float             m_fRotY;       // rota in perspective view
      float             m_fDistOrtho;  // distance in ortho view
      float             m_fScale[3];   // scaling for three orthog views
      float             m_fPosX[4];    // pos for all four views
      float             m_fPosY[4];    // pos for all four views
      float             m_fPosZ[4];    // pos for all four views

      VERTEX            m_vVerts[256]; // temp buffer for new polygon
      UCHAR             m_NumVerts;    // number of verts in buffer

      // shader ID given to us by our engine
      UINT              m_BaseVSH;     // basic shader for level geometrie
      UINT              m_OmniVSH;     // omni light additive shader 
      UINT              m_BasePSH;     // basic shader for level geometrie
      UINT              m_OmniPSH;     // omni light additive shader 

      // PRIVATE METHODS
      HRESULT           InitRenderer(void);
      HRESULT           InitShaders(void);
      void              CreateGUI(void);
      void              UpdateCursor(void);
      void              SetSelectMode(SELECTMODE);
      bool              ChangeCheckState(UINT MenuID);
      void              OrthoSelection(bool,POINT);
      void              CheckTbState(UINT,UINT,SELECTMODE);

      void              RenderOrthoGrid(UCHAR n);
      void              RenderPerspGrid(void);
      void              RenderPolyUnderConstr(UINT);
      void              RenderSelectionRect(void);
      void              RenderInActive(bool);
      void              RenderGeometry(bool,float);
      void              RenderPortals(bool);
      void              RenderLights(bool);
      void              RenderEntities(bool);
      void              RenderSpawns(bool);

      bool              InvalidateSelectionRect(bool);
      void              OnPolyCreationEvent(bool bEnd, const ZFXVector*);

   }; // class
/*----------------------------------------------------------------*/

#endif