//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES CLASSES                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Properties.h"
#ifdef    _DEBUG
#include  "Properties.inl"
#endif

#include  "LevelFiles.h"
#include  "LinkLoad.h"
#include  "Persistent.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CPropString,CProperty)
IMPLEMENT_RTTI(CPropFloat, CProperty)
IMPLEMENT_RTTI(CPropBool,  CProperty)
IMPLEMENT_RTTI(CPropU32,   CProperty)
IMPLEMENT_RTTI(CPropVect2D,CProperty)
IMPLEMENT_RTTI(CPropVect3D,CProperty)
IMPLEMENT_RTTI(CPropVect4D,CProperty)
IMPLEMENT_RTTI(CPropPtr,   CProperty)
IMPLEMENT_RTTI(CPropSP,    CPropPtr )
IMPLEMENT_RTTI(CPropFct,   CProperty)

//--------------------------------------------------------------------------------------------------------------------//
//                                                CProperties                                                         //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- GetTypeName --------------------------------------------------------//
// return type name - static
// in : type
// out: name
//--------------------------------------------------------------------------------------------------------------------//

CStdString CProperties::GetTypeName(u32 u32Type)
  {
  CStdString strTmp("Unknown");
  if(u32Type == String) strTmp = "String";
  if(u32Type == Float ) strTmp = "Float";
  if(u32Type == Bool  ) strTmp = "Bool";
  if(u32Type == U32   ) strTmp = "U32";
  if(u32Type == Vect2D) strTmp = "Vect2D";
  if(u32Type == Vect3D) strTmp = "Vect3D";
  if(u32Type == Vect4D) strTmp = "Vect4D";
  if(u32Type == Ptr   ) strTmp = "Ptr";
  if(u32Type == SP    ) strTmp = "SP";
  if(u32Type == Fct   ) strTmp = "Fct";
  return strTmp;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropString                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(String)

// value

bool CPropString::SetValue(const CPersistent* pObj, const CStdString& strVal) const
  {
  if(!pObj) return false;

  // convert "\n" to chrs 13+10
  CStdString strTmp(strVal);
  char szCR[3] = { 13,10,0 };
  strTmp.Replace("\\n",szCR);

  *(CStdString*)((BYTE*)pObj+m_u32Offset) = strTmp;
  return true;
  }

  //

const CStdString CPropString::GetRealValue(const CPersistent* pObj) const
  {
  if(!pObj) return "";

  // convert chrs 13+10 to "\n"
  CStdString strVal(*(CStdString*)((BYTE*)pObj+m_u32Offset));
  char szCR[3] = { 13,10,0 };
  strVal.Replace(szCR,"\\n");

  return strVal;
  }

// streaming

bool CPropString::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  return CPersistent::WriteString2Bin(Level.GetDataFile(),GetRealValue(pObj));
  }

  //

bool CPropString::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CStdString strVal;
  if(!CPersistent::ReadStringBin(Level.GetDataFile(),&strVal)) return false;

  if(GetOffset() != u32(-1)) return SetValue(pObj,strVal);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFloat                                                          //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Float)

// value

bool CPropFloat::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  float fVal = 0.f;
  if(sscanf(strVal,"%f",&fVal) != 1) return false;

  *(float*)((BYTE*)pObj+m_u32Offset) = fVal;
  return true;
  }

  //

bool CPropFloat::String2Float(const CStdString& strVal,float* pfVal) const
  {
  if(!pfVal) return false;
  float f0;
  if(sscanf(strVal,"%f",&f0) != 1) return false;

  *pfVal = f0;
  return true;
  }

// streaming

bool CPropFloat::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  float fVal = Get(pObj);
  return(Level.GetDataFile().Write(&fVal,sizeof(float)) == sizeof(float));
  }

  //

bool CPropFloat::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  float fVal;
  if(Level.GetDataFile().Read(&fVal,4) != 4) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,fVal);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropBool                                                           //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Bool)

// value

bool CPropBool::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;

  if(stricmp(strVal.c_str(),"true") == 0)
    {
    *(bool*)((BYTE*)pObj+m_u32Offset) = true;
    return true;
    }

  if(stricmp(strVal.c_str(),"false") == 0)
    {
    *(bool*)((BYTE*)pObj+m_u32Offset) = false;
    return true;
    }

  return false;
  }

// streaming

bool CPropBool::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  bool boVal = Get(pObj);
  return(Level.GetDataFile().Write(&boVal,sizeof(bool)) == sizeof(bool));
  }

  //

bool CPropBool::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  bool boVal;
  if(Level.GetDataFile().Read(&boVal,sizeof(bool)) != sizeof(bool)) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,boVal);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropU32                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(U32)

// value

bool CPropU32::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  u32 u32Val = 0;
  if(sscanf(strVal,"%u",&u32Val) != 1) return false;

  *(u32*)((BYTE*)pObj+m_u32Offset) = u32Val;
  return true;
  }

  //

bool CPropU32::String2U32(const CStdString& strVal,u32* pu32Val) const
  {
  if(!pu32Val) return false;
  u32 u32Val;
  if(sscanf(strVal,"%u",&u32Val) != 1) return false;

  *pu32Val = u32Val;
  return true;
  }

// streaming

bool CPropU32::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  u32 u32Val = Get(pObj);
  return(Level.GetDataFile().Write(&u32Val,sizeof(u32)) == sizeof(u32));
  }

  //

bool CPropU32::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  u32 u32Val;
  if(Level.GetDataFile().Read(&u32Val,4) != 4) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,u32Val);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect2D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Vect2D)

// value

const CStdString CPropVect2D::GetRealValue(const CPersistent* pObj) const
  {
  if(!pObj) return "";
  float* pAddr = (float*)((BYTE*)pObj+m_u32Offset);

  return CPropFloat::Float2String(pAddr) + "; " + CPropFloat::Float2String(pAddr+1);
  }

  //

bool CPropVect2D::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  float f0,f1;
  if(sscanf(strVal,"%f; %f",&f0,&f1) != 2) return false;

  float*  pf = (float*)((BYTE*)pObj+m_u32Offset);
  *pf++ = f0;
  *pf++ = f1;
  return true;
  }

  //

bool CPropVect2D::String2Floats(const CStdString& strVal,float* pfVect) const
  {
  if(!pfVect) return false;
  float f0,f1;
  if(sscanf(strVal,"%f; %f",&f0,&f1) != 2) return false;

  float*  pf = pfVect;
  *pf++ = f0;
  *pf++ = f1;
  return true;
  }

// streaming

bool CPropVect2D::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect2D v2Val = Get(pObj);
  return(Level.GetDataFile().Write(&v2Val.V,sizeof(CVect2D)) == sizeof(CVect2D));
  }

  //

bool CPropVect2D::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect2D v2Val;
  if(Level.GetDataFile().Read(&v2Val.V,sizeof(CVect2D)) != sizeof(CVect2D)) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,v2Val);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect3D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Vect3D)

// value

const CStdString CPropVect3D::GetRealValue(const CPersistent* pObj) const
  {
  if(!pObj) return "";
  float* pAddr = (float*)((BYTE*)pObj+m_u32Offset);

  return CPropFloat::Float2String(pAddr) + "; " + CPropFloat::Float2String(pAddr+1) + "; " + 
         CPropFloat::Float2String(pAddr+2);
  }

  //

bool CPropVect3D::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  float f0,f1,f2;
  if(sscanf(strVal,"%f; %f; %f",&f0,&f1,&f2) != 3) return false;

  float*  pf = (float*)((BYTE*)pObj+m_u32Offset);
  *pf++ = f0;
  *pf++ = f1;
  *pf++ = f2;
  return true;
  }

  //

bool CPropVect3D::String2Floats(const CStdString& strVal,float* pfVect) const
  {
  if(!pfVect) return false;
  float f0,f1,f2;
  if(sscanf(strVal,"%f; %f; %f",&f0,&f1,&f2) != 3) return false;

  float*  pf = pfVect;
  *pf++ = f0;
  *pf++ = f1;
  *pf++ = f2;
  return true;
  }

// streaming

bool CPropVect3D::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect3D v3Val = Get(pObj);
  return(Level.GetDataFile().Write(&v3Val.V,sizeof(CVect3D)) == sizeof(CVect3D));
  }

  //

bool CPropVect3D::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect3D v3Val;
  if(Level.GetDataFile().Read(&v3Val.V,sizeof(CVect3D)) != sizeof(CVect3D)) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,v3Val);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect4D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Vect4D)

// value

const CStdString CPropVect4D::GetRealValue(const CPersistent* pObj) const
  {
  if(!pObj) return "";
  float* pAddr = (float*)((BYTE*)pObj+m_u32Offset);

  return CPropFloat::Float2String(pAddr)   + "; " + CPropFloat::Float2String(pAddr+1) + "; " + 
         CPropFloat::Float2String(pAddr+2) + "; " + CPropFloat::Float2String(pAddr+3);
  }

  //

bool CPropVect4D::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  float f0,f1,f2,f3;
  if(sscanf(strVal,"%f; %f; %f; %f",&f0,&f1,&f2,&f3) != 4) return false;

  float*  pf = (float*)((BYTE*)pObj+m_u32Offset);
  *pf++ = f0;
  *pf++ = f1;
  *pf++ = f2;
  *pf++ = f3;
  return true;
  }

  //

bool CPropVect4D::String2Floats(const CStdString& strVal,float* pfVect) const
  {
  if(!pfVect) return false;
  float f0,f1,f2,f3;
  if(sscanf(strVal,"%f; %f; %f; %f",&f0,&f1,&f2,&f3) != 4) return false;

  float*  pf = pfVect;
  *pf++ = f0;
  *pf++ = f1;
  *pf++ = f2;
  *pf++ = f3;
  return true;
  }

// streaming

bool CPropVect4D::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect4D v4Val = Get(pObj);
  return(Level.GetDataFile().Write(&v4Val.V,sizeof(CVect4D)) == sizeof(CVect4D));
  }

  //

bool CPropVect4D::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  CVect4D v4Val;
  if(Level.GetDataFile().Read(&v4Val.V,sizeof(CVect4D)) != sizeof(CVect4D)) return false;

  if(GetOffset() != u32(-1)) return Set(pObj,v4Val);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropPtr                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// copy

PROPERTY_CREATECOPY(Ptr)

// value

const CStdString CPropPtr::Ptr2String(const void* pAddr)
  {
  CStdString strVal;
  strVal.Format("%x",*((u32*)pAddr));
  strVal.ToUpper();
  return "0x"+strVal;
  }

  //

bool CPropPtr::String2Ptr(const CStdString& strVal,void** ppVoid) const
  {
  if(!ppVoid) return false;
  u32 u32Val = 0;
  if(sscanf(strVal,"%x",&u32Val) != 1) return false;

  *ppVoid = (void*)u32Val;
  return true;
  }

  //

bool CPropPtr::SetValue(const CPersistent* pObj,const CStdString& strVal) const
  {
  if(!pObj) return false;
  u32 u32Val = 0;
  if(sscanf(strVal,"%x",&u32Val) != 1) return false;

  *(u32*)((BYTE*)pObj+m_u32Offset) = u32Val;
  return true;
  }

// streaming

bool CPropPtr::Write(const CPersistent* pObj,CLevelFiles& Level) const
  {
  if(!pObj) return false;
  if(!CProperty::Write(pObj,Level)) return false;

  CPersistent* pPointed = (CPersistent*)*(u32*)((BYTE*)pObj+m_u32Offset);
  if(!pPointed) return true;

  Level.AddObjToSave(pPointed);
  return true;
  }

  //

bool CPropPtr::WriteBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  if(!pObj) return false;
  CPersistent* pPointed = (CPersistent*)*(u32*)((BYTE*)pObj+m_u32Offset);

  if(Level.GetDataFile().Write(&pPointed,4) != 4) return false;
  if(!pPointed) return true;

  Level.AddObjToSave(pPointed);
  return true;
  }

  //

bool CPropPtr::Read(const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level) const
  {
  if(!pObj) return false;
  SetValue(pObj,"0x0");                                     // default = NULL

  if(strVal != "0x0")
    {
    CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropPtr*>(this),strVal,NULL);
    if(!pLink) return false;
    Level.AddLink(pLink);
    }
  return true;
  }

  //

bool CPropPtr::ReadBin(const CPersistent* pObj,CLevelFiles& Level) const
  {
  if(!pObj) return false;
  Set(pObj,NULL);

  CPersistent* pPtr;
  if(Level.GetDataFile().Read(&pPtr,4) != 4) return false;

  if((GetOffset() != u32(-1)) && pPtr)
    {
    CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropPtr*>(this),"",pPtr);
    if(!pLink) return false;
    Level.AddLink(pLink);
    }
  return true;
  }

  //

bool CPropPtr::Link(const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const
  {
  if(!pObj) return false;
  if(pPointed) return Set(pObj,pPointed);
  return SetValue(pObj,strVal);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropSP                                                             //
//--------------------------------------------------------------------------------------------------------------------//

// copy

CProperty* CPropSP::CreateCopy(CProperty* pDest)
  {
  CPropSP* pCopy = DYNAMIC_CAST(CPropSP,pDest);
  if(!pCopy)
    {
    pCopy = new CPropSP("",0,0);
    if(!pCopy) return NULL;
    }

  return CPropPtr::CreateCopy(pCopy);
  }

// streaming

bool CPropSP::Link(const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const
  {
  if(!pObj || !pPointed)  return false;
  if(!Set(pObj,pPointed)) return false;

  pPointed->AddRef();                                       // ref counter ++
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFct                                                            //
//--------------------------------------------------------------------------------------------------------------------//

// copy

CProperty* CPropFct::CreateCopy(CProperty* pDest)
  {
  CPropFct* pCopy = DYNAMIC_CAST(CPropFct,pDest);
  if(!pCopy)
    {
    pCopy = new CPropFct("",0,0);
    if(!pCopy) return NULL;
    }

  pCopy->m_pfnGet   = this->m_pfnGet;
  pCopy->m_pfnSet   = this->m_pfnSet;
  pCopy->m_pfnWrite = this->m_pfnWrite;
  pCopy->m_pfnRead  = this->m_pfnRead;
  pCopy->m_pfnLink  = this->m_pfnLink;

  return CProperty::CreateCopy(pCopy);
  }

// get/set params

void CPropFct::SetFct(pfnGet pGet,pfnSet pSet,pfnWrite pWrite,pfnRead pRead,pfnLink pLink)
  {
  m_pfnGet   = pGet;
  m_pfnSet   = pSet;
  m_pfnWrite = pWrite;
  m_pfnRead  = pRead;
  m_pfnLink  = pLink;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
