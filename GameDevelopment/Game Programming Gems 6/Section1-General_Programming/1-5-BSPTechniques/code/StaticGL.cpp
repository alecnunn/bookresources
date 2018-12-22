/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#include "stdafx.h"
#include "Bsp_Techniques.h"
#include "Geometry.h"
#include "StaticGL.h" 

static REAL _Depth = 1;
static REAL _Fov   = 90;

BEGIN_MESSAGE_MAP(CStaticGL, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()


static BOOL BOnCreate=FALSE;
static struct Bppform
{
    int bpp;
    int gl1;
    int gl2;
} GBpp[5] = 
{
	{0, -1          ,         -1  },
    {1, GL_LUMINANCE, GL_LUMINANCE},
    {2, -1          ,         -1  },
    {3, GL_RGB,  GL_UNSIGNED_BYTE },
    {4, GL_RGBA, GL_UNSIGNED_BYTE }
};

static UINT	_GlGenTex(int x, int y, int bpp, BYTE* pBuff, DWORD flags)
{
	UINT    hTex;
    ::glGenTextures(1,&hTex);
    glBindTexture(GL_TEXTURE_2D, hTex);
    UINT texTarget = GL_TEXTURE_2D;
	glTexParameteri(texTarget,GL_TEXTURE_WRAP_S,GL_REPEAT);		
	glTexParameteri(texTarget,GL_TEXTURE_WRAP_T,GL_REPEAT);		
    glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(texTarget,0,bpp,x,y,0, GBpp[bpp].gl1, GBpp[bpp].gl2,pBuff);
	gluBuild2DMipmaps(texTarget,bpp,x,y, GBpp[bpp].gl1, GBpp[bpp].gl2,pBuff);
	return hTex;
}

BOOL CStaticGL::CreateWnd(CWnd* parent, UINT isctrl, LPCTSTR pszClassName)
{
    BOnCreate = TRUE;
    RECT rt={0,0,1,1};
    CWnd* hwPrev = 0;//
    _vmodefire = 0;
    if(isctrl){
        hwPrev = parent->GetDlgItem(isctrl)->GetNextWindow(GW_HWNDPREV);
        parent->GetDlgItem(isctrl)->GetWindowRect(&rt);
        parent->ScreenToClient(&rt);
        parent->GetDlgItem(isctrl)->ShowWindow(SW_HIDE);
        parent->GetDlgItem(isctrl)->DestroyWindow();
    }
    BOOL b = Create(pszClassName, "", WS_CHILD, rt, parent, 9876);
    if(isctrl){
	    SetWindowPos(hwPrev,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
        SetDlgCtrlID(isctrl);
        ShowWindow(SW_SHOW);
    }
	return b;
}


int CStaticGL::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
    _hdc = ::GetDC(m_hWnd);   
    _pBrush = 0;
    if(_pcbinit)
        _pcbinit(this);
    else{
        InitGL(3,_hdc, m_hRC);
        SetFaceMode();
    }
    _zoom    = 0.136;                 
    _shifty  = 0.0f;
    _shiftx  = 0.0f;
    _shiftz  = 0.0f; 
    _moving  = 0;
    _scaling = 0;
    _nMode   = V_3DFRONT;
	_pushMode= 0; 
    _vt      = 'z';
    _bHide   = FALSE;   
    _vireframe = 0;

    _cam.SetFov(2*PIPE6); 
    _cam.SetFar(24800.0);
	_cam.SetAngles(-PIPE6,-PIPE6,0);
	_cam.Euler2Dir();
	_cam._wpos = V3(0.0f,0.0f,0.0f);
    wglMakeCurrent(_hdc, m_hRC); 
    // get the dafult wall texture from registry
	HRSRC  hrsrc = FindResource(AfxGetResourceHandle(),"WALL", "SCE_ITEM");
    if(hrsrc)
    {
        HGLOBAL hResLoad = LoadResource(AfxGetResourceHandle(), hrsrc);
        if(hResLoad)
        {
            // read the dim here
            DWORD dwSize = SizeofResource(  AfxGetResourceHandle(), hrsrc);

            void* pData = LockResource(hResLoad);
            if(pData)
            {
                _htexture = _GlGenTex(128,128,3,(BYTE*)pData,0);            
            }
        }
    }
    
    _glAddSwapHintRectWIN = (PFNGLADDSWAPHINTRECTWINPROC) wglGetProcAddress("glAddSwapHintRectWIN");
	return 0;
}

void CStaticGL::InitialUpdate()
{
    wglMakeCurrent(_hdc, m_hRC); 
}

void CStaticGL::OnDestroy() 
{
    wglMakeCurrent(_hdc, m_hRC); 
    glDeleteTextures(1, (UINT*)&_htexture);
    wglDeleteContext(m_hRC);    
    wglMakeCurrent(0,0);
    ::ReleaseDC(m_hWnd,_hdc);
	CWnd::OnDestroy();
}
    
void CStaticGL::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    GLPaint();
}

void CStaticGL::OnMouseMove(UINT nFlags, CPoint pt) 
{
    REAL mxw = Wnd2World(pt.x, 'h');
    REAL myw = Wnd2World(pt.y, 'v');
    _curPos = Wnd2World2(pt.x, pt.y);
    switch(_nMode){
        case V_3DFRONT:
            if(_mm._rdown){
                int dz = pt.y-_mm._pt.y;
                _dist -= dz*10;
                _mm._pt = pt;
                if(_dist>0)
                    _dist=0;
                if(_dist<-8000)
                    _dist=-8000;
                GLPaint();
            }else if(_mm._ldown){
                REAL dx = (REAL)(pt.x-_mm._pt.x)/1.0f;
                REAL dy = (REAL)(pt.y-_mm._pt.y)/1.0f;
                _angle.x+=dy;
                _angle.y+=dx;
                _mm._pt = pt;
                GLPaint();
            }
            break;
        case V_3D:
            {
                V3      scrPt;
                CPoint  ptl = _mm._pt - pt;
                _mm._pt   = pt; //due to SetCursor Pos
                if(_mm._rdown || _mm._ldown){
		            if(!AKEY(VK_LBUTTON))
			            _mm._ldown=0;

		            if(!AKEY(VK_RBUTTON))
			            _mm._rdown=0;
                }
                if(_mm._ldown){
                    if(abs(ptl.x) > 0|| 
                       abs(ptl.y) > 0){
                        if(_mm._rdown){
                            _cam.MoveUp(ptl.y*2.f);
    			            _cam.MoveSide(-ptl.x*2.f);
	                    }else{
				            _cam.Rotate(-ptl.x/128.f,-ptl.y/128.f,0);
    		            }
                    }
                    GLPaint();
                    GetParent()->PostMessage(WM_REPAINT);
	            }else if(_mm._rdown){

                    _cam.MoveFwd(ptl.y*4.f);

                    GLPaint();
                    GetParent()->PostMessage(WM_REPAINT);
                }
            }
            break;
        case V_2D:
            if(_mm._ldown){
                CPoint ptl = _mm._pt - pt;
                _mm._pt    = pt;
                V3          shift;
                switch(_vt){
                    case 'x':
                        shift.z += (REAL)ptl.x / _zoom;
                        shift.y -= (REAL)ptl.y / _zoom;
                        break;
                    case 'y':
                        shift.x += (REAL)ptl.x / _zoom;
                        shift.z -= (REAL)ptl.y / _zoom;
                        break;
                    case 'z':
                        shift.x += (REAL)ptl.x / _zoom;
                        shift.y -= (REAL)ptl.y / _zoom;
                        break;
                }
                if(_pcbmouse){
                    if(!(_pcbmouse)(WM_MOUSEMOVE, A_SHIFTVIEW, pt, shift)){
                        _shiftx += shift.x;
                        _shifty += shift.y;
                        _shiftz += shift.z;
                    }
                }else{
                    _shiftx += shift.x;
                    _shifty += shift.y;
                    _shiftz += shift.z;

                }
            }else if(_mm._rdown)  //scale
            {
                CPoint ptl = _mm._pt - pt;
                _mm._pt   = pt;

                if(_zoom<.01)
                    _zoom += (REAL)ptl.y/8256.0;
                else if(_zoom<.1)
                    _zoom += (REAL)ptl.y/1256.0;
                else
                    _zoom += (REAL)ptl.y/256.0;
	            CLAMPVAL(_zoom,.006, 6.f);
            }
            GLPaint();
            break;
    }
	CWnd::OnMouseMove(nFlags, pt);
}

void CStaticGL::OnLButtonDown(UINT nFlags, CPoint pt) 
{
    if(GetCapture()!=this)
        SetCapture();
    _mm._ldown = 1;
    _mm._pt    = pt;
    _mm._ptd   = pt;
    if(_pcbmouse){
        _curPos = Wnd2World2(pt.x, pt.y);
        (_pcbmouse)(WM_LBUTTONDOWN, A_NONE, pt, _curPos);
    }
    CWnd::OnLButtonDown(nFlags, pt);
    GetParent()->SetFocus();
}

void CStaticGL::OnLButtonUp(UINT nFlags, CPoint pt) 
{
    if(GetCapture()==this)
        ReleaseCapture();
    _mm._ldown = 0;
    _curPos = Wnd2World2(pt.x, pt.y);
    if(_pcbmouse){
        (_pcbmouse)(WM_LBUTTONUP, A_SHIFTVIEW, pt, _curPos);
    }
	CWnd::OnLButtonUp(nFlags, pt);
}

void CStaticGL::OnRButtonDown(UINT nFlags, CPoint pt) 
{
    if(GetCapture()!=this)
        SetCapture();
    _mm._rdown = 1;
    _mm._pt = pt;
    _mm._ptd   = pt;
    if(_pcbmouse){
        _curPos = Wnd2World2(pt.x, pt.y);
        (_pcbmouse)(WM_RBUTTONDOWN, A_NONE, pt, _curPos);
    }
	CWnd::OnRButtonDown(nFlags, pt);
    GetParent()->SetFocus();
}

void CStaticGL::OnRButtonUp(UINT nFlags, CPoint pt) 
{
    if(GetCapture()==this)
        ReleaseCapture();
    _mm._rdown = 0;
    if(_pcbmouse){
        _curPos = Wnd2World2(pt.x, pt.y);
        (_pcbmouse)(WM_RBUTTONUP, A_NONE, pt, _curPos);
    }
	CWnd::OnRButtonUp(nFlags, pt);
}

void CStaticGL::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
}

BOOL CStaticGL::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN ){
		if(GetParent())
			GetParent()->PostMessage(WM_KEYDOWN, pMsg->wParam,0);		
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CStaticGL::OnMButtonDown(UINT nFlags, CPoint pt) 
{
	CWnd::OnMButtonDown(nFlags, pt);
}

void CStaticGL::OnMButtonUp(UINT nFlags, CPoint pt) 
{
	CWnd::OnMButtonUp(nFlags, pt);
}


void    CStaticGL::GLPaint()
{
    RECT rt;
    GetClientRect(&rt);
    wglMakeCurrent(_hdc, m_hRC); 
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    Perspective(rt.right, rt.bottom); // perspective
    if(_pcbpreppaint)
        _pcbpreppaint(this ,_nMode, 1);
    glPushMatrix();
    glEnable(GL_BLEND);
    glColor3f(1.0, 1.0f, 1.0);
    switch(_nMode){
        
        case V_3DFRONT:
            glTranslatef(0,0,_dist);
	        glRotatef(_angle.y,0,1,0);
	        glRotatef(_angle.x,1,0,0);
            break;
        case V_3D:
            {

                float position[4] = {-150.0, 150.0, 150.0,1/*non-directional*/};
                REAL a[4]         = {.7,.7,.7,1};//{DOC()->_scene._si.defLumin.r/255.0, DOC()->_scene._si.defLumin.r/255.0, DOC()->_scene._si.defLumin.r/255.0, 1.0};
                REAL dif[4]       = {1.0, 1.0, 1.0, 1.0};
                
                glEnable(GL_LIGHTING);
                glEnable(GL_COLOR_MATERIAL) ;
                glEnable(GL_LIGHT0);

                glLightfv(GL_LIGHT0, GL_POSITION, (float *) _cam._wpos);
                glLightfv(GL_LIGHT0, GL_AMBIENT,  a);
	            glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);

    	        const M4&		mt = _cam.GetViewMtx(0);
                _cam.UpdateFrustrumPlanes();
                glLoadMatrixf((REAL*)&mt);
            }
            break;
        case V_2D:
            DrawGrids();
            switch(_vt){
                case 'x':
                    glRotatef(90,0,1,0);
                    break;
                case 'y':
                    glRotatef(90,1,0,0);
                    break;
                case 'z':
                    break;
            }
            DrawCamFrustrum();
            break;
    }
    glDisable(GL_BLEND);    
    if(_pcbpaint){
        _pcbpaint(this ,_nMode);
    }
    else if(_pBrush)
	{
        glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT);

        if(_vireframe){
	        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }else{
            glEnable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if(_pBrush)
            DrawSolidBrush();
	}

    glDisable(GL_BLEND);    
    glPopMatrix();
    if(_pcbpreppaint)
        _pcbpreppaint(this ,_nMode, 0);
    if(_glAddSwapHintRectWIN)
        (_glAddSwapHintRectWIN)(0,0,_rt.right,_rt.bottom);

    SwapBuffers(_hdc);
    GdiFlush();
    wglMakeCurrent(0, 0); 
}

BOOL CStaticGL::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if(_pcbsetcursor){
        CPoint pt;
        GetCursorPos(&pt);
        ScreenToClient(&pt);
        V3 tp = Wnd2World2(pt.x, pt.y);
        if( (_pcbsetcursor)(WM_MOUSEMOVE, A_NONE, pt, _curPos))
            return 1; // cursor has been set
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CStaticGL::OnSize(UINT nType, int iWidth, int iHeight) 
{
	CWnd::OnSize(nType, iWidth, iHeight);
    wglMakeCurrent(_hdc, m_hRC); 
    if(iHeight == 0)
        iHeight = 1;
    if(_pcbsz){
        _pcbsz(this, iWidth, iHeight);
        return;
    }
    Perspective(iWidth,iHeight);
    GetClientRect(&_rt);
}

void CStaticGL::Perspective(int iWidth, int iHeight) 
{
    switch(_nMode){
        case V_3DFRONT:
            {
                glClearColor(.200f, .200f, .200f, 0.0f);
                glViewport(0, 0, iWidth, iHeight);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(_Fov,(GLfloat)iWidth/(GLfloat)iHeight, _Depth, 8000);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
            break;
        case V_3D:
            {
                glViewport(0, 0, iWidth, iHeight);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(_Fov,(GLfloat)iWidth/(GLfloat)iHeight, _Depth, 16384);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
            break;
        case V_2D:
                glViewport(0, 0, iWidth, iHeight);
                glClearColor(.200f, .200f, .200f, 0.0f);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();

                switch(_vt){
                        case 'x':
                            _vport.left   = -(((REAL)iWidth * 0.5)   / _zoom)   + _shiftz;
                            _vport.right  =  (((REAL)iWidth * 0.5)   / _zoom)   + _shiftz;
                            _vport.top    = -(((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
                            _vport.bottom  = (((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
                            break;
                        case 'y':
                            _vport.left   = -(((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
                            _vport.right  =  (((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
                            _vport.top    = -(((REAL)iHeight * 0.5)  / _zoom)   + _shiftz;
                            _vport.bottom  = (((REAL)iHeight * 0.5)  / _zoom)   + _shiftz;
                            break;
                        case 'z':
                            _vport.left   = -(((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
                            _vport.right  =  (((REAL)iWidth * 0.5)   / _zoom)   + _shiftx;
                            _vport.top    = -(((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
                            _vport.bottom  = (((REAL)iHeight * 0.5)  / _zoom)   + _shifty;
                            break;
                }

                glOrtho(_vport.left, _vport.right, _vport.top , _vport.bottom, -32768, 32768);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            break;
    }
   
}

V3  CStaticGL::Wnd2World2(int x, int y )
{
    V3   rez;
	int      itempX = 0;
    int      itempY = 0;
	REAL     fWp = 0.0;
    RECT     rt;
    GetClientRect(&rt);

    switch(_vt){
        case 'x':
            itempX = (int)(x + (-0.5 * rt.right));
            itempY = (int)(-1 * (y + (-0.5 * rt.bottom)));
            rez.z = itempX / _zoom + _shiftz;
            rez.y = itempY / _zoom + _shifty;
            break;
        case 'y':
            itempX = (int)(x + (-0.5 * rt.right));
            itempY = (int)(y + (-0.5 * rt.bottom));
            rez.x = itempX / _zoom + _shiftx;
            rez.z = itempY / _zoom - _shiftz;
            break;
        case 'z':
            itempX = (int)(x + (-0.5 * rt.right));
            itempY = (int)(-1 * (y + (-0.5 * rt.bottom)));
            rez.x = itempX / _zoom + _shiftx;
            rez.y = itempY / _zoom + _shifty;
            break;
    }
    return rez;
}

REAL    CStaticGL::Wnd2World(REAL coord, char dir)
{
	int      itemp = 0;
	REAL     fWp = 0.0;
    RECT     rt;
    GetClientRect(&rt);

    if(dir=='h'){
        itemp = (int)(coord + (-0.5 * rt.right));
		fWp = itemp / _zoom + _shiftx;
    }else{
        if(_vt != 'y'){
			itemp = (int)(-1 * (coord + (-0.5 * rt.bottom)));
			fWp = itemp / _zoom + _shifty;
        }else{
			itemp = (int)(coord + (-0.5 * rt.bottom));
			fWp = itemp / _zoom - _shifty;
        }
    }
    return fWp;
}


void CStaticGL::DrawGrids()
{
}

void    CStaticGL::DrawViewModes()
{
    RECT    rt;
    GetClientRect(&rt);
    REAL    as = (REAL)rt.right/(REAL)rt.bottom;
    REAL    dy = tan(PIPE4);
    REAL    dx = tan(PIPE4)*as;
	REAL	offset=_bhght/2;

    glViewport(0, 0, rt.right, rt.bottom);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(_Fov,(GLfloat)rt.right/(GLfloat)rt.bottom, _Depth, 8000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void CStaticGL::InitGL(int i3d, HDC hDC, HGLRC& hrc)
{

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize       = sizeof(pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 24;
    pfd.cDepthBits  = 24;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    int iFormat     = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, iFormat, &pfd);            

    hrc = wglCreateContext(hDC);
    
    wglMakeCurrent(hDC, hrc); 
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearDepth( 1 );
    glDisable(GL_FOG);
    glDisable(GL_DITHER);
    glEnable(GL_ALPHA_TEST);
	glClearColor(0.0f,0.0f,0.0f,0.f);
	SetFaceMode();
    glDisable(GL_LIGHTING);
}


DWORD CStaticGL::SetFaceMode()
{
    glDisable(GL_CULL_FACE);
    if(_vmodefire)
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	return GL_FRONT;
}


void  CStaticGL::DrawSolidBrush()
{
    
    PPoly   pb = _pBrush->_polys.begin();
    PPoly   pe = _pBrush->_polys.end();
    int mode = _vireframe ? GL_LINE_LOOP : GL_POLYGON;
    glColor4ubv(CLR_WHITE);
    glBindTexture(GL_TEXTURE_2D, _htexture);
    for(;pb!=pe;pb++){
        Poly& pl = *pb;
        
        if(pl._flags & POLY_SELECTED){
            glColor4ubv(CLR_WHITE);
        }
        else
            glColor4ubv(CLR_GREY);

        V3& normal = pb->_n;

        glBegin(mode);
        PVertex vb = pl._vtci.begin();
        PVertex ve = pl._vtci.end();
        for(;vb!=ve;vb++){
            glTexCoord2f(vb->_uv[0].u,vb->_uv[0].v);
            glNormal3fv(normal);
            glVertex3fv(vb->_xyz);
            
            
            
        }
        glEnd();
    }
}


void CStaticGL::DrawPolyVertexes(Poly& poly)
{
    PVertex vb,ve; 
    glBegin(GL_POLYGON);
        vb = poly._vtci.begin();
        ve = poly._vtci.end();
        for(;vb!=ve;vb++){
            glTexCoord2f(vb->_uv[0].u,vb->_uv[0].v);
            glVertex3f(vb->_xyz.x, vb->_xyz.y, vb->_xyz.z);
        }
    glEnd();
}

void CStaticGL::DrawPolyLines(Poly& poly)
{
    PVertex vb,ve; 
    glBegin(GL_LINE_LOOP);
        vb = poly._vtci.begin();
        ve = poly._vtci.end();
        for(;vb!=ve;vb++){
            glVertex3f(vb->_xyz.x, vb->_xyz.y, vb->_xyz.z);
        }
    glEnd();
}

void CStaticGL::DrawCamFrustrum()
{
	_camray[0] =_cam._wpos;

	glPointSize(5.f);
	glColor4ubv(CLR_WHITE);
	glBegin(GL_POINTS);
	glVertex3f(_camray[0].x,_camray[0].y,_camray[0].z);
	glEnd();
	glPointSize(1.f);
    _camray[1] =_cam._wpos + _cam._ldir * 200;
    glBegin(GL_LINES);
        glVertex3fv(_camray[0]);
        glVertex3fv(_camray[1]);
    glEnd();
}