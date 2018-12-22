/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#ifndef __STATIC_GL_H__
#define __STATIC_GL_H__
#include <gl/gl.h>			
#include <gl/glu.h>			
#include <gl/glaux.h>	
#include "mmove.h"	
#include "camera.h"	
#include "types.h"
#include "geometry.h"	

#define WM_REPAINT       (WM_USER+1001)
#define WM_COMPILE_DONE  (WM_USER+1000)
#define BUTS             8

enum V_MODE{
    V_3DFRONT,
    V_3D,
    V_2D,
};
enum V_ACTION
{
    A_NONE  =   -1,
    A_SHIFTVIEW,
    A_SIZEVIEW,
};


typedef void (*CbSize)(CWnd* pwnd, int x, int y);
typedef void (*CbPrepPaint)(CWnd* pdc, V_MODE mode, BOOL init);
typedef void (*CbPaint)(CWnd* pdc, V_MODE mode);
typedef void (*CbInit)(CWnd* pwnd);
typedef BOOL (*CbMouse)(UINT msg, V_ACTION m, CPoint& pt, V3& xyz) ;

class Brush;
class CStaticGL : public CWnd
{
public:
    CStaticGL(){
        _pBrush = 0;
        _dist  = -800;
        _bsel   = FALSE;
        _pcbsz    = 0;
        _pcbpaint = 0;
        _pcbpreppaint = 0;
        _pcbinit  = 0;
        _pcbmouse = 0;
        _pcbsetcursor = 0;
        _bHideCoords  = 0;
        _glAddSwapHintRectWIN=0;
    };

    ~CStaticGL()
    {
        TRACE("~CStaticGL()");
    }
    void    InitialUpdate();
    BOOL    CreateWnd(CWnd* parent, UINT isctrl, LPCTSTR);
    void    GLPaint();
    void    UseSelection(BOOL b= TRUE){_bsel=b;}
    REAL    Wnd2World(REAL coord, char dir);
    V3      Wnd2World2(int x, int y );
    void    DrawGrids();
    void    DrawViewModes();
    void    SelectTextCtx();
    void    DeSelectTextCtx();
    void    PrintOut(V3& v, const char* text, CLR& clr=ZLIGHTGRAY);
    void    PrintOut(int x, int y, const char* text, CLR& clr=ZLIGHTGRAY);
    void    DrawPolyVertexes(Poly& poly);
    void    DrawPolyLines(Poly& poly);
    void    SetMode(V_MODE vm, char c){
        SetViewport(c); 
        _nMode = vm; 
    };
    void    SetViewport(char c){
        switch(c){
            case 'x':
                _nMode = V_2D;
                _pushMode=2;
                _vt = c; 
                break;
            case 'y':
                _nMode = V_2D;
                _pushMode=3;
                _vt = c; 
                break;
            case 'z':
                _nMode = V_2D;
                _pushMode=4;
                _vt = c; 
                break;
            default:
                _pushMode=0;
                _vt = '*'; 
                break;
        }
        GLPaint();
    }
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
    Brush*  _pBrush;
    HDC     _hdc;
    HGLRC   m_hRC;

protected:
    void  DrawSolidBrush();
    DWORD SetFaceMode();
    void InitGL(int i3d, HDC hDC, HGLRC& hrc);
    void DrawCamFrustrum();
	void OnSelchangePrefs();
	void OnDblclkPrefs();
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnPaint();
	void OnSize(UINT nType, int cx, int cy);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnMButtonUp(UINT nFlags, CPoint point);
    BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
    void Perspective(int iWidth, int iHeight) ;
   
    V3      _angle;
    UINT    _timer;
    BOOL    _bsel;

public:
    REAL        _dist;
    CbSize      _pcbsz;
    CbPaint     _pcbpaint;
    CbPrepPaint _pcbpreppaint;
    CbInit      _pcbinit;
    CbMouse     _pcbmouse;
    CbMouse     _pcbsetcursor;
    BOOL        _bHide;
    V3          _curPos;
    FRECT       _vport;
    RECT        _rt;
    REAL        _zoom;
    REAL        _shifty2;
    REAL        _shiftx2;

    REAL        _shifty;
    REAL        _shiftx;
    REAL        _shiftz;	
    BOOL        _moving;
    BOOL        _scaling;
    BOOL        _vireframe;
    MMove       _mm;
    char        _vt;
    V_MODE      _nMode;
    V_ACTION    _nAction;
    Camera      _cam;
	REAL		_bhght;
	int			_selMode;
	int			_pushMode;
    BOOL        _bHideCoords;
    BOOL        _vmodefire;
    UINT        _htexture;
    V3          _camray[3];
    PFNGLADDSWAPHINTRECTWINPROC  _glAddSwapHintRectWIN;
};

#endif // __STATIC_GL_H__
