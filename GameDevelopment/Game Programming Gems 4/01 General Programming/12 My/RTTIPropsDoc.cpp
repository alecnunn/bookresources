// RTTIPropsDoc.cpp : implementation of the CRTTIPropsDoc class
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"

#include "CustomMsgs.h"
#include "ErrorAfxMsg.h"
#include "ListObj.h"
#include "MenuEnum.h"

#include "DirC.h"
#include "FileC.h"
#include "FindFileC.h"

#include "EditorCamera.h"
#include "EditorTool.h"

#include "EngineBox.h"
#include "EngineCamera.h"
#include "EngineNode.h"
#include "EngineSphere.h"

#include "DlgItemName.h"

#include "LevelFiles.h"
#include "Properties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//

CString CRTTIPropsDoc::m_strLevelPath;
CString CRTTIPropsDoc::m_strTexturePath;
CString CRTTIPropsDoc::m_strMeshPath;

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsDoc

IMPLEMENT_DYNCREATE(CRTTIPropsDoc, CDocument)

BEGIN_MESSAGE_MAP(CRTTIPropsDoc, CDocument)
	//{{AFX_MSG_MAP(CRTTIPropsDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsDoc construction/destruction

CRTTIPropsDoc::CRTTIPropsDoc()
{
  m_boReadOnlyProps = true;

  m_Logger.SetReportCallback(LogReportCallback,(DWORD)this);
  m_Logger.SetRootPath("d:\\mythos\\gpg4\\");

  m_pTool = NULL;

  m_pSelection      = NULL;
  m_boLockSelection = false;
}

CRTTIPropsDoc::~CRTTIPropsDoc()
{
  delete m_pSelection;
  delete m_pTool;
}

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsDoc serialization

void CRTTIPropsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsDoc diagnostics

#ifdef _DEBUG
void CRTTIPropsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRTTIPropsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//
//

void CRTTIPropsDoc::SetLevelPath(const CString& strPath)
  {
  m_strLevelPath = strPath;
  }

void CRTTIPropsDoc::SetTexturePath(const CString& strPath)
  {
  m_strTexturePath = strPath;
  }

void CRTTIPropsDoc::SetMeshPath(const CString& strPath)
  {
  m_strMeshPath = strPath;
  }

const CString& CRTTIPropsDoc::GetLevelPath()
  {
  return m_strLevelPath;
  }

const CString& CRTTIPropsDoc::GetTexturePath()
  {
  return m_strTexturePath;
  }

const CString& CRTTIPropsDoc::GetMeshPath()
  {
  return m_strMeshPath;
  }

//

GPG4::CEditorScene* CRTTIPropsDoc::GetEditorScene()
  {
  return &m_EditorScene;
  }

GPG4::CEngineNode* CRTTIPropsDoc::GetScene() const
  {
  return m_spScene;
  }

GPG4::CLogger* CRTTIPropsDoc::GetLogger()
  {
  return &m_Logger;
  }

//

bool CRTTIPropsDoc::IsSelectionLocked() const
  {
  return m_boLockSelection;
  }

void CRTTIPropsDoc::LockSelection(const bool boLock)
  {
  m_boLockSelection = boLock;
  }

//

bool CRTTIPropsDoc::GetReadOnlyProps() const
  {
  return m_boReadOnlyProps;
  }

void CRTTIPropsDoc::SetReadOnlyProps(const bool boReadOnly)
  {
  m_boReadOnlyProps = boReadOnly;
  }

//

GPG4::CEditorTool* CRTTIPropsDoc::GetTool() const
  {
  return m_pTool;
  }

void CRTTIPropsDoc::SetTool(GPG4::CEditorTool* pTool)
  {
  m_pTool = pTool;
  }

const CStdString& CRTTIPropsDoc::GetToolName()
  {
  static CStdString strEmpty("");
  if(!m_pTool) return strEmpty;
  return m_pTool->GetName();
  }

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsDoc commands

BOOL CRTTIPropsDoc::OnNewDocument()
  {
	if (!CDocument::OnNewDocument())
		return FALSE;

  m_pSelection = new GPG4::CListObj;
  if(!m_pSelection)
    {
    AfxMessageBox("ERROR creating empty selection",MB_OK);
    return FALSE;
    }

  m_spScene = new GPG4::CEngineNode;
  if(!m_spScene) return FALSE;
  m_spScene->SetName("scene root");

  GPG4::CEngineCameraSP spCam = new GPG4::CEngineCamera;
  if(!spCam) return FALSE;
  spCam->SetName("default camera");
  m_spScene->AddSubnode(spCam);

  if(!BuildEditorScene())
    {
    AfxMessageBox("ERROR adding engine objects",MB_OK);
    return FALSE;
    }

  // default camera is not renamable

  GPG4::CEditorSceneItem* pCamItem = m_EditorScene.GetItem("default camera");
  if(!pCamItem) return FALSE;
  GPG4::CEditorObj* pCamObj = pCamItem->GetObject();
  if(!pCamObj)  return FALSE;
  pCamObj->SetRenameFlag(false);

  UpdateAllViews(NULL,SCENE_DBLCLK,(CObject*)pCamItem);     // select camera in 3DView
	return TRUE;
  }

//
// save
//

BOOL CRTTIPropsDoc::OnSaveDocument(LPCTSTR lpszPathName)
  {
  GPG4::CErrorAfxMsg ErrorMsg1("error saving file "+CStdString(lpszPathName),&m_Logger);

  // look for previous corresponding directories (= backups)

  u32 u32NextNum = 0;
  CStdString strFind = lpszPathName+CStdString("???");
  GPG4::FindFile Finder;

  if(Finder.FindFirst(strFind.c_str(),false))
    {
    u32 u32New;
    u32 u32Len = strlen(lpszPathName);

    do
      {
      if(Finder.IsDirectory())
        {
        const char* pszFile;
        char  cDigit;
        strFind = Finder.GetFilePath();
        pszFile = strFind.c_str();

        cDigit  = pszFile[u32Len+0];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New  = u32(cDigit-'0')*100;

        cDigit  = pszFile[u32Len+1];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New += u32(cDigit-'0')*10;

        cDigit  = pszFile[u32Len+2];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New += u32(cDigit-'0')*1;

        if(u32New >= u32NextNum) u32NextNum = u32New+1;
        }
      }
    while(Finder.FindNext());

    Finder.FindClose();
    }

  if(u32NextNum > 999)
    {
    GPG4::CErrorAfxMsg ErrorMsg("doc op : error creating directory, max num = 999",&m_Logger);
    return FALSE;
    }

  // create new directory

  CStdString strPath;
  strPath.Format("%s%s%s%d",lpszPathName,u32NextNum<100?"0":"",u32NextNum<10?"0":"",u32NextNum);
  GPG4::Dir Dir;

  GPG4::CErrorAfxMsg ErrorMsg("doc op : error creating directory ["+strPath+"]",&m_Logger);
  if(!Dir.MakeDir(strPath.c_str())) return FALSE;

  _DLOG2_(m_Logger,(LPCTSTR)("create level directory ["+strPath+"]"),"doc op");

  // save logs

  SaveLogs(strPath+"\\logs.txt");

    // clear logs

  m_LogReportData.clear();
  UpdateAllViews(NULL,LOGREPORT_CLEAR);

  // save level in multiple files : a main file, a metadata description, and the level data
  // this is where we use the properties & RTTI system to get the job done

  CStdString strDir;
  ::GetFileTitle(strPath,strDir.GetBuffer(_MAX_PATH),_MAX_PATH);
  strDir.ReleaseBuffer();

  GPG4::CLevelFiles Level(GPG4::CEditorObj::GetRTTIMap(),CreateObject);
  Level.SetLogger(&m_Logger);
  Level.SetDir(strDir);

    // main file

//  ErrorMsg.m_strMsg = "item [level] not found";
//  GPG4::CEditorSceneItem* pLevel = m_EditorScene.GetItem("level");
  ErrorMsg.m_strMsg = "root item not found";
  GPG4::CEditorSceneItem* pLevel = m_EditorScene.GetRootItem();
  if(!pLevel) return FALSE;
  pLevel = pLevel->GetFirstChild();
  if(!pLevel) return FALSE;

  ErrorMsg.m_strMsg = "error creating file ["+CStdString(lpszPathName)+"]";
  if(!Level.GetMainFile().Open(lpszPathName,GPG4::IFile::WRITE_TEXT)) return FALSE;

  ErrorMsg.m_strMsg = "error writing file ["+CStdString(lpszPathName)+"]";
  bool boRes = true;
  boRes &= Level.WriteMainFile();
  boRes &= Level.GetMainFile().Close();

  if(!boRes) return FALSE;

    // class descriptions (PropDesc.xml) & level (LevelData.xml)

  ErrorMsg.m_strMsg = "error creating PropDesc.xml";
  if(!Level.GetDescFile().Open(strPath+"\\PropDesc.xml",GPG4::IFile::WRITE_TEXT)) return FALSE;

  ErrorMsg.m_strMsg = "error creating LevelData.xml";
  if(!Level.GetDataFile().Open(strPath+"\\LevelData.xml",GPG4::IFile::WRITE_TEXT)) return FALSE;

  boRes &= Level.GetDescFile().PutString("<xml>\n\n");
  boRes &= Level.GetDataFile().PutString("<xml>\n\n");

  boRes &= Level.GetDataFile().PutString("<engine_root obj='"+m_spScene->GetName()+"' addr='"+GPG4::CPropPtr::Ptr2String(&m_spScene)+"'/>\n\n");
  boRes &= Level.GetDataFile().PutString("<editor_root obj='"+   pLevel->GetName()+"' addr='"+GPG4::CPropPtr::Ptr2String(&pLevel)   +"'/>\n\n");

  pLevel->Write(Level);                                     // recursive

  boRes &= Level.GetDescFile().PutString("</xml>\n");
  boRes &= Level.GetDataFile().PutString("</xml>\n");
  boRes &= Level.GetDescFile().Close();
  boRes &= Level.GetDataFile().Close();

    // binary format: class descriptions (PropDesc.bin) & level (LevelData.bin)

  GPG4::CLevelFiles LevelBin(GPG4::CEditorObj::GetRTTIMap(),CreateObject);
  LevelBin.SetLogger(&m_Logger);
  LevelBin.SetDir(strDir);
  LevelBin.SetMode(GPG4::CLevelFiles::BINARY);

  ErrorMsg.m_strMsg = "error creating PropDesc.bin";
  if(!LevelBin.GetDescFile().Open(strPath+"\\PropDesc.bin",GPG4::IFile::WRITE_BINARY)) return FALSE;

  ErrorMsg.m_strMsg = "error creating LevelData.bin";
  if(!LevelBin.GetDataFile().Open(strPath+"\\LevelData.bin",GPG4::IFile::WRITE_BINARY)) return FALSE;

  boRes &= (LevelBin.GetDataFile().Write(&m_spScene,4) == 4);         // engine root
  boRes &= (LevelBin.GetDataFile().Write(&pLevel,   4) == 4);         // editor root

  pLevel->Write(LevelBin);                                            // recursive

  boRes &= LevelBin.GetDescFile().Close();
  boRes &= LevelBin.GetDataFile().Close();

  // OK

  ErrorMsg1.m_boOK = ErrorMsg.m_boOK = true;

  CStdString strFile = CStdString(lpszPathName)+" saved";
  AfxMessageBox(strFile.c_str(),MB_OK|MB_ICONINFORMATION);
  _DLOG2_(m_Logger,LPCTSTR(strFile+" to ["+strDir+"]"),"doc op");

  SetModifiedFlag(false);
  return TRUE;
  }

//
// load
//

BOOL CRTTIPropsDoc::OnOpenDocument(LPCTSTR lpszPathName) 
  {
  GPG4::CErrorAfxMsg ErrorMsg1("error loading level",&m_Logger);

  // main file

  GPG4::CErrorAfxMsg ErrorMsg("error loading file "+CStdString(lpszPathName),&m_Logger);
  GPG4::CLevelFiles  Level(GPG4::CEditorObj::GetRTTIMap(),CreateObject);
  Level.SetLogger(&m_Logger);
  if(!Level.GetMainFile().Open(lpszPathName,GPG4::IFile::READ_TEXT)) return FALSE;

  ErrorMsg.m_strMsg = "error reading file ["+CStdString(lpszPathName)+"]";
  if(!Level.ReadMainFile()) return FALSE;
  Level.GetMainFile().Close();

    // get subdirectory

  char szPath [_MAX_PATH];
  char szDrive[_MAX_DRIVE];
  char szDir  [_MAX_DIR];
  char szName [_MAX_FNAME];
  char szExt  [_MAX_EXT];
  _splitpath(lpszPathName,szDrive,szDir,szName,szExt);
  _makepath (szPath,      szDrive,szDir,"","");

  CStdString strDir(szPath+Level.GetDir());

#define _LOAD_XML_
#ifdef  _LOAD_XML_

  // PropDesc.xml (metadata description)

  ErrorMsg.m_strMsg = "error loading PropDesc.xml ";
  if(!Level.GetDescFile().Open(strDir+"\\PropDesc.xml",GPG4::IFile::READ_TEXT)) return FALSE;

  GPG4::CPersistent Persist;
  if(!Persist.ReadPropFile(Level))
    {
    SaveLogs(strDir+"\\logs load.txt");
    return FALSE;
    }
  Level.GetDescFile().Close();

  // LevelData.xml

  _DLOG2_(m_Logger,"LevelData.xml","doc op");
  ErrorMsg.m_strMsg = "error loading LevelData.xml";
  if(!Level.GetDataFile().Open(strDir+"\\LevelData.xml",GPG4::IFile::READ_TEXT)) return FALSE;

  if(!Persist.ReadDataFile(Level))
    {
    SaveLogs(strDir+"\\logs load.txt");
    return FALSE;
    }
  Level.GetDataFile().Close();

  GPG4::CPersistent* pEditorRoot = Level.GetCreatedObj(Level.GetEditorRoot());
  GPG4::CPersistent* pEngineRoot = Level.GetCreatedObj(Level.GetEngineRoot());

#else // end of xml format

  // PropDesc.bin (metadata description)

  Level.SetMode(GPG4::CLevelFiles::BINARY);

  ErrorMsg.m_strMsg = "error loading PropDesc.bin ";
  if(!Level.GetDescFile().Open(strDir+"\\PropDesc.bin",GPG4::IFile::READ_BINARY)) return FALSE;

  GPG4::CPersistent Persist;
  if(!Persist.ReadPropFileBin(Level))
    {
    SaveLogs(strDir+"\\logs load.txt");
    return FALSE;
    }
  Level.GetDescFile().Close();

  // LevelData.bin

  _DLOG2_(m_Logger,"LevelData.bin","doc op");
  ErrorMsg.m_strMsg = "error loading LevelData.bin";
  if(!Level.GetDataFile().Open(strDir+"\\LevelData.bin",GPG4::IFile::READ_BINARY)) return FALSE;

  if(!Persist.ReadDataFileBin(Level))
    {
    SaveLogs(strDir+"\\logs load.txt");
    return FALSE;
    }
  Level.GetDataFile().Close();

  GPG4::CPersistent* pEditorRoot = Level.GetCreatedObj(Level.GetEditorRootAddr());
  GPG4::CPersistent* pEngineRoot = Level.GetCreatedObj(Level.GetEngineRootAddr());

#endif // end of binary format

  // create root folder

  ErrorMsg.m_strMsg = "error creating item [root]";
  GPG4::CEditorSceneItem* pRootFolder = CreateEditorObject(GPG4::_MENU_FOLDER_,"root folder");
  if(!pRootFolder) return FALSE;
  m_EditorScene.SetRootItem(pRootFolder);

    // link editor root to m_EditorScene

  ErrorMsg.m_strMsg = "bad editor root in LevelData";
  GPG4::CEditorSceneItem* pItem = DYNAMIC_CAST(GPG4::CEditorSceneItem,pEditorRoot);
  if(!pItem) return FALSE;

  pRootFolder->AddChild(pItem);

    // link engine root to m_spScene

  ErrorMsg.m_strMsg = "bad engine root in LevelData";
  GPG4::CEngineNode* pNode = DYNAMIC_CAST(GPG4::CEngineNode,pEngineRoot);
  if(!pNode) return FALSE;
  
  m_spScene = pNode;

  // objects linking

  Level.LinkObjects();
  Level.PostProcess();

  // set m_pScene in SceneItems (= register items in scene) + CEngineObj::SetEditorObj

  pItem = m_EditorScene.GetRootItem();
  SetScene(pItem);

  // re-create selection

  ErrorMsg.m_strMsg = "error re-creating selection";
  if(!RecreateSelection())
    {
    SaveLogs(strDir+"\\logs load.txt");
    return FALSE;
    }

  // select default camera in 3D view

  pItem = m_EditorScene.GetItem("default camera");
  if(pItem)
    {
    UpdateAllViews(NULL,SCENE_DBLCLK,(CObject*)pItem);
    UpdateAllViews(NULL,SCENE_SELECT,(CObject*)pItem);
    }

  // OK

  ErrorMsg1.m_boOK = ErrorMsg.m_boOK = true;

  CStdString strFile = CStdString(lpszPathName)+" loaded";
  _DLOG2_(m_Logger,LPCTSTR(strFile+" from ["+Level.GetDir()+"]"),"doc op");

  SaveLogs(strDir+"\\logs load.txt");
  m_LogReportData.clear();

  _DLOG2_(m_Logger,LPCTSTR(strFile+" from ["+Level.GetDir()+"]"),"doc op");
	return TRUE;
  }

//
//

void CRTTIPropsDoc::SaveLogs(const CStdString& strLog)
  {
  GPG4::File LogFile;

  if(!LogFile.Open(strLog.c_str(),GPG4::IFile::WRITE_TEXT))
    {
    _DWAR_(m_Logger,"doc op : error creating log file");
    }
  else
    {
    CString strMsg;
    CLogReportData LogData;
    listLogData::reverse_iterator iter = m_LogReportData.rbegin();

    while(iter != m_LogReportData.rend())
      {
      LogData = *iter;
      strMsg  = LogData.m_strMsg.Left(80);
      u32 Len = strMsg.GetLength();
      while(Len++ < 80) strMsg += " ";

      CStdString strTxt;
      strTxt.Format("%4u  %s  %s  %12s - %80s  %5u  %s\n",
                    LogData.m_dwID,LogData.m_strTime,LogData.m_strDate,LogData.m_strLevel.Left(12),
                    strMsg,LogData.m_dwLine,LogData.m_strFile);
      LogFile.PutString(strTxt.c_str());
      ++iter;
      }

    LogFile.Close();
    }
  }

//

void CRTTIPropsDoc::OnFileSave() 
  {
  //CDocument::OnFileSave();
  //return;

  if(m_strPathName.IsEmpty()) OnFileSaveAs();
  else CDocument::DoSave(m_strPathName,true);
  }

// overriden to change destination directory

void CRTTIPropsDoc::OnFileSaveAs() 
  {
  //CDocument::OnFileSaveAs();
  //return;

  CFileDialog dlgSave(FALSE,"xml",NULL,OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT,"RTTIProps level (*.xml)|*.xml||");
  dlgSave.m_ofn.lpstrInitialDir = m_strLevelPath;
  if(dlgSave.DoModal() != IDOK) return;

  m_strPathName = dlgSave.GetPathName();
  OnFileSave();
  }

// overriden to change the document's title (The MFC Answer Book, 3.21)

void CRTTIPropsDoc::SetModifiedFlag(BOOL bModified)
  {
  CString strTitle = GetTitle();
  CString strIndicator = " *";

  if(!IsModified() && bModified)
    {
    SetTitle(strTitle+strIndicator);
    }
  else if(IsModified() && !bModified)
    {
    SetTitle(strTitle.Left(strTitle.GetLength()-strIndicator.GetLength()));
    }

  UpdateFrameCounts();
  CDocument::SetModifiedFlag(bModified);
  }

// overriden so that an unnamed document doesn't end in My Documents

BOOL CRTTIPropsDoc::SaveModified()
  {
  if(!IsModified()) return TRUE;

  CString name;
  if(m_strPathName.IsEmpty())
    {
    // get name based on caption
    CString strIndicator = " *";
    name = m_strTitle.Left(m_strTitle.GetLength()-strIndicator.GetLength());
    if(name.IsEmpty()) VERIFY(name.LoadString(AFX_IDS_UNTITLED));
    }
  else
    {
    // get name based on file title of path name
    name = m_strPathName;
    ::GetFileTitle(m_strPathName, name.GetBuffer(_MAX_PATH), _MAX_PATH);
    name.ReleaseBuffer();
    }

  CString prompt;
  AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
  switch(AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
    {
    case IDCANCEL:
      return FALSE;                                         // don't continue

    case IDYES:
      // If so, either Save or Update, as appropriate

      // modifs
      if(m_strPathName.IsEmpty())
        {
        m_strPathName = m_strLevelPath+"\\"+name+".xml";
        }
      // modifs end

      if (!DoFileSave())
        return FALSE;                                       // don't continue
      break;

    case IDNO:
      // If not saving changes, revert the document
      break;

    default:
      ASSERT(FALSE);
      break;
    }
  return TRUE;                                              // keep going
  }

// add m_spScene objects to m_EditorScene

bool CRTTIPropsDoc::BuildEditorScene()
  {
  // root item

  GPG4::CEditorSceneItem* pRoot = m_EditorScene.GetRootItem();
  if(!pRoot)
    {
    pRoot = CreateEditorObject(GPG4::_MENU_FOLDER_,"root folder");
    if(!pRoot) return false;
    pRoot->SetExpanded(true);
    m_EditorScene.SetRootItem(pRoot);
    }

  // "level" folder
/*
  GPG4::CEditorSceneItem* pLevel = m_EditorScene.GetItem("level");
  if(!pLevel)
    {
    pLevel = CreateEditorObject(GPG4::_MENU_FOLDER_,"level");
    if(!pLevel) return false;
    
    pRoot ->AddChild(pLevel);
    pLevel->GetObject()->SetDropFlag  (true);
    pLevel->GetObject()->SetRenameFlag(false);
    pLevel->SetExpanded(true);
    }
*/
  //

  GPG4::CEngineNode* pNode = m_spScene;
//  BuildSubScene(pLevel,pNode);
  BuildSubScene(pRoot,pNode);
  return true;
  }

// recursive

void CRTTIPropsDoc::BuildSubScene(GPG4::CEditorSceneItem* pItem,GPG4::CEngineNode* pNode)
  {
  if(!pItem || !pNode) return;

  u32 u32ObjType =  GPG4::_MENU_NONE_;
  if(IS_EXACT_CLASS(GPG4::CEngineBox,     pNode)) u32ObjType = GPG4::_MENU_BOX_;
  if(IS_EXACT_CLASS(GPG4::CEngineCamera,  pNode)) u32ObjType = GPG4::_MENU_CAMERA_;
  if(IS_EXACT_CLASS(GPG4::CEngineNode,    pNode)) u32ObjType = GPG4::_MENU_NODE_;
  if(IS_EXACT_CLASS(GPG4::CEngineSphere,  pNode)) u32ObjType = GPG4::_MENU_SPHERE_;

  GPG4::CEditorSceneItem* pSubItem = CreateObject(u32ObjType,pNode->GetName(),false);
  if(!pSubItem) pSubItem = pItem;
  else
    {
    pItem->AddChild(pSubItem);
    pItem->SetExpanded(true);
    pSubItem->GetObject()->SetEngineObj(pNode);
    pNode->SetEditorObj(pSubItem->GetObject());
    }

  GPG4::CEngineNode* pChild = pNode->GetFirstSubnode();
  while(pChild)
    {
    BuildSubScene(pSubItem,pChild);
    pChild = pNode->GetNextSubnode();
    }
  }

// the general factory

GPG4::CPersistent* CRTTIPropsDoc::CreateObject(const CStdString& strClass)
  {
  if(strClass == "CEditorSceneItem") return new GPG4::CEditorSceneItem;

  GPG4::CPersistent* pTmp = GPG4::CEditorObj::CreateObject(strClass);
  if(pTmp) return pTmp;

  return GPG4::CEngineObj::CreateObject(strClass);
  }

//

GPG4::CEditorSceneItem* CRTTIPropsDoc::CreateObject(const u32 u32ObjType,const CStdString& strName,const bool boEditName)
  {
  GPG4::CEditorObj* pObj = GPG4::CEditorObj::CreateObject(u32ObjType);
  if(!pObj) return NULL;

  // name

  if(!strName.empty())
    {
    pObj->SetName(strName);
    pObj->SetName(BuildUnregisteredName(pObj->GetName(),false));
    }
  else
    {
    CStdString strNew = pObj->GetDefaultName();
    pObj->SetName(strNew);
    pObj->SetName(BuildUnregisteredName(pObj->GetName(),true));
    }

  if(boEditName)
    {
    CDlgItemName dlgName;
    dlgName.m_pEditorScene = &m_EditorScene;
    dlgName.m_strName      = pObj->GetName().c_str();

    if(dlgName.DoModal() != IDOK)
      {
      delete pObj;
      return NULL;
      }

    pObj->SetName((LPCTSTR)dlgName.m_strName);
    }

  // scene item

  GPG4::CEditorSceneItem* pItem = new GPG4::CEditorSceneItem(&m_EditorScene,pObj);
  if(!pItem)
    {
    delete pObj;
    return NULL;
    }
  return pItem;
  }

//

const CStdString CRTTIPropsDoc::BuildUnregisteredName(const CStdString& strName,const bool boDefaultName)
  {
  if(!boDefaultName) if(!m_EditorScene.GetItem(strName)) return strName;

  CStdString strFinalName;
  char  szNum[8];
  char* pcSpace;
  DWORD dwNum = 1;

  while(true)
    {
    sprintf(szNum,"%4u",dwNum);
    pcSpace = szNum;
    while(*pcSpace == ' ') *pcSpace++ = '0';
    strFinalName = strName+szNum;

    if(!m_EditorScene.GetItem(strFinalName)) return strFinalName;
    if(dwNum >= 9999) return "";
    dwNum++;
    }
  }

//

GPG4::CEditorSceneItem* CRTTIPropsDoc::CreateEditorObject(const u32 u32ObjType,const CStdString& strName)
  {
  GPG4::CEditorObj* pObj = GPG4::CEditorObj::CreateObject(u32ObjType);
  if(!pObj) return NULL;
  pObj->SetName(strName);
  FlagEditorObject(pObj);

  GPG4::CEditorSceneItem* pItem = new GPG4::CEditorSceneItem(&m_EditorScene,pObj);
  if(!pItem)
    {
    delete pObj;
    return NULL;
    }
  pItem->SetName(strName);
  return pItem;
  }

//

void CRTTIPropsDoc::FlagEditorObject(GPG4::CEditorObj* pObj)
  {
  if(!pObj) return;
  pObj->SetDeleteFlag(false);
  pObj->SetDragFlag  (false);
  pObj->SetDropFlag  (false);
  pObj->SetRenameFlag(false);
  }

//

void CRTTIPropsDoc::SetScene(GPG4::CEditorSceneItem* pItem)
  {
  if(!pItem) return;
  pItem->SetScene(&m_EditorScene);

  GPG4::CEditorSceneItem* pChild = pItem->GetFirstChild();
  while(pChild)
    {
    SetScene(pChild);
    pChild = pItem->GetNextChild();
    }

  GPG4::CEditorObj* pObj = pItem->GetObject();
  if(pObj)
    {
    GPG4::CEngineObj* pEng = pObj->GetEngineObj();
    if(pEng) pEng->SetEditorObj(pObj);
    }
  }

//
// log
//

void CRTTIPropsDoc::AddLogMsg(CView* pSender,const char* pszMsg,const DWORD dwID/*=0*/,const char* pszLevel/*=NULL*/,
                                             const char* pszFile/*=NULL*/,const DWORD dwLine/*=0*/)
  {
  // LogReportView

  CLogReportData LogData;
  LogData.m_dwID   = dwID;
  LogData.m_dwLine = dwLine;
  LogData.m_strMsg = pszMsg;
  if(pszLevel) LogData.m_strLevel = pszLevel;
  if(pszFile)  LogData.m_strFile  = pszFile;

  time_t  t = time(NULL);
  struct  tm *local = localtime(&t);
  LogData.m_strTime.Format("%02d:%02d:%02d",local->tm_hour,local->tm_min,  local->tm_sec);
  LogData.m_strDate.Format("%02d/%02d/%02d",local->tm_mday,local->tm_mon+1,local->tm_year%100);

  m_LogReportData.push_back(LogData);
  UpdateAllViews(pSender,LOGREPORT_ADDTXT,(CObject*)&LogData);
  }

//

/*static*/void CRTTIPropsDoc::LogReportCallback(const char* pszMsg, const DWORD dwID,  const char* pszLevel,
                                              const char* pszFile,const DWORD dwLine,const DWORD dwExtraData)
  {
  CRTTIPropsDoc* pDoc = (CRTTIPropsDoc*)dwExtraData;
  if(!pDoc) return;
  pDoc->AddLogMsg(NULL,pszMsg,dwID,pszLevel,pszFile,dwLine);
  }

//

bool CRTTIPropsDoc::GetFirstLogData(CLogReportData* pData)
  {
  m_LogCurrData = m_LogReportData.begin();
  return GetNextLogData(pData);
  }

bool CRTTIPropsDoc::GetNextLogData(CLogReportData* pData)
  {
  if(m_LogCurrData == m_LogReportData.end()) return false;
  if(!pData) return false;
  *pData = (*m_LogCurrData);
  ++m_LogCurrData;
  return true;
  }

//
// selection

GPG4::CListObj* CRTTIPropsDoc::GetSelection() const
  {
  return m_pSelection;
  }

//

bool CRTTIPropsDoc::ModifySelection(GPG4::CEditorObj* pObj)
  {
  if(!pObj) return false;
  GPG4::CEngineObj* pObjSel = pObj->GetEngineObj();
  bool           boSelected = pObj->IsSelected();
  if(!pObjSel) return false;

  if(!m_pSelection) return false;
  if(boSelected) m_pSelection->AddObj   (pObjSel);
  else           m_pSelection->RemoveObj(pObjSel);

  SetModifiedFlag();
  UpdateAllViews(NULL,PROP_UPDATE,(CObject*)pObj);

  CString strTmp("selection : object [");
  strTmp += pObjSel->GetName().c_str();
  strTmp += "] ";
  strTmp += boSelected ? "added" : "removed";
  _DLOG2_(m_Logger,(LPCTSTR)strTmp,"doc op");

  return true;
  }

//

void CRTTIPropsDoc::ClearSelection()
  {
  if(!m_pSelection) return;
  GPG4::CEngineObj* pEngineObj = m_pSelection->GetFirstObj();
  while(pEngineObj)
    {
    GPG4::CEditorObj* pEditObj = pEngineObj->GetEditorObj();
    if(pEditObj) pEditObj->Select(false);
    pEngineObj = m_pSelection->GetNextObj();
    }

  m_pSelection->RemoveAll();

  _DLOG2_(m_Logger,"selection cleared","doc op");
  UpdateAllViews(NULL,DOC_SELECTION_CLEARED);
  UpdateAllViews(NULL,PROP_UPDATE,NULL);
  }

//

bool CRTTIPropsDoc::RecreateSelection()
  {
  m_pSelection = new GPG4::CListObj;
  if(!m_pSelection) return false;

  GPG4::CEditorSceneItem* pItem = m_EditorScene.GetRootItem();
  CheckSelectedItems(pItem);                                // recursive

  // to avoid putting the same obj several times in the selection list, its Selected flag has been set to false ;
  // put it back to true for every object in the selection

  GPG4::CEngineObj* pEngineObj = m_pSelection->GetFirstObj();
  while(pEngineObj)
    {
    GPG4::CEditorObj* pEditObj = pEngineObj->GetEditorObj();
    if(pEditObj) pEditObj->Select(true);
    pEngineObj = m_pSelection->GetNextObj();
    }
  return true;
  }

// recursive

void CRTTIPropsDoc::CheckSelectedItems(GPG4::CEditorSceneItem* pItem)
  {
  if(!pItem) return;

  GPG4::CEditorSceneItem* pChild = pItem->GetFirstChild();
  while(pChild)
    {
    CheckSelectedItems(pChild);
    pChild = pItem->GetNextChild();
    }

  GPG4::CEditorObj* pObj = pItem->GetObject();
  if(pObj && pObj->IsSelected())
    {
    GPG4::CEngineObj* pEng = pObj->GetEngineObj();
    if(pEng)
      {
      m_pSelection->AddObj(pEng);
      pObj->Select(false);                                  // avoid adding the same object twice
      }
    }
  }

//
// cameras

bool CRTTIPropsDoc::TogglePerspective(GPG4::CEditorCamera* pCam,CView* pView)
  {
  if(!pCam) return false;
  GPG4::CEngineCamera* pCamera = DYNAMIC_CAST(GPG4::CEngineCamera,pCam->GetEngineObj());
  if(!pCamera || !pCamera->CanBePerspective()) return false;
  pCamera->SetPerspective(!pCamera->IsPerspective());

  CString strTmp("camera [");
  strTmp += pCam->GetName().c_str();
  strTmp += "] perspective turned ";
  strTmp += (pCamera->IsPerspective() ? "ON" : "OFF");
  _DLOG2_(m_Logger,(LPCTSTR)strTmp,"scene op");

  SetModifiedFlag();
  UpdateAllViews(pView,PROP_UPDATE,  (CObject*)pCam);
  //UpdateAllViews(pView,CAMERA_UPDATE,(CObject*)pCam);
  return true;
  }

//

bool CRTTIPropsDoc::UpdateCameraPerspective(CCmdUI* pCmdUI,GPG4::CEditorCamera* pCam)
  {
  pCmdUI->Enable  (false);
  pCmdUI->SetCheck(false);

  if(!pCam)    return false;
  GPG4::CEngineCamera* pCamera = DYNAMIC_CAST(GPG4::CEngineCamera,pCam->GetEngineObj());
  if(!pCamera) return false;

  pCmdUI->Enable  (pCamera->CanBePerspective());
  pCmdUI->SetCheck(pCamera->IsPerspective());
  return true;
  }
