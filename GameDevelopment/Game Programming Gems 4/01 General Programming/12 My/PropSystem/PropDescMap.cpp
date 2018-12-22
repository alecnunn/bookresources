//--------------------------------------------------------------------------------------------------------------------//
//                                                MAP OF LOADED PROPERTIES                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "PropDescMap.h"
#ifdef    _DEBUG
#include  "PropDescMap.inl"
#endif

#include  "PropListLoad.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CPropDescMap::CPropDescMap()
  {}

CPropDescMap::~CPropDescMap()
  {
  std::for_each(m_map.begin(),m_map.end(),DeleteListLoad);
  }

//

void CPropDescMap::DeleteListLoad(std::pair<CStdString,CPropListLoad*> Iter)
  {
	CPropListLoad* pLoad = Iter.second;
	delete pLoad;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

void CPropDescMap::AddPropDesc(const CStdString& strClass,CPropListLoad* pListLoad)
  {
  m_map[strClass] = pListLoad;
  }

CPropListLoad* CPropDescMap::GetPropDesc(const CStdString& strClass)
  {
  iterStr2ListLoad iter = m_map.find(strClass);
  if(iter == m_map.end()) return NULL;
  return (*iter).second;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
