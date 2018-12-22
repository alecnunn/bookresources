//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SCENE CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORSCENE_H_
#define   _GPG4_EDITORSCENE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"
#include  "EditorSceneItem.h"
#include  "StlIncludes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorScene                                                        //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorScene : public CPersistent
  {
  DECLARE_RTTI;

  // typedefs

  public:

    typedef std::map< CStdString, CEditorSceneItemSP >      mapItem;
    typedef mapItem::iterator                               iterItem;

  // public methods

  public:

    // constructors & destructor

                              CEditorScene        (void);
    virtual                  ~CEditorScene        (void);

    // get/set

    CEditorSceneItem*         GetRootItem         (void) const;
    void                      SetRootItem         (CEditorSceneItem* pRoot);

    CEditorSceneItem*         GetItem             (const CStdString& strItem);
    CEditorSceneItem*         GetFirstItem        (void);
    CEditorSceneItem*         GetNextItem         (const CStdString* pstrCurrent=NULL);
    u32                       GetNbItem           (void) const;

    // map

    bool                      RegisterItem        (const CStdString& strName,CEditorSceneItem* pItem);
    bool                      UnregisterItem      (const CStdString& strName,CEditorSceneItem* pItem);
    void                      UnregisterAllItems  (void);

    // streaming

    bool                      Write               (CLevelFiles& Level);

  // protected data

  protected:

    CEditorSceneItemSP        m_spRoot;

    mapItem                   m_Items;
    iterItem                  m_itCurrItem;
  };

  // smart ptr

  SMART_POINTER(CEditorScene);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorScene.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORSCENE_H_
