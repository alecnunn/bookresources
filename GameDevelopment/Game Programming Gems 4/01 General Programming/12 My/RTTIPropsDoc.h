// RTTIPropsDoc.h : interface of the CRTTIPropsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTTIPROPSDOC_H__636565A2_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
#define AFX_RTTIPROPSDOC_H__636565A2_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include  "EditorCamera.h"
#include  "EditorScene.h"
#include  "EngineNode.h"
#include  "Logger.h"
#include  "StlIncludes.h"

namespace GPG4
{
  class CEditorTool;
  class CListObj;
}

//

class CLogReportData
  {
  public:

    DWORD           m_dwID;
    CString         m_strLevel;
    CString         m_strMsg;
    CString         m_strTime;
    CString         m_strDate;
    CString         m_strFile;
    DWORD           m_dwLine;
  };

//

class CRTTIPropsDoc : public CDocument
{
  // typedefs

  public:

    typedef std::list< CLogReportData >           listLogData;
    typedef listLogData::iterator                 iterLogData;

  //

protected: // create from serialization only
	CRTTIPropsDoc();
	DECLARE_DYNCREATE(CRTTIPropsDoc)

// Attributes
public:

protected:

  static CString              m_strLevelPath;
  static CString              m_strTexturePath;
  static CString              m_strMeshPath;

  // scene
  GPG4::CEditorScene          m_EditorScene;
  GPG4::CEngineNodeSP         m_spScene;

  GPG4::CListObj*             m_pSelection;
  bool                        m_boLockSelection;

  // log
  CString                     m_strLog;
  listLogData                 m_LogReportData;
  iterLogData                 m_LogCurrData;
  GPG4::CLogger               m_Logger;

  // commands
  bool                        m_boReadOnlyProps;

  // tools
  GPG4::CEditorTool*          m_pTool;

// Operations
public:

  static void                 SetLevelPath        (const CString& strPath);
  static void                 SetTexturePath      (const CString& strPath);
  static void                 SetMeshPath         (const CString& strPath);
  static const CString&       GetLevelPath        (void);
  static const CString&       GetTexturePath      (void);
  static const CString&       GetMeshPath         (void);

  GPG4::CEditorScene*         GetEditorScene      (void);
  GPG4::CEngineNode*          GetScene            (void) const;
  GPG4::CLogger*              GetLogger           (void);

  bool                        IsSelectionLocked   (void) const;
  void                        LockSelection       (const bool boLock);

  bool                        GetReadOnlyProps    (void) const;
  void                        SetReadOnlyProps    (const bool boReadOnly);

  GPG4::CEditorTool*          GetTool             (void) const;
  void                        SetTool             (GPG4::CEditorTool* pTool);
  const CStdString&           GetToolName         (void);

  void                        AddLogMsg           (CView* pSender,const char* pszMsg,      const DWORD dwID=0,const char* pszLevel=NULL,
                                                                  const char* pszFile=NULL,const DWORD dwLine=0);
  static void                 LogReportCallback   (const char* pszMsg, const DWORD dwID,   const char* pszLevel,
                                                   const char* pszFile,const DWORD dwLine, const DWORD dwExtraData);
  bool                        GetFirstLogData     (CLogReportData* pData);
  bool                        GetNextLogData      (CLogReportData* pData);

  GPG4::CEditorSceneItem*     CreateObject        (const u32 u32ObjType,const CStdString& strName,const bool boEditName);

  GPG4::CListObj*             GetSelection        (void) const;
  bool                        ModifySelection     (GPG4::CEditorObj* pObj);
  void                        ClearSelection      (void);

  bool                        TogglePerspective   (GPG4::CEditorCamera* pCam,CView* pView=NULL);
  bool                        UpdateCameraPerspective(CCmdUI* pCmdUI,GPG4::CEditorCamera* pCam);

protected:

  bool                        BuildEditorScene    (void);
  void                        BuildSubScene       (GPG4::CEditorSceneItem* pItem,GPG4::CEngineNode* pNode);
  static GPG4::CPersistent*   CreateObject        (const CStdString& strClass);
  const CStdString            BuildUnregisteredName(const CStdString& strName,const bool boDefaultName);
  GPG4::CEditorSceneItem*     CreateEditorObject  (const u32 u32ObjType,const CStdString& strName);
  void                        FlagEditorObject    (GPG4::CEditorObj* pObj);
  void                        SetScene            (GPG4::CEditorSceneItem* pItem);

  void                        SaveLogs            (const CStdString& strLog);
  bool                        RecreateSelection   (void);
  void                        CheckSelectedItems  (GPG4::CEditorSceneItem* pItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTTIPropsDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

  virtual void SetModifiedFlag (BOOL bModified=TRUE);
  virtual BOOL SaveModified    ();

// Implementation
public:
	virtual ~CRTTIPropsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRTTIPropsDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTTIPROPSDOC_H__636565A2_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
