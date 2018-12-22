/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __ZED3DV_H__
#define __ZED3DV_H__

#include "basemath.h"
#include "basearrays.h"
#include "mmove.h"
#include <stdio.h>
#include <gl/gl.h>			
#include <gl/glu.h>			
#include <gl/glaux.h>	
#include "zedtypes.h"


//---------------------------------------------------------------------------------------
// 3D viewport class
class CZ_ed2Doc;
class z_ed3View : public CView
{
public:
    z_ed3View();           
    virtual ~z_ed3View();
    void Project3D(int iWidth, int iHeight);
    void ReleaseRDI();
    void Redraw();
    void DrawDirty(int i){_dd=i;}
    void TextOut(V3& v, const char* text, CLR& clr);
    DECLARE_DYNCREATE(z_ed3View)
    DECLARE_MESSAGE_MAP()

protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void         GLPaint();
    void OnPaint();
    int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnMove(int x, int y);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnSize(UINT nType, int cx, int cy);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnRButtonUp(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void BuildFont(HDC hDC);
    void DrawInHrc(HDC hdc);

private:
    FRECT   _vport;
    BOOL    _active;
    MMove   _mm;
    BOOL    _rotating;

public:
    HDC     _hdc;
    HGLRC   _hhRc;
    BOOL    _dd;    
    RECT    _rt;
    REAL    _fov;
    REAL    _fnear;
    REAL    _ffar;
    int     _font1;
    HFONT   _hf;

    static z_ed3View* P3DView;
};

//---------------------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Z_ED3VIEW_H__21E16863_9BBC_40BF_BEF3_17D6CEC43D72__INCLUDED_)
