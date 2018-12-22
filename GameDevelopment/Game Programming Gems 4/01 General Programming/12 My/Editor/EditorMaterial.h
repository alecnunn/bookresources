//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORMATERIAL_H_
#define   _GPG4_EDITORMATERIAL_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorObj.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorMaterial                                                     //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorMaterial : public CEditorObj
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorMaterial     (void);
    virtual                  ~CEditorMaterial     (void);

    virtual bool              CreateEngineObj     (void);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;

    // editing

    virtual bool              SpecialEditing      (CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc);
    virtual CStdString        SpecialGetValue     (CProperty* pProp);
  };

  // smart ptr

  SMART_POINTER(CEditorMaterial);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorMaterial.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORMATERIAL_H_
