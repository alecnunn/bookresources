#include "stdafx.h"
#include "Bsp_Techniques.h"
#include "Bsp_TechniquesDlg.h"

CBsp_TechniquesDlg* CBsp_TechniquesDlg::_PThis;

//-----------------------------------------------------------------------------
CBsp_TechniquesDlg::CBsp_TechniquesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBsp_TechniquesDlg::IDD, pParent),p_brush(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    CBsp_TechniquesDlg::_PThis= this;
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_prog);
}

//-----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CBsp_TechniquesDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
    ON_BN_CLICKED(PB_EXIT, OnExit)
	ON_BN_CLICKED(RB_SIMPLE, OnSimple)
	ON_BN_CLICKED(RB_COMPLEX, OnComplex)
	ON_BN_CLICKED(PB_COMPILE, OnCompile)
	ON_BN_CLICKED(RB_ORIG, OnOrig)
	ON_BN_CLICKED(RB_BSP, OnBsp)
	ON_BN_CLICKED(RB_PORTALS, OnPortals)
	ON_BN_CLICKED(RB_PORTALSBSP, OnPortalsbsp)
	ON_BN_CLICKED(RB_TERR, OnTerr)
	ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(WM_COMPILE_DONE, OnCompiled)
    ON_MESSAGE(WM_REPAINT, OnRepaint)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
BOOL CBsp_TechniquesDlg::OnInitDialog()
{
	
    gl_3d.CreateWnd(this, ST_3D,"BSP_TECH_GL3D_WND");
    gl_2d.CreateWnd(this, ST_2D,"BSP_TECH_GL3D_WND");

    gl_2d._vireframe=1;
    gl_3d._nMode=V_3D;
    gl_2d._nMode=V_2D;
    gl_2d._vt   = 'y';
    b_terr      = 0;

    CheckRadioButton(RB_ORIG, RB_PORTALSBSP, RB_ORIG);

    t_timer = SetTimer(333, 50, 0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnPaint() 
{
	CDialog::OnPaint();
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnExit()
{
    EndDialog(0);
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnSimple() 
{
    b_terr        = 0;
    gl_3d._pBrush = 0;
    gl_2d._pBrush = 0;
    delete p_brush;
    p_brush = 0;
    if(LoadFile("simple.brush"))
    {
        gl_3d._pBrush = p_brush;
        gl_2d._pBrush = p_brush;

        GetDlgItem(RB_BSP)->EnableWindow(0);
        GetDlgItem(RB_PORTALS)->EnableWindow(0);
        GetDlgItem(RB_PORTALSBSP)->EnableWindow(0);
         GetDlgItem(PB_COMPILE)->EnableWindow(1);
        CheckRadioButton(RB_ORIG, RB_PORTALSBSP, RB_ORIG);
        OnOrig() ;
    }
    else
        GetDlgItem(PB_COMPILE)->EnableWindow(0);
    gl_2d.Invalidate();
    gl_3d.Invalidate();
	
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnTerr() 
{
	gl_3d._pBrush = 0;
    gl_2d._pBrush = 0;
    delete p_brush;
    p_brush = 0;
    if(LoadFile("terrain.brush"))
    {
        gl_3d._pBrush = p_brush;
        gl_2d._pBrush = p_brush;
        p_brush->_flags |= BRSH_TERRAIN;
        GetDlgItem(RB_BSP)->EnableWindow(0);
        GetDlgItem(RB_PORTALS)->EnableWindow(0);
        GetDlgItem(RB_PORTALSBSP)->EnableWindow(0);
        GetDlgItem(PB_COMPILE)->EnableWindow(1);
        CheckRadioButton(RB_ORIG, RB_PORTALSBSP, RB_ORIG);
        OnOrig() ;
        b_terr = 1;
    }
    else
        GetDlgItem(PB_COMPILE)->EnableWindow(0);
    gl_2d.Invalidate();
    gl_3d.Invalidate();
	
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnComplex() 
{
    b_terr        = 0;
	gl_3d._pBrush = 0;
    gl_2d._pBrush = 0;
    delete p_brush;
    p_brush = 0;
    if(LoadFile("complex.brush"))
    {
        gl_3d._pBrush = p_brush;
        gl_2d._pBrush = p_brush;

        GetDlgItem(RB_BSP)->EnableWindow(0);
        GetDlgItem(RB_PORTALS)->EnableWindow(0);
        GetDlgItem(RB_PORTALSBSP)->EnableWindow(0);
        CheckRadioButton(RB_ORIG, RB_PORTALSBSP, RB_ORIG);
        OnOrig() ;
        GetDlgItem(PB_COMPILE)->EnableWindow(1);

    }
    else
        GetDlgItem(PB_COMPILE)->EnableWindow(0);
    gl_2d.Invalidate();
    gl_3d.Invalidate();
    
}

//-----------------------------------------------------------------------------
BOOL CBsp_TechniquesDlg::LoadFile(LPCTSTR filename)
{
    
    FILE* pf = fopen(filename,"rb");
    if(pf)
    {
        p_brush = new Brush;
        int vertexes = 0;
        Poly p;
        while(!feof(pf))
        {
            fread(&vertexes,1,sizeof(int),pf);
            if(vertexes==0)
            {
                fclose(pf);
                return FALSE;
            }
            for(int i=0; i<vertexes; i++)
            {
            
                Vtx  v;
                fread(&v,1,sizeof(Vtx),pf);
                //v._uv[0].u*=8;
                //v._uv[0].v*=8;
                p << v;
            }
            (*p_brush) << p;
            p.Clear();
            
            if(feof(pf))
                break;

        }
        p_brush->Recalc();
        fclose(pf);

        return TRUE;
    }
    return FALSE;
}


//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnCompile() 
{
    if(p_brush)
    {
        GetDlgItem(PB_EXIT)->EnableWindow(0);
        GetDlgItem(PB_COMPILE)->EnableWindow(0);
        try{
            SetDlgItemText(ST_OUT6,"Compiling. Please Wait");
	        o_compiler.Compile(*p_brush, *this, IsDlgButtonChecked(CK_USETHR));
        }catch(...)
        {
            GetDlgItem(PB_EXIT)->EnableWindow(1);
            GetDlgItem(PB_COMPILE)->EnableWindow(1);
        }
    }
    else
        AfxMessageBox("Please Load Simple Scene or Complex Scene !", MB_OK|MB_ICONINFORMATION);
}

//-----------------------------------------------------------------------------
LRESULT CBsp_TechniquesDlg::OnCompiled(WPARAM wp, LPARAM lp)
{
    TextOut(ST_OUT7, "");
    GetDlgItem(PB_EXIT)->EnableWindow(1);
    SetDlgItemText(ST_OUT6,"Select Different View Modes and walk into the scene");
    if(wp!=-1)
    {
        
        GetDlgItem(RB_BSP)->EnableWindow(1);
        if(!b_terr)
        {
            GetDlgItem(RB_PORTALS)->EnableWindow(1);
            GetDlgItem(RB_PORTALSBSP)->EnableWindow(1);
            TextOut(ST_OUT2, "Portal Time: %d ms", o_compiler.portal_procesor.dw_deltatime);
            TextOut(ST_OUT3, "PVS Time: %d ms", o_compiler.pvs_procesor.dw_deltatime);
        }
        else
        {
            TextOut(ST_OUT2, "N/A");
            TextOut(ST_OUT3, "N/A");
        }
        GetDlgItem(PB_COMPILE)->EnableWindow(0);
        TextOut(ST_OUT, "BSP Time: %d ms", o_compiler.bsp_tree.dw_deltatime );
    }
    else
    {
        AfxMessageBox("Compiler Error !", MB_OK|MB_ICONHAND);
    }

    return 1l;
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnOrig() 
{
    gl_3d._pcbpaint=0;
    gl_2d._pcbpaint=0;
    gl_3d.Invalidate();
    gl_2d.Invalidate();
    if(p_brush)
        TextOut(ST_OUT4, "Polygons: %d ", p_brush->_polys.size());
    else
        TextOut(ST_OUT4, "Please Load a Scene!");

}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnBsp() 
{
    rb_id = RB_BSP;
    gl_3d._pcbpaint=Paint_Here;
    gl_2d._pcbpaint=Paint_Here;
    gl_3d.Invalidate();
    gl_2d.Invalidate();

    TextOut(ST_OUT4, "Polygons: %d Leafs: %d", o_compiler.bsp_tree._polys.size(), 
                                               o_compiler.bsp_tree._leafs.size());
	
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnPortals() 
{
    rb_id = RB_PORTALS;
    gl_3d._pcbpaint=Paint_Here;
    gl_2d._pcbpaint=Paint_Here;
    gl_3d.Invalidate();
    gl_2d.Invalidate();
    TextOut(ST_OUT4, "Portals: %d ", o_compiler.portal_procesor._portals.size());
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnPortalsbsp() 
{
    rb_id = RB_PORTALSBSP;
    gl_3d._pcbpaint=Paint_Here;
    gl_2d._pcbpaint=Paint_Here;
    gl_3d.Invalidate();
    gl_2d.Invalidate();

    TextOut(ST_OUT4, "Pvs Size: %d bytes", o_compiler.pvs_procesor._cPvss);
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::Paint_Here(CWnd* pdc, V_MODE mode)
{
    CBsp_TechniquesDlg::_PThis->o_compiler.Render(CBsp_TechniquesDlg::_PThis->rb_id, 
                                                  V_3D==mode);
}

//-----------------------------------------------------------------------------
LRESULT CBsp_TechniquesDlg::OnRepaint(WPARAM, LPARAM)
{
    gl_2d.Invalidate();
    gl_2d._cam.Copy(&gl_3d._cam);

    return 1l;
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnDestroy() 
{
    KillTimer(t_timer);
	CDialog::OnDestroy();
	
    o_compiler.Clear()	;
    delete p_brush;
    p_brush = 0;
}

//-----------------------------------------------------------------------------
void    TextOut(int ctrlID, LPCTSTR lpszFormat,...)
{
    va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[512];
    nBuf = ::_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), lpszFormat, args);
    CBsp_TechniquesDlg::_PThis->SendDlgItemMessage(ctrlID, WM_SETTEXT, 0, (LPARAM)szBuffer);
	va_end(args);
}

//-----------------------------------------------------------------------------
void CBsp_TechniquesDlg::OnTimer(UINT te)
{
    if(!GetDlgItem(RB_BSP)->IsWindowEnabled())
        return;

    int     b = 0;
    V3      prevpt = gl_3d._cam._wpos;

    if(AKEY('W'))
    {
        gl_3d._cam.MoveFwd(8.0f);
        b |= 1;
    }
        
    if(AKEY('A') )
    {
        gl_3d._cam.MoveSide(8.0f);
        b |= 1;
    }
        
    if(AKEY('D') )
    {
        gl_3d._cam.MoveSide(-8.0f);
        b |= 1;
    }
        
    if(AKEY('X') || AKEY('S') )
    {
        gl_3d._cam.MoveFwd(-8.0f);
        b |= 1;
    }

    if(b)
    {
        gl_3d.Invalidate();
        
        V3 dirdepl = (gl_3d._cam._wpos-prevpt);
        dirdepl.norm();
        V3 curpt = gl_3d._cam._wpos + dirdepl*32;

        V3 hitpoint;
        if(o_compiler.bsp_tree.SegmentIntersect(prevpt, curpt, 0 , hitpoint))
        {
            gl_3d._cam._wpos = prevpt;
        }
    }
}


