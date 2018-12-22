//--------------------------------------------------------------------------------------------------------------------//
//                                                VERTEX BUFFER CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VBUFFER_INL_
#define   _GPG4_VBUFFER_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CVBuffer::CVBuffer() : m_pBuffer(NULL), m_u32Count(0), m_u32ActiveCount(0), m_boRAMCopy(false)
  {
  }

INLINE CVBuffer::~CVBuffer()
  {
  Destroy();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CVBuffer::GetCount() const
  {
  return m_u32Count;
  }

INLINE u32 CVBuffer::GetActiveCount() const
  {
  return m_u32ActiveCount;
  }

INLINE bool CVBuffer::IsStatic() const
  {
  return m_boStatic;
  }

INLINE bool CVBuffer::IsDynamic() const
  {
  return !IsStatic();
  }

INLINE bool CVBuffer::HasRAMCopy() const
  {
  return m_boRAMCopy;
  }

INLINE u32 CVBuffer::GetVertexSize() const
  {
  return m_u32VtxSize;
  }

INLINE u32 CVBuffer::SetActiveCount(const u32 u32Count)
  {
  m_u32ActiveCount = u32Count;
  if(m_u32ActiveCount > m_u32Count) m_u32ActiveCount = m_u32Count;
  return m_u32ActiveCount;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_VBUFFER_INL_
