//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITOROBJ_H_
#define   _GPG4_EDITOROBJ_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"
#include  "EngineObj.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CRTTIMap;
  class IRenderer;
}

class CRTTIPropsDoc;

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorObj                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorObj : public CPersistent
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEditorObj,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEditorObj          (void);
    virtual                  ~CEditorObj          (void);

    static CEditorObj*        CreateObject        (const u32 u32ObjType);
    static CEditorObj*        CreateObject        (const CStdString& strClassName);
    virtual bool              CreateEngineObj     (void);

    // editing

    virtual bool              Store               (void);
    virtual bool              Reset               (void);
    virtual bool              Edit                (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              EditExisting        (CRTTIPropsDoc* pDoc,IRenderer* pRenderer);
    virtual bool              SpecialEditing      (CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc);
    virtual CStdString        SpecialGetValue     (CProperty* pProp);

    // get/set

    virtual CStdString        GetEditName         (void) const;

    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;
    virtual CEngineObj*       GetEngineObj        (void) const;
    virtual void              SetEngineObj        (CEngineObj* pEngineObj);

    // flags

    virtual bool              IsSelected          (void) const;
    virtual bool              IsDeletable         (void) const;
    virtual bool              IsDraggable         (void) const;
    virtual bool              IsDropTarget        (void) const;
    virtual bool              IsRenamable         (void) const;

    virtual void              Select              (const bool boSelect);
    virtual void              SetDeleteFlag       (const bool boDelete);
    virtual void              SetDragFlag         (const bool boDrag);
    virtual void              SetDropFlag         (const bool boDrop);
    virtual void              SetRenameFlag       (const bool boRename);

    // RTTI map

    static CRTTIMap*          GetRTTIMap          (void);
    static bool               FillRTTIMap         (CRTTIMap& map);

  // protected data

  protected:

    CEngineObjSP              m_spEngineObj;

    bool                      m_boSelected;
    bool                      m_boDeletable;
    bool                      m_boDraggable;
    bool                      m_boDropTarget;
    bool                      m_boRenamable;

    bool                      m_boStored;                   // cf Store/Reset
  };

  // smart ptr

  SMART_POINTER(CEditorObj);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorObj.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITOROBJ_H_
