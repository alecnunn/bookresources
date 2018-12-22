/*
 * WinUtil
 *
 * Utilities for dealing with Windows.
 */

#ifndef _WINUTIL_H
#define _WINUTIL_H

// Message macros

#define IPControl_GetAddress(hwnd, ptr) \
SendMessage(hwnd, IPM_GETADDRESS, 0, (LPARAM) ptr)
#define IPControl_SetAddress(hwnd, addr) \
SendMessage(hwnd, IPM_SETADDRESS, 0, addr)

#define Trackbar_SetRange(hwnd, minv, maxv, redraw) \
SendMessage(hwnd, TBM_SETRANGEMIN, false, minv), \
SendMessage(hwnd, TBM_SETRANGEMAX, redraw, maxv)
#define Trackbar_GetPos(hwnd) \
SendMessage(hwnd, TBM_GETPOS, 0, 0)
#define Trackbar_SetPos(hwnd, pos, redraw) \
SendMessage(hwnd, TBM_SETPOS, redraw, pos)
#define Trackbar_SetBuddy(hwnd, hwndBuddy, location) \
SendMessage(hwnd, TBM_SETBUDDY, location, (LPARAM) hwndBuddy)
#define Trackbar_SetPageSize(hwnd, size) \
SendMessage(hwnd, TBM_SETPAGESIZE, 0, size)

#define UpDown_SetRange(hwnd, low, high) \
SendMessage(hwnd, UDM_SETRANGE32, low, high)

#endif
