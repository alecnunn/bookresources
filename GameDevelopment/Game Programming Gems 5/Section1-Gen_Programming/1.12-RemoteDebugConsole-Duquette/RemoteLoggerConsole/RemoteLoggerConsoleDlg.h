#pragma once
#include <vector>                       // As a
#include <map>                          // As a

class CTabPage;

// CRemoteLoggerConsoleDlg dialog
class CRemoteLoggerConsoleDlg : public CDialog
{
// Construction
public:
	CRemoteLoggerConsoleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_REMOTELOGGERCONSOLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    SOCKET  m_Socket;
    bool    m_bConnected;

    std::vector<CTabPage*>  m_vTabPage;

    // Page name to vector index
    typedef std::map<std::string, int> MapStringToInt;
    MapStringToInt m_mapPageNameIndex;

    CTabCtrl m_TabCtrl;

    void AddTabPage( const RemoteLoggingGlobal::CNewPageData* const in_pNewPageData );
    void ProcessData( const RemoteLoggingGlobal::CBasePageData* const in_pData );

    void NetworkCleanUp();

    // Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg LRESULT OnWSAEvent( WPARAM in_wParam, LPARAM in_lParam );
    afx_msg void OnSelchangeTab( NMHDR* io_pNMHDR, LRESULT* io_pResult );
    afx_msg void OnSelchangingTab( NMHDR* io_pNMHDR, LRESULT* io_pResult );
    DECLARE_MESSAGE_MAP()
    virtual void OnCancel();

public:
    afx_msg void OnRemotelogConnect();
    afx_msg void OnRemotelogDisconnect();
};
