//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTY BASE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPERTY_INL_
#define   _GPG4_PROPERTY_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CProperty                                                           //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CProperty::CProperty(const CStdString& strName,const u32 u32Offset,const u32 u32Size) :
                            m_strName(strName), m_u32Offset(u32Offset), m_u32Size(u32Size), m_u32CtrlType(NONE)
  {}

INLINE CProperty::~CProperty()
  {}

// get/set params

INLINE const CStdString CProperty::GetType() const
  {
  return "";
  }

INLINE const CStdString& CProperty::GetName() const
  {
  return m_strName;
  }

INLINE u32 CProperty::GetOffset() const
  {
  return m_u32Offset;
  }

INLINE u32 CProperty::GetSize() const
  {
  return m_u32Size;
  }

INLINE const CStdString& CProperty::GetHelp() const
  {
  return m_strHelp;
  }

INLINE void CProperty::SetHelp(const CStdString& strHelp)
  {
  m_strHelp = strHelp;
  }

// value

INLINE const CStdString CProperty::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE bool CProperty::SetValue(const CPersistent* CPersistent, const CStdString& strVal) const
  {
  return false;
  }

INLINE const CStdString CProperty::GetRealValue(const CPersistent* pObj) const
  {
  return "";
  }

// flags

INLINE bool CProperty::IsExposed() const
  {
  return m_boExpose;
  }

INLINE bool CProperty::IsStreamed() const
  {
  return m_boStream;
  }

INLINE bool CProperty::IsReadOnly() const
  {
  return m_boReadOnly;
  }

INLINE void CProperty::SetFlags(const u32 u32Flags)
  {
  m_boExpose   = (u32Flags & EXPOSE)   != 0;
  m_boStream   = (u32Flags & STREAM)   != 0;
  m_boReadOnly = (u32Flags & READONLY) != 0;
  }

// control for editing

INLINE u32 CProperty::GetCtrlType() const
  {
  return m_u32CtrlType;
  }

INLINE void CProperty::SetCtrlType(u32 u32Type)
  {
  m_u32CtrlType = u32Type;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_PROPERTY_INL_
