//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORTEXTURE_H_
#define   _GPG4_EDITORTEXTURE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorObj.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorTexture                                                      //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorTexture : public CEditorObj
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorTexture      (void);
    virtual                  ~CEditorTexture      (void);

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

  SMART_POINTER(CEditorTexture);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorTexture.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORTEXTURE_H_
