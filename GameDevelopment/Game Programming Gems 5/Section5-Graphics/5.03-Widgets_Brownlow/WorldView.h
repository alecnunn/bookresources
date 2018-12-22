#if !defined(AFX_WORLDVIEW_H__022012F7_0493_47E2_9437_FA34BE37E756__INCLUDED_)
#define AFX_WORLDVIEW_H__022012F7_0493_47E2_9437_FA34BE37E756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorldView.h : header file
//

#include "world.h"
#include "widgetbsp.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldView window
typedef struct
{
	D3DXVECTOR4	lightDir;
	D3DXVECTOR4	lightColor;
	D3DXVECTOR4	ambientColor;

} LIGHTINGINFO;

class CWorldView : public CStatic
{
// Construction
public:
	CWorldView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldView)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldView();

	bool	Init();
	void	Uninit();
	void	UpdateViewMatrix();
	void	Draw();

	void	CreateWidgets( u32 uNumWidgets );
	void	SetDrawDistance( float fDistance );

protected:
	bool			m_bInit;
	DWORD			m_uMouseButtons;
	CD3DHelper		m_d3d;
	CWorld			m_world;
	CWidgetMesh		m_widgetMesh[2];
	CWidgetBSP		m_widgetBSP[2];

	LIGHTINGINFO	m_lighting;


	D3DXVECTOR3		m_viewPos;
	D3DXVECTOR3		m_euler;
	D3DXMATRIX		m_viewMtx;
	D3DXMATRIX		m_projMtx;
	float			m_fov[2];

	POINT			m_lastMousePos;
	u32				m_uNumWidgets;
	float			m_drawDistance;


	void	UpdateMouse( DWORD buttonsDown, DWORD buttonsUp, CPoint point );

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorldView)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDVIEW_H__022012F7_0493_47E2_9437_FA34BE37E756__INCLUDED_)
