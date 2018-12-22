//--------------------------------------------------------------------------------------------------------------------//
//                                                LIST OF PROPERTIES                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "PropList.h"
#ifdef    _DEBUG
#include  "PropList.inl"
#endif

#include  "File.h"
#include  "Persistent.h"
#include  "Properties.h"
#include  "VectorMatrix.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CPropList::~CPropList()
  {
  for(iterProp iter = m_Properties.begin(); iter != m_Properties.end(); ++iter)
    {                                                       // could use for_each
    CProperty* pProp = *iter;
    delete pProp;
    }
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- RegisterProp -------------------------------------------------------//
// create new property according to Type
// in : prop type,name,variable's offset,flags,help
// out: new property
//--------------------------------------------------------------------------------------------------------------------//

CProperty* CPropList::RegisterProp(const CProperties::PROPERTY_TYPE Type,const CStdString& strName,const u32 u32Off,
                                   const u32 u32Flags,const CStdString& strHelp)
  {
  CProperty* pProp;

  switch(Type)
    {
    case CProperties::String : pProp = new CPropString(strName,u32Off,sizeof(CStdString)); break;
    case CProperties::Float  : pProp = new CPropFloat (strName,u32Off,sizeof(float));      break;
    case CProperties::Bool   : pProp = new CPropBool  (strName,u32Off,sizeof(bool));       break;
    case CProperties::U32    : pProp = new CPropU32   (strName,u32Off,sizeof(u32));        break;
    case CProperties::Ptr    : pProp = new CPropPtr   (strName,u32Off,sizeof(void*));      break;
    case CProperties::SP     : pProp = new CPropSP    (strName,u32Off,sizeof(void*));      break;
    case CProperties::Fct    : pProp = new CPropFct   (strName,u32Off,sizeof(void*));      break;
    case CProperties::Vect2D : pProp = new CPropVect2D(strName,u32Off,sizeof(CVect2D));    break;
    case CProperties::Vect3D : pProp = new CPropVect3D(strName,u32Off,sizeof(CVect3D));    break;
    case CProperties::Vect4D : pProp = new CPropVect4D(strName,u32Off,sizeof(CVect4D));    break;
    default                  : pProp = NULL;
    }

  if(!pProp) return NULL;

  pProp->SetFlags(u32Flags);
  pProp->SetHelp (strHelp);

  AddProp(pProp);
  return pProp;
  }

//----------------------------------------------- RegisterProp -------------------------------------------------------//
// create new property according to strType
// in : type,name
// out: new property
// rem: this function is called when loading descriptions from a file, before mathing them with those in the exe;
//      that's why offset is set to (-1)
//--------------------------------------------------------------------------------------------------------------------//

CProperty* CPropList::RegisterProp(const CStdString& strType,const CStdString& strName)
  {
  CProperties::PROPERTY_TYPE Type = CProperties::Unknown;
  if(strType == "String")    Type = CProperties::String;
  if(strType == "Float" )    Type = CProperties::Float;
  if(strType == "Bool"  )    Type = CProperties::Bool;
  if(strType == "U32"   )    Type = CProperties::U32;
  if(strType == "Vect2D")    Type = CProperties::Vect2D;
  if(strType == "Vect3D")    Type = CProperties::Vect3D;
  if(strType == "Vect4D")    Type = CProperties::Vect4D;
  if(strType == "Ptr"   )    Type = CProperties::Ptr;
  if(strType == "SP"    )    Type = CProperties::SP;
  if(strType == "Fct"   )    Type = CProperties::Fct;

  if(Type == CProperties::Unknown) return NULL;
  return RegisterProp(Type,strName,u32(-1),CProperty::STREAM,"");
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Write --------------------------------------------------------------//
// write properties in text file
// in : file
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPropList::Write(IFile& File)
  {
  CProperty* pProp = GetFirstProp();

  while(pProp)
    {
    if(pProp->IsStreamed())
      {
      File.PutString("  <prop name='");
      File.PutString(pProp->GetName() + "'");
      File.PutString(" type='");
      File.PutString(pProp->GetType() + "'/>\n");
      }
    pProp = GetNextProp();
    }

  return true;
  }

//----------------------------------------------- WriteBin -----------------------------------------------------------//
// write properties in binary file
// in : file
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPropList::WriteBin(IFile& File)
  {
  CProperty* pProp = GetFirstProp();

  while(pProp)
    {
    if(pProp->IsStreamed())
      {
      CPersistent::WriteString2Bin(File,pProp->GetName());
      u32 u32Type = pProp->GetEnumType();
      File.Write(&u32Type,4);
      }
    pProp = GetNextProp();
    }

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
