////////////////////////////////////////////////////////////
//
//  WIN_TEXT.H - MS-Windows Text File Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _WIN_TEXT_H
#define _WIN_TEXT_H

#include <string.h>
#include "general.h"

// NOTE: Requires LARGE memory model for Win16

class WinText   // MS-Windows text file
{
  private:
    FILE *pfile;        // File pointer

  public:

    // Close file
    void Close() { (void) fclose(pfile); }

    // Read next line from file
    void GetLine( char *pline, int max )
    {
      char *pstr;       // String pointer

      if ((pstr = fgets(pline, max, pfile)) != NULL)
        pstr = strchr(pline, '\n');

      if (pstr == NULL)
        pstr = pline;

      *pstr = '\0';     // Strip off newline
    }

    BOOL Open( char *fname )    // Open text file
    {
      if ((pfile = fopen(fname, "r")) != NULL)
        return TRUE;
      else
        return FALSE;
    }
};

#endif

