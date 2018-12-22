//--------------------------------------------------------------------------------------------------------------------//
//                                                CLASS-ID CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_CLASSID_INL_
#define   _GPG4_CLASSID_INL_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "MemC.h"
#include  "StringC.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CClassID::CClassID()
  {
  Memory::MemSet(m_szID,0,16);
  }

INLINE CClassID::CClassID(const CClassID& ID)
  {
  Memory::MemCpy(m_szID,ID.m_szID,16);
  }

INLINE CClassID::CClassID(const char* pszID)
  {
  Memory::MemSet (m_szID,0,16);
  String::StrnCpy(m_szID,pszID,16-1);
  }

INLINE CClassID::CClassID(const u32 u32A,const u32 u32B,const u32 u32C,const u32 u32D)
  {
  m_u32ID[0] = u32A; m_u32ID[1] = u32B; m_u32ID[2] = u32C; m_u32ID[3] = u32D;
  }

INLINE CClassID::~CClassID()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATORS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CClassID& CClassID::operator = (const CClassID& ID)
  {
  Memory::MemCpy(m_szID,ID.m_szID,16);
  return *this;
  }

INLINE CClassID& CClassID::operator = (const char* pszID)
  {
  Memory::MemSet (m_szID,0,16);
  String::StrnCpy(m_szID,pszID,16-1);
  return *this;
  }

INLINE CClassID& CClassID::operator = (const u32* pu32ID)
  {
  Memory::MemCpy(m_u32ID,pu32ID,16);
  return *this;
  }

//

INLINE bool CClassID::operator == (const CClassID& ID) const
  {
  return(*this == ID.m_u32ID);
  }

INLINE bool CClassID::operator != (const CClassID& ID) const
  {
  return(*this != ID.m_u32ID);
  }

INLINE bool CClassID::operator == (const char* pszID) const
  {
  CClassID ID(pszID);
  return(*this == ID.m_u32ID);
  }

INLINE bool CClassID::operator != (const char* pszID) const
  {
  CClassID ID(pszID);
  return(*this != ID.m_u32ID);
  }

INLINE bool CClassID::operator == (const u32* pu32ID) const
  {
  return(m_u32ID[0]==pu32ID[0] && m_u32ID[1]==pu32ID[1] && m_u32ID[2]==pu32ID[2] && m_u32ID[3]==pu32ID[3]);
  }

INLINE bool CClassID::operator != (const u32* pu32ID) const
  {
  return(m_u32ID[0]!=pu32ID[0] || m_u32ID[1]!=pu32ID[1] || m_u32ID[2]!=pu32ID[2] || m_u32ID[3]!=pu32ID[3]);
  }

//

INLINE bool CClassID::operator <  (const CClassID& ID) const
  {
  if(m_u32ID[0] < ID.m_u32ID[0]) return true;
  if(m_u32ID[0] > ID.m_u32ID[0]) return false;

  if(m_u32ID[1] < ID.m_u32ID[1]) return true;
  if(m_u32ID[1] > ID.m_u32ID[1]) return false;

  if(m_u32ID[2] < ID.m_u32ID[2]) return true;
  if(m_u32ID[2] > ID.m_u32ID[2]) return false;

  if(m_u32ID[3] < ID.m_u32ID[3]) return true;
  return false;
  }

INLINE bool CClassID::operator >  (const CClassID& ID) const
  {
  if(m_u32ID[0] > ID.m_u32ID[0]) return true;
  if(m_u32ID[0] < ID.m_u32ID[0]) return false;

  if(m_u32ID[1] > ID.m_u32ID[1]) return true;
  if(m_u32ID[1] < ID.m_u32ID[1]) return false;

  if(m_u32ID[2] > ID.m_u32ID[2]) return true;
  if(m_u32ID[2] < ID.m_u32ID[2]) return false;

  if(m_u32ID[3] > ID.m_u32ID[3]) return true;
  return false;
  }

// alphabetical ordering

INLINE bool CClassID::CompareNoCase(const CClassID& ID1,const CClassID& ID2)
  {
  return String::StriCmp(ID1.GetIDsz(),ID2.GetIDsz()) < 0;
  }

INLINE bool CClassIDCompareNoCase::operator () (const CClassID& ID1,const CClassID& ID2)
  {
  return CClassID::CompareNoCase(ID1,ID2);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE const char* CClassID::GetIDsz() const
  {
  return m_szID;
  }

INLINE const u32* CClassID::GetIDu32() const
  {
  return m_u32ID;
  }

INLINE void CClassID::GetID(u32* pu32A,u32* pu32B,u32* pu32C,u32* pu32D) const
  {
  if(pu32A) *pu32A = m_u32ID[0];
  if(pu32B) *pu32B = m_u32ID[1];
  if(pu32C) *pu32C = m_u32ID[2];
  if(pu32D) *pu32D = m_u32ID[3];
  }

//

INLINE void CClassID::Set(const char* pszID)
  {
  Memory::MemSet (m_szID,0,16);
  String::StrnCpy(m_szID,pszID,16-1);
  }

INLINE void CClassID::Set(const u32 u32A,const u32 u32B,const u32 u32C,const u32 u32D)
  {
  m_u32ID[0] = u32A; m_u32ID[1] = u32B; m_u32ID[2] = u32C; m_u32ID[3] = u32D;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_CLASSID_INL_
