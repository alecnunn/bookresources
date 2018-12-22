//--------------------------------------------------------------------------------------------------------------------//
//                                                INDEX BUFFER CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_IBUFFER_INL_
#define   _GPG4_IBUFFER_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CIBuffer::CIBuffer() : m_pBuffer(NULL), m_u32Count(0), m_u32ActiveCount(0), m_boRAMCopy(false)
  {
  }

INLINE CIBuffer::~CIBuffer()
  {
  Destroy();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CIBuffer::GetCount() const
  {
  return m_u32Count;
  }

INLINE u32 CIBuffer::GetActiveCount() const
  {
  return m_u32ActiveCount;
  }

INLINE bool CIBuffer::IsStatic() const
  {
  return m_boStatic;
  }

INLINE bool CIBuffer::IsDynamic() const
  {
  return !IsStatic();
  }

INLINE bool CIBuffer::HasRAMCopy() const
  {
  return m_boRAMCopy;
  }

INLINE u32 CIBuffer::SetActiveCount(const u32 u32Count)
  {
  m_u32ActiveCount = u32Count;
  if(m_u32ActiveCount > m_u32Count) m_u32ActiveCount = m_u32Count;
  return m_u32ActiveCount;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_IBUFFER_INL_
