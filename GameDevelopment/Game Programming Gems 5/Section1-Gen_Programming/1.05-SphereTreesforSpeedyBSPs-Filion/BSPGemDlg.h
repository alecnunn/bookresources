// BSPGemDlg.h : header file
//

#pragma once

#include <d3d9.h>
#include <d3dx9mesh.h>
#include "afxwin.h"
#include "SphereTree.h"
#include "BSPTreeBuilder.h"
#include "afxcmn.h"

// The application dialog
// Standard MFC code

class CBSPGemDlg : public CDialog
{

public:

	enum { IDD = IDD_BSPGEM_DIALOG };

	CBSPGemDlg(CWnd* pParent = NULL);
	virtual ~CBSPGemDlg();

protected:

	IDirect3D9*			m_pD3D;					// IDirect3D9 interface used for mesh loading
	IDirect3DDevice9*	m_pD3DDevice;			// Direct3DDevice9 interface used formesh loading

	// Dialog controls
	CStatic				m_VertexCountLabel;
	CStatic				m_TriCountLabel;
	CStatic				m_FilenameLabel;
	CStatic				m_StatusLabel;
	CStatic				m_SphereTreeNodeCountLabel;
	CStatic				m_SphereTreeLeavesLabel;
	CStatic				m_SphereTreeDepthLabel;
	CStatic				m_AvgSphereTreeDepthLabel;
	CButton				m_GoBtn;
	CEdit				m_LogEdt;

	CComPtr<ID3DXMesh>	m_pXMesh;

	CSphereTree				m_SphereTree;
	CBSPTreeBuilder		m_BSPTreeBuilder;

	virtual void		DoDataExchange(CDataExchange* pDX);

	virtual BOOL		OnInitDialog();

	// Loads an .x file into the sample application.
	bool				LoadFile( const char* szFilename );

	// Load button was clicked.
	afx_msg void		OnBnClickedLoadBtn();

	// Go button was clicked.
	afx_msg void		OnBnClickedGobtn();

	DECLARE_MESSAGE_MAP()

public:
	CProgressCtrl m_Progress;
	CStatic m_AvgBSPDepth;
	CStatic m_BSPDepth;
	CStatic m_BSPLeaves;
	CStatic m_BSPNodes;
	CStatic m_BSPSplits;
};
