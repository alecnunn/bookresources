//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES CLASSES                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPERTIES_INL_
#define   _GPG4_PROPERTIES_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropString                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropString::CPropString(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropString::GetType() const
  {
  return "String";
  }

INLINE u32 CPropString::GetEnumType() const
  {
  return CProperties::String;
  }

//

INLINE CStdString CPropString::Get(const CPersistent* pObj) const
  {
  return pObj ? *(CStdString*)((BYTE*)pObj+m_u32Offset) : "";
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFloat                                                          //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropFloat::CPropFloat(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropFloat::GetType() const
  {
  return "Float";
  }

INLINE u32 CPropFloat::GetEnumType() const
  {
  return CProperties::Float;
  }

// value

INLINE const CStdString CPropFloat::Float2String(const void* pAddr)
  {
  CStdString strVal;
  strVal.Format("%g",*((float*)pAddr));
  return strVal;
  }

INLINE const CStdString CPropFloat::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE const CStdString CPropFloat::GetRealValue(const CPersistent* pObj) const
  {
  return pObj ? Float2String((BYTE*)pObj+m_u32Offset) : "";
  }

//

INLINE float CPropFloat::Get(const CPersistent* pObj) const
  {
  return *(float*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropFloat::Set(const CPersistent* pObj,const float fVal) const
  {
  if(!pObj) return false;
  *(float*)((BYTE*)pObj+m_u32Offset) = fVal;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropBool                                                           //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropBool::CPropBool(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropBool::GetType() const
  {
  return "Bool";
  }

INLINE u32 CPropBool::GetEnumType() const
  {
  return CProperties::Bool;
  }

// value

INLINE const CStdString CPropBool::Bool2String(const void* pAddr)
  {
  return *((bool*)pAddr) ? "true" : "false";
  }

INLINE const CStdString CPropBool::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE const CStdString CPropBool::GetRealValue(const CPersistent* pObj) const
  {
  return pObj ? Bool2String((BYTE*)pObj+m_u32Offset) : "";
  }

//

INLINE bool CPropBool::Get(const CPersistent* pObj) const
  {
  return *(bool*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropBool::Set(const CPersistent* pObj,const bool boVal) const
  {
  if(!pObj) return false;
  *(bool*)((BYTE*)pObj+m_u32Offset) = boVal;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropU32                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropU32::CPropU32(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropU32::GetType() const
  {
  return "U32";
  }

INLINE u32 CPropU32::GetEnumType() const
  {
  return CProperties::U32;
  }

// value

INLINE const CStdString CPropU32::U32ToString(const void* pAddr)
  {
  CStdString strVal;
  strVal.Format("%u",*((u32*)pAddr));
  return strVal;
  }

INLINE const CStdString CPropU32::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE const CStdString CPropU32::GetRealValue(const CPersistent* pObj) const
  {
  return pObj ? U32ToString((BYTE*)pObj+m_u32Offset) : "";
  }

//

INLINE u32 CPropU32::Get(const CPersistent* pObj) const
  {
  return *(u32*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropU32::Set(const CPersistent* pObj,const u32 u32Val) const
  {
  if(!pObj) return false;
  *(u32*)((BYTE*)pObj+m_u32Offset) = u32Val;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect2D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropVect2D::CPropVect2D(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropVect2D::GetType() const
  {
  return "Vect2D";
  }

INLINE u32 CPropVect2D::GetEnumType() const
  {
  return CProperties::Vect2D;
  }

// value

INLINE const CStdString CPropVect2D::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

//

INLINE CVect2D CPropVect2D::Get(const CPersistent* pObj) const
  {
  return *(CVect2D*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropVect2D::Set(const CPersistent* pObj,const CVect2D& v2Val) const
  {
  if(!pObj) return false;
  *(CVect2D*)((BYTE*)pObj+m_u32Offset) = v2Val;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect3D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropVect3D::CPropVect3D(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropVect3D::GetType() const
  {
  return "Vect3D";
  }

INLINE u32 CPropVect3D::GetEnumType() const
  {
  return CProperties::Vect3D;
  }

// value

INLINE const CStdString CPropVect3D::GetValue(const CPersistent* pObj) const
  {
  // use this line to verify that "this" is a property of pObj, and include Persistent.h
  // if(!pObj || !pObj->IsPropOfThis(this)) return "";
  return GetRealValue(pObj);
  }

//

INLINE CVect3D CPropVect3D::Get(const CPersistent* pObj) const
  {
  return *(CVect3D*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropVect3D::Set(const CPersistent* pObj,const CVect3D& v3Val) const
  {
  if(!pObj) return false;
  *(CVect3D*)((BYTE*)pObj+m_u32Offset) = v3Val;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect4D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropVect4D::CPropVect4D(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropVect4D::GetType() const
  {
  return "Vect4D";
  }

INLINE u32 CPropVect4D::GetEnumType() const
  {
  return CProperties::Vect4D;
  }

// value

INLINE const CStdString CPropVect4D::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

//

INLINE CVect4D CPropVect4D::Get(const CPersistent* pObj) const
  {
  return *(CVect4D*)((BYTE*)pObj+m_u32Offset);
  }

INLINE bool CPropVect4D::Set(const CPersistent* pObj,const CVect4D& v4Val) const
  {
  if(!pObj) return false;
  *(CVect4D*)((BYTE*)pObj+m_u32Offset) = v4Val;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropPtr                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropPtr::CPropPtr(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropPtr::GetType() const
  {
  return "Ptr";
  }

INLINE u32 CPropPtr::GetEnumType() const
  {
  return CProperties::Ptr;
  }

// value

INLINE const CStdString CPropPtr::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE const CStdString CPropPtr::GetRealValue(const CPersistent* pObj) const
  {
  return pObj ? Ptr2String((BYTE*)pObj+m_u32Offset) : "";
  }

//

INLINE void* CPropPtr::Get(const CPersistent* pObj) const
  {
  u32 u32Val = *(u32*)((BYTE*)pObj+m_u32Offset);
  return((void*)u32Val);
  }

INLINE bool CPropPtr::Set(const CPersistent* pObj,const void* pPtr) const
  {
  if(!pObj) return false;
  if(m_u32Offset == u32(-1)) return false;
  *(u32*)((BYTE*)pObj+m_u32Offset) = (u32)pPtr;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropSP                                                             //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropSP::CPropSP(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CPropPtr(strName,u32Offset,u32Size)
  {}

// get/set params

INLINE const CStdString CPropSP::GetType() const
  {
  return "SP";
  }

INLINE u32 CPropSP::GetEnumType() const
  {
  return CProperties::SP;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFct                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// constructors & destructor

INLINE CPropFct::CPropFct(const CStdString& strName,const u32 u32Offset,const u32 u32Size) : CProperty(strName,u32Offset,u32Size)
  {
  m_pfnGet   = NULL;
  m_pfnSet   = NULL;
  m_pfnWrite = NULL;
  m_pfnRead  = NULL;
  m_pfnLink  = NULL;
  }

// get/set params

INLINE const CStdString CPropFct::GetType() const
  {
  return "Fct";
  }

INLINE u32 CPropFct::GetEnumType() const
  {
  return CProperties::Fct;
  }

// value

INLINE const CStdString CPropFct::GetValue(const CPersistent* pObj) const
  {
  return GetRealValue(pObj);
  }

INLINE bool CPropFct::SetValue(const CPersistent* pObj, const CStdString& strVal) const
  {
  return m_pfnSet ? (*m_pfnSet)(this,pObj,strVal) : false;
  }

INLINE const CStdString CPropFct::GetRealValue(const CPersistent* pObj) const
  {
  return m_pfnGet ? (*m_pfnGet)(this,pObj) : "";
  }

// streaming

INLINE bool CPropFct::Write(const CPersistent* pObj,CLevelFiles& Level) const
  {
  return m_pfnWrite ? (*m_pfnWrite)(this,pObj,Level) : false;
  }

INLINE bool CPropFct::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  return m_pfnWrite ? (*m_pfnWrite)(this,pObj,Level) : false;
  }

INLINE bool CPropFct::Read(const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level) const
  {
  return m_pfnRead ? (*m_pfnRead)(this,pObj,strVal,Level) : false;
  }

INLINE bool CPropFct::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  return m_pfnRead ? (*m_pfnRead)(this,pObj,"",Level) : false;
  }

INLINE bool CPropFct::Link(const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const
  {
  return m_pfnLink ? (*m_pfnLink)(this,pObj,strVal,pPointed) : false;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_PROPERTIES_INL_
