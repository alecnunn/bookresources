// File: ComboBoxHelper.cpp

#include "include/ComboBoxHelper.h"



HWND cbhCreate(HWND hWnd, HINSTANCE hInst, HMENU id,
               int x, int y, int dx, int dy, bool bSort) {
   DWORD dwStyle;
   
   if (bSort)
      dwStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|
                CBS_HASSTRINGS|CBS_SORT;
   else 
      dwStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|
                CBS_HASSTRINGS;

   return CreateWindow("COMBOBOX", 0, dwStyle, 
                       x, y, dx, dy, hWnd,
                       id, hInst, 0);
   }
/*----------------------------------------------------------------*/

void cbhAddItem(HWND hWnd, char *ch, void *pData, bool bSelect) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)ch));
   if (pData)
      SendMessage(hWnd,CB_SETITEMDATA, nI, (LPARAM)pData);

   if (bSelect)
      SendMessage(hWnd,CB_SETCURSEL, nI, 0);
   }
/*----------------------------------------------------------------*/
 
void cbhResetContent(HWND hWnd) {
   SendMessage(hWnd,CB_RESETCONTENT,0,0);
   }
/*----------------------------------------------------------------*/

void cbhRemoveSelectedItem(HWND hWnd) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_GETCURSEL,0,0));
   SendMessage(hWnd,CB_DELETESTRING, (WPARAM)nI, 0);
   }
/*----------------------------------------------------------------*/

void* cbhGetSelectedItem(HWND hWnd) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_GETCURSEL,0,0));
   return ((void*)SendMessage(hWnd,CB_GETITEMDATA,nI,0));
   }
/*----------------------------------------------------------------*/

void cbhGetSelectedText(HWND hWnd, void *pData) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_GETCURSEL,0,0));
   SendMessage(hWnd,CB_GETLBTEXT, nI, (LPARAM)(LPCSTR)pData);
   }
/*----------------------------------------------------------------*/

UINT cbhGetSelectedIndex(HWND hWnd) {
   return ((UINT)SendMessage(hWnd,CB_GETCURSEL,0,0));
   }
/*----------------------------------------------------------------*/

void cbhSelectFirstItem(HWND hWnd) {
   SendMessage(hWnd,CB_SETCURSEL,(WPARAM)0,0);
   }
/*----------------------------------------------------------------*/

int cbhContainsString(HWND hWnd, char *ch) {
   int n = (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
   char buffer[200];

   for(int nIndex=0; nIndex<n; nIndex++) {
      SendMessage(hWnd, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)buffer);
      if (lstrcmp(buffer, ch) == 0)
         return nIndex;
    }
   return -1;
   }
/*----------------------------------------------------------------*/