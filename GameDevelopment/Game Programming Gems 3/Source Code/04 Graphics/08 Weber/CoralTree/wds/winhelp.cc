/*
**  Bristol Technology Incorporated
**  241 Ethan Allen Highway, Ridgefield, Connecticut 06877
**
**  Copyright (c) 1990-1996 Bristol Technology Inc.
**  Property of Bristol Technology Inc.
**  All rights reserved.
**
**  File:         WinHelp.c
**
**  Description:  Invokes HyperHelp application and passes optional data
**                indicating the nature of the help requested by the
**                application.  Version 5.1 of HH.
**
*/

// use osd for thread-safe, Jason Weber 980331
#include "osd/system.h"
// mod with ifndef, Jason Weber 971023
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __vms
#include <sys/wait.h>
#else
#include <processes.h>
#include <unixio.h>
#include <unixlib.h>
#if !(defined(__DECC_VER) && (__DECC_VER >= 50090003))
typedef int pid_t;
#endif
#endif /* __vms */

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include "winhelp.h"

/* Change "#undef" to "#define" if your prototype for osdMemcpy() expcets 
 * K&R "char *" instead of ANSI "void *".  Sun4 platfoms have this problem.
 */
#undef BAD_MEMCPY_PROTO
#ifdef BAD_MEMCPY_PROTO
#include <stdlib.h> /* osdMemcpy() proto */
/* osdMemcpy() takes "char *" instead of ANSI "void *" */
typedef char * (*_sun4_osdMemcpy_fp_t)(char *, const char *, int);
static _osdMemcpy_fp_t _sun4_osdMemcpy_fp=osdMemcpy;
#define osdMemcpy(to,fm,sz) (void *)((*_osdMemcpy_fp)((char *)(to), (char *)(fm), (int)(sz)))
#endif /* BAD_MEMCPY_PROTO */


#ifndef __cplusplus
#ifndef __STDC__
#define _USING_KR_C_
#endif
#endif

/* 
 * Prototypes 
 */
#ifndef _USING_KR_C_
   static void childend(int);
   static int SendEvent(Display *, char *, pid_t, pid_t);
   static int CheckServer(Display *);
   static void ChangeProperty(Display *, int);
   static void SetupCommunications(Display *, pid_t);
   static void MakeHOHPath(char *, char *);
   static void setup_tcard_atom(Display *, unsigned long w);
   static void remove_tcard_atom_entry(Display *, int, Window);
#else
   static void childend();
   static int SendEvent();
   static int CheckServer();
   static void ChangeProperty();
   static void SetupCommunications();
   static void MakeHOHPath();
   static void setup_tcard_atom();
   static void remove_tcard_atom_entry();
#endif

   /* Used in call to waitpid() in sig handler */
   static pid_t HH_PID;

/*
 * ViewMode 
 * Default: Individual viewer instance
 *          (each application "owns" its own HyperHelp viewer) 
 *
 * Values: 0 = Individual viewer instance
 *         1 = Single server mode
 *
 * To run HyperHelp in single server mode, modify
 * the following initialization to 1.
 */
static int ViewMode = 0; 

void WinHelpSetViewMode(int nMode) {
    if (nMode == 0 || nMode == 1) {
	ViewMode = nMode;
    }
    else {
	fprintf(stderr, "WARNING: WinHelpSetViewMode() - unrecognized mode.\n");
    }
}

extern int errno;

static Atom HHTCardAtom=0;
static Window HH_ApplicationWindowId=0;
static pid_t HH_ApplicationPid=0;

/**************************************************************
 * Function: WinHelp()
 * Purpose:  Pass a command to the HyperHelp viewer.
 *           Invokes the HyperHelp executable if not already
 *           running.
 *
 * Parameters: 
 * hWnd        The display pointer used to specify a connection
 *             to the server.
 * lpHelpFile  Name (with optional directory path) of the help
 *             help file containing the desired topic.
 * wCommand    Specifies the action type to be taken by the 
 *             viewer.  Consult the manual for types.
 * dwData      Integer value indicating context of the topic.
 *             also used as a string by some wCommand types.
 *
 * Return Values:
 *    Returns 1 if successful, 0 otherwise.
 *
 ****************************************************************/ 
#ifdef _USING_KR_C_
int WinHelp(hWnd, lpHelpFile, wCommand, dwData)
   Display        *hWnd;
   char           *lpHelpFile;
   unsigned short  wCommand;
   unsigned long   dwData;
#else
int WinHelp(Display        *hWnd,
            char           *lpHelpFile,
            unsigned short  wCommand,
            unsigned long   dwData)
#endif /*_USING_KR_C_*/
{
   static pid_t pid = 0, parentPid=0;
   char         path[1024], hohpath[1024], sComm[128];
   int          nSendEventResult;
   int          nexeclresult=0;
   char        *pHHHOME;
   char		szKeywordTable[2];
   int my_own_pid;
   int tc_command = 0;
   char szMyPid[32];
   char szMyWindowid[32];

   my_own_pid = getpid();

   /* Check for Display Pointer */
   if (hWnd == NULL) {
       hWnd = XOpenDisplay(NULL);
/*      fprintf(stderr, "HyperHelp Error: hWnd passed as NULL to WinHelp");
      return(0); */
   }	

   /* Force an X update */
   XFlush(hWnd);

   /* Set up signal handler for SIGCHLD children signals */
   /* Allows children to die without becoming zombies    */
   /* Handling of this seems to vary across platforms    */
#ifndef __vms
    signal(SIGCHLD, childend); 
#endif

   /* Get HHHOME value (used to find hyperhelp and hoh.hlp) */
   pHHHOME=getenv("HHHOME");

   /* Check if this command is meant for the training card viewer instance (do not move 
      this set of commands to a later part of this function, because they modify the wCommand
      value). */
   if (wCommand & HELP_TCARD) {
     wCommand = wCommand & (~HELP_TCARD);	/* take the HELP_TCARD bit off the command */
     tc_command = 1;				/* record that we are dealing with the tcard instance */
     setup_tcard_atom(hWnd, HH_ApplicationWindowId);	/* make sure the atom is set up */
     HH_ApplicationPid = getpid();		/* record our application process id */
   }

   if ((dwData == (unsigned long) NULL) && (wCommand == HELP_COMMAND))
      wCommand = HELP_INDEX;

   /* Check diagnostics if in Server view mode */
   if (ViewMode == 1) {
      parentPid = CheckServer(hWnd);
      /*Checks if server is available */
      if (parentPid) {
         pid = 1;
      }
      else pid = 0;
   }

   switch (wCommand) {
      case HELP_MINIMIZE:
      case HELP_MAXIMIZE:
      case HELP_RESTORE:
         /* Not currently supported */
         return (0);
      case HELP_SET_WINDOWID:	
	/* record the window id of the application for use in tcard messages */
         	HH_ApplicationWindowId = dwData;
         	return(1);
      case HELP_QUIT:
         remove_tcard_atom_entry(hWnd, HH_ApplicationPid, HH_ApplicationWindowId);
         if (ViewMode==1) {
            return (0); /* Server HH can only be killed by user*/
	 }
      case HELP_FORCE_QUIT:
	  remove_tcard_atom_entry(hWnd, HH_ApplicationPid, HH_ApplicationWindowId);
	  if (wCommand == HELP_FORCE_QUIT) {
	      if (ViewMode == 1) {
		  pid = HH_PID;
	      }
	  }
	  if (pid) {
	      if (pid != -1)
		  kill(pid,SIGTERM);
	      XGrabServer(hWnd);
	      ChangeProperty(hWnd, parentPid);
	      XUngrabServer(hWnd);
	      pid = 0;
	      /*Make sure HH Viewer gets this window event*/
	      XFlush(hWnd);
	  }
	  return (1);
	 
      case HELP_TCARD:
      case HELP_CONTEXT:
      case HELP_COMMAND:
      case HELP_INDEX:
      case HELP_KEY:
      case HELP_PARTIALKEY:
      case HELP_HELPONHELP:
      case HELP_MULTIKEY:
      case HELP_FINDER:
       	/* If viewer not up, exec it */
        if (!pid) {
            char        sContext[10];
            char        sMacro[256];
            char       *pHHPATH;
	    int         bFound;
            char        FullCommand[6];
            char        ParentID[15];
            int         fd[2];
            struct stat xFileStat;

            if (!parentPid)
               parentPid = getpid();

            SetupCommunications(hWnd,parentPid);
            FullCommand[0]='\0';
            sprintf(FullCommand,"-s %d",ViewMode);
            ParentID[0]='\0';
            sprintf(ParentID,"-d %d",parentPid);

            /* Create path to viewer */
            pHHPATH=getenv("HHPATH");
	    bFound = -1;

	    if (pHHHOME) {
#ifndef __vms
	       sprintf(path, "%s/bin/hyperhelp", pHHHOME);
#else
	       /* HHHOME is a rooted logical */
	       sprintf(path, "HHHOME:[BIN]HYPERHELP.EXE");
#endif
	       bFound = stat(path, &xFileStat);
            }
            if (bFound == -1 && pHHPATH) {
#ifndef __vms
                sprintf(path, "%s/hyperhelp", pHHPATH);
#else
		/* HHPATH logical is a search list */
                sprintf(path, "HHPATH:HYPERHELP.EXE", pHHPATH);
#endif
	        bFound = stat(path, &xFileStat);
	    }
	    if (bFound == -1) {
#ifndef __vms
	        sprintf(path,"/usr/bin/X11/hyperhelp");
#else
	        sprintf(path, "HYPERHELP_DIR:[BIN]HYPERHELP.EXE");
#endif
                bFound = stat(path, &xFileStat);
	    }
	    if (bFound == -1) {
               /* viewer executable not found */
               fprintf(stderr,"HyperHelp Error:  Could not find HyperHelp executable.\n");
               fprintf(stderr,"Check that \"HHHOME\" "
#ifndef __vms
		       "environment variable"
#else
		       "logical"
#endif
		       " is set.\n");
               return (0);
            }
	
            sprintf(szMyPid, "%d", my_own_pid);
            sprintf(szMyWindowid, "%d", (int)HH_ApplicationWindowId);

            /* Fork process */
#ifndef __vms
            pipe(fd);
            if ((pid = fork()) == 0) {
               /* child doesn't need to read pipe */
               close(fd[0]);
               /* set close on exec flag */
               fcntl(fd[1], F_SETFD, 1);
#else
            if ((pid = vfork()) == 0) {
#endif

               switch (wCommand) {
                  case HELP_CONTEXT:
                     sprintf(sContext, "%d", (int)dwData);
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-tcard", szMyPid, szMyWindowid,
                                          "-c", sContext, FullCommand, ParentID, 
                                          "-display", DisplayString(hWnd), "-winhelp", 0); 
                     else
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-c", sContext, FullCommand, ParentID, 
                                          "-display", DisplayString(hWnd), "-winhelp", 0); 
                     break;
                  case HELP_INDEX:
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-tcard", szMyPid, szMyWindowid,
                                          FullCommand, ParentID, "-display", 
                                          DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          FullCommand, ParentID, "-display", 
                                          DisplayString(hWnd), "-winhelp", 0);
                     break;
                  case HELP_FINDER:
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-tcard", szMyPid, szMyWindowid,
                                          "-f", FullCommand, ParentID, "-display", 
                                          DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-f", FullCommand, ParentID, "-display", 
                                          DisplayString(hWnd), "-winhelp", 0);
                     break;
                 case HELP_KEY:
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-tcard", szMyPid, szMyWindowid,
                                          "-k", dwData, FullCommand, ParentID,
                                          "-display", DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-k", dwData, FullCommand, ParentID,
                                          "-display", DisplayString(hWnd), "-winhelp", 0);
                     break;
                 case HELP_PARTIALKEY:
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-tcard", szMyPid, szMyWindowid,
                                          "-p", dwData, FullCommand, ParentID,
                                          "-display", DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path, "hyperhelp", lpHelpFile,
                                          "-p", dwData, FullCommand, ParentID,
                                          "-display", DisplayString(hWnd), "-winhelp", 0);
                     break;
                 case HELP_COMMAND:
                     sprintf(sMacro,"%s",(char *)(dwData));
                     if (tc_command)
                             nexeclresult = execl(path,	"hyperhelp", lpHelpFile,
                                         "-tcard", szMyPid, szMyWindowid,
                                         "-m", sMacro, FullCommand, ParentID,
                                         "-display", DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path,	"hyperhelp", lpHelpFile,
                                         "-m", sMacro, FullCommand, ParentID,
                                         "-display", DisplayString(hWnd), "-winhelp", 0);
                     break;
                 case HELP_MULTIKEY:
		     szKeywordTable[0] = ((MULTIKEYHELP *)dwData)->mkKeylist;
		     szKeywordTable[1] = (char) 0;
		     
                     if (tc_command)
        		     nexeclresult = execl(path,	"hyperhelp", lpHelpFile,
                                                  "-tcard", szMyPid, szMyWindowid,
        					  "-w", szKeywordTable,
        					  ((MULTIKEYHELP *)dwData)->szKeyphrase,
        					  FullCommand, ParentID,
        					  "-display", DisplayString(hWnd), "-winhelp", 0);
                     else
        		     nexeclresult = execl(path,	"hyperhelp", lpHelpFile,
        					  "-w", szKeywordTable,
        					  ((MULTIKEYHELP *)dwData)->szKeyphrase,
        					  FullCommand, ParentID,
        					  "-display", DisplayString(hWnd), "-winhelp", 0);
		     
		     break;
		    
                 case HELP_HELPONHELP:
		     MakeHOHPath(pHHHOME, hohpath);
                     if (tc_command)
                             nexeclresult = execl(path, "hyperhelp", hohpath, 
                                            "-tcard", szMyPid, szMyWindowid,
                                            FullCommand, ParentID, "-display", 
                                            DisplayString(hWnd), "-winhelp", 0);
                     else
                             nexeclresult = execl(path, "hyperhelp", hohpath, 
                                            FullCommand, ParentID, "-display", 
                                            DisplayString(hWnd), "-winhelp", 0);
                     break;
               }

               /* if exec successful, we won't get here */
#ifndef __vms
               write(fd[1], &errno, sizeof(errno));
               close(fd[1]);
#endif
               exit(EXIT_FAILURE);
            } /* End of forked proc */

#ifndef __vms
            close(fd[1]); /* parent doesn't need to write to pipe */
#endif
            if (pid == -1)
               perror("process fork failed");
#ifndef __vms
            if (read(fd[0], &errno, sizeof (errno)) == sizeof(errno)) {
               close(fd[0]);
               perror("Error starting hyperhelp.");
               return(0);
            }
            close(fd[0]);
#endif
            /* This would be the parent process; child spawned
               viewer already*/
            /* Save HyperHelp Viewer's pid */
            HH_PID = pid;
            XFlush(hWnd);
            return(1);
         }
         else {
            /* Viewer is already up */

            switch (wCommand) {
               case HELP_CONTEXT:
                  sprintf(sComm, "-c %d %s\n", (int)dwData, lpHelpFile);
                  break;
               case HELP_INDEX:
                  sprintf(sComm, "-i %s\n", lpHelpFile);
                  break;
               case HELP_FINDER:
                  sprintf(sComm, "-f %s\n", lpHelpFile);
                  break;
               case HELP_KEY:
		  /* Quote Keyword to make parsing easier */
                  sprintf(sComm, "-k %s %s\n", (char *)dwData, lpHelpFile);
                  break;
               case HELP_PARTIALKEY:
                  sprintf(sComm, "-p %s %s\n", (char *)dwData, lpHelpFile);
                  break;
               case HELP_COMMAND:
                  sprintf(sComm, "-m %s\n", (char *)dwData); 
                  break;
		  
               case HELP_MULTIKEY:
		  sprintf(sComm, "-w %c %s %s\n",
			  ((MULTIKEYHELP *)dwData)->mkKeylist,
			  ((MULTIKEYHELP *)dwData)->szKeyphrase,
			  lpHelpFile);
		  break;
               case HELP_HELPONHELP:
		  MakeHOHPath(pHHHOME, hohpath);
		  sprintf(sComm, "-i %s\n", hohpath); 
		  break;
            } /* switch (wCommand) */
         }
         break;
		  
      case HELP_FORCEFILE:
      case HELP_CONTEXTPOPUP:
      case HELP_SETINDEX:
      case HELP_SETWINPOS:
         /* unimplemented */
         return (0);
      default:
         return (0);
   } /* switch (wCommand) */

   if (tc_command) {
      char buf[128];
      sprintf(buf, "-t %d %d %s", HH_ApplicationPid,(int)HH_ApplicationWindowId,
         sComm);
      strcpy(sComm, buf);
   }

   XGrabServer(hWnd);
   nSendEventResult = SendEvent(hWnd, sComm, parentPid, pid);
   XUngrabServer(hWnd);
   if (nSendEventResult < 0){
      /* Must restart new executable for whatever reason */
      pid=0;
      WinHelp(hWnd, lpHelpFile, (tc_command) ? HELP_TCARD | wCommand : wCommand, dwData);
   }
   XFlush(hWnd);
   return(1);	
}

/**************************************************************
 * Function: SetupCommunications()
 * Purpose:  Checks for the HyperHelp atom to put data up on,
 *           and attempts to splice up the HH pid & Viewer 
 *           mode on it.
 **************************************************************/
#ifdef _NO_PROTO
static
void SetupCommunications(pDisplay, pid)
   Display *pDisplay;
   pid_t pid;
#else
static void SetupCommunications(Display *pDisplay, pid_t pid)
#endif
{
   Atom           HHAtom;
   HHInstance_t   HHInstance;

   HHAtom = XInternAtom(pDisplay,HHATOMNAME,False);
   if (!HHAtom){
      return;
   }

   HHInstance.pid = pid;
   HHInstance.HHWindow = 0;
   if (ViewMode==1)
      HHInstance.bServer = 1;
   else
      HHInstance.bServer = 0;

   XChangeProperty(pDisplay, 
      RootWindow(pDisplay,DefaultScreen(pDisplay)), HHAtom, HHAtom,
      8, PropModeAppend, (unsigned char *)(&HHInstance), 
      (int)(sizeof(HHInstance_t)/*AAB/4*/ ));
}

/**************************************************************
 * Function: CheckServer()
 * Purpose:  Checks for the HyperHelp atom to put data up on,
 *           and searches for pid element.
 *           Returns pid of parent if found, 0 otherwise. 
 **************************************************************/
#ifdef _NO_PROTO
static
int CheckServer(pDisplay)
   Display *pDisplay;
#else
static int CheckServer(Display *pDisplay)
#endif
{
   Atom           HHAtom, DummyAtom;
   unsigned long  nitems, bytes_after, limit;
   int            actual_format, i, nReturn = 0;
   unsigned char *pReceived;
   HHInstance_t  *pServerData;

   HHAtom = XInternAtom(pDisplay,HHATOMNAME,False);
   if (!HHAtom){
      return(0);
   }

   XGetWindowProperty(pDisplay,
      RootWindow(pDisplay,DefaultScreen(pDisplay)),
      HHAtom, 0L, 0x0f000L, False, AnyPropertyType, &DummyAtom,
      &actual_format, &nitems, &bytes_after, &pReceived);

   pServerData = (HHInstance_t *) pReceived;
   limit = nitems/(sizeof(HHInstance_t)/*/4*/);
   for (i=0; i<(long)limit; i++) {
      if (pServerData[i].bServer) {
         nReturn = pServerData[i].pid;
         /* Save HyperHelp Viewer's pid */
         HH_PID = pServerData[i].bServer;
         break;
      }
   }
   if (pReceived)
      XFree(pReceived);
   return (nReturn);
}

/**************************************************************
 * Function: valid_process()
 * Purpose:  Determine if the given pid is valid (currently running).
 *           Needed because current VMS C RTL doesn't support 
 *           kill(pid, 0).  Also, kill() returns 0 on success, -1
 *           on failure - this routines uses C conventions and returns
 *           0 on failure, non-zero otherwise.
 **************************************************************/
#ifdef _NO_PROTO
static 
int valid_process(pid)
  pid_t pid;
#else
static int valid_process(pid_t pid)
#endif
{
#ifdef __vms
#include <jpidef.h>
#include <libdef.h>
#include <lib$routines.h>
#include <ssdef.h>

    long jpicode = JPI$_UIC;
    unsigned long vms_pid = pid, tmp;

    if (lib$getjpi(&jpicode, &vms_pid, 0, &tmp, 0, 0) == SS$_NORMAL)
#else
    if (kill(pid, 0) == 0)
#endif
      return 1;
    else
      return 0;
} /* valid_process() */

/**************************************************************
 * Function: SendEvent()
 * Purpose:  Trigger the HyperHelp viewer or server to act
 *           based on a ClientMessage event. 
 *           Returns -1 if viewer must be restarted,
 *           0 if viewer is already up, 1 if success.
 **************************************************************/
#ifdef _NO_PROTO
static
int SendEvent(pDisplay, pSendString, app_pid, hh_pid)
   Display *pDisplay;
   char    *pSendString;
   pid_t      app_pid;
   pid_t      hh_pid;
#else
static int SendEvent(Display *pDisplay,
		char	*pSendString,
		pid_t	app_pid,
		pid_t	hh_pid)
#endif
{
   XEvent           myEvent;
   Atom             HHAtom, DummyAtom;
   int              actual_format, i, limit;
   unsigned long    nitems, bytes_after;
   unsigned char   *pReceived;
   HHInstance_t    *pServerData;
   HHInstance_t    *pNewServerData=NULL;
   pid_t viewer_pid;

  HHAtom = XInternAtom(pDisplay,HHATOMNAME,False);
  if (!HHAtom){
     return (0);
  }

  XGetWindowProperty(pDisplay,
     RootWindow(pDisplay,DefaultScreen(pDisplay)),
     HHAtom, 0L, 0x0f000L, False, AnyPropertyType, &DummyAtom,
     &actual_format, &nitems, &bytes_after, &pReceived);

   if (nitems == 0) {
      if (pReceived)
         XFree(pReceived);
      return (-1);
   }

   limit = nitems/(sizeof(HHInstance_t)/*AAB/4*/);
   pServerData = (HHInstance_t *) osdCalloc(1, sizeof(HHInstance_t)*limit);
   osdMemcpy(pServerData, pReceived, sizeof(HHInstance_t)*limit);

   /* Check for pid */
   for (i=0; i<limit; i++) {
      if (app_pid == pServerData[i].pid)
         break;
   }
   if (i==limit || app_pid != pServerData[i].pid){
      /*No match found*/
      if (pReceived)
         XFree(pReceived);
      if (pServerData)
	 free(pServerData); 
      return (-1);
   }

   /* Validate HyperHelp's existence */
   if (!pServerData[i].bServer) 
     viewer_pid = hh_pid;
   else 
     viewer_pid = pServerData[i].bServer;
   if (!valid_process(viewer_pid)) {
      /* Abort! Window does not exist */
      if (pReceived)
         XFree(pReceived);

      /* Delete the property - HH never had a chance to */
      pNewServerData = (HHInstance_t *)osdCalloc(1, (int)(sizeof(HHInstance_t)*(limit-1)));
      osdMemcpy(pNewServerData, pServerData, (int)(sizeof(HHInstance_t)*i));
      if (i == 0) {
         XDeleteProperty(pDisplay, RootWindow(pDisplay, DefaultScreen(pDisplay)), 
                         HHAtom);
         XFlush(pDisplay);
      }
      else
         XChangeProperty(pDisplay, RootWindow(pDisplay, DefaultScreen(pDisplay)),
                         HHAtom, HHAtom, 8, PropModeReplace,
                         (unsigned char *)pNewServerData,
                         (int)((i*sizeof(HHInstance_t))));
      i++;
      if (i < limit) {
         osdMemcpy(pNewServerData, pServerData+i, 
                (int)(sizeof(HHInstance_t)*(limit-i)));
         XChangeProperty(pDisplay, RootWindow(pDisplay, DefaultScreen(pDisplay)),
                         HHAtom, HHAtom, 8, PropModeAppend, 
                         (unsigned char *)pNewServerData,
                         (int)(((limit-i)*sizeof(HHInstance_t))));
      }
      XFlush(pDisplay);
      if (pNewServerData)
         free(pNewServerData);
      return(-1);
  } /* !valid_process() */

   if (pServerData[i].HHWindow == 0){
      /* Window not ready to receive yet */		
      if (pReceived)
         XFree(pReceived);
      if (pServerData)
	 free(pServerData); 
      return (0);
   }

   /* Create and send event */
   strcpy(pServerData[i].data, pSendString);
   XChangeProperty(pDisplay,
                   RootWindow(pDisplay,DefaultScreen(pDisplay)),
                   HHAtom, HHAtom, 8, PropModeReplace,
                   (unsigned char *)pServerData,
                   (int)((limit*sizeof(HHInstance_t))/*/4*/) );
   myEvent.type = ClientMessage;
   myEvent.xclient.message_type=HHAtom;
   myEvent.xclient.format=8;
   sprintf(myEvent.xclient.data.b,"%d",app_pid);
   XSendEvent(pDisplay,pServerData[i].HHWindow,True,0,&myEvent);
   if (pReceived)
      XFree(pReceived);
   if (pServerData)
      free(pServerData);
   return (1);
}


/**************************************************************
 * Function: ChangeProperty()
 * Purpose:  Checks for the HyperHelp atom to put data up on,
 *           searches for pid element and attempts to splice
 *           up the HH window id on it.
 **************************************************************/
#ifdef _NO_PROTO
static
void ChangeProperty(pDisplay, pid)
   Display *pDisplay;
   int      pid;
#else
static void ChangeProperty(Display *pDisplay, int pid)
#endif
{
   Atom           HHAtom, DummyAtom;
   int            actual_format,i;
   unsigned long  nitems, bytes_after, limit;
   unsigned char *pReceived;
   HHInstance_t  *pServerData;
   HHInstance_t  *pNewServerData = NULL;

   if (!pid)
      return;

   HHAtom = XInternAtom(pDisplay,HHATOMNAME,False);
   if (!HHAtom)
      return;

   XGetWindowProperty(pDisplay,
      RootWindow(pDisplay,DefaultScreen(pDisplay)),
      HHAtom, 0L, 0x0f000L, False, AnyPropertyType, &DummyAtom,
      &actual_format, &nitems, &bytes_after, &pReceived);

   if (nitems==0) {
      if (pReceived)
         XFree(pReceived);
      return;
   }

   limit = nitems/(sizeof(HHInstance_t)/*AAB/4*/);
   pServerData = (HHInstance_t *)osdCalloc(1,sizeof(HHInstance_t)*limit);
   osdMemcpy(pServerData, pReceived, sizeof(HHInstance_t)*limit);

   /*See if element with pid exists*/
   for (i=0; i<(long)limit; i++) {
      if (pid == pServerData[i].pid)
      break;
   }
   if (i >= (long)limit){
      /*No match found*/
      if (pReceived)
         XFree(pReceived);
      if (pServerData)
         free(pServerData);
      return;
   }
   else {
      pNewServerData = (HHInstance_t *)osdCalloc(1,
                       sizeof(HHInstance_t)*(limit-1));
      osdMemcpy(pNewServerData, pServerData, sizeof(HHInstance_t)*i);
      if (i == 0)
         XDeleteProperty(pDisplay,
            RootWindow(pDisplay,DefaultScreen(pDisplay)), HHAtom);
      else XChangeProperty(pDisplay,
              RootWindow(pDisplay,DefaultScreen(pDisplay)),
              HHAtom, HHAtom, 8, PropModeReplace,
              (unsigned char *)pNewServerData,
              (int)((i*sizeof(HHInstance_t))/*AAB/4*/) );

      i++;
      if (i < (long)limit) {
         osdMemcpy(pNewServerData, pServerData+i,
                sizeof(HHInstance_t)*(limit-i));
         XChangeProperty(pDisplay,
                RootWindow(pDisplay,DefaultScreen(pDisplay)),
                HHAtom, HHAtom, 8, PropModeAppend,
                (unsigned char *)pNewServerData,
                (int)(((limit-i)*sizeof(HHInstance_t))/*AAB/4*/) );
      }

      XFlush(pDisplay);
      if (pNewServerData)
         free(pNewServerData);
   }
   if (pReceived)
      XFree(pReceived);
   if (pServerData)
      free(pServerData);
}


/**************************************************************
 * Function: childend()
 * Purpose:  Do-nothing signal handler which allows 
 *           children to die off when killed. 
 **************************************************************/
#ifdef _NO_PROTO
static 
void childend(sig)
  int sig;
#else
static void childend(int sig)
#endif
{
   /*Default SIGCHLD interrupt handler..*/
   /* Need this on some platforms to allow viewer process to die */
#ifndef __vms
   waitpid(HH_PID, NULL, WNOHANG);
#endif

   return;
} /* childend() */

#ifdef _NO_PROTO
static 
void MakeHOHPath(pHHHOME, hohbuf)
  char *pHHHOME;
  char *hohbuf;
#else
static void MakeHOHPath(char *pHHHOME, char *hohbuf)
#endif
{
    FILE *fp_hoh = NULL;
    char *pHOHPATH;
    
    if (pHHHOME) {
#ifndef __vms
	sprintf(hohbuf, "%s/hoh.hlp", pHHHOME);
#else
	(void) strcpy(hohbuf, "HHHOME:[000000]HOH.HLP");
#endif
	fp_hoh = fopen(hohbuf, "r");
    }

    pHOHPATH = getenv("HOHPATH");
    if (!fp_hoh && pHOHPATH) {
#ifndef __vms
	sprintf(hohbuf, "%s/hoh.hlp", pHOHPATH);
#else
	(void) strcpy(hohbuf, "HOHPATH:HOH.HLP");
#endif
	fp_hoh = fopen(hohbuf, "r");
    }

    if (fp_hoh)
      fclose(fp_hoh);
    else 
#ifndef __vms
      strcpy(hohbuf,  "/usr/lib/hyperhelp/hoh.hlp");
#else
      strcpy(hohbuf, "HYPERHELP_DIR:[000000]HOH.HLP");
#endif
    
} /* MakeHOHPath() */

#ifdef _NO_PROTO
static void remove_tcard_atom_entry(display, pid, w)
Display *display;
int pid;
Window w;
#else
static void remove_tcard_atom_entry(Display *display, int pid, Window w)
#endif
{
Atom DummyAtom;
int actual_format, i, limit;
unsigned long nitems, bytes_after;
unsigned char *pReceived=NULL;
HHInstance_t    *pServerData=NULL;

        if (HHTCardAtom) {

        	XGrabServer(display);

                /* get the current TCard atom data */
                XGetWindowProperty(display, RootWindow(display,
                        DefaultScreen(display)),
                        HHTCardAtom, 0L, 0x0f000L, False, AnyPropertyType,
                        &DummyAtom, &actual_format, &nitems, &bytes_after,
                        &pReceived);

                /* if there are no existing items, there's nothing to do... */
                if (nitems == 0) {
                        if (pReceived)
                                free(pReceived);
                } else { /* we got the existing items */
                        limit = nitems / sizeof(HHInstance_t);
                        pServerData = (HHInstance_t *) osdCalloc(1, sizeof(HHInstance_t)*limit);
                        osdMemcpy(pServerData, pReceived, sizeof(HHInstance_t)*limit);

                        /* find the one for the app we are dealing with */
                        for (i = 0; i < limit; i++) {
                                if ((pServerData[i].pid == pid) && (pServerData[i].ClientWindow == w)) {
                                        break;
                                }
                        }
                        /* make sure we found it, and its window is valid */
                        if ((i == limit) || (pServerData[i].pid != pid)) {
                        } else if ((pServerData[i].ClientWindow == 0) || (pServerData[i].ClientWindow != w)) {
                        } else if ((pServerData[i].pid == 0) || (pServerData[i].pid == 1)) {
                        } else {        /* we found the entry and it is valid */

				/* delete the entry from the atom data */
				for (; i < (limit - 1); i++) {
					osdMemcpy(&pServerData[i+1], &pServerData[i], sizeof(HHInstance_t));
				}

				if (limit > 0) {
                                    XChangeProperty(display,
                                        RootWindow(display,DefaultScreen(display)),
                                        HHTCardAtom, HHTCardAtom, 8, PropModeReplace,
                                        (unsigned char *)pServerData,
                                        (int)(((limit - 1)*sizeof(HHInstance_t))) );
				}
                                if (pReceived)
                                        XFree(pReceived);
                                if (pServerData)
                                        free(pServerData);
                        }
                }

        	XUngrabServer(display);
	        XFlush(display);
        }
}

#ifdef _NO_PROTO
static void setup_tcard_atom(pDisplay, w)
Display *pDisplay;
unsigned long w;
#else
static void setup_tcard_atom(Display *pDisplay, unsigned long w)
#endif
{
   //HHInstance_t   HHInstance;

   if (!HHTCardAtom)
   	HHTCardAtom = XInternAtom(pDisplay,HHTCARDATOMNAME,False);

   if (!HHTCardAtom){
      return;
   }

   XFlush(pDisplay);
}

/* This function returns true if the X event structure pointed to by event was
   generated by a training card message sent from the help viewer. This function
   can be used in the application's main loop to check for training card
   messages from the help viewer. */
#ifdef _NO_PROTO
int HH_IsHelpMessage(event)
XEvent *event;
#else
int HH_IsHelpMessage(XEvent *event)
#endif
{
	if (HHTCardAtom && (event->xclient.message_type == HHTCardAtom))
		return(1);
	else
		return(0);
}

/* This function receives data sent by the help viewer to the application.
   It should be called from the application's main loop if a training card event
   (an event for which HH_IsHelpMessage(event) returns true) is received by the
   application. The application developer use the return parameters idAction
   and dwActionData that are returned by the function to determine what
   action by the user caused the event to be sent from the viewer to the
   application. This function returns 1 if it succeeds in retrieving and
   training card message and decoding its action and (possibly) its action
   data fields. Otherwise it returns 0. */
#ifdef _NO_PROTO
int HH_HandleHelpMessage(display, event, pAction, pActionData)
Display *display;
XEvent *event;
int *pAction;
int *pActionData;
#else
int HH_HandleHelpMessage(Display *display, XEvent *event, int *pAction, int *pActionData)
#endif
{
Atom           DummyAtom;
int            actual_format,i;
unsigned long  nitems, bytes_after, limit;
unsigned char *pReceived=NULL;
HHInstance_t  *pServerData=NULL;
//HHInstance_t  *pNewServerData = NULL;
int found, best;
char *message_data;
int idAction, dwActionData;

	/* If the atom is not initialized, something is wrong... */
	if (!HHTCardAtom)
		return(0);

	XGrabServer(display);

	/* get the message from the property */
	XGetWindowProperty(display,
		RootWindow(display,DefaultScreen(display)),
		HHTCardAtom, 0L, 0x0f000L, False, AnyPropertyType, &DummyAtom,
		&actual_format, &nitems, &bytes_after, &pReceived);

	/* if there's nothing in there, just return */
	if (nitems == 0) {
		XUngrabServer(display);
		if (pReceived)
			XFree(pReceived);
		return(0);
	}

	limit = nitems / sizeof(HHInstance_t);
	pServerData = (HHInstance_t *)osdCalloc(1,(int)(sizeof(HHInstance_t)*limit));
	osdMemcpy(pServerData, pReceived, (int)(sizeof(HHInstance_t)*limit));

	found = 0;
	best = -1;
	/* look for the entry for our pid and window with lowest index value */
	for (i = 0; i < (long)limit; i++) {
		/* if it has the right window id and process id */
		if ((pServerData[i].ClientWindow == HH_ApplicationWindowId) &&
		    (pServerData[i].pid == HH_ApplicationPid)) {
			/* check to see if it is the oldest message */
			if (!found || (pServerData[i].bServer < pServerData[best].bServer)) {
				best = i;
			}
			found++;
		}
	}

	/* if we got a matching entry save its data and delete it */
	if (best >= 0) {
		
		message_data = (char *)osdMalloc(sizeof(pServerData[best].data));
		osdMemcpy(message_data, pServerData[best].data, sizeof(pServerData[best].data));
		/* now that we have copied its data, delete the entry from the atom data */
		for (i = best; i < ((long)limit - 1); i++) {
			osdMemcpy(&pServerData[i], &pServerData[i + 1], sizeof(HHInstance_t));
		}

		XChangeProperty(display,
			RootWindow(display,DefaultScreen(display)),
			HHTCardAtom, HHTCardAtom, 8, PropModeReplace,
			(unsigned char *)pServerData,
			(int)(((limit - 1)*sizeof(HHInstance_t))) );
	}

	XUngrabServer(display);

	/* if we did not find an entry for our pid and window, something is wrong, 
	   just return */
	if (!found) {
		if (pServerData)
			free(pServerData);
		if (pReceived)
			XFree(pReceived);
		return(0);
	}

	dwActionData = 0;
	idAction = IDABORT;	/* use this as default */

	if (!strstr(message_data, "WM_TCARD")) {
		return(0);
	} else if (strstr(message_data, "IDABORT"))
		idAction = IDABORT;
	else if (strstr(message_data, "IDCANCEL"))
		idAction = IDCANCEL;
	else if (strstr(message_data, "IDCLOSE"))
		idAction = IDCLOSE;
	else if (strstr(message_data, "IDHELP"))
		idAction = IDHELP;
	else if (strstr(message_data, "IDIGNORE"))
		idAction = IDIGNORE;
	else if (strstr(message_data, "IDOK"))
		idAction = IDOK;
	else if (strstr(message_data, "IDNO"))
		idAction = IDNO;
	else if (strstr(message_data, "IDRETRY"))
		idAction = IDRETRY;
	else if (strstr(message_data, "HELP_TCARD_DATA")) {
		idAction = HELP_TCARD_DATA;
		if (sscanf(message_data, "WM_TCARD HELP_TCARD_DATA %d\n", &dwActionData) != 1) {
			return(0);
		}
	} else if (strstr(message_data, "HELP_TCARD_NEXT"))
		idAction = HELP_TCARD_NEXT;
	else if (strstr(message_data, "HELP_TCARD_OTHER_CALLER"))
		idAction = HELP_TCARD_OTHER_CALLER;
	else if (strstr(message_data, "IDYES"))
		idAction = IDYES;
	else {
		return(0);
	}

	if (message_data)
		free(message_data);
	if (pServerData)
		free(pServerData);
	if (pReceived)
		XFree(pReceived);

	*pAction = idAction;
	*pActionData = dwActionData;

	return(1);
}
