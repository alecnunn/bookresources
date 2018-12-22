//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEOBJ_INL_
#define   _GPG4_ENGINEOBJ_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineObj::CEngineObj()
  {
  m_pEditorObj = NULL;
  m_boDeleted  = false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CEngineObj::IsDeleted() const
  {
  return m_boDeleted;
  }

INLINE void CEngineObj::SetDeleted(const bool boDeleted)
  {
  m_boDeleted = boDeleted;
  }

INLINE CEditorObj* CEngineObj::GetEditorObj() const
  {
  return m_pEditorObj;
  }

INLINE void CEngineObj::SetEditorObj(CEditorObj* pEditorObj)
  {
  m_pEditorObj = pEditorObj;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINEOBJ_INL_
