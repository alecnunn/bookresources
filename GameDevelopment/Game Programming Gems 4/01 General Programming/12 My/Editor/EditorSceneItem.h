//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SCENE ITEM CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORSCENEITEM_H_
#define   _GPG4_EDITORSCENEITEM_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"
#include  "EditorObj.h"
#include  "StlIncludes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEditorScene;
  class CPropFct;
  class IFile;
  class CLevelFiles;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorSceneItem                                                    //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  SMART_POINTER(CEditorSceneItem);

  class CEditorSceneItem : public CPersistent
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEditorSceneItem,CExtraProp);

  // typedefs

  public:

    typedef std::list< CEditorSceneItemSP >       listItem;
    typedef listItem::iterator                    iterItem;

  // public methods

  public:

    // constructors & destructor

                              CEditorSceneItem    (CEditorScene* pScene,CEditorObj* pObj);
                              CEditorSceneItem    (void);
    virtual                  ~CEditorSceneItem    (void);

    // get/set

    CEditorObj*               GetObject           (void) const;
    CEditorSceneItem*         GetParent           (void) const;
    void                      SetParent           (CEditorSceneItem* pParent);
    HTREEITEM                 GetTreeHandle       (void) const;
    void                      SetTreeHandle       (HTREEITEM hItem);
    const CStdString&         GetPrevParent       (void) const;
    void                      SetPrevParent       (const CStdString& strParent);

    bool                      IsDeleted           (void) const;
    void                      SetDeleted          (const bool boDeleted);
    bool                      IsExpanded          (void) const;
    void                      SetExpanded         (const bool boExpanded);

    // children

    CEditorSceneItem*         GetFirstChild       (void);
    CEditorSceneItem*         GetNextChild        (CEditorSceneItem* pCurrent=NULL);
    u32                       GetNbChildren       (void) const;

    bool                      AddChild            (CEditorSceneItem* pChild);
    bool                      RemoveChild         (CEditorSceneItem* pChild);

    // streaming

    static bool               PropWriteChildren   (const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level);
    static bool               PropReadChildren    (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level);
    static bool               PropLinkChildren    (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed);
    void                      SetScene            (CEditorScene* pScene);

  // protected data

  protected:

    HTREEITEM                 m_hItem;
    CStdString                m_strPrevParent;              // when in the recycle bin (Restore)
    bool                      m_boDeleted;                  // in recycle bin
    bool                      m_boExpanded;

    CEditorObjSP              m_spEditorObj;                // object corresponding to this item

    CEditorScene*             m_pScene;                     // scene the item belongs to
    CEditorSceneItem*         m_pParentItem;
    listItem                  m_ChildItems;
    iterItem                  m_itCurrItem;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorSceneItem.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORSCENEITEM_H_
