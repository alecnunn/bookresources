//--------------------------------------------------------------------------------------------------------------------//
//                                                OBJECTS LIST CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LISTOBJ_INL_
#define   _GPG4_LISTOBJ_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CListObj::CListObj()
  {}

INLINE CListObj::~CListObj()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                NODES                                                               //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CListObj::GetNbObj() const
  {
  return m_Objs.size();
  }

INLINE CEngineObj* CListObj::GetFirstObj()
  {
  m_CurrObj = m_Objs.begin();
  return GetNextObj();
  }

INLINE CEngineObj* CListObj::GetNextObj(CEngineObj* pCurrent)
  {
  // using pCurrent is slow, but enables multiple GetFirst/GetNext at the same time with only 1 m_CurrObj

  if(pCurrent)
    {
    m_CurrObj = std::find(m_Objs.begin(),m_Objs.end(),pCurrent);
    if(m_CurrObj == m_Objs.end()) return NULL;
    ++m_CurrObj;
    }

  //

  if(m_CurrObj == m_Objs.end()) return NULL;
  CEngineObj* pObj = *m_CurrObj;
  ++m_CurrObj;
  return pObj;
  }

//

INLINE bool CListObj::AddObj(CEngineObj* pObj)
  {
  if(!pObj) return false;
  iterObj iter = std::find(m_Objs.begin(),m_Objs.end(),pObj);
  if(iter != m_Objs.end()) return true;                     // already in the list

  m_Objs.push_back(pObj);
  return true;
  }

INLINE bool CListObj::RemoveObj(CEngineObj* pObj)
  {
  if(!pObj) return false;
  m_Objs.remove(pObj);
  return true;
  }

INLINE void CListObj::RemoveAll()
  {
  m_Objs.clear();
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_LISTOBJ_INL_
