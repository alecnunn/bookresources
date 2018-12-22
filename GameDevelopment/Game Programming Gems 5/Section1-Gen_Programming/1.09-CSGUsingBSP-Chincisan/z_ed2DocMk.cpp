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
#include "DlgNewBrush.h"


//---------------------------------------------------------------------------------------
#define VIEW2D(x_,y_) (CZ_ed2View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)
#define VIEW3D(x_,y_) (z_ed3View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)
static int	GID=0;

//---------------------------------------------------------------------------------------
// Set the flags from setting dialog into the brush
void CZ_ed2Doc::ApplyFlags(Brush* pBrush)
{
    CZ_ed2View* pTX = VIEW2D(1,1);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,0);
    z_ed3View*  pT3 = VIEW3D(0,0);

    pBrush->Move(V3(pTY->_shiftx, pTZ->_shifty, -pTY->_shifty));
    pBrush->Recalc();
    if(_cachedSett._isSolid){
        pBrush->_flags |= BRSH_SOLID;
    }
    else{
        pBrush->FlagFaces(BACK_CUT_POLY);
        if(_cachedSett._cutAll)
        {
            pBrush->_flags |= BRSH_CUTALL;
        }
    }

    if(_cachedSett._splitable)
        pBrush->_flags |= BRSH_DETAIL;

    if(_cachedSett._reversed){
        pBrush->_flags |= BRSH_REV;
        pBrush->Reverse();
    }
    pBrush->_flags |= BRSH_NEW;
    FOREACH(vvector<Poly>,pBrush->_polys,pPoly){
        Poly& poly = *pPoly;
    }
    _pSelBrushes.push_back(pBrush); //place in sel brushes witht eh new flag set
    Invalidate();
    SBT(0,"Press 'Enter' To Commit!")  ;
}


//---------------------------------------------------------------------------------------
// makes a box 
void CZ_ed2Doc::OnMkBox()
{
    OnEscape() ;                    
    _newBrshDlg._brshSett=_cachedSett;
    int rez = _newBrshDlg.DoModal();
    if(rez==IDCANCEL)
        return;
    _cachedSett = _newBrshDlg._brshSett;
    _selmode = SEL_BRUSH;

    CZ_ed2View* pTX = VIEW2D(1,1);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,0);
    V3	boxdim = V3(  pTZ->_vport.right-pTX->_vport.left,
                     pTX->_vport.bottom-pTZ->_vport.top,
                     pTX->_vport.right-pTX->_vport.left);
    boxdim *= .3f;
    Brush*    pBrush   = new Brush();
    pBrush->MakeCube(boxdim, _cachedSett._tm);

    if(_cachedSett._thikness != 0){
        pBrush->StartSecGrp(_cachedSett._thikness);
        V3 intBox       = boxdim - _cachedSett._thikness;
        Brush	 b2;
        b2.MakeCube(intBox, _cachedSett._tm);
        b2.Reverse();
        FOREACH(Polys,b2._polys,pp){
            pp->_pBrush = pBrush;
            pBrush->_polys.push_back(*pp);
        }
    }
    ApplyFlags(pBrush);
    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._flags   = pBrush->_flags;
    pBrush->_cf._hstrips = 0;
    pBrush->_cf._vstrips = 0;
    pBrush->_cf._thick   = _cachedSett._thikness;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::MkCylinder()
{
    OnEscape() ;                    
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=8;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;
    _newBrshDlg._brshSett = _cachedSett;
    if(IDCANCEL == _newBrshDlg.DoModal())
        return;
    _cachedSett = _newBrshDlg._brshSett;
    _selmode = SEL_BRUSH;
    if(_cachedSett._radstrips<3 || _cachedSett._vstrips < 1 ||
            _cachedSett._radstrips>90 || _cachedSett._vstrips>16){
        AfxMessageBox("Cannot create a cylinder with provided parameters",MB_OK|MB_ICONINFORMATION);
        return;
    }

    CZ_ed2View* pTX = VIEW2D(1,1);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,0);
    V3	boxdim = V3(  pTZ->_vport.right-pTX->_vport.left,
                     pTX->_vport.bottom-pTZ->_vport.top,
                     pTX->_vport.right-pTX->_vport.left);
    boxdim *= .3f;
    REAL    rad        = min(boxdim.x,boxdim.z) / 2;
    REAL    height     = boxdim.y;
    Brush* pBrush = new Brush();
    pBrush->MakeCylinder(rad, height, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);
    if(_cachedSett._thikness != 0){
        pBrush->StartSecGrp(_cachedSett._thikness);

        Brush tb;
        tb.MakeCylinder(rad-_cachedSett._thikness,
                        height-_cachedSett._thikness, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);
        tb.Reverse();
        FOREACH(Polys,tb._polys,pp){
            pp->_pBrush = pBrush;
            pBrush->_polys.push_back(*pp);
        }
    }
    ApplyFlags(pBrush);
    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._flags   = pBrush->_flags;
    pBrush->_cf._hstrips = _cachedSett._radstrips;
    pBrush->_cf._vstrips = _cachedSett._vstrips;
    pBrush->_cf._thick   = _cachedSett._thikness;
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnBucone()
{

    OnEscape() ;    
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=8;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;
    _newBrshDlg._brshSett = _cachedSett;
    if(IDCANCEL == _newBrshDlg.DoModal())
        return;
    _cachedSett = _newBrshDlg._brshSett;
    _selmode = SEL_BRUSH;

    if(_cachedSett._radstrips<3 || _cachedSett._vstrips < 1 ||
            _cachedSett._radstrips>90 || _cachedSett._vstrips>16){
        AfxMessageBox("Cannot create a cone with provided parameters",MB_OK|MB_ICONINFORMATION);
        return;
    }

    CZ_ed2View* pTX = VIEW2D(1,1);
    CZ_ed2View* pTY = VIEW2D(0,1);
    CZ_ed2View* pTZ = VIEW2D(1,0);

    V3	boxdim = V3(  pTZ->_vport.right-pTX->_vport.left,
                     pTX->_vport.bottom-pTZ->_vport.top,
                     pTX->_vport.right-pTX->_vport.left);
    boxdim *= .3f;

    REAL    rad        = min(boxdim.x,boxdim.z) / 2;
    REAL    height     = boxdim.y;

    Brush* pBrush = new Brush();
    pBrush->MakeCone(rad, height, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);
    if(_cachedSett._thikness != 0){
        pBrush->StartSecGrp(_cachedSett._thikness);

        Brush tb;
        tb.MakeCone(rad-_cachedSett._thikness,
                    height-_cachedSett._thikness, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);
        tb.Reverse();
        FOREACH(Polys,tb._polys,pp){
            pp->_pBrush = pBrush;
            pBrush->_polys.push_back(*pp);
        }
    }
    ApplyFlags(pBrush);

    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._flags   = pBrush->_flags;
    pBrush->_cf._hstrips = _cachedSett._radstrips;
    pBrush->_cf._vstrips = _cachedSett._vstrips;
    pBrush->_cf._thick   = _cachedSett._thikness;

}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::OnFileOpen()
{
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::SaveInFile(LPCTSTR pSzName, BOOL bBrshes)
{
}
