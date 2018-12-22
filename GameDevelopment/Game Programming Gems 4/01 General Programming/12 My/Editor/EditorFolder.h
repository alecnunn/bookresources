//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR FOLDER CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORFOLDER_H_
#define   _GPG4_EDITORFOLDER_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorObj.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorFolder                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorFolder : public CEditorObj
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CEditorFolder       (void);
    virtual                  ~CEditorFolder       (void);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual CStdString        GetEditName         (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;
  };

  // smart ptr

  SMART_POINTER(CEditorFolder);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorFolder.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORFOLDER_H_
