//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR TOOL BASE CLASS                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORTOOL_H_
#define   _GPG4_EDITORTOOL_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "StdString.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineNode;
}
class CView;

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorTool                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorTool
  {
  // public methods

  public:

    // constructors & destructor

                              CEditorTool         (CView* pView);
    virtual                  ~CEditorTool         (void);

    // get

    const CStdString&         GetName             (void) const;
    const CView*              GetView             (void) const;
    const bool                Modified            (void) const;

    // overridables

    virtual u32               Start               (void);
    virtual u32               End                 (void);
    virtual u32               Cancel              (void);

    virtual u32               OnKeyDown           (UINT nChar,UINT nRepCnt,UINT nFlags);  // eventually returns a modified chr
    virtual u32               OnKeyUp             (UINT nChar,UINT nRepCnt,UINT nFlags);  // eventually returns a modified chr
    virtual u32               OnMouseMove         (UINT nFlags,CPoint point);             // 0 = not handled
    virtual u32               OnMouseWheel        (UINT nFlags,short  zDelta,CPoint point);
    virtual u32               OnLButtonDown       (UINT nFlags,CPoint point);
    virtual u32               OnRButtonDown       (UINT nFlags,CPoint point);
    virtual u32               OnLButtonUp         (UINT nFlags,CPoint point);
    virtual u32               OnRButtonUp         (UINT nFlags,CPoint point);
    virtual u32               OnLButtonDblClk     (UINT nFlags,CPoint point);
    virtual u32               OnRButtonDblClk     (UINT nFlags,CPoint point);

    virtual u32               OnTimer             (UINT nIDEvent);

  // protected methods

  protected:

    // helpers

    bool                      NoRepeatKey         (u32 u32Key);
    bool                      AsyncKey            (u32 u32Key);
    void                      InitKeys            (void);
    void                      CenterCursor        (void);
    void                      SetHelpText         (const CStdString& strHelp);
    bool                      IsChildOfSelected   (CEngineNode* pNode);

  // protected data

  protected:

    CStdString                m_strName;
    CView*                    m_pView;
    bool                      m_boModified;

    bool                      m_boRightBtn;
    bool                      m_boLeftBtn;
    CPoint                    m_ptNew,m_ptOld;
    s32                       m_s32Wheel;

    UINT                      m_nTimer;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORTOOL_H_
