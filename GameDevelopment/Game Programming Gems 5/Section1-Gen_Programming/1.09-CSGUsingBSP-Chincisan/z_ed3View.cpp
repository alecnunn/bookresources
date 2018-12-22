/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "z_ed3View.h"
#include "camera.h"
#include "MainFrm.h"

//---------------------------------------------------------------------------------------
z_ed3View* z_ed3View::P3DView;
IMPLEMENT_DYNCREATE(z_ed3View, CView)

//---------------------------------------------------------------------------------------
z_ed3View::z_ed3View()
{
    _rotating = FALSE;
    _fov      = 64.0f;
    _fnear    = 8.0f;
    _ffar     = 64435.f;
    z_ed3View::P3DView=this;
}

//---------------------------------------------------------------------------------------
z_ed3View::~z_ed3View()
{
}

//---------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(z_ed3View, CView)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_MOVE()
    ON_WM_KEYDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SIZE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
// z_ed3View drawing
void z_ed3View::OnDraw(CDC* pDC)
{
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnPaint()
{
    CPaintDC dc(this);
    DrawInHrc(dc.m_hDC);
}

//---------------------------------------------------------------------------------------
void z_ed3View::DrawInHrc(HDC hdc)
{
    wglMakeCurrent(hdc, _hhRc);
    GetClientRect(&_rt);
    if(_rt.right && _rt.bottom){
        Project3D(_rt.right, _rt.bottom);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLPaint();
        SwapBuffers(hdc);
    }
}

//---------------------------------------------------------------------------------------
void  z_ed3View::Project3D(int iWidth, int iHeight)
{
    if(iHeight == 0){
        iHeight = 1;
    }
    if(DOC()->_bShowCsg)
        glClearColor(0.11f, 0.11f, 0.11f, 0.0f);
    else
        glClearColor(.200f, .200f, .200f, 0.0f);
    glViewport(0, 0, iWidth, iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(_fov,(GLfloat)iWidth/(GLfloat)iHeight, _fnear, _ffar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//---------------------------------------------------------------------------------------
BOOL z_ed3View::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = AfxRegisterWndClass(CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                       LoadCursor(0,IDC_CROSS),
                                       (HBRUSH)0,0);
    return CView::PreCreateWindow(cs);
}

//---------------------------------------------------------------------------------------
int z_ed3View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    _hdc = ::GetDC(m_hWnd);
    DOC()->InitGL(_hdc, _hhRc);
    glPolygonMode(GL_BACK,GL_FILL);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    // dummy texture
    BYTE dBits[16*3] = {255,	0,	255,0,	255,	0,		0,	0,	255,		255,	0,	255,
                        255,	255,0,	0,	255,	0,		0,	255,255,		255,	255,255,
                        255,	0,	0,	0,	255,	0,		255,0,	255,		255,	0,	255,
                        255,	255,255,0,	255,	0,		0,	0,	255,		255,	255,255};
    DOC()->_dumptex = TexRef::GlGenTex( 4, 4, 3 , dBits,0);
    BuildFont(_hdc);
    wglMakeCurrent(0, 0);
    return 0;
}

//---------------------------------------------------------------------------------------
void z_ed3View::ReleaseRDI()
{
    (HFONT)SelectObject(_hdc, 0);
    ::DeleteObject(_hf);
    wglMakeCurrent(_hdc, _hhRc);
    glDeleteLists(_font1,   96);
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(_hhRc);
    ::ReleaseDC(m_hWnd,_hdc);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnDestroy()
{
    CView::OnDestroy();
}

//---------------------------------------------------------------------------------------
void z_ed3View::GLPaint()
{
    glPushMatrix();
    Camera& cam  = DOC()->_cam;
    const M4& mt = cam.GetViewMtx(0);
    cam.UpdateFrustrumPlanes();
    glColor3f(1.0, 1.0f, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf((REAL*)&mt);
    DOC()->Draw3DScene(this);
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnMove(int x, int y)
{
    CView::OnMove(x, y);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    REAL ms = 32.f;
    if(AKEY(VK_SHIFT) )
        ms*=10;
    switch(nChar)
    {
        case 'A':
        case VK_LEFT:
            DOC()->_cam.MoveSide(-ms);
            break;
        case 'S':
        case VK_DOWN:
            DOC()->_cam.MoveFwd(-ms);
            break;
        case 'D':
        case VK_RIGHT:
            DOC()->_cam.MoveSide(ms);
            break;
        case 'W':
        case VK_UP:
            DOC()->_cam.MoveFwd(ms);
            break;
        case 'E':
        case VK_PRIOR:
            DOC()->_cam.MoveUp(ms);
            break;
        case 'C':
        case VK_NEXT:
            DOC()->_cam.MoveUp(-ms);
            break;
    }
    SBT(SB_CAMPOS,MKSTR("CAM: %04.2f %04.2f z:%04.2f",DOC()->_cam._wpos.x,DOC()->_cam._wpos.y,DOC()->_cam._wpos.z));
    DrawInHrc(_hdc);
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnMouseMove(UINT nFlags, CPoint point)
{
    V3  scrPt;
    DOC()->BuildCamFrustrum(this, point, _rt, scrPt);
    CPoint pt = _mm._pt - point;
    _mm._pt   = point;
    if(_mm._ldown){
        if(abs(_mm._pt.x) > 2||
                abs(_mm._pt.y) > 2){
            if(_mm._rdown){
                if(AKEY(VK_CONTROL) ){
                    DOC()->_cam.MoveUp(pt.y*64.f);
                }
                else{
                    DOC()->_cam.MoveUp(pt.y*16.f);
                }

                if(AKEY(VK_CONTROL)){
                    DOC()->_cam.MoveSide(-pt.x*64.f);
                }
                else{
                    DOC()->_cam.MoveSide(-pt.x*16.f);
                }
            }
            else{
                DOC()->_cam.Rotate(-pt.x/64.f,-pt.y/64.f,0);
                if(_rotating==FALSE)
                {
                    _rotating = TRUE;
                }
            }
        }
        DrawInHrc(_hdc);
        SBT(SB_CAMPOS,MKSTR("CAM: %04.2f %04.2f z:%04.2f",DOC()->_cam._wpos.x,DOC()->_cam._wpos.y,DOC()->_cam._wpos.z));
    }
    else if(_mm._rdown)  {
        if(AKEY(VK_CONTROL) )
            DOC()->_cam.MoveFwd(pt.y*64.f);
        else
            DOC()->_cam.MoveFwd(pt.y*16.f);
        DrawInHrc(_hdc);
        SBT(SB_CAMPOS,MKSTR("CAM: %04.2f %04.2f z:%04.2f",DOC()->_cam._wpos.x,DOC()->_cam._wpos.y,DOC()->_cam._wpos.z));
    }
    CView::OnMouseMove(nFlags, point);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnLButtonDown(UINT nFlags, CPoint point)
{
    _rotating=FALSE;
    _mm._ldown=1;
    _mm._pt = point;
    if(GetCapture()!=this)
    {
        ReleaseCapture();
        SetCapture();
    }
    DOC()->Invalidate();
    CView::OnLButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CView::OnLButtonDblClk(nFlags, point);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
    _mm._ldown = FALSE;
    if(!_rotating ){
        _EL_SEL sel = DOC()->_selmode;
        if(SEL_NONE!=sel){
            DOC()->DoSelection3D(point, this, sel);
        }
        DrawInHrc(_hdc);
    }
    CView::OnLButtonUp(nFlags, point);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    wglMakeCurrent(_hdc, _hhRc);
    Project3D(cx, cy);
    GetClientRect(&_rt);
    if(_rt.bottom==0)_rt.bottom=1;
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnRButtonDown(UINT nFlags, CPoint point)
{
    _mm._rdown = TRUE;
    _rotating  = FALSE;
    _mm._pt    = point;
    if(GetCapture()!=this){
        ReleaseCapture();
        SetCapture();
    }
    CView::OnRButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void z_ed3View::OnRButtonUp(UINT nFlags, CPoint point)
{
    _mm._rdown=FALSE;
    ReleaseCapture();
    CView::OnRButtonUp(nFlags, point);
}

//---------------------------------------------------------------------------------------
BOOL z_ed3View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    DrawInHrc(_hdc);
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

//---------------------------------------------------------------------------------------
// builds the font in this HRC
void z_ed3View::BuildFont(HDC hDC)
{
    wglMakeCurrent(_hdc, _hhRc);
    _font1 = glGenLists(96);
    _hf    = CreateFont( -12, 0, 0, 0, FW_MEDIUM, FALSE,
                         FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                         CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                         FF_DONTCARE | DEFAULT_PITCH, "Arial" );
    HFONT hf = (HFONT)SelectObject(_hdc, _hf);
    wglUseFontBitmaps(_hdc, 32, 96, _font1);
    SelectObject(_hdc, hf);
}

//---------------------------------------------------------------------------------------
void z_ed3View::TextOut(V3& v, const char* text, CLR& clr)
{
    glEnable(GL_BLEND);
    glColor3ub(clr.r,clr.g,clr.b);
    glPushMatrix();{
        glRasterPos3f(v.x,v.y,v.z);
        glPushAttrib(GL_LIST_BIT);
        glListBase( _font1-32 );
        glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    }
    glPopMatrix();
    glDisable(GL_BLEND);
}

