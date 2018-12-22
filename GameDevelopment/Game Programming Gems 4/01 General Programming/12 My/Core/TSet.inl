//--------------------------------------------------------------------------------------------------------------------//
//                                                SET WITH LIMITED INTERFACE                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_TSET_INL_
#define   _GPG4_TSET_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

template <class CObj> inline TSet<CObj>::TSet()
  {}

template <class CObj> inline TSet<CObj>::~TSet()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

template <class CObj> inline void TSet<CObj>::ClearSet()
  {
  m_Set.clear();
  }

template <class CObj> inline void TSet<CObj>::AddToSet(const CObj& Obj)
  {
  m_Set.insert(Obj);
  }

template <class CObj> inline bool TSet<CObj>::RemoveFromSet(const CObj& Obj)
  {
  return(m_Set.erase(Obj) > 0);
  }

template <class CObj> inline bool TSet<CObj>::IsInSet(const CObj& Obj)
  {
  return(m_Set.find(Obj) != m_Set.end());
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_TSET_INL_
