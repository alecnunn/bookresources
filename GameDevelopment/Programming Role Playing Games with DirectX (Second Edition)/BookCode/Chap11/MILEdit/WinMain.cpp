/**************************************************
WinMain.cpp
Chapter 11 Master Item List Editor

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "MIL.h"

// Application variables ////////////////////////////////////////////
HWND g_hWnd;                   // Window handle
char g_szClass[] = "MILEDIT";  // Class name

OPENFILENAME g_ofn;            // Open/Save dialog data
char g_MILFile[MAX_PATH];      // Filename for item files

sItem g_Items[1024];          // Item list
long  g_EditItem;              // Item to modify

// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ModifyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL UpdateEntry(HWND hWnd, long ItemNum);
BOOL LoadItems(char *Filename);
BOOL NewMIL();
BOOL LoadMIL();
BOOL SaveMIL();

// Application //////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG      Msg;

  // Register window class
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = DLGWINDOWEXTRA;
  wc.hInstance     = hInst;
  wc.hIcon         = LoadIcon(hInst, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = g_szClass;
  RegisterClass(&wc);

  // Create the dialog box window and show it
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_EDIT), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Force a load of items from default.mil
  LoadItems("..\\Data\\Default.mil");

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Clean up
  UnregisterClass(g_szClass, hInst);

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int Selection;
  char Text[256];

  switch(uMsg) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {

        // New MIL file
        case IDC_NEW: 
          NewMIL(); 
          break;

        // Load a MIL file
        case IDC_LOAD: 
          LoadMIL();
          break;

        // Save a MIL file
        case IDC_SAVE: 
          SaveMIL();
          break;

        // Clear an entry
        case IDC_CLEAR:
          // See if an item was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          ZeroMemory(&g_Items[Selection], sizeof(sItem));
          SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Items[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_INSERTSTRING, Selection, (LPARAM)Text);
          break;

        // Edit an entry
        case IDC_ITEMS:
          if(HIWORD(wParam) != LBN_DBLCLK)
            break;
        case IDC_EDIT:
          // See if an item was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Setup the item to edit
          g_EditItem = Selection;

          // Call the modify item dialog
          DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc);

          // Update item
          SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Items[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_INSERTSTRING, Selection, (LPARAM)Text);
          break;
      }
      break;
    
    case WM_CREATE:
      // Initialize the save/load dialog box info
      ZeroMemory(&g_ofn, sizeof(OPENFILENAME));
      g_ofn.lStructSize = sizeof(OPENFILENAME);
      g_ofn.nMaxFile = MAX_PATH;
      g_ofn.nMaxFileTitle = MAX_PATH;
      g_ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;

      // Set default MIL filename
      strcpy(g_MILFile, "..\\Data\\Default.mil");

      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

BOOL CALLBACK ModifyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  WORD Categories[11] = {
    IDC_MONEY, IDC_WEAPON, IDC_ARMOR, IDC_SHIELD,
    IDC_ACCESSORY, IDC_EDIBLE, IDC_HEALING, 
    IDC_COLLECTION, IDC_TRANSPORTATION, IDC_CONTAINER, 
    IDC_OTHER 
  };
  WORD Flags[4] = { 
    IDC_SELLABLE, IDC_CANDROP, IDC_USEONCE, IDC_UNKNOWN 
  };
  char Text[16];
  long i;

  switch(uMsg) {
    case WM_INITDIALOG:
      // Return an error if there's no item to modify
      if(g_EditItem > 9999) {
        EndDialog(hWnd, FALSE);
        return FALSE;
      }

      // Set up current item attributes

      // Category
      SendMessage(GetDlgItem(hWnd, Categories[g_Items[g_EditItem].Category]), BM_SETCHECK, 1, 0);

      // Flags
      for(i=0;i<4;i++)
        SendMessage(GetDlgItem(hWnd, Flags[i]), BM_SETCHECK, CheckItemFlag(g_Items[g_EditItem].Flags, i), 0);

      // Usage
      SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_RESETCONTENT, 0, 0);
      for(i=0;i<32;i++) {
        sprintf(Text, "%5lu", i);
        SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_INSERTSTRING, i, (LPARAM)Text);
        if(CheckUsageBit(g_Items[g_EditItem].Usage, i))
          SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_SETSEL, TRUE, i);
      }

      // Item Number
      sprintf(Text, "%lu", g_EditItem);
      SetWindowText(GetDlgItem(hWnd, IDC_NUM), Text);

      // Entries
      SetWindowText(GetDlgItem(hWnd, IDC_NAME), g_Items[g_EditItem].Name);
      SetWindowText(GetDlgItem(hWnd, IDC_DESCRIPTION), g_Items[g_EditItem].Description);

      sprintf(Text, "%lf", g_Items[g_EditItem].Weight);
      SetWindowText(GetDlgItem(hWnd, IDC_WEIGHT), Text);
      sprintf(Text, "%lf", g_Items[g_EditItem].Size);
      SetWindowText(GetDlgItem(hWnd, IDC_ITEMSIZE), Text);
      sprintf(Text, "%ld", g_Items[g_EditItem].Value);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text);
      sprintf(Text, "%lu", g_Items[g_EditItem].Price);
      SetWindowText(GetDlgItem(hWnd, IDC_PRICE), Text);

      SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT), g_Items[g_EditItem].ScriptFilename);
      SetWindowText(GetDlgItem(hWnd, IDC_MESH), g_Items[g_EditItem].MeshFilename);
      SetWindowText(GetDlgItem(hWnd, IDC_IMAGE), g_Items[g_EditItem].ImageFilename);

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_OK: 
          UpdateEntry(hWnd, g_EditItem);
          EndDialog(hWnd, TRUE);
          return TRUE;

        case IDC_CANCEL: 
          EndDialog(hWnd, FALSE);
          return TRUE;

        case IDC_SETALL:
          for(i=0;i<32;i++)
            SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_SETSEL, TRUE, i);
          break;

        case IDC_CLEARALL:
          for(i=0;i<32;i++)
            SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_SETSEL, FALSE, i);
          break;
      }
      break;
  }

  return FALSE;
}

BOOL UpdateEntry(HWND hWnd, long ItemNum)
{
  WORD Categories[11] = {
    IDC_MONEY, IDC_WEAPON, IDC_ARMOR, IDC_SHIELD,
    IDC_ACCESSORY, IDC_EDIBLE, IDC_HEALING, 
    IDC_COLLECTION, IDC_TRANSPORTATION, IDC_CONTAINER,
    IDC_OTHER 
  };
  WORD Flags[4] = { 
    IDC_SELLABLE, IDC_CANDROP, IDC_USEONCE, IDC_UNKNOWN 
  };
  char Text[32];
  long i;

  // Get category
  for(i=0;i<11;i++) {
    if(SendMessage(GetDlgItem(hWnd, Categories[i]), BM_GETCHECK, 0, 0) == TRUE) {
      g_Items[ItemNum].Category = i;
      break;
    }
  }

  // Get flags
  g_Items[ItemNum].Flags = 0;
  for(i=0;i<4;i++) {
    if(SendMessage(GetDlgItem(hWnd, Flags[i]), BM_GETCHECK, 0, 0) == TRUE)
      SetItemFlag(g_Items[ItemNum].Flags, i);
  }

  // Get usage
  g_Items[ItemNum].Usage = 0;
  for(i=0;i<32;i++) {
    if(SendMessage(GetDlgItem(hWnd, IDC_USAGE), LB_GETSEL, i, 0) == TRUE)
      SetUsageBit(g_Items[ItemNum].Usage, i);
  }

  // Get entries
  GetWindowText(GetDlgItem(hWnd, IDC_NAME),        g_Items[ItemNum].Name, 32);
  GetWindowText(GetDlgItem(hWnd, IDC_DESCRIPTION), g_Items[ItemNum].Description, 128);
  
  GetWindowText(GetDlgItem(hWnd, IDC_WEIGHT), Text, 32);
  g_Items[ItemNum].Weight = (float)atof(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_ITEMSIZE), Text, 32);
  g_Items[ItemNum].Size = (float)atof(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text, 32);
  g_Items[ItemNum].Value = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_PRICE), Text, 32);
  g_Items[ItemNum].Price = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT), g_Items[ItemNum].ScriptFilename, 16);
  GetWindowText(GetDlgItem(hWnd, IDC_MESH),   g_Items[ItemNum].MeshFilename,   16);
  GetWindowText(GetDlgItem(hWnd, IDC_IMAGE),  g_Items[ItemNum].ImageFilename,  16);

  return TRUE;
}

BOOL LoadItems(char *Filename)
{
  FILE *fp;
  long i;
  char Text[256];

  // Clear item structures
  for(i=0;i<1024;i++)
    ZeroMemory(&g_Items[i], sizeof(sItem));

  // Open the file
  if((fp=fopen(Filename, "rb")) != NULL) {
    // Read in all items
    for(i=0;i<1024;i++)
      fread(&g_Items[i], 1, sizeof(sItem), fp);

    // Close file 
    fclose(fp);
  }

  // Update items box
  SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_RESETCONTENT, 0, 0);
  for(i=0;i<1024;i++) {
    sprintf(Text, "%5lu: %s", i, g_Items[i].Name);
    SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_INSERTSTRING, i, (LPARAM)Text);
  }
  

  return TRUE;
}

BOOL NewMIL()
{
  char Buf[16];

  if(MessageBox(g_hWnd, "Are you sure? (Looses any unsaved MIL information)", "New MIL", MB_YESNO) == IDYES) {
    SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_RESETCONTENT, 0, 0);
    for(long i=0;i<1024;i++) {
      ZeroMemory(&g_Items[i], sizeof(sItem));

      sprintf(Buf, "%5lu:", i);
      SendMessage(GetDlgItem(g_hWnd, IDC_ITEMS), LB_INSERTSTRING, i, (LPARAM)Buf);
    }
    return TRUE;
  }

  return FALSE;
}

BOOL LoadMIL()
{
  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MILFile;
  g_ofn.lpstrTitle  = "Load MIL File";
  g_ofn.lpstrFilter = "MIL Item Files (*.mil)\0*.mil\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "mil";

  // Ask for filename
  if(!GetOpenFileName(&g_ofn))
    return FALSE;

  // Load the file and return result
  if(LoadItems(g_MILFile) == FALSE) {
    MessageBox(g_hWnd, g_MILFile, "Unable to open file.", MB_OK);
    return FALSE;
  }

  return TRUE;
}

BOOL SaveMIL()
{
  FILE *fp;
  long i;

  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MILFile;
  g_ofn.lpstrTitle  = "Load MIL File";
  g_ofn.lpstrFilter = "MIL Item Files (*.mil)\0*.mil\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "mil";

  // Ask for filename
  if(!GetSaveFileName(&g_ofn))
    return FALSE;

  // Open filename for saving
  if((fp=fopen(g_MILFile, "wb")) == NULL)
    return FALSE;

  // Save all objects
  for(i=0;i<1024;i++)
    fwrite(&g_Items[i], 1, sizeof(sItem), fp);

  // Close file and return success
  fclose(fp);
  
  return TRUE;
}
