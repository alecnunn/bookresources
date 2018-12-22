#pragma once


// CRemoteAddressDlg dialog

class CRemoteAddressDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoteAddressDlg)

public:
	CRemoteAddressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoteAddressDlg();

// Dialog Data
	enum { IDD = IDD_REMOTE_ADDRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    CString m_Edit_Address;
    CString m_Edit_Port;
};
