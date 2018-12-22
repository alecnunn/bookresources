//--------------------------------------------------------------------------------------------------------------------//
//                                                INDEX BUFFER CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "IBuffer.h"
#ifdef    _DEBUG
#include  "IBuffer.inl"
#endif

#include  "MemC.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CIBuffer,CRefCount);

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// copy given parameters and that's it !
// in : buffer,nb indices,static & copy flags
// out: 0=OK
// rem: pBuffer's memory is now owned by this class
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::Create(u16* pBuffer,const u32 u32Count,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;

  m_pBuffer   = pBuffer;
  m_u32Count  = m_u32ActiveCount = u32Count;
  m_boStatic  = boStatic;
  m_boRAMCopy = true;                                       // if we have come to this point, the buffer is in RAM

  return 0;
  }

//----------------------------------------------- CreateCopy ---------------------------------------------------------//
// allocate memory for the buffer, and copy
// in : source buffer,nb indices,static & copy flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::CreateCopy(u16* pBuffer,const u32 u32Count,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;
  
  m_pBuffer = new u16[u32Count];
  if(!m_pBuffer) return -2;

  Create(m_pBuffer,u32Count,boStatic,boRAMCopy);
  WriteToRAM(pBuffer,u32Count);
  return 0;
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// free buffer's memory
// in :
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::Destroy()
  {
  if(m_pBuffer)
    {
    delete [] m_pBuffer;
    m_pBuffer = NULL;
    }

  m_u32Count  = m_u32ActiveCount = 0;
  m_boRAMCopy = false;
  return 0;
  }

//----------------------------------------------- Write --------------------------------------------------------------//
// modify buffer's content
// in : source buffer,nb indices,start pos
// out: 0=OK
// rem: DON'T forget to override in derived classes !
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::Write(u16* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  return WriteToRAM(pBuffer,u32Count,u32Pos);
  }

//----------------------------------------------- Read ---------------------------------------------------------------//
// get buffer's content
// in : dest buffer,nb indices,start pos
// out: 0=OK
// rem: only allowed if a copy exists in RAM
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::Read(u16* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!HasRAMCopy() || !m_u32Count) return -2;               // no buffer in RAM
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  Memory::MemCpy(pBuffer,m_pBuffer+u32Pos,u32Count*sizeof(u16));
  return 0;
  }

//----------------------------------------------- WriteToRAM ---------------------------------------------------------//
// modify RAM buffer's content
// in : source buffer,nb indices,start pos
// out: 0=OK
// rem: use on derived class when several updates are needed consecutively, then call UpdateFromRAM
//      (to update HW buffer if any)
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::WriteToRAM(u16* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!m_pBuffer    || !m_u32Count) return -2;               // no buffer
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  Memory::MemCpy(m_pBuffer+u32Pos,pBuffer,u32Count*sizeof(u16));
  return 0;
  }

//----------------------------------------------- UpdateFromRAM ------------------------------------------------------//
// copy RAM to video
// in : nb indices,start pos
// out: 0=OK
// rem: designed for derived classes
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBuffer::UpdateFromRAM(const u32 u32Count,const u32 u32Pos)
  {
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
