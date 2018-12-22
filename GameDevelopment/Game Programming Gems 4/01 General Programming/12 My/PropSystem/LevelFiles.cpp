//--------------------------------------------------------------------------------------------------------------------//
//                                                CLASS GROUPING THE FILES MAKING A LEVEL                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "LevelFiles.h"
#ifdef    _DEBUG
#include  "LevelFiles.inl"
#endif

#include  "Logger.h"
#include  "Persistent.h"
#include  "Property.h"
#include  "Properties.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING : WRITE                                                   //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- WriteMainFile ------------------------------------------------------//
// write main file
// in :
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CLevelFiles::WriteMainFile()
  {
  char  Version[] = "01.00\0";
  bool  boRes     = true;
  File& File      = GetMainFile();

  boRes &= File.PutString("<xml>\n\n");

  boRes &= File.PutString("<level version='");
  boRes &= File.PutString(Version);
  boRes &= File.PutString("' folder='");
  boRes &= File.PutString(GetDir().c_str());
  boRes &= File.PutString("'>\n");
  boRes &= File.PutString("</level>\n\n");

  boRes &= File.PutString("</xml>\n");
  return boRes;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING : READ                                                    //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- ReadMainFile -------------------------------------------------------//
// read main file
// in :
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CLevelFiles::ReadMainFile()
  {
  CStdString  strTags[5] = { "</xml>","<level ","<camera ","<scene ","<context " };
  u32         u32Tag;
  const u32   u32Size = 1024;
  char szLine[u32Size];

  if(CPersistent::LookForTag(m_MainFile,szLine,u32Size,"<xml>"))
    {
    while((u32Tag = CPersistent::LookForTags(m_MainFile,szLine,u32Size,strTags,5)) != u32(-1))
      {
      if(u32Tag == 0) return true;                          // </xml>
      if(u32Tag == 1)
        {                                                   // <level
        m_strVersion = CPersistent::GetTagParam(szLine,"version");
        m_strDir     = CPersistent::GetTagParam(szLine,"folder");
        if(!CPersistent::LookForTag(m_MainFile,szLine,u32Size,"</level>")) return false;
        }

      if(u32Tag == 2)
        {                                                   // <camera
        m_strCurrentCam = CPersistent::GetTagParam(szLine,"current");
        if(!CPersistent::LookForTag(m_MainFile,szLine,u32Size,"</camera>")) return false;
        }

      if(u32Tag == 3)
        {                                                   // <scene
        m_strSceneSel = CPersistent::GetTagParam(szLine,"selected");
        if(!CPersistent::LookForTag(m_MainFile,szLine,u32Size,"</scene>")) return false;
        }

      if(u32Tag == 4)
        {                                                   // <context
        m_strContextObj = CPersistent::GetTagParam(szLine,"obj");
        if(!CPersistent::LookForTag(m_MainFile,szLine,u32Size,"</context>")) return false;
        }
      }
    }

  return false;
  }

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create object using creation function
// in : object's class
// out: object created or NULL
//--------------------------------------------------------------------------------------------------------------------//

CPersistent* CLevelFiles::CreateObject(const CStdString& strClass,const CStdString& strID)
  {
  if(!m_pfnCreate) return NULL;
  CPersistent* pObj = (*m_pfnCreate)(strClass);
  if(pObj) AddCreatedObj(strID,pObj);                       // add to map
  return pObj;
  }

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create object using creation function
// in : object's class
// out: object created or NULL
//--------------------------------------------------------------------------------------------------------------------//

CPersistent* CLevelFiles::CreateObject(const CStdString& strClass,CPersistent* pID)
  {
  if(!m_pfnCreate) return NULL;
  CPersistent* pObj = (*m_pfnCreate)(strClass);
  if(pObj) AddCreatedObj(pID,pObj);                         // add to map
  return pObj;
  }

//----------------------------------------------- LinkObjects --------------------------------------------------------//
// link loaded objects
// in :
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CLevelFiles::LinkObjects()
  {
  iterLinks iter = m_listLinks.begin();

  while(iter != m_listLinks.end())
    {
    CLinkLoad* pLink = *iter;

    if(pLink)
      {
      // binary format

      if(BinaryMode())
        {
        iterAddr2Obj iterID = m_mapCreatedBin.find(const_cast<CPersistent*>(pLink->GetParam()));
        if(iterID == m_mapCreatedBin.end())
          {
          if(m_pLogger)
            {
            CStdString strID;
            strID.Format("0x%x",pLink->GetParam());
            _DWAR_((*m_pLogger),"linking: object ["+strID+"] not found");
            }
          }
        else
          {
          CPersistent* pPointed = (*iterID).second;
          pLink->GetProp()->Link(pLink->GetObj(),"",pPointed);
          }
        }

      // text format

      else
        {
        iterStr2Obj iterID = m_mapCreated.find(pLink->GetID());
        if(iterID == m_mapCreated.end())
          {
          if(m_pLogger) _DWAR_((*m_pLogger),"linking: object ["+pLink->GetID()+"] not found");
          }
        else
          {
          CPersistent* pPointed = (*iterID).second;
          CStdString strPointed;
          strPointed.Format("0x%x",u32(pPointed));
          pLink->GetProp()->Link(pLink->GetObj(),strPointed,pPointed);
          }
        }
      }

    ++iter;
    }

  return true;
  }

//----------------------------------------------- PostProcess --------------------------------------------------------//
// post-process loaded objects
// in :
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CLevelFiles::PostProcess()
  {
  if(BinaryMode())
    {
    iterAddr2Obj iter = m_mapCreatedBin.begin();
    while(iter != m_mapCreatedBin.end())
      {
      CPersistent* pObj = (*iter).second;
      if(pObj)
        {
        if(!pObj->PostRead())
          {
          if(m_pLogger) _DWAR_((*m_pLogger),"post-processing of object ["+pObj->GetName()+"] failed");
          }
        }
      else
        {
        if(m_pLogger) _DWAR_((*m_pLogger),"post-processing: NULL object ?!");
        }
      ++iter;
      }
    }

  else
    {
    iterStr2Obj iter = m_mapCreated.begin();
    while(iter != m_mapCreated.end())
      {
      CPersistent* pObj = (*iter).second;
      if(pObj)
        {
        if(!pObj->PostRead())
          {
          if(m_pLogger) _DWAR_((*m_pLogger),"post-processing of object ["+pObj->GetName()+"] failed");
          }
        }
      else
        {
        if(m_pLogger) _DWAR_((*m_pLogger),"post-processing: NULL object ?!");
        }
      ++iter;
      }
    }

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
