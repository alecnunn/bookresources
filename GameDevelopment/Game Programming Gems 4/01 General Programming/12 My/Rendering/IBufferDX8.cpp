//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 INDEX BUFFER CLASS                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "IBufferDX8.h"
#ifdef    _DEBUG
#include  "IBufferDX8.inl"
#endif

#include  "MemC.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CIBufferDX8,CIBuffer);

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create HW IB
// in : buffer,nb indices,static & copy flags
// out: 0=OK
// rem: pBuffer's memory is now owned by this class if boRAMCopy==true
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::Create(u16* pBuffer,const u32 u32Count,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;

  D3DINDEXBUFFER_DESC ibDesc;
  Memory::MemSet(&ibDesc,0,sizeof(ibDesc));
  ibDesc.Format = D3DFMT_INDEX16;
  ibDesc.Size   = u32Count*sizeof(u16);
  ibDesc.Type   = D3DRTYPE_INDEXBUFFER;
  ibDesc.Pool   = D3DPOOL_DEFAULT;
  ibDesc.Usage  = D3DUSAGE_WRITEONLY;
  if(!boStatic) ibDesc.Usage |= D3DUSAGE_DYNAMIC;

  HRESULT hrErr = m_pDevice->CreateIndexBuffer(ibDesc.Size,ibDesc.Usage,ibDesc.Format,ibDesc.Pool,&m_pIB);
  if(FAILED(hrErr)) return hrErr;

  // base members

  m_u32Count  = m_u32ActiveCount = u32Count;
  m_boStatic  = boStatic;
  m_boRAMCopy = boRAMCopy;
  if(boRAMCopy) m_pBuffer = pBuffer;

  return WriteToHW(pBuffer,u32Count);                       // copy data to IB
  }

//----------------------------------------------- CreateCopy ---------------------------------------------------------//
// allocate memory for the buffer, and copy
// in : source buffer,nb indices,static & copy flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::CreateCopy(u16* pBuffer,const u32 u32Count,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;
  
  if(boRAMCopy)
    {
    m_pBuffer = new u16[u32Count];
    if(!m_pBuffer) return -2;
    m_u32Count = m_u32ActiveCount = u32Count;

    WriteToRAM(pBuffer,u32Count);                           // DON'T changer order !
    Create(m_pBuffer,u32Count,boStatic,boRAMCopy);
    }

  else
    {
    Create(pBuffer,u32Count,boStatic,boRAMCopy);
    }

  return 0;
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// release buffer's interface
// in :
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::Destroy()
  {
  if(m_pIB) m_pIB->Release();
  m_pIB = NULL;
  return 0;
  }

//----------------------------------------------- Write --------------------------------------------------------------//
// modify buffer's content
// in : source buffer,nb indices,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::Write(u16* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  WriteToRAM(pBuffer,u32Count,u32Pos);
  return WriteToHW(pBuffer,u32Count,u32Pos);
  }

//----------------------------------------------- WriteToHW ----------------------------------------------------------//
// modify HW buffer's content
// in : source buffer,nb indices,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::WriteToHW(u16* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!m_pIB)                       return -2;               // no buffer
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  // lock

  u32 u32Flags = D3DLOCK_NOSYSLOCK;
  if(IsDynamic()) u32Flags |= D3DLOCK_DISCARD;              // flush
  u16* pLockedData = NULL;

  HRESULT hrErr = m_pIB->Lock(u32Pos*sizeof(u16),u32Count*sizeof(u16),(BYTE**)&pLockedData,u32Flags);
  if(FAILED(hrErr)) return -4;

  // copy

  Memory::MemCpy(pLockedData,pBuffer,u32Count*sizeof(u16));

  // unlock

  hrErr = m_pIB->Unlock();
  if(FAILED(hrErr)) return -5;
  return 0;
  }

//----------------------------------------------- UpdateFromRAM ------------------------------------------------------//
// copy RAM to video
// in : nb indices,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CIBufferDX8::UpdateFromRAM(const u32 u32Count,const u32 u32Pos)
  {
  if(!HasRAMCopy()) return -1;

  // TBI
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
