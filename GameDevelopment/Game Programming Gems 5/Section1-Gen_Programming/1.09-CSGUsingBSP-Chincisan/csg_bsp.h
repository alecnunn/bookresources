/*
	Csg by Bsp.
	Author:			Marius Chincisan, 
	Developed for	Game Programming Gems 5
*/

#ifndef __CSG_BSP_H__
#define __CSG_BSP_H__

#include "resource.h"       // main symbols

#define WM_REDRAW   WM_USER+1000

//---------------------------------------------------------------------------------------
// Win App class
class CZ_ed2App : public CWinApp
{
public:
    CZ_ed2App();
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
    void Dbg_ResolveCurDir();
};

#endif // !__CSG_BSP_H__
