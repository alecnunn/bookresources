/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "mainfrm.h"
#include "Z_ed2View.h"
#include "Z_ed3View.h"
#include "baseutils.h"
#include <algorithm>

//---------------------------------------------------------------------------------------
#define VIEW2D(x_,y_) (CZ_ed2View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)
#define VIEW3D(x_,y_) (z_ed3View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)
CZ_ed2Doc*   CZ_ed2Doc::PDoc;

//---------------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CZ_ed2Doc, CDocument)

//---------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CZ_ed2Doc, CDocument)
    ON_COMMAND(ID_TB_LOCKXYZ, OnTbLockxyz)
    ON_UPDATE_COMMAND_UI(ID_TB_LOCKXYZ, OnUpdateTbLockxyz)
    ON_COMMAND(ID_MK_BOX, OnMkBox)
    ON_COMMAND(ID_BUROTATE, OnBurotate)
    ON_UPDATE_COMMAND_UI(ID_BUROTATE, OnUpdateBurotate)
    ON_COMMAND(ID_BUMOVE, OnBumove)
    ON_UPDATE_COMMAND_UI(ID_BUMOVE, OnUpdateBumove)
    ON_COMMAND(ID_BUScale, OnBUScale)
    ON_UPDATE_COMMAND_UI(ID_BUScale, OnUpdateBUScale)
    ON_COMMAND(ID_BUSHCAM, OnBushcam)
    ON_UPDATE_COMMAND_UI(ID_BUSHCAM, OnUpdateBushcam)
    ON_COMMAND(ID_BUREVERSE, OnBureverse)
    ON_COMMAND(ID_BUTSELBRUSH, OnButselbrush)
    ON_UPDATE_COMMAND_UI(ID_BUTSELBRUSH, OnUpdateButselbrush)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_BUWIREFRAME, OnBuwireframe)
    ON_UPDATE_COMMAND_UI(ID_BUWIREFRAME, OnUpdateBuwireframe)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_BUSHOWBACK, OnBushowback)
    ON_UPDATE_COMMAND_UI(ID_BUSHOWBACK, OnUpdateBushowback)
    ON_COMMAND(ID_BUCSG, DoHSR)
    ON_COMMAND(ID_BUSHOWCSG, OnBushowcsg)
    ON_UPDATE_COMMAND_UI(ID_BUSHOWCSG, OnUpdateBushowcsg)
    ON_COMMAND(ID_DELETE, OnDelete)
    ON_COMMAND(ID_TAB_NEXTSEL, OnTabNextsel)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(MN_CUT, OnCut)
    ON_UPDATE_COMMAND_UI(MN_CUT, OnUpdateCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
    ON_COMMAND(ID_BUCYL, MkCylinder)
    ON_COMMAND(MN_ESCAPE, OnEscape)
    ON_COMMAND(MN_ENTER, OnEnter)
    ON_UPDATE_COMMAND_UI(MN_ENTER, OnUpdateEnter)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_BUCONE, OnBucone)
    ON_COMMAND(ID_TAB_PREVSEL, OnTabPrevsel)
    ON_COMMAND(ID_SELECTALL, OnSelectall)
    ON_COMMAND(ID_TAB_SELNEXT, OnTabSelnext)
    ON_COMMAND(ID_TAB_SELPREV, OnTabSelprev)
END_MESSAGE_MAP()


//---------------------------------------------------------------------------------------
// Default document constructor
CZ_ed2Doc::CZ_ed2Doc()
{
    CZ_ed2Doc::PDoc      = this;
    // Load standard cursor shapes fora ll movements and edge conditions
    _cursors[CUR_SCROLL] = AfxGetApp()->LoadCursor(IDC_SCROLL);
    _cursors[CUR_MOVE]   = AfxGetApp()->LoadCursor(IDC_CMOVE);
    _cursors[CUR_SCALE]  = AfxGetApp()->LoadCursor(IDC_CSCALE);
    _cursors[CUR_ROTATE] = AfxGetApp()->LoadCursor(IDC_CROTATE);
    _cursors[CUR_NS]     = AfxGetApp()->LoadStandardCursor(IDC_SIZENS); 
    _cursors[CUR_EW]     = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
    _cursors[CUR_NWSE]   = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
    _cursors[CUR_NESW]   = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
    _curgroup   = 0;
    _alignment  = FALSE;
    _glLight	= FALSE;
    _leafNo     = -1;
    _cachedSett._isSolid    =1;
    _cachedSett._radstrips  =-1;
    _cachedSett._reversed   =0;
    _cachedSett._splitable  =0;
    _cachedSett._thikness   =20.f;
    _cachedSett._vstrips    =-1;
    _cachedSett._tm         = TM_LIN;
    _pActiveView        = 0;
}

//---------------------------------------------------------------------------------------
CZ_ed2Doc::~CZ_ed2Doc()
{
}

//---------------------------------------------------------------------------------------
BOOL CZ_ed2Doc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    _gridSizeFactor = 11;
    _gridSize   = (int)pow(2, _gridSizeFactor) * 10;
    _gridStep   = 64;
    _xyz_lock   = 0;
    _bDrawCamRay= TRUE;
    _brmode     = BR_VIEW;
    _bSnapGrid  = FALSE;
    _selmode    = SEL_NONE;
    _vmodefire  = FALSE;
    _vmodeback  = FALSE;
    _bShowCsg   = FALSE;
    _bCutOn     = FALSE;
    // position the camera
    _cam.SetFar(65535.0f);
    _cam.SetAngles(-PIPE6,-PIPE6,0);
    _cam.AnglesToDir();
    _cam._wpos = V3(1000.0f,1000.0f,1600.0f);
    SBT(SB_CAMPOS,MKSTR("3D: x:%05.2f y:%05.2f z:%5.2f",_cam._wpos.x,_cam._wpos.y,_cam._wpos.z));
    return TRUE;
}



//---------------------------------------------------------------------------------------
// 
void CZ_ed2Doc::Serialize(CArchive& ar)
{
}

//---------------------------------------------------------------------------------------
// All views shares same translation and zooming facor  or not
void CZ_ed2Doc::OnTbLockxyz()
{
    _xyz_lock = !_xyz_lock;
    CZ_ed2View* pSrc = VIEW2D(1,0); // allways use this as a reference
    if(_xyz_lock==TRUE){
        _shift          = V0;
        CZ_ed2View* pTX = VIEW2D(1,0);
        CZ_ed2View* pTY = VIEW2D(0,1);
        CZ_ed2View* pTZ = VIEW2D(1,1);
        pTY->_zoom   = pSrc->_zoom;
        pTX->_zoom   = pSrc->_zoom;
        pTZ->_zoom   = pSrc->_zoom;
        pTY->_shiftx = _shift.x;            // top
        pTY->_shifty = -_shift.z;
        pTX->_shiftx = -_shift.z;			// side
        pTX->_shifty = _shift.y;
        pTZ->_shiftx = _shift.x;            // front
        pTZ->_shifty = _shift.y;
        _zoom = V3(pSrc->_zoom, pSrc->_zoom, pSrc->_zoom);
    }
    SetShift(pSrc);
    SetZoom(pSrc);
    Invalidate();
}

//---------------------------------------------------------------------------------------
// Toobar button update
void CZ_ed2Doc::OnUpdateTbLockxyz(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_xyz_lock);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::SetShift(CZ_ed2View* pSrc)
{
    CZ_ed2View* pTX = VIEW2D(1,0);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,1);

    switch(pSrc->_vt)
    {
        case 'x':                 // xYZ        // SIDE
            pTY->_shifty = -pSrc->_shiftx;
            pTZ->_shifty = pSrc->_shifty;
            _shift.y = -pSrc->_shifty;
            _shift.z = pSrc->_shiftx;
            break;
        case 'y':               // XyZ          // TOP
            pTX->_shiftx = -pSrc->_shifty;  // side
            pTZ->_shiftx = pSrc->_shiftx;              // front
            _shift.z = pSrc->_shifty;
            _shift.x = pSrc->_shiftx;
            break;
        case 'z':               // XYz          // FRONT
            pTX->_shifty = pSrc->_shifty;
            pTY->_shiftx = pSrc->_shiftx;               // top
            _shift.x = pSrc->_shiftx;
            _shift.y = pSrc->_shifty;
            break;
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::SetZoom(CZ_ed2View* pSrc)
{
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTX = VIEW2D(1,0);
    CZ_ed2View* pTZ = VIEW2D(1,1);
    pTY->_zoom = pSrc->_zoom;
    pTX->_zoom = pSrc->_zoom;
    pTZ->_zoom = pSrc->_zoom;
    _zoom = pTZ->_zoom;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::Invalidate()
{
    CZ_ed2View* pTX   = VIEW2D(1,0);
    CZ_ed2View* pTY   = VIEW2D(0,1);
    CZ_ed2View* pTZ   = VIEW2D(1,1);
    pTX->RedrawDC();
    pTZ->RedrawDC();
    pTY->RedrawDC();
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::Invalidate3D()
{
    CZ_ed2View* pTXYZ = VIEW2D(0,0);
    pTXYZ->RedrawDC();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBUScale()
{
    if(_brmode!=BR_SCALE)
        _brmode = BR_SCALE;
    else
        _brmode = BR_VIEW;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBUScale(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size() );
    pCmdUI->SetCheck(_brmode == BR_SCALE);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBurotate()
{
    if(_brmode!=BR_ROTATE)
        _brmode = BR_ROTATE;
    else
        _brmode = BR_VIEW;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBurotate(CCmdUI* pCmdUI)
{
    if(_bShowCsg ){
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->Enable((_pSelBrushes.size()) && _selmode==SEL_BRUSH);
    pCmdUI->SetCheck(_brmode == BR_ROTATE);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBumove()
{
    if(_brmode!=BR_MOVE)
        _brmode = BR_MOVE;
    else
        _brmode = BR_RISE;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBumove(CCmdUI* pCmdUI)
{
    if(_bShowCsg ){
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size());
    pCmdUI->SetCheck(_brmode == BR_MOVE);

}

//---------------------------------------------------------------------------------------
// shoot a ray onto the focused view from camera pos, then let 3D view to do the selection
void    CZ_ed2Doc::DoSelection2D(REAL x, REAL y, CZ_ed2View* pV, _EL_SEL sel)
{
    V3	 start = _cam._wpos;
    V3	 end   = _cam._wpos;
    switch(pV->_vt)
    {
        case 'x':
            start.y = y;
            start.z = x;
            if (_cam._ldir.x>0)
            {
                end.x   += 32768.f;
            }
            else
            {
                end.x -= 32768.f;
            }
            break;
        case 'y':
            start.x = x;
            start.z = y;

            if (_cam._ldir.y>0)
            {
                end.y += 32768.f;
            }
            else
            {
                end.y -= 32768.f;
            }
            break;
        case 'z':
            start.x = x;
            start.y = y;
            if (_cam._ldir.z>0)
            {
                end.z += 32768.f;
            }
            else
            {
                end.z -= 32768.f;
            }
            break;
    }
    SegmentCollideBrush(start,end,sel);
}

//---------------------------------------------------------------------------------------
// Builds a segment from camera pos, screen point and finds a brush in it's way
void    CZ_ed2Doc::DoSelection3D(CPoint& point, z_ed3View* pV, _EL_SEL sel)
{
    RECT& rt = pV->_rt;
    if(rt.bottom==0)rt.bottom=1;
    V3  scrPt;
    BuildCamFrustrum(pV, point, rt, scrPt);
    V3  start = _cam._wpos;                 // build the seg
    V3  vdir  = (scrPt - start).norm();
    V3  end   = start + vdir * 32768.f;     // 64 units long
    SegmentCollideBrush(start, end, sel);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::SegmentCollideBrush(V3& s, V3& end, _EL_SEL sel)
{
    int brSel,faceSel;

    if(BrushIntersect(s, end, brSel, faceSel)){
        switch(sel)
        {
            default:
            case SEL_NONE:
                break;
            case SEL_BRUSH:
                SelectBrush(brSel, faceSel);
                break;
        }
    }
}

//---------------------------------------------------------------------------------------
// Builds the camera frustrum. You can see this in one of the 2D views if you zoom 
// the camera position to maximum. 
void    CZ_ed2Doc::BuildCamFrustrum(z_ed3View* pV, CPoint& point, RECT rt, V3& out)
{
    if(rt.bottom==0) rt.bottom=1;
    double rr = (double)rt.right /2.0;
    double rb = (double)rt.bottom/2.0;

    // Tranform screen cusor position
    double    ptxpC  = ((double)point.x - rr) / rr;
    double    ptypC  = (rb - (double)point.y) / rb;   //inverty

    // make the fov
    double aspect     = rr/rb;
    double halfScrV   = tan(G2R((double)pV->_fov/2.0)) * (double)pV->_fnear;
    double halfScrH   = halfScrV * aspect;
    _cam.AnglesToDir();
    // calc the on screeen pos
    double scrX = ptxpC * halfScrH;
    double scrY = ptypC * halfScrV;
    V3  pointer =  _cam._wpos + (_cam._ldir * pV->_fnear) + (_cam._lup * scrY) + (_cam._lside * scrX);
    _frustDots[0] = _cam._wpos;
    _frustDots[1] = _cam._wpos + _cam._ldir*pV->_fnear + (_cam._lup * halfScrV)  +   _cam._lside *  (-halfScrH);
    _frustDots[2] = _cam._wpos + _cam._ldir*pV->_fnear + (_cam._lup * halfScrV)  +   _cam._lside *  (halfScrH);
    _frustDots[3] = _cam._wpos + _cam._ldir*pV->_fnear + (_cam._lup * (-halfScrV))  + _cam._lside *  (halfScrH);
    _frustDots[4] = _cam._wpos + _cam._ldir*pV->_fnear + (_cam._lup * (-halfScrV))  + _cam._lside *  (-halfScrH);
    
    // builds a segment where camera is lokking at. Shown as purple line in the 2D view
    _camray[2] = pointer;
    _camray[0] =_cam._wpos;
    _camray[1] = (pointer-_cam._wpos);
    _camray[1].norm();
    _camray[1] = _cam._wpos + _camray[1] * 6400.f;
    out = pointer;
    Invalidate();
}

//---------------------------------------------------------------------------------------
// Iterates brushes, finds intersection, Holds the closest one to the camera.
BOOL    CZ_ed2Doc::BrushIntersect(V3& vstart, V3& vend, int& bsIdx, int& psIdx)
{
    V3      a,b,d,cp;
    REAL    dist  = (99999999.f);
    int     bIdx = 0;
    bsIdx   = -1;
    psIdx   = -1;
    a		= vstart;
    b		= vend;
    cp		= vstart;//_cam._wpos;
    d		= (b-a).norm();
    BBox	box;
    BOOL	bstartInSolid = FALSE;

    _cam.AnglesToDir();
    Plane	camPlane(_cam._ldir, _cam._wpos+_cam._lside);
    PBrushes* rbs = this->GetForms();

    FOREACH(PBrushes, (*rbs), ppB){
        Brush& rb = (**ppB);
        box		  = rb._box;
        box.Expand(8.f);
        if(!rb.IsSolid() && box.ContainPoint(a))
            bstartInSolid=TRUE;
        else
            bstartInSolid=FALSE;

        // we handle here even the polygon we hit
        Polys& polys = rb._polys;
        FOREACH(Polys, polys, pB){
            Poly& pl = *pB;
            V3    ip;
            // normally we shoot from front face DP>0
            REAL shf  = VDp(d, pl._normal);
            if(bstartInSolid)		//shoot from behind 
                shf=-shf;
            if(shf > 0)
                continue;
            if(camPlane.GetSide(pl._box.GetCenter())<0)
                continue;
            if(pl.RayIntersect(a, d, ip) ){
                if(camPlane.GetSide(ip)<0)
                    continue;
                if(pl.ContainPoint(ip) && box.ContainPoint(ip)){
                    REAL d2   = vdist(ip, cp);
                    if(d2 < dist )
                    {
                        bsIdx = ppB - rbs->begin();
                        psIdx = pB -  rb._polys.begin();
                        dist  = d2;
                        _ip[0]   = ip;
                        _ip[1]   = ip+pl._normal * 64.0f;;
                    }
                }
            }
        }
    }
    return (bsIdx>=0);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::Draw2DScene(CZ_ed2View *pV)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0, 1.0f, 1.0);
    glPushMatrix();
    {
        switch(pV->_vt)
        {
            case 'x':
                glRotatef(90,0,1,0);
                break;
            case 'y':
                glRotatef(90,1,0,0);
                break;
            case 'z':

                break;
        }
        Draw2DBrushes(pV);
        if(_bDrawCamRay)
        {
            DrawCameraRay(pV);
            DrawCamFrustrum(pV);
        }
    }
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBushcam()
{
    _bDrawCamRay = !_bDrawCamRay;

}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBushcam(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_bDrawCamRay);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBureverse()
{
    int nCount;
    Brush** b = GetSelBrushes(nCount);
    for(int i=0;i<nCount;i++,b++){
        (*b)->Reverse();
    }
    Recut();
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnButselbrush()
{
    DeselectAll();
    if(_selmode != SEL_BRUSH){
        _selmode = SEL_BRUSH;
    }
    else{
        _selmode = SEL_NONE;
    }
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateButselbrush(CCmdUI* pCmdUI)
{
    if(_bShowCsg ){
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->SetCheck(_selmode == SEL_BRUSH);
}

//---------------------------------------------------------------------------------------
// Allways select the primitives. 
void CZ_ed2Doc::SelectBrush(int brSel, int faceSel)
{
    if(AKEY(VK_CONTROL) ){
        // cumulative multiple selection
        Brush* b = GetForm(brSel);
        if(!b->IsSelected())
            SelectBrush(b);
        else
            DeSelectBrush(b);
    }
    else{
        // exclusive selection
        DeselectBrushes();
        SelectBrush(GetForm(brSel));
    }
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DeselectBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes,a){
        (*a)->DeSelect();
        (*a)->Dirty(0);
        if((*a)->_flags & BRSH_NEW){
            delete *a;
        }
    }
    _pSelBrushes.clear();
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DeleteUncomittedNewBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes, a){
        if((*a)->_flags & BRSH_NEW){
            delete *a;
            a = _pSelBrushes.erase(a)-1;
        }
    }
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DeselectPolys()
{
    FOREACH(PPolys,  _pSelPolys, a)
    (*a)->DeSelect();
    _pSelPolys.clear();
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DeselectAll()
{
    DeselectPolys();
    DeselectBrushes();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnFileSave()
{
    if(GetTitle() == "Untitled")
        OnFileSaveAs();
    else{
        SaveInFile(GetTitle());
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnFileSaveAs()
{
    LPTSTR lpszPathName = NULL;
    char szFilters[] = "Geometry (*.myg)|*.myg|Hsr Files (*.cg)|*.cg|Compiled Files (*.ccg;*.ccg)|*.ccg; *.ccg|All Files (*.*)|*.*||";

    // Prompt the user for a filename
    CFileDialog dlg(FALSE, _T(".myg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    dlg.m_ofn.lpstrTitle = _T("Select Geometry File");
    CString PathString = AfxGetApp()->GetProfileString( _T("DIR"), _T("MY_GEOM") );
    dlg.m_ofn.lpstrInitialDir = PathString;
    if (dlg.DoModal() != IDOK) return;
    SaveInFile(dlg.m_ofn.lpstrFile);
    SetTitle(dlg.m_ofn.lpstrFile);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnEditPaste()
{
    DeselectAll();
    CZ_ed2View* pTX = VIEW2D(1,0);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,1);
    CView* pv = FRM()->GetActiveView();
    V3 depl = (pTY->_shiftx, pTZ->_shifty, -pTY->_shifty);

    if(_pClipboard.size()){
        FOREACH(PBrushes, _pClipboard, ppb){
            Brush* pB     = *ppb;
            Brush* pbNew  = new Brush(*pB);
            pbNew->_flags |=BRSH_NEW;
            pbNew->Recalc();
            SelectBrush(pbNew);
        }
    }
    _brmode = BR_MOVE;
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBushowback()
{
    _vmodeback=!_vmodeback;
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBushowback(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_vmodeback);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBuwireframe()
{
    _vmodefire=!_vmodefire;
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBuwireframe(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_vmodefire);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DeleteContents()
{
    DeselectAll();
    _scene.Clear();
    CDocument::DeleteContents();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBushowcsg()
{
    DeselectAll();
    _selmode = SEL_NONE;
    _bShowCsg=!_bShowCsg;
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateBushowcsg(CCmdUI* pCmdUI)
{
    if(_scene.GetBrushes()->size()==0){
        pCmdUI->SetCheck(0);
        pCmdUI->Enable(FALSE);
    }
    else
        pCmdUI->SetCheck(_bShowCsg);
}

//---------------------------------------------------------------------------------------
void	CZ_ed2Doc::Recut()
{
    AfxGetApp()->DoWaitCursor(1);
    CZ_ed2View* pTZ   = VIEW2D(1,1);
    int         nCount = 0;
    Brush** ppb = GetSelBrushes(nCount);
    pTZ->UpdateCutsCollisionList(ppb, nCount);
    SBT(ID_SEPARATOR,MKSTR("Scene has %d Polygons", _scene.GetPolyCnt()));
    AfxGetApp()->DoWaitCursor(0);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(HasSomeSelected());
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateCutBrush(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_bCutOn);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateCut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(HasSomeSelected());
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pClipboard.size()>0);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateDelete(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(HasSomeSelected());
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnEscape()
{
    DeleteUncomittedNewBrushes();
    UndoUncomittedBrushes();
    DeselectAll();
    _pClipboard.clear();
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnEditCopy()
{
    DeleteUncomittedNewBrushes();
    int nCount = 0;
    Brush** ppb = GetSelBrushes(nCount);
    if(nCount==0){
        Beep(800,100);
        return ;
    }
    _pClipboard = _pSelBrushes;
    DeselectAll();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnCut()
{
    OnEditCopy() ;
    OnDelete();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnDelete()
{
    DeleteUncomittedNewBrushes();
    if(_pSelBrushes.size()){
        _pSelBrushes.clear();
        if(HasForms())
        {
            PBrushes* pBSs = GetForms();
            FOREACH(PBrushes, (*pBSs), ppB){
                if((*ppB)->IsSelected())
                {
                    if((*ppB)->IsSolid())
                    {
                        FOREACH(PBrushes, (*ppB)->_pCutBrshes, ppCB){
                            (*ppCB)->RemoveBrush((*ppB));
                        }
                        delete (*ppB);
                        ppB = pBSs->erase(ppB)-1;
                    }
                    else{
                        FOREACH(PBrushes, (*ppB)->_pSldBrushes, ppSB){
                            _pSelBrushes.push_back(*ppSB);
                            (*ppSB)->RemoveCutter((*ppB));
                        }
                        delete (*ppB);
                        ppB = pBSs->erase(ppB)-1;
                    }
                }
            }
            TRACEX("brsize = %d \r\n", pBSs->size());
            Recut();
            DeselectAll();
        }
    }
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnEnter()
{
    if(0==_pSelBrushes.size())
        return;

    FOREACH(PBrushes,  _pSelBrushes, ppb){
        if((*ppb)->_flags & BRSH_NEW)    
        {
            (*ppb)->_flags &= ~BRSH_NEW;
            (*ppb)->Dirty(0);
            (*ppb)->Recalc();
            _scene.AddPrimitive((*ppb));
        }
        else if((*ppb)->IsDirty()){
            (*ppb)->Recalc();
        }
    }
    Recut();
    DeselectBrushes();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateEnter(CCmdUI* pCmdUI)
{
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::SelectBrush(Brush* pB)
{
    PBrushes::iterator tf = find(_pSelBrushes.begin(),_pSelBrushes.end(),pB);
    if(tf == _pSelBrushes.end()){
        pB->Select();
        _pSelBrushes.push_back(pB);
    }
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DeSelectBrush(Brush* pB)
{
    PBrushes::iterator tf = find(_pSelBrushes.begin(),_pSelBrushes.end(),pB);
    if(tf != _pSelBrushes.end())
    {
        pB->DeSelect();
        pB->Dirty(0);
        _pSelBrushes.erase(tf);
    }

}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::OnStartMoveRotScale()
{
    FOREACH(PBrushes,  _pSelBrushes, ppb){
        (*ppb)->_t = V0;
        (*ppb)->_r = V0;
        (*ppb)->_s = V1;
        (*ppb)->Dirty(0);
    }
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::OnFinishMoveRotScale(int dirty)
{
    if(dirty == 0)
        return;
    int is = 0;
    int id = 0;
    FOREACH(PBrushes,  _pSelBrushes, ppb){
        if((*ppb)->_flags & BRSH_NEW)
            continue;
        (*ppb)->Recalc();
        is++;              
    }                      
    if(is){
        Recut();
    }
    if(id){
        Invalidate();
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnEditUndo()
{
    OnEscape();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(1);
}


//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnTabPrevsel()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(-1, 0);				//walk backward
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnTabNextsel()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(1, 0);				//walk forwards
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnTabSelnext()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(1, 1);				//walk forwards
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnTabSelprev()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(-1, 1);				//walk forwards
}

//---------------------------------------------------------------------------------------
void	 CZ_ed2Doc::SelectTabBrush(int direction, BOOL cumulate)
{
    if(_selmode == SEL_BRUSH){
        if(_pSelBrushes.size()){
            Brush* pB;

            if(direction>0)
                pB = _pSelBrushes.back();
            else
                pB = _pSelBrushes[0];
            PBrushes::iterator fb = find(_scene.GetPrimitives()->begin(),
                                         _scene.GetPrimitives()->end(),pB);
            ASSERT(fb != _scene.GetPrimitives()->end());
            if(direction>0){
                fb++;
                if(fb == _scene.GetPrimitives()->end())
                    fb=_scene.GetPrimitives()->begin();
            }
            else{
                if(fb == _scene.GetPrimitives()->begin())
                {
                    fb = _scene.GetPrimitives()->end();
                }
                fb--;
            }
            if(cumulate==0){
                DeselectBrushes();
            }
            SelectBrush(*fb);
        }
    }
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnSelectall()
{
    if(_selmode == SEL_NONE)
        return;
    if(_selmode == SEL_BRUSH){
        OnEscape();
        PBrushes* rbs = _scene.GetPrimitives();
        FOREACH(PBrushes, (*rbs), ppB){
            SelectBrush(*ppB);
        }
    }
    Invalidate();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DisableAll(BOOL bd)
{
}

//---------------------------------------------------------------------------------------
// more brushes. the other operations can be done by acting on specific brush
void    CZ_ed2Doc::UndoUncomittedBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes, ppb){
        if((*ppb)->IsDirty()){
            (*ppb)->Move(-(*ppb)->_t);
        }
        (*ppb)->Recalc();
        (*ppb)->Dirty(0);
    }
    Recut();
}

//---------------------------------------------------------------------------------------
// Do hidden surface removal. (Unions the Solids between them removing any polygon 
// fragments ending up in a solid space)
void CZ_ed2Doc::DoHSR()
{
    _compiler.PerformHSR(_scene);
}

