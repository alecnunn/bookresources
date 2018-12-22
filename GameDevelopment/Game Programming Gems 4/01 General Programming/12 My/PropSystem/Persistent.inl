//--------------------------------------------------------------------------------------------------------------------//
//                                                PERSISTENT OBJECT CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PERSISTENT_INL_
#define   _GPG4_PERSISTENT_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CPersistent::CPersistent()
  {}

INLINE CPersistent::CPersistent(const CPersistent& Persistent) : CRefCount(Persistent)
  {}

INLINE CPersistent::~CPersistent()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CPersistent::GetDefaultName() const
  {
  return "";
  }

INLINE CStdString CPersistent::GetEditName() const
  {
  return "";
  }

INLINE const CStdString& CPersistent::GetName() const
  {
  return m_strName;
  }

INLINE void CPersistent::SetName(const CStdString& strName)
  {
  m_strName = strName;
  }

INLINE const CRTTI* CPersistent::GetWriteRTTI() const
  {
  return GetRTTI();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATORS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CPersistent& CPersistent::operator = (const CPersistent& Persistent)
  {
  return *this;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CPersistent::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_PERSISTENT_INL_
