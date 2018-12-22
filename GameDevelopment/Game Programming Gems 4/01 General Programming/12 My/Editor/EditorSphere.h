//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SPHERE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORSPHERE_H_
#define   _GPG4_EDITORSPHERE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorNode.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorSphere                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorSphere : public CEditorNode
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorSphere       (void);
    virtual                  ~CEditorSphere       (void);

    virtual bool              CreateEngineObj     (void);

    // editing

    virtual bool              Edit                (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              EditExisting        (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              SpecialEditing      (CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;
  };

  // smart ptr

  SMART_POINTER(CEditorSphere);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorSphere.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORSPHERE_H_
