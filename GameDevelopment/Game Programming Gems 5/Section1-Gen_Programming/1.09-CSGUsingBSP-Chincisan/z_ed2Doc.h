/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __ED2_DOC_H__
#define __ED2_DOC_H__

#include <list>
#include "basemath.h"
#include "zedtypes.h"
#include "scene.h"
#include "camera.h"
#include "DlgNewBrush.h"
#include "DlgNewBrush.h"
#include "BspTree.h"
#include "Compiler.h"

class CZ_ed2View;
class z_ed3View;
class CZ_ed2Doc : public CDocument
{
public:
    CZ_ed2Doc();
    virtual ~CZ_ed2Doc();
    static CZ_ed2Doc*   PDoc;
    void InitGL(HDC hDC, HGLRC& hrc);
    void SetShift(CZ_ed2View* pSrc);
    void SetZoom(CZ_ed2View* pSrc);
    void DisableAll(BOOL bd);
    void SegmentCollideBrush(V3& s, V3& end,_EL_SEL sel);
    void SetActiveWiew(CWnd* pWnd){_pActiveView = pWnd;};
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual void DeleteContents();
    void OnTbLockxyz();
    void OnUpdateTbLockxyz(CCmdUI* pCmdUI);
    void OnMkBox();
    void OnBurotate();
    void OnUpdateBurotate(CCmdUI* pCmdUI);
    void OnBumove();
    void OnUpdateBumove(CCmdUI* pCmdUI);
    void OnBUScale();
    void OnUpdateBUScale(CCmdUI* pCmdUI);
    void OnBushcam();
    void OnUpdateBushcam(CCmdUI* pCmdUI);
    void OnBureverse();
    void OnButselbrush();
    void OnUpdateButselbrush(CCmdUI* pCmdUI);
    void OnFileSave();
    void OnFileOpen();
    void OnBuwireframe();
    void OnUpdateBuwireframe(CCmdUI* pCmdUI);
    void OnEditCopy();
    void OnEditPaste();
    void OnBushowback();
    void OnUpdateBushowback(CCmdUI* pCmdUI);
    void DoHSR();
    void OnBushowcsg();
    void OnUpdateBushowcsg(CCmdUI* pCmdUI);
    void OnDelete();
    void OnCutBrush();
    void OnUpdateCutBrush(CCmdUI* pCmdUI);
    void OnTabNextsel();
    void OnUpdateEditCopy(CCmdUI* pCmdUI);
    void OnCut();
    void OnUpdateCut(CCmdUI* pCmdUI);
    void OnUpdateEditPaste(CCmdUI* pCmdUI);
    void OnUpdateDelete(CCmdUI* pCmdUI);
    void MkCylinder();
    void OnEscape();
    void OnEnter();
    void OnUpdateEnter(CCmdUI* pCmdUI);
    void OnEditUndo();
    void OnUpdateEditUndo(CCmdUI* pCmdUI);
    void OnFileSaveAs();
    void OnBusplitable();
    void OnUpdateBusplitable(CCmdUI* pCmdUI);
    void OnBuredcuts();
    void OnBucone();
    void OnTabPrevsel();
    void OnSelectall();
    void OnTabSelnext();
    void OnTabSelprev();
    void OnStartMoveRotScale();
    void OnFinishMoveRotScale(int dirty);
    void UndoUncomittedBrushes();
    void DeleteUncomittedNewBrushes();
    void SetViewMode(){_brmode=BR_VIEW;};
    void GLDrawBBox(BBox&,BOOL);
    void Draw2DScene(CZ_ed2View* pV);
    void Draw3DScene(z_ed3View* pV);
    void Invalidate() ;
    void Invalidate3D() ;
    void DoSelection3D(CPoint& point, z_ed3View* pV, _EL_SEL sel);
    void DoSelection2D(REAL x, REAL y, CZ_ed2View* pV, _EL_SEL sel);
    void BuildCamFrustrum(z_ed3View* pV,CPoint& pt, RECT rt, V3& out);
    BOOL BrushIntersect(V3& vstart, V3& end, int&, int& );
    void DeselectAll();
    void DeselectBrushes();
    void DeselectPolys();
    void Recut();
    BOOL HasSomeSelected(){return (_pSelBrushes.size()!=0 ||_pSelPolys.size()!=0) ;}
    void SelectBrush(Brush* pB);
    void DeSelectBrush(Brush* pB);
    void SelectTabBrush(int direction, BOOL cumulate);
    Brush** GetSelBrushes(int& count){
        count = _pSelBrushes.size();
        return &_pSelBrushes[0];
    }
    Poly** GetSelPolys(int& count){
        count = _pSelPolys.size();
        return &_pSelPolys[0];
    }
    int HasForms(){
        if(_bShowCsg){
            return _scene.GetBrushes()->size();
        }
        return _scene.GetPrimitives()->size();
    }

    PBrushes* GetForms(){
        if(_bShowCsg){
            return _scene.GetBrushes();
        }
        return _scene.GetPrimitives();
    }

    Brush* GetForm(int index){
        if(_bShowCsg)
            return _scene.GetBrush(index);
        return _scene.GetPrimitive(index);
    }
    PBrushes* GetPrimitives(){
        return _scene.GetPrimitives();
    }
    DECLARE_DYNCREATE(CZ_ed2Doc)
    DECLARE_MESSAGE_MAP()
    
protected:
    void SaveInFile(LPCTSTR pSzName, BOOL prims=FALSE);
    void ApplyFlags(Brush* b);
    void DrawCameraRay(CZ_ed2View *pV);
    void DrawCamFrustrum(CZ_ed2View *pV);
    void SelectBrush(int brSel, int faceSel);
    void SelectFace(int brSel, int faceSel);
    void Draw2DBrushes(CZ_ed2View *pV);
    void DrawTextured(z_ed3View *pV);
    void DrawSelBrushes(z_ed3View *pV);
    void DrawCutBrush(Brush& rb);
    void DrawSolidBrush(Brush& rb);
    void DrawAxis();


public:
    CWnd            *_pActiveView;
    BR_MODE         _brmode;
    EL_SEL          _selmode;
    BOOL            _bSnapGrid;
    Camera		    _cam;
    BOOL            _alignment;
    BOOL            _snapToGrid;
    int             _gridSizeFactor;
    int             _gridSize;
    int             _gridStep;
    CFont           _cfont;
    BOOL            _xyz_lock;
    int             _iselbrush;
    V3              _shift;
    V3              _zoom;
    BOOL			_glLight;
    BOOL            _bDrawCamRay;
    Scene           _scene;
    UINT		    _dumptex;
    V3			    _camray[3];
    V3			    _frustDots[5];
    V3              _ip[2];
    HCURSOR         _cursors[32];
    BOOL		    _vmodefire;
    BOOL		    _vmodeback;
    BOOL            _bCutOn;
    int             _curgroup;
    PBrushes        _pClipboard;
    PBrushes        _pSelBrushes;
    PPolys          _pSelPolys;
    Compiler        _compiler;
    int             _leafNo;
    CDlgNewBrush    _newBrshDlg;
    SNewBrush       _cachedSett;
    BOOL            _bShowCsg;
};

#define DOC() CZ_ed2Doc::PDoc

extern BYTE	CLR_SEL[4];
extern BYTE	CLR_NORM[4];
extern BYTE	CLR_BBOX[4];

#endif // __ED2_DOC_H__
