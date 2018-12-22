#ifndef __BSP_TCH_DLG_H__
#define __BSP_TCH_DLG_H__

#include "StaticGL.h"
#include "geometry.h"
#include "compiler.h"

class CBsp_TechniquesDlg : public CDialog
{
public:
	CBsp_TechniquesDlg(CWnd* pParent = NULL);
    static CBsp_TechniquesDlg* _PThis;
	enum { IDD = IDD_BSP_TECHNIQUES_DIALOG };
	CProgressCtrl	m_prog;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
    static void Paint_Here(CWnd* pdc, V_MODE mode);
protected:
	HICON m_hIcon;
    BOOL LoadFile(LPCTSTR filename);
	BOOL OnInitDialog();
	void OnSysCommand(UINT nID, LPARAM lParam);
	void OnPaint();
	void OnOk();
	void OnCancel();
	void OnSimple();
    void OnExit();
	void OnComplex();
	void OnCompile();
	void OnOrig();
	void OnBsp();
	void OnPortals();
	void OnPortalsbsp();
	void OnTerr();
	void OnDestroy();
	void OnWalk();
	void OnTimer(UINT nIDEvent);
    LRESULT OnCompiled(WPARAM,LPARAM);
    LRESULT OnRepaint(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
    CStaticGL   gl_3d;
    CStaticGL   gl_2d;
    Brush*      p_brush;
    Compiler    o_compiler;
    UINT        rb_id;
    UINT        t_timer;
    BOOL        b_terr;
};
void    TextOut(int,LPCTSTR,...);
#endif // 
