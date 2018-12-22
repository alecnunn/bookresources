//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTY BASE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Property.h"
#ifdef    _DEBUG
#include  "Property.inl"
#endif

#include  "File.h"
#include  "LevelFiles.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_ROOT_RTTI(CProperty);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CProperty                                                           //
//--------------------------------------------------------------------------------------------------------------------//

// copy

CProperty* CProperty::CreateCopy(CProperty* pDest)
  {
  CProperty* pCopy = pDest;
  if(!pCopy) return NULL;

  pCopy->m_strName    = this->m_strName;
  pCopy->m_u32Offset  = this->m_u32Offset;
  pCopy->m_u32Size    = this->m_u32Size;
  pCopy->m_strHelp    = this->m_strHelp;

  pCopy->m_boExpose   = this->m_boExpose;
  pCopy->m_boStream   = this->m_boStream;
  pCopy->m_boReadOnly = this->m_boReadOnly;
  return pCopy;
  }

// streaming

bool CProperty::Write(const CPersistent* pObj,CLevelFiles& Level) const
  {
  return Level.GetDataFile().PutString(GetRealValue(pObj));
  }

  //

bool CProperty::Read(const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level) const
  {
  return SetValue(pObj,strVal);
  }

  //

bool CProperty::Link(const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const
  {
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
