//--------------------------------------------------------------------------------------------------------------------//
//                                                MEMORY FUNCTIONS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_MEMC_INL_
#define   _GPG4_MEMC_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <memory.h>

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//

INLINE void* CMemC::MemSet(void* pvDest,ui uiChr,u32 u32Count)
  {
  return memset(pvDest,uiChr,u32Count);
  }

//

INLINE void* CMemC::MemCpy(void* pvDest,const void* pvSrc,u32 u32Count)
  {
  return memcpy(pvDest,pvSrc,u32Count);
  }

//

INLINE void* CMemC::Malloc(u32 u32Size)
  {
  return new u8[u32Size];
  }

//

INLINE bool CMemC::Free(void* pvAddr)
  {
  if(!pvAddr) return true;
  delete [] pvAddr;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_MEMC_INL_
