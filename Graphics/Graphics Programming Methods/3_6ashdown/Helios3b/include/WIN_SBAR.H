////////////////////////////////////////////////////////////
//
//  WIN_SBAR.H - MS-Windows Scroll Bar Control Class
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

#ifndef _WIN_SBAR_H
#define _WIN_SBAR_H

#include "general.h"

class WinScroll         // Scroll bar control
{
  private:
    HWND hwnd;          // Client window handle
    POINT curr_pos;     // Current scroll position
    POINT max_range;    // Maximum scroll range
    POINT inc;          // Scroll increment
    POINT size;         // Client window size

  public:
    WinScroll( HWND hw)
    {
      RECT rect;        // Rectangle structure

      curr_pos.x = curr_pos.y = 0;
      max_range.x = max_range.y = 0;
      inc.x = inc.y = 0;

      hwnd = hw;
      GetClientRect(hwnd, &rect);
      size.x = rect.right;
      size.y = rect.bottom;

      Hide();
    }

    POINT Pos() { return curr_pos; }
    void Hide();
    void Horz( WPARAM, WORD );
    void Init( int, int );
    void Set( int, int );
    void Vert( WPARAM, WORD );
};

#endif

