/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "z_ed2View.h"
#include "baseutils.h"
#include "mainfrm.h"
#include "collections.h"

//---------------------------------------------------------------------------------------
// Associates the cursor handler to the sizing direction
struct SHCUR
{
    HCURSOR hc;
    DWORD   scDir;
};

//---------------------------------------------------------------------------------------
// Default colors for view grids and axes
BYTE	CLR_FINEGRIDF[3]	= {38,39,39};
BYTE	CLR_FINEGRIDD[3]	= {44,44,44};
BYTE	CLR_GRIDF[3]		= {14,14,14};
BYTE	CLR_GRIDD[3]		= {42,42,42};
BYTE	CLR_AXES_X[3]		= {255,0,0};
BYTE	CLR_AXES_Y[3]		= {0,255,0};
BYTE	CLR_AXES_Z[3]		= {0,0,255};


//---------------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CZ_ed2View, CView)

BEGIN_MESSAGE_MAP(CZ_ed2View, CView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_WM_KEYDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_SETFOCUS()
    ON_WM_SHOWWINDOW()
    ON_WM_KILLFOCUS()
    ON_MESSAGE(WM_USR_REPAINT,OnUsrMsgRepaint)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
// 
CZ_ed2View::CZ_ed2View()
{
    _zoom   = .32f;
    _shifty = 0.0f;
    _shiftx = 0.0f;
    _selBox = FALSE;
    _active = FALSE;
    _canMove=1;
}

//---------------------------------------------------------------------------------------
// 
CZ_ed2View::~CZ_ed2View()
{
}

//---------------------------------------------------------------------------------------
// Change the window class to block the Background painting by setting background brush 
// to 0.
BOOL CZ_ed2View::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                       LoadCursor(0,IDC_CROSS),
                                       (HBRUSH)0,0);
    return CView::PreCreateWindow(cs);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnDraw(CDC* pDC)
{
}

//---------------------------------------------------------------------------------------
BOOL CZ_ed2View::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CZ_ed2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CZ_ed2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

//---------------------------------------------------------------------------------------
// Init OpenGL on this window DC 
int CZ_ed2View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    _accmode = BR_VIEW;
    _hdc     = ::GetDC(m_hWnd);
    DOC()->InitGL(_hdc, _hhRc);
    BuildFont(_hdc);
    return 0;
}

//---------------------------------------------------------------------------------------
// Releases OpenGL render context and window DC
void CZ_ed2View::ReleaseRDI()
{
    wglMakeCurrent(_hdc, _hhRc);
    glDeleteLists(_font1,   96);
    wglDeleteContext(_hhRc);
    ::ReleaseDC(m_hWnd,_hdc);
    _hdc  = 0;
    _hhRc = 0;
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnDestroy()
{
    CView::OnDestroy();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::RedrawDC()
{
    DrawInHrc(_hdc);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnPaint()
{
    CPaintDC dc(this);
    DrawInHrc(dc.m_hDC);
}

//---------------------------------------------------------------------------------------
void	CZ_ed2View::DrawInHrc(HDC hdc)
{
    wglMakeCurrent(hdc, _hhRc);
    if(_rt.right && _rt.bottom)
    {
        Project2D(_rt.right, _rt.bottom);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        GLPaint(_rt);
        glEnable(GL_DEPTH_TEST);
        SelectTextCtx();
        TextOut(0,_rt.bottom/2,MKSTR("%.2f",_vport.left));
        TextOut(_rt.right-40,_rt.bottom/2,MKSTR("%.2f",_vport.right));

        switch(_vt)
        {
        case 'x':
            TextOut(10,0,"+Z");
            TextOut(0,10,"+Y");
            TextOut(0,_rt.bottom-13,"LEFT VIEW", _active ? ZZWHITE : ZZRED);
            TextOut(_rt.right/2, _rt.bottom-13, MKSTR("%.2f",_vport.bottom));
            TextOut(_rt.right/2, 13,MKSTR("%.2f",_vport.top));
            break;
        case 'y':
            TextOut(10,0,"+X");
            TextOut(0,10,"-Z");
            TextOut(0,_rt.bottom-13,"TOP VIEW",_active ? ZZWHITE : ZZYELLOW);
            TextOut(_rt.right/2,_rt.bottom-13,MKSTR("%.2f",-_vport.bottom));
            TextOut(_rt.right/2,13,MKSTR("%.2f",-_vport.top));
            break;
        case 'z':
            TextOut(10,0,"+X");
            TextOut(0,10,"+Y");
            TextOut(0,_rt.bottom-13,"FRONT VIEW",_active ? ZZWHITE :SKYZZBLUE);
            TextOut(_rt.right/2,_rt.bottom-13,MKSTR("%.2f",_vport.bottom));
            TextOut(_rt.right/2,13,MKSTR("%.2f",_vport.top));
            break;
        }
        DeSelectTextCtx();
        SwapBuffers(hdc);
    }
    wglMakeCurrent(0, 0);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    GetClientRect(&_rt);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::Project2D(int iWidth, int iHeight)
{
    if(iHeight == 0)
        iHeight = 1;
    glViewport(0, 0, iWidth, iHeight);
    if(DOC()->_bShowCsg)
        glClearColor(0.11f, 0.11f, 0.11f, 0.0f);
    else
        glClearColor(.200f, .200f, .200f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    _vport.left   = -(((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
    _vport.right  =  (((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
    _vport.top    = -(((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
    _vport.bottom  = (((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
    glOrtho(_vport.left, _vport.right, _vport.top , _vport.bottom, -32768, 32768);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::GLPaint(RECT& rt)
{
    glPushMatrix();{
        DrawGrid(rt);
        DOC()->Draw2DScene(this);
    }
    glPopMatrix();

}

//---------------------------------------------------------------------------------------
void CZ_ed2View::DrawGrid(RECT& rt)
{
    switch(_vt)
    {
        case 'y':
            DrawTopGrid(rt);
            break;
        case 'x':
            DrawSideGrid(rt);
            break;
        case 'z':
            DrawFrontGrid(rt);
            break;
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::DrawFrontGrid(RECT& rt)
{
    int  hssz  = DOC()->_gridSize / 2;
    REAL Left  = Wnd2World(rt.left,  'h');
    REAL Right = Wnd2World(rt.right, 'h');
    REAL Top   = Wnd2World(rt.top,   'v');
    REAL Bottom= Wnd2World(rt.bottom,'v');
    int  wp;

    if (Left < -hssz)
        Left = -hssz;
    if (Right > hssz)
        Right = hssz;
    if (Top > hssz)
        Top = hssz;
    if (Bottom < -hssz)
        Bottom = -hssz;
    glPushMatrix();
    if(_zoom>.04){
        _active ? glColor3ubv(CLR_FINEGRIDF) : glColor3ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int decVal = 0; decVal <= DOC()->_gridSize; decVal += DOC()->_gridStep){
            if (decVal % (DOC()->_gridStep * 10) && decVal != hssz){
                wp = hssz - decVal;
                if (wp > (int)Left && wp < (int)Right){
                    glVertex3f(wp, Top, 0.0);
                    glVertex3f(wp, Bottom, 0.0);
                }
                if (wp > (int)Bottom && wp < (int)Top){
                    glVertex3f(Left, wp, 0.0);
                    glVertex3f(Right, wp, 0.0);
                }
            }
        }
        glEnd();
    }

    _active ?   glColor3ubv(CLR_GRIDF):glColor3ubv(CLR_GRIDD);
    glBegin(GL_LINES);
    for(int decVal = 0; decVal <= DOC()->_gridSize; decVal += (DOC()->_gridStep * 10))
    {
        wp = hssz - decVal;
        if (decVal != hssz){
            if (wp >= (int)Left && wp <= (int)Right){
                glVertex3f(wp, Top, 0.0);
                glVertex3f(wp, Bottom, 0.0);
            }
            if (wp >= (int)Bottom && wp <= (int)Top){
                glVertex3f(Left, wp, 0.0);
                glVertex3f(Right, wp, 0.0);
            }
        }
    }
    glEnd();
    glBegin(GL_LINES);
    glColor3ubv(CLR_AXES_X);
    glVertex3f(Left, 0.0, 0.0);
    glVertex3f(Right, 0.0, 0.0);
    glColor3ubv(CLR_AXES_Y);
    glVertex3f(0.0, Top, 0.0);
    glVertex3f(0.0, Bottom, 0.0);
    glEnd();
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::DrawTopGrid(RECT& rt)
{
    int hgsz = DOC()->_gridSize / 2;
    GLfloat Left    = Wnd2World(rt.left, 'h');
    GLfloat Right   = Wnd2World(rt.right, 'h');
    GLfloat Top     = Wnd2World(rt.top, 'v');
    GLfloat Bottom  = Wnd2World(rt.bottom, 'v');
    int wp;
    if (Left < -hgsz)
        Left = -hgsz;
    if (Right > hgsz)
        Right = hgsz;
    if (Top < -hgsz)
        Top = -hgsz;
    if (Bottom > hgsz)
        Bottom = hgsz;
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    if(_zoom>.04){
        _active ? glColor3ubv(CLR_FINEGRIDF) : glColor3ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int vDec = 0; vDec <= DOC()->_gridSize; vDec += DOC()->_gridStep){
            if (vDec % (DOC()->_gridStep * 10) && vDec != hgsz){
                wp = hgsz - vDec;
                if (wp > (int)Left && wp < (int)Right){
                    glVertex3f(wp, 0.0, Top);
                    glVertex3f(wp, 0.0, Bottom);
                }
                if (wp < (int)Bottom && wp > (int)Top){
                    glVertex3f(Left, 0.0, wp);
                    glVertex3f(Right, 0.0, wp);
                }
            }
        }
        glEnd();
    }
    _active ?   glColor3ubv(CLR_GRIDF):glColor3ubv(CLR_GRIDD);
    glBegin(GL_LINES);
    for(int vDec = 0; vDec <= DOC()->_gridSize; vDec += (DOC()->_gridStep * 10)){
        if (vDec != hgsz){
            wp = hgsz - vDec;
            if (wp >= (int)Left && wp <= (int)Right){
                glVertex3f(wp, 0.0, Top);
                glVertex3f(wp, 0.0, Bottom);
            }
            if (wp <= (int)Bottom && wp >= (int)Top){
                glVertex3f(Left, 0.0, wp);
                glVertex3f(Right, 0.0, wp);
            }
        }
    }
    glEnd();
    glBegin(GL_LINES);
    glColor3ubv(CLR_AXES_X);
    glVertex3f((int)Left, 0, 0);
    glVertex3f((int)Right, 0, 0);
    glColor3ubv(CLR_AXES_Z);
    glVertex3f(0.0, 0.0, Top);
    glVertex3f(0.0, 0.0, Bottom);
    glEnd();
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::DrawSideGrid(RECT& rt)
{
    int hdsz = DOC()->_gridSize / 2;
    GLfloat Left    = Wnd2World(rt.left, 'h');
    GLfloat Right   = Wnd2World(rt.right, 'h');
    GLfloat Top     = Wnd2World(rt.top, 'v');
    GLfloat Bottom  = Wnd2World(rt.bottom, 'v');
    int     wp;
    if (Left < -hdsz)
        Left = -hdsz;
    if (Right > hdsz)
        Right = hdsz;
    if (Top > hdsz)
        Top = hdsz;
    if (Bottom < -hdsz)
        Bottom = -hdsz;
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    if(_zoom>.04){
        _active ? glColor3ubv(CLR_FINEGRIDF) : glColor3ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int fd= 0; fd<= DOC()->_gridSize; fd+= DOC()->_gridStep){
            if (fd% (DOC()->_gridStep * 10) && fd!= hdsz)
            {
                wp = hdsz - fd;
                if (wp > (int)Left && wp < (int)Right){
                    glVertex3f(0.0, Top, wp);
                    glVertex3f(0.0, Bottom, wp);
                }
                if (wp > (int)Bottom && wp < (int)Top){
                    glVertex3f(0.0, wp, Left);
                    glVertex3f(0.0, wp, Right);
                }
            }
        }
        glEnd();
    }
    _active ?   glColor3ubv(CLR_GRIDF):glColor3ubv(CLR_GRIDD);
    glBegin(GL_LINES);
    for( int fd= 0; fd<= DOC()->_gridSize; fd+= (DOC()->_gridStep * 10)){
        if (fd!= hdsz){
            wp = hdsz - fd;
            if (wp >= (int)Left && wp <= (int)Right){
                // Vertical lines
                glVertex3f(0.0, Top, wp);
                glVertex3f(0.0, Bottom, wp);
            }
            if (wp >= (int)Bottom && wp <= (int)Top){
                // Horizontal lines
                glVertex3f(0.0, wp, Left);
                glVertex3f(0.0, wp, Right);
            }
        }
    }
    glEnd();
    glBegin(GL_LINES);
    glColor3ubv(CLR_AXES_Z);
    glVertex3f(0.0, 0.0, Left);
    glVertex3f(0.0, 0.0, Right);
    glColor3ubv(CLR_AXES_Y);
    glVertex3f(0.0, Top, 0.0);
    glVertex3f(0.0, Bottom, 0.0);
    glEnd();
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnLButtonDown(UINT nFlags, CPoint point)
{
    _mm._ldown=1;
    _mm._pt  =  point;
    _mm._ptd = point;
    _scaleDirDown = _scaleDir;
    _canMoveDown  = _canMove;
    CheckCoordRestrains();
    DOC()->OnStartMoveRotScale();
    SetCapture();
    DOC()->Invalidate();
    CView::OnLButtonDown(nFlags, point);
    _dirty = FALSE;
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
    _scaleDirDown = 0;
    _canMoveDown  = 0;
    _mm._ldown    = 0;
    _depl=0;
    if(_dirty)
        DOC()->OnFinishMoveRotScale(_dirty);
    if(DOC()->_selmode != SEL_NONE){
        CPoint pdiff = point - _mm._ptd;
        if(abs(pdiff.x) < 2 && abs(pdiff.y) < 2){
            if(AKEY(VK_SPACE)){
                DoSelection(point);
            }
        }
    }
    CView::OnLButtonUp(nFlags, point);
    DOC()->Invalidate();
    MannageCursor(point,0);
    _dirty = FALSE;
}

//---------------------------------------------------------------------------------------
void    CZ_ed2View::ScrollView(CPoint& pt)
{
    _shiftx += (REAL)pt.x / _zoom;
    _shifty -= (REAL)pt.y / _zoom;
    if(DOC()->_xyz_lock){
        DOC()->SetShift(this);
        DOC()->Invalidate();
    }
    else{
        switch(_vt)
        {
            case 'x':
                SBT(SB_2DPOS, MKSTR("2D: x=%04.2f z=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
            case 'y':
                SBT(SB_2DPOS, MKSTR("2D: z=%04.2f y=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
            case 'z':
                SBT(SB_2DPOS, MKSTR("2D: x=%04.2f y=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
        }
        Invalidate();
    }
}

//---------------------------------------------------------------------------------------
void    CZ_ed2View::RaiseBrush(CPoint& pt)
{
    // UNUSED
}

//---------------------------------------------------------------------------------------
// Moves the selected Brush
void    CZ_ed2View::MoveBrush(CPoint& pt)
{
    if(0 == _canMoveDown){
        ScrollView(pt);
    }
    else{
        V3  t;
        switch(_vt)
        {
            case 'x':
                t.z = -(REAL)pt.x / _zoom;
                t.y = (REAL)pt.y / _zoom;
                break;
            case 'y':
                t.x = -(REAL)pt.x / _zoom;
                t.z = -(REAL)pt.y / _zoom;
                break;
            case 'z':
                t.x = -(REAL)pt.x / _zoom;
                t.y = (REAL)pt.y / _zoom;
                break;
        }
        int count;
        Brush** b = DOC()->GetSelBrushes(count);
        for(int i=0; i < count; i++, b++){
            // movement blocked on 2 coordonates
            if(_depl==1) t.x=0;
            if(_depl==2) t.y=0;
            if(_depl==3) t.z=0;
            (*b)->Move(t);
            (*b)->Recalc();
        }
        _dirty = M_MOVED;
        DrawInHrc(_hdc);
    }
}

//---------------------------------------------------------------------------------------
// Scales teh selected Brush
void    CZ_ed2View::ScaleBrush(CPoint& pt, REAL mxw, REAL myz)
{
    V3 s,t;
    if(0 == _scaleDirDown){
        ScrollView(pt);
        return;
    }
    else{
        if(_scaleDirDown == SIZE_MOVE){
            MoveBrush(pt);
            return;
        }
        // decide which side we are sizing
        if(_scaleDirDown == SIZE_N || _scaleDirDown == SIZE_S)
            pt.x=0;
        if(_scaleDirDown == SIZE_E || _scaleDirDown == SIZE_W)
            pt.y=0;
        switch(_vt)
        {
        case 'y':	//X
            s.x = -(REAL)pt.x / _zoom;
            s.z = -(REAL)pt.y / _zoom;
            switch(_scaleDirDown)
            {
                case SIZE_NE:
                case SIZE_N:
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
                case SIZE_SW:
                case SIZE_W:
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
                case SIZE_NW:
                    s.z=-s.z;
                    t.z=-s.z;
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
            }
            break;

        case 'x':	//-z Y
            s.z = -(REAL)pt.x / _zoom;
            s.y = (REAL)pt.y  / _zoom;

            switch(_scaleDirDown)
            {
                case SIZE_NE:
                case SIZE_N:
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
                case SIZE_SW:
                case SIZE_W:
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
                case SIZE_NW:
                    s.y=-s.y;
                    t.y=-s.y;
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
            }
            break;

        case 'z':
            s.x = -(REAL)pt.x / _zoom;
            s.y = (REAL)pt.y  / _zoom;

            switch(_scaleDirDown)
            {
                case SIZE_SW:
                    s.x=-s.x;
                    t.x=-s.x;
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
                case SIZE_NW:
                case SIZE_W:
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
                case SIZE_SE:
                case SIZE_S:
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
            }
            break;
        }
        BBox box;
        int  count;
        Brush** b = DOC()->GetSelBrushes(count);
        for(int i=0;i<count;i++,b++){
            if(!AKEY(VK_CONTROL))
                (*b)->LimitScaleVector(s, _zoom);
            if(s.x==0)t.x=0;
            if(s.y==0)t.y=0;
            if(s.z==0)t.z=0;
            (*b)->Scale(s,t, _zoom);
            (*b)->Recalc();
            box.Union((*b)->_box);
        }
        V3 ex = box.GetExtends();
        SBT(SB_BRUSHBBOX,MKSTR("%3.2f %3.2f %3.2f", ex.x, ex.y, ex.z));
        DOC()->Invalidate();
        _dirty = M_SCALED;
    }
}

//---------------------------------------------------------------------------------------
// Rotates a brush
void    CZ_ed2View::RotateBrush(CPoint& pt)
{
    if(0 == _canMoveDown){
        ScrollView(pt);
    }
    else{
        V3      r;
        switch(_vt)
        {
            case 'x':
                r.x = pt.x/64.f;
                r.x = pt.y/64.f;
                break;

            case 'y':
                r.y = pt.x/64.f;
                r.y = pt.y/64.f;

                break;
            case 'z':
                r.z = pt.x/64.f;
                r.z = pt.y/64.f;
                break;
        }
        if(AKEY(VK_CONTROL)){
            if(r.x)
                r.x/=16.f;
            if(r.y)
                r.y/=16.f;
            if(r.z)
                r.z/=16.f;
        }
        int     count;
        Brush** b = DOC()->GetSelBrushes(count);
        for(int i=0;i<count;i++,b++){
            (*b)->Rotate(r);
            (*b)->Recalc();
        }
        DrawInHrc(_hdc);
        _dirty = M_ROTATED;
    }
}

//---------------------------------------------------------------------------------------
// Handles mouse movement over 2D view area
void CZ_ed2View::OnMouseMove(UINT nFlags, CPoint point)
{
    REAL mxw = Wnd2World(point.x, 'h');
    REAL myw = Wnd2World(point.y, 'v');
    switch(_vt)
    {
        case 'x':
            SBT(SB_MOUSEPOS,MKSTR("M: z:%03.2f y:%03.2f ", mxw, myw));
            break;
        case 'y':
            SBT(SB_MOUSEPOS,MKSTR("M: x:%03.2f z:%03.2f ", mxw, myw));
            break;
        case 'z':
            SBT(SB_MOUSEPOS,MKSTR("M: x:%03.2f y:%03.2f ", mxw, myw));
            break;
    }

    _accmode = DOC()->_brmode;
    if(_mm._ldown){
        CPoint pt = _mm._pt - point;
        _mm._pt   = point;
        CheckCoordRestrains();
        switch(_accmode)
        {
            case BR_VIEW:
                ScrollView(pt);
                break;
            case BR_MOVE:
                MoveBrush(pt);
                break;
            case BR_SCALE:
                ScaleBrush(pt, mxw, myw);
                break;
            case BR_ROTATE:
                RotateBrush(pt);
                break;
            case BR_RISE:
                RaiseBrush(pt);
                break;
        }
    }else{
        _scaleDir = 0;
        ClientToScreen(&point);
        if(GetCapture() && this != WindowFromPoint(point)){
            ReleaseCapture();
        }
    }
    CView::OnMouseMove(nFlags, point);
}

//---------------------------------------------------------------------------------------
// Zooms the 2D viewport
BOOL CZ_ed2View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    REAL dz = .01f;
    if(AKEY(VK_SHIFT)){ // zooms slower
        dz=.5f;
    }

    if(zDelta > 0)
        _zoom+=dz;
    else
        _zoom-=dz;
    CLAMPVAL(_zoom,.005f,128.f);
    if(DOC()->_xyz_lock){
        DOC()->SetZoom(this);
        DOC()->Invalidate() ;
    }
    else{
        switch(_vt)
        {
            case 'x':
                SBT(SB_2DPOS, MKSTR("2D: x=%04.2f z=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
            case 'y':
                SBT(SB_2DPOS, MKSTR("2D: z=%04.2f y=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
            case 'z':
                SBT(SB_2DPOS, MKSTR("2D: x=%04.2f y=%04.2f * %2.2f", _shiftx, _shifty, _zoom));
                break;
        }
        Invalidate();
    }
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

//---------------------------------------------------------------------------------------
// Transforms world coordinates to window coordinates
int CZ_ed2View::World2Wnd(REAL coord, char dir)
{
    int wpos;
    if (dir == 'h'){
        wpos = (int)(_zoom * (coord - _shiftx));
    }
    else
        if (_vt != 'y')
            wpos = (int)(_zoom * (coord - _shifty));
        else
            wpos = (int)(_zoom * (coord + _shifty));

    if (dir == 'h')
        wpos += (int)(0.5 * _rt.right);
    else{
        if (_vt != 'y')
            wpos *= -1;
        wpos += (int)(0.5 * _rt.bottom);
    }
    return wpos;
}

//---------------------------------------------------------------------------------------
// Vice versa
REAL    CZ_ed2View::Wnd2World(REAL coord, char dir)
{
    int      itemp = 0;
    REAL     fWp = 0.0;

    if(dir=='h'){
        itemp = (int)(coord + (-0.5 * _rt.right));
        fWp = itemp / _zoom + _shiftx;
    }
    else{
        if(_vt != 'y'){
            itemp = (int)(-1 * (coord + (-0.5 * _rt.bottom)));
            fWp = itemp / _zoom + _shifty;
        }
        else{
            itemp = (int)(coord + (-0.5 * _rt.bottom));
            fWp = itemp / _zoom - _shifty;
        }
    }
    return fWp;
}

//---------------------------------------------------------------------------------------
// Handles the cursor shape
BOOL CZ_ed2View::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    switch(_accmode)
    {
        case BR_VIEW:
            SetCursor(DOC()->_cursors[CUR_SCROLL]);
            return 1;
        case BR_MOVE:
            {
                _canMove = 0;
                CPoint pt;
                GetCursorPos(&pt);
                ScreenToClient(&pt);
                if(MannageCursor(pt,0)){
                    SetCursor(DOC()->_cursors[CUR_MOVE]);
                    return 1;
                }
            }
            SetCursor(DOC()->_cursors[CUR_SCROLL]);
            return 1;
        case BR_SCALE:
            {
                _scaleDir = 0;
                CPoint pt;
                GetCursorPos(&pt);
                ScreenToClient(&pt);
                if(MannageCursor(pt,1)){
                    return 1;
                }
            }
            SetCursor(DOC()->_cursors[CUR_SCROLL]);
            return 1;
        case BR_ROTATE:
            {
                _canMove = 0;
                CPoint pt;
                GetCursorPos(&pt);
                ScreenToClient(&pt);
                if(MannageCursor(pt,0))
                {
                    SetCursor(DOC()->_cursors[CUR_ROTATE]);
                    return 1;
                }
            }
            SetCursor(DOC()->_cursors[CUR_SCROLL]);
            return 1;
        case BR_RISE:
            {
                //_canMove = 1;
                CPoint pt;
                GetCursorPos(&pt);
                ScreenToClient(&pt);
                if(MannageCursor(pt,2))
                {
                    return 1;
                }
            }
            SetCursor(DOC()->_cursors[CUR_SCROLL]);
            return 1;

    }
    return CView::OnSetCursor(pWnd, nHitTest, message);
}

//---------------------------------------------------------------------------------------
// Maps some shortcuts for toolbar menu buttons as Move, Scale, Rotate and View
void CZ_ed2View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch(nChar)
    {
    case VK_DOWN:
        break;
    case VK_RIGHT:
        break;
    case VK_LEFT:
        break;
    case VK_UP:
        break;
    case 'M':
        DOC()->OnBumove();
        break;
    case 'D':
        DOC()->OnBUScale();
        break;
    case 'R':
        DOC()->OnBurotate();
        break;
    case 'V':
        DOC()->SetViewMode();
        break;
    }
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//---------------------------------------------------------------------------------------
// Test if given box is inside of the viewport. Used in scene culling
BOOL CZ_ed2View::IsBoxInViewPort(BBox& box)
{
    return TRUE;
    switch(_vt)
    {
    case 'x':
        if(box._max.z < _vport.left)
            return FALSE;
        if(box._min.z > _vport.right)
            return FALSE;
        if(box._min.y > _vport.bottom)
            return FALSE;
        if(box._max.y < _vport.top)
            return FALSE;
        break;
    case 'y':
        if(box._max.x < _vport.left)
            return FALSE;
        if(box._min.x > _vport.right)
            return FALSE;
        if(box._min.z > _vport.bottom)
            return FALSE;
        if(box._max.z < _vport.top)
            return FALSE;
        break;
    case 'z':
        if(box._max.x < _vport.left)
            return FALSE;
        if(box._min.x > _vport.right)
            return FALSE;
        if(box._min.y > -_vport.top)
            return FALSE;
        if(box._max.y < -_vport.bottom)
            return FALSE;
        break;
    }
    return TRUE;
}

//---------------------------------------------------------------------------------------
// selects a brush if is unde the cursor in 2D window
void CZ_ed2View::DoSelection(CPoint&  point)
{
    REAL mxw = Wnd2World(point.x, 'h');
    REAL myw = Wnd2World(point.y, 'v');
    _EL_SEL sel = DOC()->_selmode;
    if(SEL_NONE!=sel)
        DOC()->DoSelection2D(mxw,myw,this, sel);
    Invalidate();
}

//---------------------------------------------------------------------------------------
// Handles the cursor shape over a brush, and brush edges
BOOL    CZ_ed2View::MannageCursor(CPoint point, BOOL set)
{
    int  count = 0;
    if(DOC()->_selmode == SEL_NONE)
        return 0;
    if(_accmode == BR_VIEW )
        return 0;
    Brush** ppb = DOC()->GetSelBrushes(count);
    if(0 == count)
        return 0;

    BBox acumBox;
    int  Mx,My,mx,my;

    ResolveCurInBox(acumBox,Mx,My,mx,my);
    int hs = abs((Mx-mx)/3 + 1);
    int vs = abs((My-my)/3 + 1);
    SHCUR sScaleCurs[3][3] =
    {
        {{DOC()->_cursors[CUR_NWSE], SIZE_N|SIZE_W},  {DOC()->_cursors[CUR_NS],SIZE_N}     , {DOC()->_cursors[CUR_NESW], SIZE_N|SIZE_E}},
        {{DOC()->_cursors[CUR_EW],   SIZE_W},         {DOC()->_cursors[CUR_SCALE], SIZE_MOVE/*SIZE_0*/} , {DOC()->_cursors[CUR_EW],   SIZE_E}},
        {{DOC()->_cursors[CUR_NESW], SIZE_S|SIZE_W},  {DOC()->_cursors[CUR_NS],SIZE_S}     , {DOC()->_cursors[CUR_NWSE], SIZE_S|SIZE_E}},
    };

    SHCUR sRiseCurs[3][3] =
    {
        {{DOC()->_cursors[CUR_MOVE], SIZE_MOVE|SIZE_W},  {DOC()->_cursors[CUR_EW],SIZE_N}     , {DOC()->_cursors[CUR_MOVE], SIZE_MOVE}},
        {{DOC()->_cursors[CUR_NS],   SIZE_W},         {DOC()->_cursors[CUR_SCALE], SIZE_MOVE} , {DOC()->_cursors[CUR_NS],   SIZE_E}},
        {{DOC()->_cursors[CUR_MOVE], SIZE_MOVE},  {DOC()->_cursors[CUR_EW],SIZE_S}     , {DOC()->_cursors[CUR_MOVE], SIZE_MOVE}},
    };


    if(_vt == 'x')
    {
        DWORD t;
        t = sScaleCurs[0][0].scDir;
        sScaleCurs[0][0].scDir=sScaleCurs[2][0].scDir;
        sScaleCurs[2][0].scDir=t;
        t = sScaleCurs[0][1].scDir;
        sScaleCurs[0][1].scDir=sScaleCurs[2][1].scDir;
        sScaleCurs[2][1].scDir=t;
        t = sScaleCurs[0][2].scDir;
        sScaleCurs[0][2].scDir=sScaleCurs[2][2].scDir;
        sScaleCurs[2][2].scDir=t;
    }

    if(!set){
        CRect   rne(mx, my, Mx, My);
        if(rne.PtInRect(point)){
            _canMove=TRUE;
            return TRUE;
        }
    }
    else{
        int i=0,j=0;
        for(int xPos = mx; xPos < Mx; xPos+=hs){
            j=0;
            for(int yPos = my; yPos < My; yPos+=vs){
                CRect   rne;
                switch(_vt)
                {
                case 'x':
                    rne.left   = xPos;
                    rne.top    = yPos;
                    rne.right  = xPos+hs;
                    rne.bottom = yPos+vs;
                    break;
                case 'y':
                    rne.left   = xPos;
                    rne.top    = yPos;
                    rne.right  = xPos+hs;
                    rne.bottom = yPos+vs;
                    break;
                case 'z':
                    rne.left   = xPos;
                    rne.top    = yPos;
                    rne.right  = xPos+hs;
                    rne.bottom = yPos+vs;
                    break;
                }
                if(rne.PtInRect(point)){
                    _scaleDir = sScaleCurs[j][i].scDir;
                    if(2 == set){
                            SetCursor(sRiseCurs[j][i].hc);
                    }else{
                        SetCursor(sScaleCurs[j][i].hc);
                    }
                    return TRUE;
                }
                j++;
            }
            i++;
        }
    }
    return FALSE;
}

//---------------------------------------------------------------------------------------
//  Builds the font for 2D windows
void CZ_ed2View::BuildFont(HDC hDC)
{
    wglMakeCurrent(_hdc, _hhRc);

    _font1 = glGenLists(96);
    _hf    = CreateFont( -10, 0, 0, 0, FW_MEDIUM, FALSE,
                         FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                         CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                         FF_DONTCARE | DEFAULT_PITCH, "Arial" );
    HFONT hf = (HFONT)SelectObject(_hdc, _hf);
    wglUseFontBitmaps(_hdc, 32, 96, _font1);
    SelectObject(_hdc, hf);
}

//---------------------------------------------------------------------------------------
//  Sets the state of OpenGl in text drawing mode
void    CZ_ed2View::SelectTextCtx()
{
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(_rt.left, _rt.right, _rt.top , _rt.bottom, -32768, 32768);
    glColor3ub( 255,255,0);
    glMatrixMode(GL_MODELVIEW);
}

//---------------------------------------------------------------------------------------
//  Resets the text drawing mode
void    CZ_ed2View::DeSelectTextCtx()
{
    glPopAttrib();
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::TextOut(int x, int y, const char* text, CLR& clr)
{
    glColor3ub(clr.r,clr.g,clr.b);
    glRasterPos2i( x, y );
    glPushAttrib(GL_LIST_BIT);
    glListBase( _font1-32 );
    glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
}

//---------------------------------------------------------------------------------------
// Is kindof inapropriate for this function to be in this class.
// Iterates 'pMovedBrushes' and builds/updates the touching lists 
void CZ_ed2View::UpdateCutsCollisionList(Brush** pMovedBrushes, int nCount)
{
    int			nBrushes = nCount;
    Brush**     pWlkBrush	 = pMovedBrushes;
    map<Brush*, int>    pBrsh2Update;

    while(nBrushes--){
        Brush* pMovedBrush = *pWlkBrush++;
        //  Solid Brushes
        if(!pMovedBrush->IsSolid()){
            vvector<Brush*>::iterator ppb = pMovedBrush->_pSldBrushes.begin();
            vvector<Brush*>::iterator ppE = pMovedBrush->_pSldBrushes.end();
            for(;ppb!=ppE;ppb++){
                pBrsh2Update[*ppb] = 1;
            };
        }else // Cut brushes
        {
            pBrsh2Update[pMovedBrush] = 1;
        }
    }

    // Proecesses the cuts at realtime
    nBrushes	  = nCount;
    pWlkBrush	  = pMovedBrushes;
    while(nBrushes--){
        Brush* pMovedBrush = *pWlkBrush++;
        colex::forall2( (*DOC()->GetPrimitives()), this,
                        &CZ_ed2View::ProcCollWithCuts,
                        pMovedBrush, pBrsh2Update);
    }

    // Cut the brushes with the cuts
    map<Brush*,int>::iterator mb =  pBrsh2Update.begin();
    map<Brush*,int>::iterator me =  pBrsh2Update.end();
    for(;mb!=me;mb++){
        (*mb).first->ApplyCuts();
    }
}

//---------------------------------------------------------------------------------------
// Links a brush with it's cutters
BOOL CZ_ed2View::ProcCollWithCuts(Brush* pPrim, Brush* pMovedBrush,
                                  map<Brush*, int>& accList)
{
    if(pMovedBrush == pPrim)
        return TRUE;

    if(pPrim->_box.IsTouchesBox(pMovedBrush->_box)){
        // intesection between cut brush and solid brush
        if((pPrim->_flags & BRSH_SOLID) !=  (pMovedBrush->_flags & BRSH_SOLID)){

            if(pPrim->_flags & BRSH_SOLID ){                // we changed a cut brush pos/size
                if(pMovedBrush->_flags & BRSH_CUTALL){      // this cut cuts in all touuching brushes
                    pPrim->AddCutter(pMovedBrush);
                    pMovedBrush->AddBrush(pPrim);	        // keep a list for cutter (we move away from cutter)
                    accList[pPrim]=1;
                }
                else                                        // this cut cuts brushes which id is smaller than its id
                    if(pMovedBrush->_unicID > pPrim->_unicID){
                        pPrim->AddCutter(pMovedBrush);
                        pMovedBrush->AddBrush(pPrim);	    // keep a list for cutter 
                        accList[pPrim]=1;
                    }
            }
            else{                                           // we changed a solid brush pos/size
                if(pPrim->_flags & BRSH_CUTALL){
                    pMovedBrush->AddCutter(pPrim);
                    pPrim->AddBrush(pMovedBrush);           // keep a list for solid
                    accList[pMovedBrush]=1;
                }
                else
                    if(pPrim->_unicID > pMovedBrush->_unicID){
                        pMovedBrush->AddCutter(pPrim);
                        pPrim->AddBrush(pMovedBrush);       
                        accList[pMovedBrush]=1;
                    }
            }
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnRButtonDown(UINT nFlags, CPoint point)
{
    CView::OnRButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnRButtonUp(UINT nFlags, CPoint point)
{
    CView::OnRButtonUp(nFlags, point);
}

//---------------------------------------------------------------------------------------
// Detect where the cursor is inside of a rectangle. 
void CZ_ed2View::ResolveCurInBox(BBox& acumBox,int& Mx,int& My,int& mx,int& my)
{
    int count;
    Brush** ppb = DOC()->GetSelBrushes(count);

    for(int i=0;i<count;i++,ppb++){
        acumBox.Union((*ppb)->_box);
    }

    switch(_vt)
    {
    case 'x':	
        Mx = World2Wnd(acumBox._max.z,'h');
        mx = World2Wnd(acumBox._min.z,'h');
        my = World2Wnd(acumBox._max.y,'v');
        My = World2Wnd(acumBox._min.y,'v');
        break;
    case 'y':	//X
        Mx = World2Wnd(acumBox._max.x,'h');
        mx = World2Wnd(acumBox._min.x,'h');
        My = World2Wnd(acumBox._max.z,'v');
        my = World2Wnd(acumBox._min.z,'v');
        break;
    case 'z':
        Mx = World2Wnd(acumBox._max.x,'h');
        mx = World2Wnd(acumBox._min.x,'h');
        my = World2Wnd(acumBox._max.y,'v');
        My = World2Wnd(acumBox._min.y,'v');
        break;
    }

    int dx =  (Mx-mx)/6;
    int dy =  (My-my)/6;
    Mx += dx;
    mx -= dx;
    My += dy;
    my -= dy;
}

//---------------------------------------------------------------------------------------
// Forces a repaint of the 2D viewport
LRESULT CZ_ed2View::OnUsrMsgRepaint(WPARAM, LPARAM)
{
    DrawInHrc(_hdc);
    return 0;
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnSetFocus(CWnd* pOldWnd)
{
    CView::OnSetFocus(pOldWnd);
    _active = TRUE;
    DOC()->SetActiveWiew(this);
    DrawInHrc(_hdc);
}

//---------------------------------------------------------------------------------------
void CZ_ed2View::OnKillFocus(CWnd* pNewWnd)
{
    CView::OnKillFocus(pNewWnd);
    DOC()->SetActiveWiew(pNewWnd);
    _active = FALSE;
    DrawInHrc(_hdc);
}

//---------------------------------------------------------------------------------------
// Blocks the movement on a given set of coordinates.
void CZ_ed2View::CheckCoordRestrains()
{
    _depl = 0;
    // allow the moving scaling only on the coord that has been selected (by a key down)
    switch(_vt)
    {
    case 'x':       // allows X (SIDE)
        if(AKEY('X')) _depl = 2;
        if(AKEY('Y')) _depl = 3;
        break;
    case 'y':       // allows Y (TOP)
        if(AKEY('X'))_depl=3;
        if(AKEY('Y'))_depl=1;
        break;
    case 'z':       // allows Z (FRONT)
        if(AKEY('X'))_depl=2;
        if(AKEY('Y'))_depl=1;
        break;
    }
}
