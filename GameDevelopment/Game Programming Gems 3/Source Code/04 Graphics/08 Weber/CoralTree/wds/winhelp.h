/*
**  Bristol Technology Incorporated
**  241 Ethan Allen Highway, Ridgefield, Connecticut 06877
**
**  Copyright (c) 1990-1996 Bristol Technology Inc.
**  Property of Bristol Technology Inc.
**  All rights reserved.
**
**  File:         WinHelp.h
**
**  Description:  Defines for WinHelp.c
**                HyperHelp 5.1
**
*/

#ifndef WIN_HELP_INCLUDED
#define WIN_HELP_INCLUDED

#include <X11/Xlib.h>

/* Commands to pass WinHelp() */
#define HELP_CONTEXT		0x0001	/* Display topic in ulTopic */
#define HELP_QUIT		0x0002	/* Terminate help */
#define HELP_INDEX		0x0003	/* Display index (Kept for compatibility) */
#define HELP_CONTENTS		0x0003	/* Display index */
#define HELP_HELPONHELP		0x0004	/* Display help on using help */
#define HELP_SETINDEX		0x0005	/* Set the current Index for multi index help */
#define HELP_SETCONTENTS	0x0005	/* Set the current Index for multi index help */
#define HELP_CONTEXTPOPUP	0x0008
#define HELP_FORCEFILE		0x0009
#define HELP_FINDER		0x000B	/*
					 * Displays the Help Topics dialog. HyperHelp 
                                         * ignores the dwData parameter. 
					 */

#define HELP_KEY		0x0101	/* Display topic for keyword in offabData */
#define HELP_MULTIKEY   	0x0201  /* Displays the Help topic identified by
					   keyword in an alternate key word table. */
#define HELP_COMMAND		0x0102 
#define HELP_PARTIALKEY		0x0105
#define HELP_SETWINPOS 		0x0203

/* bristol extensions */
#define HELP_MINIMIZE		0x1000
#define HELP_MAXIMIZE		0x1001
#define HELP_RESTORE		0x1002
#define HELP_FORCE_QUIT		0x1003	/* Terminate help (even if in viewmode) */
#define	HELP_SET_WINDOWID	0x1004

/* values used for tcard functionality */
#define HELP_TCARD              0x8000	/* OR this in with a help command to direct it to the */
					/* training card instance of the viewer */
#define	IDOK			0x0001	/* HyperHelp executed a TCard macro, with IDOK as argument */
#define	IDCANCEL		0x0002	/* HyperHelp executed a TCard macro, with IDCANCEL as argument */
#define	IDABORT			0x0003	/* HyperHelp executed a TCard macro, with IDABORT as argument */
#define	IDRETRY			0x0004	/* HyperHelp executed a TCard macro, with IDRETRY as argument */
#define	IDIGNORE		0x0005	/* HyperHelp executed a TCard macro, with IDIGNORE as argument */
#define	IDYES			0x0006	/* HyperHelp executed a TCard macro, with IDYES as argument */
#define	IDNO			0x0007	/* HyperHelp executed a TCard macro, with IDNO as argument */
#define	IDCLOSE			0x0008	/* HyperHelp executed a TCard macro, with IDCLOSE as argument */
#define	IDHELP			0x0009	/* HyperHelp executed a TCard macro, with IDHELP as argument */
#define HELP_TCARD_DATA         0x0010	/* HyperHelp executed a TCard macro, with a number as argument */
#define HELP_TCARD_NEXT         0x0011	/* The user clicked an authorable NEXT button */
#define HELP_TCARD_OTHER_CALLER 0x0011	/* Another application has requested training cards */

#ifdef __cplusplus
#define EXTC extern "C"
#else
#ifdef __STDC__
#define EXTC extern
#endif
#endif

#ifdef EXTC
EXTC int WinHelp (Display       *hWnd,
                  char          *lpHelpFile,
                  unsigned short wCommand,	
                  unsigned long  dwData);
#endif	/*EXTC*/


/*
 * View modes.
 */
#define VM_INDIVIDUAL_VIEWER_INSTANCE 0
#define VM_SINGLE_SERVER_MODE 1

#ifdef EXTC
EXTC void WinHelpSetViewMode(int nMode);
EXTC int HH_HandleHelpMessage(Display *display, XEvent *event, int *pAction, int *pActionData);
EXTC int HH_IsHelpMessage(XEvent *event);
#endif



/* 
 * HH comm structs
 */
#define HHATOMNAME   "HyperHelpAtomV40"
#define HHTCARDATOMNAME   "HyperHelpTCardAtomV50"

typedef struct _HHInstance {
#ifndef decaosf
/*'filler' variables needed for Alpha 64 bit compatibility running concurrent on display
*/
   int             pid;          /*Parent id*/
   int             filler1;
   unsigned long   HHWindow;     /*Filled by HH upon invocation*/
   int             filler2;
   unsigned long   ClientWindow; /*Optional (future HH to client communication link)*/
   int             filler3;
   int             bServer;      /*Viewer Mode or TCard message index */
   int             filler4;
   char            data[1024];
#else
   int             pid;          /*Parent id*/
   unsigned long   HHWindow;     /*Filled by HH upon invocation*/
   unsigned long   ClientWindow; /*Optional (future HH to client communication link)*/
   int             bServer;      /*Viewer Mode or TCard message index */
   char            data[1024];
#endif
} HHInstance_t;

#define MAX_HHINSTANCES 5
typedef struct _HHServerData {
   int                  nItems;
   struct _HHInstance   HHInstance[MAX_HHINSTANCES];
} HHServerData_t;


typedef struct tagMULTIKEYHELP {
    unsigned int mkSize;
    char mkKeylist;
    char *szKeyphrase;
} MULTIKEYHELP;

#endif	/*WIN_HELP_INCLUDED*/
