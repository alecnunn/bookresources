//--------------------------------------------------------------------------------------------------------------------//
//                                                MOVE CAMERA (MOUSE+KEYS)                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_MOVECAMERATOOL_H_
#define   _GPG4_MOVECAMERATOOL_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorTool.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineCamera;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CMoveCameraTool                                                     //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CMoveCameraTool : public CEditorTool
  {
  // public methods

  public:

    // constructors & destructor

                              CMoveCameraTool     (CView* pView);
    virtual                  ~CMoveCameraTool     (void);

    // overridables

    virtual u32               Start               (void);
    virtual u32               End                 (void);
    virtual u32               Cancel              (void);

    virtual u32               OnKeyDown           (UINT nChar,UINT nRepCnt,UINT nFlags);
    virtual u32               OnRButtonDown       (UINT nFlags,CPoint point);
    virtual u32               OnLButtonDblClk     (UINT nFlags,CPoint point);
    virtual u32               OnRButtonDblClk     (UINT nFlags,CPoint point);

    virtual u32               OnTimer             (UINT nIDEvent);

    // specific

    virtual bool              MoveCamera          (const CVect3D& v3Move);

  // protected methods

  protected:

    // specific

    virtual void              MulSpeed            (void);
    virtual void              DivSpeed            (void);
    virtual void              MulScale            (CEngineCamera* pCam);
    virtual void              DivScale            (CEngineCamera* pCam);
    virtual bool              UpdatePrompt        (void);

  // protected data

  protected:

    float                     m_fSpeed;
    CPoint                    m_ptSaveCursor;

    // camera initial state

    CVect3D                   m_v3Pos;
    CVect3D                   m_v3Target;
    CVect3D                   m_v3Up;
    float                     m_fScaleOrtho;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_MOVECAMERATOOL_H_
