////////////////////////////////////////////////////////////
//
//  WIN_META.H - MS-Windows Metafile Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              95/10/11 - Modified Record function to
//                         create temporary file name.
//                       - Changed file_name array length
//                         from 144 to _MAX_PATH.
//                       - Added <stdlib.h> and <string.h>
//                         include directives.
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

#ifndef _WIN_META_H
#define _WIN_META_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class WinMetaFile       // MS-Windows metafile
{
  private:
    char file_name[_MAX_PATH];  // File name
    BOOL file_flag;             // File name flag
    HDC hmdc;                   // Device context handle
    HMETAFILE hmf;              // Metafile handle

  public:
    WinMetaFile()
    {
      *file_name = '\0';
      file_flag = FALSE;
      hmdc = NULL;
      hmf = NULL;
    }

    ~WinMetaFile() { Erase(); }

    void Erase()        // Erase metafile
    {
      Stop();   // Stop recording

      if (hmf != NULL)
      {
        DeleteMetaFile(hmf);    // Delete metafile handle
        hmf = NULL;
      }

      if (file_flag == TRUE)
      {
        unlink(file_name);      // Remove metafile
        file_flag = FALSE;
      }
    }

    // Play metafile to display device
    void Play( HWND hwnd, int win_w, int win_h, int view_w,
        int view_h )
    {
      HDC hdc;          // Device context handle
      PAINTSTRUCT ps;   // Paint structure

      if (hmf != NULL)
      {
        hdc = BeginPaint(hwnd, &ps);

        // Initialize window-to-viewport mapping mode
        SetMapMode(hdc, MM_ISOTROPIC);
        SetWindowExtEx(hdc, win_w, win_h, NULL);
        SetViewportExtEx(hdc, view_w, -view_h, NULL);
        SetViewportOrgEx(hdc, 0, view_h, NULL);

        PlayMetaFile(hdc, hmf);

        EndPaint(hwnd, &ps);
      }
    }
    
   // Add polygon draw instruction to metafile
    BOOL Polygon( POINT *vertex, int num )
    { return ::Polygon(hmdc, vertex, num); }

    BOOL Record( BOOL fflag )   // Start metafile recording
    {
      int len;  // Directory name length

      Erase();  // Erase previous metafile

      if (fflag == TRUE)
      {
        // Get Windows directory
        len = GetWindowsDirectory(file_name, _MAX_PATH);
        
        // Append '\' character (if necessary)
        if (file_name[len - 1] != '\\')
          lstrcat(file_name, "\\");

        // Append temporary metafile file name
        lstrcat(file_name, "METAFILE.TMP");

        file_flag = TRUE;
        
        // Create file-based metafile
        if ((hmdc = CreateMetaFile(file_name)) == NULL)
          return FALSE;
      }
      else
      {
        // Create memory-based metafile
        if ((hmdc = CreateMetaFile(NULL)) == NULL)
          return FALSE;
      }

      // Select transparent brush for polygon fill
      SelectObject(hmdc, GetStockObject(NULL_BRUSH)); 

      return TRUE;
    }

    BOOL Stop()         // Stop metafile recording
    {
      if (hmdc != NULL)
      {
        hmf = CloseMetaFile(hmdc);
        hmdc = NULL;
      }
      return (hmf != NULL) ? TRUE : FALSE;
    }
};

#endif

