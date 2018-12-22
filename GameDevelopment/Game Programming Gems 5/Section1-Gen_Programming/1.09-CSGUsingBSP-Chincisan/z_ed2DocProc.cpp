/*
	Csg by Bsp.
	Author:			Marius Chincisan, 
	Developed for	Game Programming Gems 5
*/
#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "mainfrm.h"
#include "Z_ed2View.h"
#include "Z_ed3View.h"
#include "baseutils.h"
#include <algorithm>
#include "BspTree.h"

//---------------------------------------------------------------------------------------
#define VIEW2D(x_,y_) (CZ_ed2View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)
#define VIEW3D(x_,y_) (z_ed3View*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(x_,y_)

//---------------------------------------------------------------------------------------
// make csg
void CZ_ed2Doc::DoHSR()
{
    _compiler.PerformHSR(_scene);
    if(_scene.GetBrushes()->size())
    {
        CString cs = GetTitle();
        cs+=".cg";
        SaveInFile(cs,TRUE);
        AfxGetApp()->DoWaitCursor(0);
    }
}

//---------------------------------------------------------------------------------------
LRESULT CZ_ed2Doc::OnCompileDone(WPARAM wErr, LPARAM pStr)
{
    return 0;
}

//---------------------------------------------------------------------------------------
void    CZ_ed2Doc::DrawCompiledStates(z_ed3View* pV)
{
    _compiler.Render(pV,0xFF);
}
