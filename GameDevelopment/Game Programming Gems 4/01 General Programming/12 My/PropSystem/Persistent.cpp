//--------------------------------------------------------------------------------------------------------------------//
//                                                PERSISTENT OBJECT CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Persistent.h"
#ifdef    _DEBUG
#include  "Persistent.inl"
#endif

#include  "LevelFiles.h"
#include  "Logger.h"
#include  "Properties.h"
#include  "PropListLoad.h"
#include  "RTTIMap.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CPersistent,CRefCount);
IMPLEMENT_PROPERTIES(CPersistent,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::DefineProperties()                        // static
  {
  REGISTER_PROP(String,CPersistent,m_strName,"Name",CProperty::EXPOSE|CProperty::STREAM,"");
  return true;
  }

//----------------------------------------------- IsPropOfThis -------------------------------------------------------//
// check the given property belongs to the RTTI tree of "this"
// in : prop
// out: found in RTTI tree ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::IsPropOfThis(const CProperty* pProp) const
  {
  const CRTTI* pRTTI = GetRTTI();
  while(pRTTI)
    {
    if(IsPropOfRTTI(pProp,pRTTI)) return true;
    pRTTI = pRTTI->GetBaseRTTI();
    }
  return false;
  }

//----------------------------------------------- IsPropOfRTTI -------------------------------------------------------//
// check the given property belongs to the given RTTI
// in : prop,RTTI
// out: found in RTTI ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::IsPropOfRTTI(const CProperty* pProp,const CRTTI* pRTTI) const
  {
  if(!pRTTI)  return false;
  CExtraData* pData  = pRTTI->GetExtraData();
  CExtraProp* pExtra = DYNAMIC_CAST(CExtraProp,pData);
  if(!pExtra) return false;
  CPropList*  pProps = pExtra->GetPropList();
  if(!pProps) return false;
  
  CProperty* pPropThis = pProps->GetFirstProp();
  while(pPropThis)
    {
    if(pPropThis == pProp) return true;
    pPropThis = pProps->GetNextProp();
    }
  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- IsPropExposed ------------------------------------------------------//
// determine if a property is exposed
// in : property
// out: true==exposed
// rem: virtual function, aimed at being overriden
//      the default behaviour is simply to ask the property
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::IsPropExposed(CProperty* pProp)
  {
  return pProp->IsExposed();
  }

//----------------------------------------------- IsPropReadOnly -----------------------------------------------------//
// determine if a property is read only
// in : property
// out: true==read only
// rem: virtual function, aimed at being overriden
//      the default behaviour is simply to ask the property
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::IsPropReadOnly(CProperty* pProp)
  {
  return pProp->IsReadOnly();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING : WRITE                                                   //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- WriteString2Bin ----------------------------------------------------//
// write string to binary format file: length + chrs
// in : file,string
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::WriteString2Bin(IFile& File,const CStdString& strTxt)
  {
  u32 u32Len = strTxt.size();
  if(File.Write(&u32Len,4) != 4) return false;
  if(!u32Len) return true;
  if(File.Write(strTxt.c_str(),u32Len) != u32Len) return false;
  return true;
  }

//----------------------------------------------- Write --------------------------------------------------------------//
// write THIS object to current level
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::Write(CLevelFiles& Level)
  {
  if(Level.ObjIsSaved(this)) return true;

  if(!WritePropDesc(Level))  return false;
  if(!WriteData    (Level))  return false;

  Level.AddSavedObj(this);

  // write pointed to objects

  bool boRes = true;

  while(true)
    {
    if(Level.ObjToSaveCount() == 0) break;                  // finished
    CPersistent* pPointed = Level.PopObjToSave();
    if(!pPointed) continue;

    boRes &= pPointed->Write(Level);
    }

  return boRes;
  }

//----------------------------------------------- WritePropDesc ------------------------------------------------------//
// write description of properties
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::WritePropDesc(CLevelFiles& Level)
  {
  const CRTTI* pRTTI = GetWriteRTTI();                      // avoid renderer specific classes
  if(!pRTTI) return false;
  return WriteBaseDesc(Level,pRTTI);
  }

//----------------------------------------------- WriteBaseDesc ------------------------------------------------------//
// recursive function writing the hierarchy of props desc.
// in : level, current RTTI
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::WriteBaseDesc(CLevelFiles& Level,const CRTTI* pRTTI)
  {
  if(!pRTTI) return true;                                   // we've reached the end
  if(Level.DescIsSaved(pRTTI->GetClassName())) 
    return true;                                            // already saved

  const CRTTI* pBase = pRTTI->GetBaseRTTI();
  if(pBase) WriteBaseDesc(Level,pBase);                     // recurse

  File& File = Level.GetDescFile();

  // binary format

  if(Level.BinaryMode())
    {
    WriteString2Bin(File,pRTTI->GetClassName());
    if(pBase) WriteString2Bin(File,pBase->GetClassName());
    else      WriteString2Bin(File,"");

    CExtraData* pData  = pRTTI->GetExtraData();
    CExtraProp* pExtra = DYNAMIC_CAST(CExtraProp,pData);
    if(pExtra)
      {
      CPropList* pPropList = pExtra->GetPropList();
      pPropList->WriteBin(File);                            // "this" class prop desc
      }

    u32 u32EndMarker = 0;
    File.Write(&u32EndMarker,4);
    }

  // text format

  else
    {
    File.PutString("<class name='");
    File.PutString(pRTTI->GetClassName()+"'");              // class name
  
    File.PutString(" base='");
    if(pBase) File.PutString(pBase->GetClassName());        // base class
    File.PutString("'>\n");

    CExtraData* pData  = pRTTI->GetExtraData();
    CExtraProp* pExtra = DYNAMIC_CAST(CExtraProp,pData);
    if(pExtra)
      {
      CPropList* pPropList = pExtra->GetPropList();
      pPropList->Write(File);                               // "this" class prop desc
      }
  
    if(!File.PutString("</class>\n\n")) return false;
    }

  //

  Level.AddSavedDesc(pRTTI->GetClassName());
  return true;
  }

//----------------------------------------------- WriteData ----------------------------------------------------------//
// write properties'data
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::WriteData(CLevelFiles& Level)
  {
  const CRTTI* pRTTI = GetWriteRTTI();                      // avoid renderer specific classes
  if(!pRTTI) return false;

  File& File  = Level.GetDataFile();
  bool  boRes = true;

  // binary format

  if(Level.BinaryMode())
    {
    boRes &= WriteString2Bin(File,pRTTI->GetClassName());
    const CPersistent* pThis = this;
    boRes &= (File.Write(&pThis,4) == 4);

    boRes &= WriteBaseData(Level,pRTTI);
    }

  // text mode

  else
    {
    File.PutString("<data class='");
    File.PutString(pRTTI->GetClassName()+"' id='");         // class name

    const CPersistent* pThis = this;
    File.PutString(CPropPtr::Ptr2String(&pThis)+"'>\n");    // object address (=ID) for linking
  
    bool boRes = WriteBaseData(Level,pRTTI);
  
    File.PutString("</data>\n\n");
    }

  return boRes;
  }

//----------------------------------------------- WriteBaseData ------------------------------------------------------//
// recursive function writing the hierarchy of props data
// in : level, current RTTI
// out: OK ?
// rem: traversal MUST be the same as in WriteBaseDesc
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::WriteBaseData(CLevelFiles& Level,const CRTTI* pRTTI)
  {
  if(!pRTTI) return true;
  
  const CRTTI* pBase = pRTTI->GetBaseRTTI();
  if(pBase) WriteBaseData(Level,pBase);                     // recurse
  
  CExtraData* pData  = pRTTI->GetExtraData();
  CExtraProp* pExtra = DYNAMIC_CAST(CExtraProp,pData);
  if(pExtra)
    {
    CPropList* pPropList = pExtra->GetPropList();
    CProperty* pProp     = pPropList->GetFirstProp();

    File& File = Level.GetDataFile();
      
    while(pProp)
      {
      if(pProp->IsStreamed())
        {
        // binary format

        if(Level.BinaryMode())
          {
          pProp->WriteBin(this,Level);
          }

        // text format

        else
          {
          File.PutString("  ");
          pProp->Write(this,Level);      
          File.PutString("\n");
          }
        }
      pProp = pPropList->GetNextProp();
      }
    }
  
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING : READ                                                    //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- LookForTag ---------------------------------------------------------//
// look for a given tag in a text file - static
// in : file,line to return,line buffer size,tag,stop marker
// out: found ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::LookForTag(IFile& File,char* pszLine,const u32 u32Size,const char* pszTag,const char* pszStop)
  {
  while(File.GetString(pszLine,u32Size))
    {
    if(strstr(pszLine,pszTag))  return true;                // tag  found
    if(strstr(pszLine,pszStop)) return false;               // stop found (ex: </xml>)
    }

  return false;                                             // EOF, tag not found
  }

//----------------------------------------------- LookForTags --------------------------------------------------------//
// look for several tags at a time in a text file - static
// in : file,line to return,line buffer size,tags array,nb tags in array
// out: tag number (-1 = none found)
//--------------------------------------------------------------------------------------------------------------------//

u32 CPersistent::LookForTags(IFile& File,char* pszLine,const u32 u32Size,CStdString* pstrTags,const u32 u32NbTags)
  {
  while(File.GetString(pszLine,u32Size))
    {
    for(u32 u32Tag = 0; u32Tag < u32NbTags; u32Tag++)
      {
      if(strstr(pszLine,pstrTags[u32Tag])) return u32Tag;
      }
    }

  return u32(-1);
  }

//----------------------------------------------- GetTagParam --------------------------------------------------------//
// find the value of a given parameter in a line of text - static
// in : line buffer,wanted parameter
// out: parameter value
//--------------------------------------------------------------------------------------------------------------------//

CStdString CPersistent::GetTagParam(const char* pszLine,const char* pszParam)
  {
  CStdString strVal;
  CStdString strParam(pszParam);
  strParam += "='";

  char* pszPos = strstr(pszLine,strParam.c_str());
  if(pszPos)
    {
    pszPos += strParam.GetLength();
    char* pszEnd = strchr(pszPos,'\'');
    if(pszEnd)
      {
      *pszEnd = 0;
      strVal  = pszPos;
      *pszEnd = '\'';
      }
    }

  return strVal;
  }

//----------------------------------------------- ReadStringBin ------------------------------------------------------//
// read string from binary format file: length + chrs
// in : file,&string
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadStringBin(IFile& File,CStdString* pstrTxt)
  {
  if(!pstrTxt) return false;
  *pstrTxt   = "";
  u32 u32Len = 0;
  if(File.Read(&u32Len,4) != 4) return false;
  if(!u32Len) return true;                                  // empty string

  char szTxt[1024];
  if(File.Read(szTxt,u32Len) != u32Len) return false;
  szTxt[u32Len] = 0;
  *pstrTxt = szTxt;
  return true;
  }

//----------------------------------------------- ReadPropFile -------------------------------------------------------//
// read properties descriptions file
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadPropFile(CLevelFiles& Level)
  {
  CStdString  strTags[2] = { "<class ","</xml>" };          // optim: most used comes 1st
  u32         u32Tag;
  const u32   u32Size    = 1024;
  char szLine[u32Size];

  CLogger* pLogger = Level.GetLogger();
  File&   DescFile = Level.GetDescFile();

  if(LookForTag(DescFile,szLine,u32Size,"<xml>"))
    {
    while((u32Tag = LookForTags(DescFile,szLine,u32Size,strTags,2)) != u32(-1))
      {
      if(u32Tag == 1) return true;                          // </xml>
      if(u32Tag == 0)
        {                                                   // <class
        CStdString strClass = GetTagParam(szLine,"name");
        CStdString strBase  = GetTagParam(szLine,"base");
        if(pLogger) _DLOG2_((*pLogger),LPCTSTR("class: "+strClass+"  base: "+strBase),"props file");

        // look for base class

        CPropListLoad* pListBase = NULL;
        if(!strBase.empty())
          {
          pListBase = Level.GetPropDesc(strBase);
          if(!pListBase)
            {
            if(pLogger) _DWAR_((*pLogger),LPCTSTR("base class ["+strBase+"] not found"));
            return false;
            }
          }

        // look for class prop desc

        if(Level.GetPropDesc(strClass))
          {
          if(pLogger) _DWAR_((*pLogger),LPCTSTR("class ["+strClass+"] already exists"));
          return false;
          }

        // create new prop desc & add to map

        CPropListLoad* pListLoad = new CPropListLoad(strClass,pListBase);
        if(!pListLoad)
          {
          if(pLogger) _DWAR_((*pLogger),"'new' failed");
          return false;
          }
        Level.AddPropDesc(strClass,pListLoad);

        // load properties

        if(!ReadProperties(Level,pListLoad))
          {
          if(pLogger) _DWAR_((*pLogger),"'ReadProperties' failed");
          return false;
          }
        if(!MatchProperties(Level,pListLoad))
          {
          if(pLogger) _DWAR_((*pLogger),"'MatchProperties' failed : class ["+strClass+"]");
          return false;
          }
        }
      }
    }

  return false;
  }

//----------------------------------------------- ReadPropFileBin ----------------------------------------------------//
// read properties descriptions file
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadPropFileBin(CLevelFiles& Level)
  {
  CLogger* pLogger = Level.GetLogger();
  File&   DescFile = Level.GetDescFile();
  CStdString strClass,strBase;

  while(!DescFile.Eof())
    {
    if(!ReadStringBin(DescFile,&strClass)) return true;     // EOF
    if(!ReadStringBin(DescFile,&strBase))  return false;
    if(pLogger) _DLOG2_((*pLogger),LPCTSTR("class: "+strClass+"  base: "+strBase),"props file");

    // look for base class

    CPropListLoad* pListBase = NULL;
    if(!strBase.empty())
      {
      pListBase = Level.GetPropDesc(strBase);
      if(!pListBase)
        {
        if(pLogger) _DWAR_((*pLogger),LPCTSTR("base class ["+strBase+"] not found"));
        return false;
        }
      }

    // look for class prop desc

    if(Level.GetPropDesc(strClass))
      {
      if(pLogger) _DWAR_((*pLogger),LPCTSTR("class ["+strClass+"] already exists"));
      return false;
      }

    // create new prop desc & add to map

    CPropListLoad* pListLoad = new CPropListLoad(strClass,pListBase);
    if(!pListLoad)
      {
      if(pLogger) _DWAR_((*pLogger),"'new' failed");
      return false;
      }
    Level.AddPropDesc(strClass,pListLoad);

    // load properties

    if(!ReadPropertiesBin(Level,pListLoad))
      {
      if(pLogger) _DWAR_((*pLogger),"'ReadPropertiesBin' failed");
      return false;
      }
    if(!MatchProperties(Level,pListLoad))
      {
      if(pLogger) _DWAR_((*pLogger),"'MatchProperties' failed : class ["+strClass+"]");
      return false;
      }
    }

  return true;
  }

//----------------------------------------------- ReadProperties -----------------------------------------------------//
// read properties of a class
// in : level, prop desc
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadProperties(CLevelFiles& Level,CPropListLoad* pListLoad)
  {
  CStdString  strTags[2] = { "<prop ","</class>" };
  u32         u32Tag;
  const u32   u32Size    = 1024;
  char szLine[u32Size];

  CLogger* pLogger = Level.GetLogger();
  File&   DescFile = Level.GetDescFile();

  while((u32Tag = LookForTags(DescFile,szLine,u32Size,strTags,2)) != u32(-1))
    {
    if(u32Tag == 1) return true;                            // </class>
    if(u32Tag == 0)
      {                                                     // <prop 
      CStdString strVar  = GetTagParam(szLine,"name");
      CStdString strType = GetTagParam(szLine,"type");
      if(pLogger) _DLOG2_((*pLogger),LPCTSTR("prop: "+strVar+"  type: "+strType),"props file");

      // create property

      if(!pListLoad->RegisterProp(strType,strVar))
        {
        if(pLogger) _DWAR_((*pLogger),LPCTSTR("unknown property type ["+strType+"]"));
        return false;
        }
      }
    }

  return false;
  }

//----------------------------------------------- ReadPropertiesBin --------------------------------------------------//
// read properties of a class
// in : level, prop desc
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadPropertiesBin(CLevelFiles& Level,CPropListLoad* pListLoad)
  {
  CLogger* pLogger = Level.GetLogger();
  File&   DescFile = Level.GetDescFile();
  CStdString strVar;

  while(!DescFile.Eof())
    {
    if(!ReadStringBin(DescFile,&strVar))  return false;
    if(strVar.empty()) return true;                         // end of list

    u32 u32Type;
    if(DescFile.Read(&u32Type,4) != 4) return false;
    if(pLogger) _DLOG2_((*pLogger),LPCTSTR("prop: "+strVar+"  type: "+CProperties::GetTypeName(u32Type)),"props file");

    // create property

    CProperties::PROPERTY_TYPE  Type = (CProperties::PROPERTY_TYPE)u32Type;
    if(!pListLoad->RegisterProp(Type,strVar,u32(-1),CProperty::STREAM,""))
      {
      if(pLogger) _DWAR_((*pLogger),LPCTSTR("unknown property type ["+CProperties::GetTypeName(u32Type)+"]"));
      return false;
      }
    }

  return false;
  }

//----------------------------------------------- MatchProperties ----------------------------------------------------//
// make the link between file & exe properties
// in : level,file prop desc
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::MatchProperties(CLevelFiles& Level,CPropListLoad* pListLoad)
  {
  if(!pListLoad || !Level.GetRTTIMap()) return false;

	// get RTTI & props infos

  const CRTTI* pRTTI = Level.GetRTTIMap()->GetRTTI(pListLoad->GetName());
  if(!pRTTI) return false;

  RecursiveMatch(pListLoad,pListLoad,pRTTI);
  return true;
  }

//----------------------------------------------- RecursiveMatch -----------------------------------------------------//
// make the link between file & exe properties
// in : file prop desc,base prop desc,exe RTTI
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CPersistent::RecursiveMatch(CPropListLoad* pDescFile,CPropListLoad* pDescBase,const CRTTI* pExeRTTI)
  {
	// we need to begin to parse the props with the base-most class :
	if(pDescBase->GetBase()) RecursiveMatch(pDescFile,pDescBase->GetBase(),pExeRTTI);

  CProperty* pPropLoad  = pDescBase->GetFirstProp();
  u32        u32NbProp  = 0;
  u32        u32NbMatch = 0;

	while(pPropLoad)
	  {
		const CRTTI* pRTTI = pExeRTTI;
		
		while(pRTTI)
		  {
			if(MatchProperty(pDescFile,pRTTI,pPropLoad))
			  {
				u32NbMatch++;
				break;
			  }
			pRTTI = pRTTI->GetBaseRTTI();
		  }
		
		if(!pRTTI)
		  {													                            // insert the prop as 'unmatched'
			InsertProperty(pDescFile,pPropLoad,NULL);
		  }
		
		u32NbProp++;
		pPropLoad = pDescBase->GetNextProp();
	  }
  }

//----------------------------------------------- MatchProperty ------------------------------------------------------//
// look for given property in given RTTI
// in : file prop desc,RTTI,property
// out: found?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::MatchProperty(CPropListLoad* pDescFile,const CRTTI* pRTTI,CProperty* pPropLoad)
  {
  if(!pRTTI)  return false;
  CExtraData* pData    = pRTTI->GetExtraData();
  CExtraProp* pExtra   = DYNAMIC_CAST(CExtraProp,pData);
  if(!pExtra) return false;
  CPropList*  pDescExe = pExtra  ->GetPropList();
  CProperty*  pPropExe = pDescExe->GetFirstProp();
  
  while(pPropExe)
    {
    if(pPropExe->IsStreamed())
      {
      if((pPropExe->GetName()     == pPropLoad->GetName()) &&
         (pPropExe->GetEnumType() == pPropLoad->GetEnumType()))
        {                                                   // insert the prop as 'matched'
        return InsertProperty(pDescFile,pPropLoad,pPropExe);
        }
      }
    pPropExe = pDescExe->GetNextProp();
    }
  
  return false;
  }

//----------------------------------------------- InsertProperty -----------------------------------------------------//
// insert property in flat match
// in : file prop desc,file prop,exe prop
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::InsertProperty(CPropListLoad* pDescFile,CProperty* pPropLoad,CProperty* pPropExe)
  {
  CProperty*   pProp = NULL;
  if(pPropExe) pProp = pPropExe ->CreateCopy();
  else         pProp = pPropLoad->CreateCopy();
  if(!pProp) return false;

  pDescFile->GetFlatMatch().AddProp(pProp);
  return true;
  }

//----------------------------------------------- ReadDataFile -------------------------------------------------------//
// read data file
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadDataFile(CLevelFiles& Level)
  {
  CStdString  strTags[4] = { "<data ","</xml>","<engine_root ","<editor_root " };
  u32         u32Tag;
  const u32   u32Size    = 1024;
  char szLine[u32Size];

  CLogger* pLogger = Level.GetLogger();
  File&   DataFile = Level.GetDataFile();

  if(LookForTag(DataFile,szLine,u32Size,"<xml>"))
    {
    while((u32Tag = LookForTags(DataFile,szLine,u32Size,strTags,4)) != u32(-1))
      {
      if(u32Tag == 1) return true;                          // </xml>
      if(u32Tag == 0)
        {                                                   // <data 
        CStdString strClass = GetTagParam(szLine,"class");
        CStdString strID    = GetTagParam(szLine,"id");
        if(pLogger) _DLOG2_((*pLogger),LPCTSTR("class: "+strClass+"  id: "+strID),"data file");

        // look for class prop desc

        CPropListLoad* pPropDesc = Level.GetPropDesc(strClass);
        if(!pPropDesc)
          {
          if(pLogger) _DWAR_((*pLogger),LPCTSTR("class ["+strClass+"] not found"));
          return false;
          }

        // create object

        CPersistent* pObj = Level.CreateObject(strClass,strID);
        if(!pObj)
          {
          if(pLogger) _DWAR_((*pLogger),LPCTSTR("error creating object of class ["+strClass+"]"));
          return false;
          }

        if(!ReadValues(Level,pPropDesc,pObj))
          {
          if(pLogger) _DWAR_((*pLogger),LPCTSTR("error reading object of class ["+strClass+"]"));
          return false;
          }

        // </data>

        if(!LookForTag(DataFile,szLine,u32Size,"</data>")) return false;
        }

      else if(u32Tag == 2)
        {                                                   // <engine_root 
        CStdString strObj = GetTagParam(szLine,"obj");
        CStdString strID  = GetTagParam(szLine,"addr");
        if(pLogger) _DLOG2_((*pLogger),LPCTSTR("engine root: "+strObj+"  id: "+strID),"data file");

        Level.SetEngineRootID(strID);
        }

      else if(u32Tag == 3)
        {                                                   // <editor_root 
        CStdString strObj = GetTagParam(szLine,"obj");
        CStdString strID  = GetTagParam(szLine,"addr");
        if(pLogger) _DLOG2_((*pLogger),LPCTSTR("editor root: "+strObj+"  id: "+strID),"data file");

        Level.SetEditorRootID(strID);
        }
      }
    }

  return false;
  }

//----------------------------------------------- ReadDataFileBin ----------------------------------------------------//
// read data file
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadDataFileBin(CLevelFiles& Level)
  {
  CLogger* pLogger = Level.GetLogger();
  File&   DataFile = Level.GetDataFile();

  CPersistent* pEngineRootAddr = NULL;
  CPersistent* pEditorRootAddr = NULL;
  if(DataFile.Read(&pEngineRootAddr,4) != 4) return false;
  if(DataFile.Read(&pEditorRootAddr,4) != 4) return false;

  Level.SetEngineRootAddr(pEngineRootAddr);
  Level.SetEditorRootAddr(pEditorRootAddr);

  if(pLogger)
    {
    CStdString strEngineRoot,strEditorRoot;
    strEngineRoot.Format("0x%x",(u32)pEngineRootAddr);
    strEditorRoot.Format("0x%x",(u32)pEditorRootAddr);
    _DLOG2_((*pLogger),LPCTSTR("engine root: "+strEngineRoot),"data file");
    _DLOG2_((*pLogger),LPCTSTR("editor root: "+strEditorRoot),"data file");
    }

  // objects

  CStdString strClass,strID;

  while(!DataFile.Eof())
    {
    if(!ReadStringBin(DataFile,&strClass)) return true;     // EOF
    u32 u32ID = 0;
    if(DataFile.Read(&u32ID,4) != 4) return false;
    strID.Format("0x%x",u32ID);
    if(pLogger) _DLOG2_((*pLogger),LPCTSTR("class: "+strClass+"  id: "+strID),"data file");

    // look for class prop desc

    CPropListLoad* pPropDesc = Level.GetPropDesc(strClass);
    if(!pPropDesc)
      {
      if(pLogger) _DWAR_((*pLogger),LPCTSTR("class ["+strClass+"] not found"));
      return false;
      }

    // create object

    CPersistent* pObj = Level.CreateObject(strClass,(CPersistent*)u32ID);
    if(!pObj)
      {
      if(pLogger) _DWAR_((*pLogger),LPCTSTR("error creating object of class ["+strClass+"]"));
      return false;
      }

    if(!ReadValuesBin(Level,pPropDesc,pObj))
      {
      if(pLogger) _DWAR_((*pLogger),LPCTSTR("error reading object of class ["+strClass+"]"));
      return false;
      }
    }

  return true;
  }

//----------------------------------------------- ReadValue ----------------------------------------------------------//
// read one line and format value
// in : level,value ptr
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadValue(CLevelFiles& Level,CStdString* pstrVal)
  {
  if(!pstrVal) return false;

  const u32    u32Size = 1024;
  char  szLine[u32Size];
	char* pcStart;

  if(!Level.GetDataFile().GetString(szLine,u32Size)) return false;    // EOF

  u32 u32Len = strlen(szLine);
  if(szLine[u32Len-1] == 10) szLine[u32Len-1] = 0;          // remove carriage return

  pcStart = szLine;
  while(*pcStart == ' ') pcStart++;                         // remove spaces at the beginning

  *pstrVal = pcStart;
  return true;
  }

//----------------------------------------------- ReadValues ---------------------------------------------------------//
// read data values
// in : level,prop desc,object to fill
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadValues(CLevelFiles& Level,CPropListLoad* pPropDesc,CPersistent* pObj)
  {
  if(!pPropDesc || !pObj) return false;
  CStdString strVal;
	CProperty* pProp = pPropDesc->GetFlatMatch().GetFirstProp();
	
	// parse properties
	
  while(pProp)
    {
    if(!ReadValue(Level,&strVal)) return false;

    if(pProp->GetOffset() != u32(-1))
      {                                                     // -1 means 'no match'
      pProp->Read(pObj,strVal,Level);
      }
    else
      {
      pObj->ReadUnmatched(Level,pProp,strVal);
      }

    pProp = pPropDesc->GetFlatMatch().GetNextProp();
    }

  return true;
  }

//----------------------------------------------- ReadValuesBin ------------------------------------------------------//
// read data values
// in : level,prop desc,object to fill
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadValuesBin(CLevelFiles& Level,CPropListLoad* pPropDesc,CPersistent* pObj)
  {
  if(!pPropDesc || !pObj) return false;
	CProperty* pProp = pPropDesc->GetFlatMatch().GetFirstProp();

	// parse properties
	
  while(pProp)
    {
    if(!pProp->ReadBin(pObj,Level)) return false;

    if(pProp->GetOffset() == u32(-1))
      {                                                     // -1 means 'no match'
      pObj->ReadUnmatchedBin(Level,pProp);
      }

    pProp = pPropDesc->GetFlatMatch().GetNextProp();
    }

  return true;
  }

//----------------------------------------------- ReadUnmatched ------------------------------------------------------//
// overridable function for unmatched props (ex: old format compatibility)
// in : level,property,value
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadUnmatched(CLevelFiles& Level,CProperty* pProp,const CStdString& strVal)
  {
  CLogger* pLogger = Level.GetLogger();
  if(pLogger) _DWAR_((*pLogger),"unmatched property ["+pProp->GetName()+"]");
  return true;
  }

//----------------------------------------------- ReadUnmatchedBin ---------------------------------------------------//
// overridable function for unmatched props (ex: old format compatibility)
// in : level,property
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::ReadUnmatchedBin(CLevelFiles& Level,CProperty* pProp)
  {
  CLogger* pLogger = Level.GetLogger();
  if(pLogger) _DWAR_((*pLogger),"unmatched property ["+pProp->GetName()+"]");
  return true;
  }

//----------------------------------------------- LinkObjects --------------------------------------------------------//
// link loaded objects
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::LinkObjects(CLevelFiles& Level)
  {
  return Level.LinkObjects();
  }

//----------------------------------------------- PostRead -----------------------------------------------------------//
// overridable function for special actions when loading is completed
// in :
// out: OK ?
// rem: called by PostProcess below
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::PostRead()
  {
  return true;
  }

//----------------------------------------------- PostProcess --------------------------------------------------------//
// post-process loaded objects
// in : level
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CPersistent::PostProcess(CLevelFiles& Level)
  {
  return Level.PostProcess();
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
