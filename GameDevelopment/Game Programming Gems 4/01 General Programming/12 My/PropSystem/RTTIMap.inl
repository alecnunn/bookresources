//--------------------------------------------------------------------------------------------------------------------//
//                                                RTTI MAP CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RTTIMAP_INL_
#define   _GPG4_RTTIMAP_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

inline CRTTIMap::CRTTIMap()
  {
  }

inline CRTTIMap::~CRTTIMap()
  {
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

inline const CRTTI* CRTTIMap::GetRTTI(const CStdString& strClass)
  {
  iterStr2RTTI iter = m_map.find(strClass);
  if(iter == m_map.end()) return NULL;
  return (*iter).second;
  }

//

inline void CRTTIMap::AddRTTI(const CStdString& strClass,const CRTTI* pRTTI)
  {
  if(!pRTTI) return;
  m_map[strClass] = pRTTI;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_RTTIMAP_INL_
