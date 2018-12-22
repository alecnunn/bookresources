/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __DLG_HELP_H__
#define __DLG_HELP_H__

//---------------------------------------------------------------------------------------
class DlgHelp : public CDialog
{
public:
	DlgHelp(CWnd* pParent = NULL);   // standard constructor
	enum { IDD = IDD_DIALOG2 };
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual void OnOK();
	afx_msg void OnDoubleclickedOk();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

#endif // !__DLG_HELP_H__
