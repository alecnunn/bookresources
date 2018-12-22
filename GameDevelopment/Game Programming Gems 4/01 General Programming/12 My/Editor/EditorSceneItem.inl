//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SCENE ITEM CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORSCENEITEM_INL_
#define   _GPG4_EDITORSCENEITEM_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorObj* CEditorSceneItem::GetObject() const
  {
  return m_spEditorObj;
  }

INLINE CEditorSceneItem* CEditorSceneItem::GetParent() const
  {
  return m_pParentItem;
  }

INLINE void CEditorSceneItem::SetParent(CEditorSceneItem* pParent)
  {
  m_pParentItem = pParent;
  }

INLINE HTREEITEM CEditorSceneItem::GetTreeHandle() const
  {
  return m_hItem;
  }

INLINE void CEditorSceneItem::SetTreeHandle(HTREEITEM hItem)
  {
  m_hItem = hItem;
  }

INLINE const CStdString& CEditorSceneItem::GetPrevParent() const
  {
  return m_strPrevParent;
  }

INLINE void CEditorSceneItem::SetPrevParent(const CStdString& strParent)
  {
  m_strPrevParent = strParent;
  }

//

INLINE bool CEditorSceneItem::IsDeleted() const
  {
  return m_boDeleted;
  }

INLINE void CEditorSceneItem::SetDeleted(const bool boDeleted)
  {
  m_boDeleted = boDeleted;
  if(m_spEditorObj && m_spEditorObj->GetEngineObj())
    m_spEditorObj->GetEngineObj()->SetDeleted(boDeleted);
  }

INLINE bool CEditorSceneItem::IsExpanded() const
  {
  return m_boExpanded;
  }

INLINE void CEditorSceneItem::SetExpanded(const bool boExpanded)
  {
  m_boExpanded = boExpanded;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CHILDREN                                                            //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorSceneItem* CEditorSceneItem::GetFirstChild()
  {
  m_itCurrItem = m_ChildItems.begin();
  return GetNextChild();
  }

//

INLINE CEditorSceneItem* CEditorSceneItem::GetNextChild(CEditorSceneItem* pCurrent)
  {
  // using pCurrent is slow, but enables multiple GetFirst/GetNext at the same time with only 1 m_itCurrItem

  if(pCurrent)
    {
    m_itCurrItem = std::find(m_ChildItems.begin(),m_ChildItems.end(),pCurrent);
    if(m_itCurrItem == m_ChildItems.end()) return NULL;
    ++m_itCurrItem;
    }

  //

  if(m_itCurrItem == m_ChildItems.end()) return NULL;
  CEditorSceneItem* pItem = *m_itCurrItem;
  ++m_itCurrItem;
  return pItem;
  }

//

INLINE u32 CEditorSceneItem::GetNbChildren() const
  {
  return m_ChildItems.size();
  }

//

INLINE bool CEditorSceneItem::AddChild(CEditorSceneItem* pChild)
  {
  if(!pChild) return false;
  m_ChildItems.push_back(pChild);
  pChild->SetParent(this);
  return true;
  }

//

INLINE bool CEditorSceneItem::RemoveChild(CEditorSceneItem* pChild)
  {
  if(!pChild) return false;
  m_ChildItems.remove(pChild);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITORSCENEITEM_INL_
