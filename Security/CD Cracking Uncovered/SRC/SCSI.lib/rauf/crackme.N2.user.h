#ifndef __CRACKME_N2_USER_H__
#define __CRACKME_N2_USER_H__

#ifdef __CONSOLE__
	#include "crackme.N2.txt.con.h"
#else
	#include "crackme.N2.txt.win.h"
#endif

_pm_scan()
{
#ifdef __CONSOLE__
	printf(sSCAN);
#else
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sSCAN);
#endif
	return 0;	
}

_pm_print_wait()
{
#ifdef __CONSOLE__
	printf(sWAIT);
#else
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sWAIT);
#endif
	return 0;
}

_pm_print_TOC_OK()
{
	printf("\r"sWAIT sREADY_OK); return 0;
}

_ICQ_display_progress(int from, int cur, int to)
{
	int a;
	a = 100*(cur-from)/((to-from)?(to-from):1);
#ifdef __CONSOLE__
	printf(".");
#else
	AfxGetMainWnd()->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, a, 0);
#endif
	
	return  a; 
}

_pm_print_ICQ()
{
	#ifdef __CONSOLE__		
	printf(sICQ);
	#else
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sICQ);
	#endif
	
	return 0;
	
}


_pm_print_OK()
{
	printf(sREADY_OK); return 0;
}


_pm_print_not_found()
{
#ifdef __CONSOLE__
	printf(sNOT_FOUND);
#else
	MessageBox(0, sNOT_FOUND, 0, 0);
#endif
	 return 0;
}

_pm_print_not_support()
{
	printf(sNOT_SUPPORT); return 0;
}

_pm_print_not_ready()
{
#ifdef __CONSOLE__
	printf("\r"sWAIT sNOT_READY_ERR);return 0;
#else
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sWAIT sNOT_READY);
#endif
}

_pm_print_hello_hacker()
{
#ifdef __CONSOLE__
	printf("\r" sICQ sICQ_NOT);
#endif
	return 0;	
}

_pm_print_hello_legal_user()
{
#ifdef __CONSOLE__
	printf("\r" sICQ sICQ_OK);
#endif
	return 0;	
}

_pm_print_select(char *CD)
{
#ifdef __CONSOLE__
	printf("%s%s\n",sSELECT, CD);
#else

#endif
	return 0;
}


#endif //__CRACKME_N2_USER_H__