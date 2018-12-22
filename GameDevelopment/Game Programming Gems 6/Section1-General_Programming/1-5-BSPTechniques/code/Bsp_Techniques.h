#ifndef __BSP_TCH_H__
#define __BSP_TCH_H__

#include "resource.h"		

class CBsp_TechniquesApp : public CWinApp
{
public:
	CBsp_TechniquesApp();
public:
	BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
extern CBsp_TechniquesApp theApp;

#endif // 
