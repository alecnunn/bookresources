//--------------------------------------------------------------------------------------------------------------------//
//                                                LIST OF LOADED PROPERTIES                                           //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPLISTLOAD_INL_
#define   _GPG4_PROPLISTLOAD_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CPropListLoad* CPropListLoad::GetBase() const
  {
  return m_pBase;
  }

INLINE const CStdString& CPropListLoad::GetName() const
  {
  return m_strName;
  }

INLINE CPropList& CPropListLoad::GetFlatMatch()
  {
  return m_FlatMatch;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_PROPLISTLOAD_INL_
