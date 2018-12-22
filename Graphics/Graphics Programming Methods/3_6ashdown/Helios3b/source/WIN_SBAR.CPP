////////////////////////////////////////////////////////////
//
//  WIN_SBAR.CPP - MS-Windows Scroll Bar Control Class
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

#include "win_sbar.h"

// Set scroll bar positions and ranges
void WinScroll::Set( int width, int height )
{
  RECT rect;

  GetClientRect(hwnd, &rect);
  size.x = rect.right;
  size.y = rect.bottom;

  max_range.x = max(0, width - size.x);
  curr_pos.x = min(curr_pos.x, max_range.x);

  SetScrollRange(hwnd, SB_HORZ, 0, max_range.x, FALSE);
  SetScrollPos(hwnd, SB_HORZ, curr_pos.x, TRUE);

  max_range.y = max(0, height - size.y);
  curr_pos.y = min(curr_pos.y, max_range.y);

  SetScrollRange(hwnd, SB_VERT, 0, max_range.y, FALSE);
  SetScrollPos(hwnd, SB_VERT, curr_pos.y, TRUE);
}

// Initialize scroll bar positions and ranges
void WinScroll::Init( int width, int height )
{
  curr_pos.x = curr_pos.y = 0;
  Set(width, height);
}

void WinScroll::Hide()  // Hide scroll bars
{
  SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
  SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
}

// Process vertical scroll bar message
void WinScroll::Vert( WPARAM sb_code, WORD sb_pos )
{
  switch (sb_code)
  {
    case SB_LINEUP:
      inc.y = -1;
      break;
    case SB_LINEDOWN:
      inc.y = 1;
      break;
    case SB_PAGEUP:
      inc.y = min(-1, -size.y >> 2);
      break;
    case SB_PAGEDOWN:
      inc.y = max(1, size.y >> 2);
      break;
    case SB_TOP:
      inc.y = -inc.y;
      break;
    case SB_BOTTOM:
      inc.y = max_range.y - curr_pos.y;
      break;
    case SB_THUMBPOSITION:
      inc.y = sb_pos - curr_pos.y;
      break;
    default:
      inc.y = 0;
  }

  if ((inc.y = max(-curr_pos.y, min(inc.y, max_range.y -
      curr_pos.y))) != 0)
  {
    curr_pos.y += inc.y;
    ScrollWindow(hwnd, 0, -inc.y, NULL, NULL);
    SetScrollPos(hwnd, SB_VERT, curr_pos.y, TRUE);
    UpdateWindow(hwnd);
  }
}

// Process horizontal scroll bar message
void WinScroll::Horz( WPARAM sb_code, WORD sb_pos )
{
  switch (sb_code)
  {
    case SB_LINEUP:
      inc.x = -1;
      break;
    case SB_LINEDOWN:
      inc.x = 1;
      break;
    case SB_PAGEUP:
      inc.x = min(-1, -size.x >> 2);
      break;
    case SB_PAGEDOWN:
      inc.x = max(1, size.x >> 2);
      break;
    case SB_THUMBPOSITION:
      inc.x = sb_pos - curr_pos.x;
      break;
    default:
      inc.x = 0;
  }

  if ((inc.x = max(-curr_pos.x, min(inc.x, max_range.x -
      curr_pos.x))) != 0)
  {
    curr_pos.x += inc.x;
    ScrollWindow(hwnd, -inc.x, 0, NULL, NULL);
    SetScrollPos(hwnd, SB_HORZ, curr_pos.x, TRUE);
    UpdateWindow(hwnd);
  }
}

