//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR CAMERA CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORCAMERA_H_
#define   _GPG4_EDITORCAMERA_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorNode.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorCamera                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorCamera : public CEditorNode
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorCamera       (void);
    virtual                  ~CEditorCamera       (void);

    virtual bool              CreateEngineObj     (void);

    //

    virtual bool              Edit                (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              EditExisting        (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);

    //

    virtual bool              Store               (void);
    virtual bool              Reset               (void);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;

  // protected data

  protected:

    // stored state

    CVect3D                   m_v3Pos,m_v3Target,m_v3Up;
    float                     m_fScaleOrtho;
  };

  // smart ptr

  SMART_POINTER(CEditorCamera);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorCamera.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORCAMERA_H_
