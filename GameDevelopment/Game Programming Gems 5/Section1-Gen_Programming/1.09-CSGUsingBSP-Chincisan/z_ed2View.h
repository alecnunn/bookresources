/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __ZED2VIEW_H__
#define __ZED2VIEW_H__

#pragma warning (disable: 4251)
#include "basemath.h"
#include "basegeom.h"
#include "basearrays.h"
#include <stdio.h>
#include <gl/gl.h>			
#include <gl/glu.h>			
#include <gl/glaux.h>	
#include "mmove.h"
#include "zedtypes.h"

//---------------------------------------------------------------------------------------
// 2D view projection class
class Brush;
class CZ_ed2Doc;
class CZ_ed2View : public CView
{

public:
    CZ_ed2View();
    virtual ~CZ_ed2View();
    CZ_ed2Doc*   GetDocument();
    void         UpdateCutsCollisionList(Brush** ppB, int brushes );
    void         ReleaseRDI();
    BOOL         IsBoxInViewPort(BBox& box);
    void         Redraw();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void         RedrawDC();

protected: 
    DECLARE_DYNCREATE(CZ_ed2View)
    DECLARE_MESSAGE_MAP()

    LRESULT OnUsrMsgRepaint(WPARAM,LPARAM);
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    void Project2D(int iWidth, int iHeight);
    void GLPaint(RECT&);
    void DoSelection(CPoint&  point);
    int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnPaint();
    void OnSize(UINT nType, int cx, int cy);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnRButtonUp(UINT nFlags, CPoint point);
    void OnSetFocus(CWnd* pOldWnd);
    void OnKillFocus(CWnd* pNewWnd);
    void DrawInHrc(HDC);
    void ScrollView(CPoint& pt);
    void MoveBrush(CPoint& pt);
    void RaiseBrush(CPoint& pt);
    void ScaleBrush(CPoint& pt,REAL,REAL);
    void RotateBrush(CPoint& pt);
    void BuildFont(HDC hDC);
    BOOL MannageCursor(CPoint point,BOOL sel);
    void DrawGrid(RECT& rt);
    void DrawTopGrid(RECT& rt);
    void DrawFrontGrid(RECT& rt);
    void DrawSideGrid(RECT& rt);
    REAL Wnd2World(REAL coord, char dir);
    int  World2Wnd(REAL coord, char dir);
    void TextOut(int x, int y, const char *fmt, CLR& clr=ZZWHITE);
    void SelectTextCtx();
    void DeSelectTextCtx();
    void CheckCoordRestrains();
    BOOL ProcCollWithCuts(Brush* pPrim, Brush* pMovedBrush, map<Brush*, int>& accList);
    void ResolveCurInBox(BBox& acumBox,int& Mx,int& My,int& mx,int& my);
    

public:
    HGLRC   _hhRc;
    HDC     _hdc;
    char    _vt;
    REAL    _zoom;
    REAL    _shifty;
    REAL    _shiftx;
    RECT    _rt;
    MMove   _mm;
    FRECT   _vport;
    BOOL    _active;
    FPOINT	_prepdn;
    int     _font1;
    HFONT   _hf;
    int	    _dirty;
    _BR_MODE  _accmode;
    BOOL    _selBox;
    BOOL    _dd;            
    DWORD	_scaleDir;
    DWORD	_canMove;
    DWORD	_canMoveDown;
    DWORD	_scaleDirDown;
    int		_depl;			
};

#endif // !__ZED2VIEW_H__
