#pragma once


// SetBaseDim dialog

class SetBaseDim : public CDialog
{
	DECLARE_DYNAMIC(SetBaseDim)

public:
	SetBaseDim(CWnd* pParent = NULL);   // standard constructor
	virtual ~SetBaseDim();
	void OnSetDim();

// Dialog Data
	enum { IDD = IDD_SETBASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
