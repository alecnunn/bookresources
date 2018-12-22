//--------------------------------------------------------------------------------------------------------------------//
//                                                CLASS GROUPING THE FILES MAKING A LEVEL                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LEVELFILES_INL_
#define   _GPG4_LEVELFILES_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CLevelFiles::CLevelFiles(CRTTIMap* pmapRTTI,pfnCreate CreateFct) : m_pLogger(NULL), m_pmapRTTI(pmapRTTI), 
                                                                          m_pfnCreate(CreateFct), m_Mode(TEXT)
  {
  m_pEngineRootAddr = NULL;
  m_pEditorRootAddr = NULL;
  }

INLINE CLevelFiles::~CLevelFiles()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                 GET                                                                //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString& CLevelFiles::GetDir()
  {
  return m_strDir;
  }

INLINE File& CLevelFiles::GetMainFile()
  {
  return m_MainFile;
  }

INLINE File& CLevelFiles::GetDescFile()
  {
  return m_DescFile;
  }

INLINE File& CLevelFiles::GetDataFile()
  {
  return m_DataFile;
  }

INLINE CLogger* CLevelFiles::GetLogger() const
  {
  return m_pLogger;
  }

INLINE CRTTIMap* CLevelFiles::GetRTTIMap() const
  {
  return m_pmapRTTI;
  }

INLINE CStdString& CLevelFiles::GetEngineRoot()
  {
  return m_strEngineRootID;
  }

INLINE CStdString& CLevelFiles::GetEditorRoot()
  {
  return m_strEditorRootID;
  }

INLINE CPersistent* CLevelFiles::GetEngineRootAddr() const
  {
  return m_pEngineRootAddr;
  }

INLINE CPersistent* CLevelFiles::GetEditorRootAddr() const
  {
  return m_pEditorRootAddr;
  }

INLINE bool CLevelFiles::BinaryMode() const
  {
  return(m_Mode == BINARY);
  }

INLINE bool CLevelFiles::TextMode() const
  {
  return(m_Mode == TEXT);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                 SET                                                                //
//--------------------------------------------------------------------------------------------------------------------//

INLINE void CLevelFiles::SetDir(const CStdString& strDir)
  {
  m_strDir = strDir;
  }

INLINE void CLevelFiles::SetLogger(CLogger* pLogger)
  {
  m_pLogger = pLogger;
  }

INLINE void CLevelFiles::SetEngineRootID(const CStdString& strRoot)
  {
  m_strEngineRootID = strRoot;
  }

INLINE void CLevelFiles::SetEditorRootID(const CStdString& strRoot)
  {
  m_strEditorRootID = strRoot;
  }

INLINE void CLevelFiles::SetEngineRootAddr(CPersistent* pRoot)
  {
  m_pEngineRootAddr = pRoot;
  }

INLINE void CLevelFiles::SetEditorRootAddr(CPersistent* pRoot)
  {
  m_pEditorRootAddr = pRoot;
  }

INLINE void CLevelFiles::SetMode(const MODE Mode)
  {
  m_Mode = Mode;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                 CONTAINERS                                                         //
//--------------------------------------------------------------------------------------------------------------------//

INLINE void CLevelFiles::AddSavedDesc(const CStdString& strClass)
  {
  m_SavedDesc.AddToSet(strClass);
  }

INLINE bool CLevelFiles::DescIsSaved(const CStdString& strClass)
  {
  return m_SavedDesc.IsInSet(strClass);
  }

//

INLINE void CLevelFiles::AddSavedObj(CPersistent* pObj)
  {
  m_SavedObj.AddToSet(pObj);
  }

INLINE bool CLevelFiles::ObjIsSaved(CPersistent* pObj)
  {
  return m_SavedObj.IsInSet(pObj);
  }

//

INLINE void CLevelFiles::AddObjToSave(CPersistent* pObj)
  {
  m_ObjToSave.push(pObj);
  }

INLINE u32 CLevelFiles::ObjToSaveCount()
  {
  return m_ObjToSave.size();
  }

INLINE CPersistent* CLevelFiles::PopObjToSave()
  {
  if(m_ObjToSave.empty()) return NULL;

  CPersistent* pObj = m_ObjToSave.front();
  m_ObjToSave.pop();
  return pObj;
  }

//

INLINE void CLevelFiles::AddLink(CLinkLoad* pLink)
  {
  m_listLinks.push_back(pLink);
  }

//

INLINE void CLevelFiles::AddPropDesc(const CStdString& strClass,CPropListLoad* pListLoad)
  {
  m_mapPropDesc.AddPropDesc(strClass,pListLoad);
  }

INLINE CPropListLoad* CLevelFiles::GetPropDesc(const CStdString& strClass)
  {
  return m_mapPropDesc.GetPropDesc(strClass);
  }

//

INLINE void CLevelFiles::AddCreatedObj(const CStdString& strID,CPersistent* pObj)
  {
  if(!pObj) return;
  m_mapCreated[strID] = pObj;
  }

INLINE CPersistent* CLevelFiles::GetCreatedObj(const CStdString& strID)
  {
  iterStr2Obj iter = m_mapCreated.find(strID);
  if(iter == m_mapCreated.end()) return NULL;
  return(*iter).second;
  }

//

INLINE void CLevelFiles::AddCreatedObj(CPersistent* pID,CPersistent* pObj)
  {
  if(!pID || !pObj) return;
  m_mapCreatedBin[pID] = pObj;
  }

INLINE CPersistent* CLevelFiles::GetCreatedObj(CPersistent* pID)
  {
  iterAddr2Obj iter = m_mapCreatedBin.find(pID);
  if(iter == m_mapCreatedBin.end()) return NULL;
  return(*iter).second;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_LEVELFILES_INL_
