//--------------------------------------------------------------------------------------------------------------------//
//                                                SMART POINTER TEMPLATE                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_SMARTPTR_INL_
#define   _GPG4_SMARTPTR_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

template <class CObj> inline TSmartPtr<CObj>::TSmartPtr(CObj* pObj)
  {
  m_pDumbPtr = pObj;
  if(m_pDumbPtr) m_pDumbPtr->AddRef();
  }

//

template <class CObj> inline TSmartPtr<CObj>::TSmartPtr(const TSmartPtr& Smart)
  {
  m_pDumbPtr = Smart.m_pDumbPtr;
  if(m_pDumbPtr) m_pDumbPtr->AddRef();
  }

//

template <class CObj> inline TSmartPtr<CObj>::~TSmartPtr()
  {
  if(m_pDumbPtr) m_pDumbPtr->Release();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATORS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

template <class CObj> inline TSmartPtr<CObj>& TSmartPtr<CObj>::operator = (const TSmartPtr& Smart)
  {
  if(m_pDumbPtr != Smart.m_pDumbPtr)                        // check assignment to self
    {
    if(m_pDumbPtr) m_pDumbPtr->Release();

    m_pDumbPtr = Smart.m_pDumbPtr;
    if(m_pDumbPtr) m_pDumbPtr->AddRef();
    }

  return *this;
  }

//

template <class CObj> inline TSmartPtr<CObj>& TSmartPtr<CObj>::operator = (CObj* pObj)
  {
  if(m_pDumbPtr != pObj)                                    // check assignment to self
    {
    if(m_pDumbPtr) m_pDumbPtr->Release();

    m_pDumbPtr = pObj;
    if(m_pDumbPtr) m_pDumbPtr->AddRef();
    }

  return *this;
  }

//

template <class CObj> inline CObj* TSmartPtr<CObj>::operator -> () const
  {
  return m_pDumbPtr;
  }

//

template <class CObj> inline CObj& TSmartPtr<CObj>::operator * () const
  {
  return *m_pDumbPtr;
  }

//

template <class CObj> inline TSmartPtr<CObj>::operator CObj* () const
  {
  return m_pDumbPtr;
  }

//

template <class CObj> inline bool TSmartPtr<CObj>::operator == (CObj* pObj) const
  {
  return(m_pDumbPtr == pObj);
  }

//

template <class CObj> inline bool TSmartPtr<CObj>::operator != (CObj* pObj) const
  {
  return(m_pDumbPtr != pObj);
  }

//

template <class CObj> inline bool TSmartPtr<CObj>::operator == (const TSmartPtr& Smart) const
  {
  return(m_pDumbPtr == Smart.m_pDumbPtr);
  }

//

template <class CObj> inline bool TSmartPtr<CObj>::operator != (const TSmartPtr& Smart) const
  {
  return(m_pDumbPtr != Smart.m_pDumbPtr);
  }

// this can sometimes be useful to cast the dumb pointer :

template <class CObj> inline CObj* TSmartPtr<CObj>::GetDumb() const
  {
  return m_pDumbPtr;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_SMARTPTR_INL_
