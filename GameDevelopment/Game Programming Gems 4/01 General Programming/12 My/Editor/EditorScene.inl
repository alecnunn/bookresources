//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SCENE CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORSCENE_INL_
#define   _GPG4_EDITORSCENE_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorScene::CEditorScene()
  {}

INLINE CEditorScene::~CEditorScene()
  {
  m_Items.clear();
  m_spRoot = NULL;
  // this is automatic BUT needed to avoid a crash in UnregisterItem
  // (m_Items must still exist when the SceneItems are deleted)
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorSceneItem* CEditorScene::GetRootItem() const
  {
  return m_spRoot;
  }

//

INLINE void CEditorScene::SetRootItem(CEditorSceneItem* pRoot)
  {
  m_spRoot = pRoot;
  }

//

INLINE CEditorSceneItem* CEditorScene::GetItem(const CStdString& strItem)
  {
  iterItem Iter = m_Items.find(strItem);
  if(Iter == m_Items.end()) return NULL;
  return Iter->second;
  }

//

INLINE CEditorSceneItem* CEditorScene::GetFirstItem()
  {
  m_itCurrItem = m_Items.begin();
  return GetNextItem();
  }

//

INLINE CEditorSceneItem* CEditorScene::GetNextItem(const CStdString* pstrCurrent)
  {
  if(pstrCurrent)
    {
    m_itCurrItem = m_Items.find(*pstrCurrent);
    if(m_itCurrItem == m_Items.end()) return NULL;
    ++m_itCurrItem;
    }

  if(m_itCurrItem == m_Items.end()) return NULL;
  CEditorSceneItem* pItem = m_itCurrItem->second;
  ++m_itCurrItem;
  return pItem;
  }

//

INLINE u32 CEditorScene::GetNbItem() const
  {
  return m_Items.size();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                MAP                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CEditorScene::RegisterItem(const CStdString& strName,CEditorSceneItem* pItem)
  {
  if(!pItem || strName.empty()) return false;
  iterItem iter = m_Items.find(strName);
  if(iter != m_Items.end()) return false;

  std::pair<iterItem,bool> Result = m_Items.insert(std::make_pair(strName,pItem));
  return Result.second;
  }

//

INLINE bool CEditorScene::UnregisterItem(const CStdString& strName,CEditorSceneItem* pItem)
  {
  if(!pItem) return false;
  iterItem Iter = m_Items.find(strName);
  if(Iter == m_Items.end()) return false;                   // not found
  if(Iter->second != pItem) return false;                   // another item is registered with this name
  m_Items.erase(Iter);
  return true;
  }

//

INLINE void CEditorScene::UnregisterAllItems()
  {
  m_Items.clear();
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITORSCENE_INL_
