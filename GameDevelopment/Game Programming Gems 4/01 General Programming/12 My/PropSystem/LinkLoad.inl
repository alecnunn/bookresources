//--------------------------------------------------------------------------------------------------------------------//
//                                                LINK CLASS                                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LINKLOAD_INL_
#define   _GPG4_LINKLOAD_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

inline CLinkLoad::CLinkLoad(const CPersistent* pObj,const CProperty* pProp,const CStdString& strID,const CPersistent* pID) : 
                            m_pObj(pObj), m_pProp(pProp), m_strID(strID), m_pID(pID)
  {}

inline CLinkLoad::~CLinkLoad()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

inline const CPersistent* CLinkLoad::GetObj() const
  {
  return m_pObj;
  }

inline const CProperty* CLinkLoad::GetProp() const
  {
  return m_pProp;
  }

inline const CStdString& CLinkLoad::GetID() const
  {
  return m_strID;
  }

inline const CPersistent* CLinkLoad::GetParam() const
  {
  return m_pID;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_LINKLOAD_INL_
