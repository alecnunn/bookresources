//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR BOX CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORBOX_H_
#define   _GPG4_EDITORBOX_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorNode.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorBox                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorBox : public CEditorNode
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorBox          (void);
    virtual                  ~CEditorBox          (void);

    virtual bool              CreateEngineObj     (void);

    // editing

    virtual bool              Edit                (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              EditExisting        (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              SpecialEditing      (CProperty*  pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;
  };

  // smart ptr

  SMART_POINTER(CEditorBox);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorBox.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORBOX_H_
