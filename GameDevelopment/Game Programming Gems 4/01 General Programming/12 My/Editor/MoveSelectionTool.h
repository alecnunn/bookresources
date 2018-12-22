//--------------------------------------------------------------------------------------------------------------------//
//                                                MOVE SELECTION (MOUSE+KEYS)                                         //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_MOVESELECTIONTOOL_H_
#define   _GPG4_MOVESELECTIONTOOL_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorTool.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEditorObj;
  class CEngineNode;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CMoveSelectionTool                                                  //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CMoveSelectionTool : public CEditorTool
  {
  // public methods

  public:

    // constructors & destructor

                              CMoveSelectionTool  (CView* pView);
    virtual                  ~CMoveSelectionTool  (void);

    // overridables

    virtual u32               Start               (void);
    virtual u32               End                 (void);
    virtual u32               Cancel              (void);

    virtual u32               OnKeyDown           (UINT nChar,UINT nRepCnt,UINT nFlags);
    virtual u32               OnMouseWheel        (UINT nFlags,short zDelta,CPoint point);
    virtual u32               OnRButtonDown       (UINT nFlags,CPoint point);

    virtual u32               OnTimer             (UINT nIDEvent);

    // specific

    virtual bool              MoveSelection       (const CVect3D& v3Move);

  // protected methods

  protected:

    // specific

    virtual void              MulSpeed            (void);
    virtual void              DivSpeed            (void);
    virtual bool              UpdatePrompt        (void);

  // protected data

  protected:

    float                     m_fSpeed;
    CVect3D                   m_v3DoneMove;                 // displacement already done

    CEditorObj*               m_pObjSelected;               // NULL if multi-sel
    CEngineNode*              m_pNodeSelected;

    u32                       m_u32Cursor;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_MOVESELECTIONTOOL_H_
