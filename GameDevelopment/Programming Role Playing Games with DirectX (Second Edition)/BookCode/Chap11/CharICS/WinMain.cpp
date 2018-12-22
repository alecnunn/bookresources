/**************************************************
WinMain.cpp
Chapter 11 Character Inventory Control System Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "Core_Global.h"
#include "MIL.h"
#include "CharICS.h"

// Application variables ////////////////////////////////////////////
sItem     g_Items[1024];       // MIL Item list
cCharICS  g_CharICS;           // Character inventory control system

cGraphics g_Graphics;
cCamera   g_Camera;

// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK InventoryDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL ListItems(HWND hWnd);

// Application //////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  FILE *fp;
  long i;
  
  // Load the MIL list
  for(i=0;i<1024;i++)
    ZeroMemory(&g_Items[i], sizeof(sItem));
  if((fp=fopen("..\\Data\\Default.mil", "rb")) != NULL) {
    for(i=0;i<1024;i++)
      fread(&g_Items[i], 1, sizeof(sItem), fp);
    fclose(fp);
  }

  // Get the character items
  g_CharICS.Load("..\\Data\\Char.ci");

  // Initialize the graphics core for displaying objects
  g_Graphics.Init();
  g_Camera.Point(0.0f, 50.0f, -100.0f, 0.0f, 0.0f, 0.0f);

  // Start the dialog box
  DialogBox(hInst, MAKEINTRESOURCE(IDD_INVENTORY), NULL, InventoryDialogProc);

  // Save the character items
  g_CharICS.Save("..\\Data\\Char.ci");

  g_Graphics.Shutdown();

  return 0;
}

BOOL CALLBACK InventoryDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static BOOL    Loaded = FALSE;
  static cMesh   Mesh;
  static cObject Object;
  sCharItem     *Item;
  int            Selection;
  char           Text[256];

  switch(uMsg) {
    case WM_INITDIALOG:
      // Set the graphics mode to this dialog box
      g_Graphics.SetMode(GetDlgItem(hWnd, IDC_PREVIEW), TRUE, TRUE);

      // Add items to inventory list
      ListItems(hWnd);
      return TRUE;

    case WM_PAINT:
      g_Graphics.Clear(D3DCOLOR_RGBA(0,64,128,255));
      if(Loaded) {
        g_Graphics.SetCamera(&g_Camera);
        if(g_Graphics.BeginScene() == TRUE) {
          Object.Rotate(0.0f, (float)timeGetTime() / 1000.0f, 0.0f);
          Object.Render();
          g_Graphics.EndScene();
        }
      }
      g_Graphics.Display();
      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_CLOSE: 
          Loaded = FALSE;
          Mesh.Free();
          Object.Free();

          EndDialog(hWnd, TRUE);
          return TRUE;

        case IDC_ADD:
          // Add a random item
          while(1) {
            Selection = rand() % 1024;
            if(g_Items[Selection].Name[0])
              break;
          }
          g_CharICS.Add(Selection, 1, NULL);
          
          // List items and free mesh
          ListItems(hWnd);
          Mesh.Free();
          Object.Free();
          Loaded = FALSE;

          break;

        case IDC_USE:
          // Get the item that was selected
          if((Selection = SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;
          Item = g_CharICS.GetItem(Selection);

          // Drop item if flagged
          if(CheckItemFlag(g_Items[Item->ItemNum].Flags, USEONCE)) {
            g_CharICS.Remove(Item);

            // List items and free mesh
            ListItems(hWnd);
            Mesh.Free();
            Object.Free();
            Loaded = FALSE;
          }
          break;

        case IDC_UP:
          // Get the item that was selected
          if((Selection = SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;
          Item = g_CharICS.GetItem(Selection);
          if(Item->Prev != NULL) {
            g_CharICS.MoveUp(Item);

            // List items and free mesh
            ListItems(hWnd);
            Mesh.Free();
            Object.Free();
            Loaded = FALSE;

            // Reselect item in list and force selection
            SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_SETCURSEL, Selection-1, 0);
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_ITEMS, LBN_SELCHANGE), 0);
          }
          break;

        case IDC_DOWN:
          // Get the item that was selected
          if((Selection = SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;
          Item = g_CharICS.GetItem(Selection);
          if(Item->Next != NULL) {
            g_CharICS.MoveDown(Item);

            // List items and free mesh
            ListItems(hWnd);
            Mesh.Free();
            Object.Free();
            Loaded = FALSE;

            // Reselect item in list and force selection
            SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_SETCURSEL, Selection+1, 0);
            SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_ITEMS, LBN_SELCHANGE), 0);
          }
          break;

        case IDC_ARRANGE:
          g_CharICS.Arrange();

          // List items and free mesh
          ListItems(hWnd);
          Mesh.Free();
          Object.Free();
          Loaded = FALSE;
          break;

        case IDC_DROP:
          // Get the item that was selected
          if((Selection = SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;
          Item = g_CharICS.GetItem(Selection);
          g_CharICS.Remove(Item);

          // List items and free mesh
          ListItems(hWnd);
          Mesh.Free();
          Object.Free();
          Loaded = FALSE;

          // Reselect item in list and force selection
          SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_SETCURSEL, Selection, 0);
          SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_ITEMS, LBN_SELCHANGE), 0);

          break;

        case IDC_ITEMS:
          if(HIWORD(wParam) == LBN_SELCHANGE) {
            if((Selection = SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_GETCURSEL, 0, 0)) == LB_ERR)
              break;
     
            // Get the item that was selected
            Item = g_CharICS.GetItem(Selection);
  
            // Display description
            SetWindowText(GetDlgItem(hWnd, IDC_DESCRIPTION), g_Items[Item->ItemNum].Description);

            // Set up drop button
            EnableWindow(GetDlgItem(hWnd, IDC_DROP), (CheckItemFlag(g_Items[Item->ItemNum].Flags, CANDROP) ? TRUE : FALSE));

            // Set up equip/use buttons to select
            EnableWindow(GetDlgItem(hWnd, IDC_EQUIP), FALSE);
            EnableWindow(GetDlgItem(hWnd, IDC_USE), FALSE);
            switch(g_Items[Item->ItemNum].Category) {
              case WEAPON:
              case ARMOR:
              case SHIELD:
              case ACCESSORY:
                EnableWindow(GetDlgItem(hWnd, IDC_EQUIP), TRUE);
                break;
              case EDIBLE:
              case HEALING:
              case TRANSPORTATION:
                EnableWindow(GetDlgItem(hWnd, IDC_USE), TRUE);
                break;
            }


            // Load the mesh
            Loaded = FALSE;
            Mesh.Free();
            Object.Free();
            sprintf(Text, "..\\Data\\%s", g_Items[Item->ItemNum].MeshFilename);
            Mesh.Load(&g_Graphics, Text, "..\\Data\\");
            if((Loaded = Mesh.IsLoaded()) == TRUE)
              Object.Create(&g_Graphics, &Mesh);
          }
          break;
      }
      break;
  }

  return FALSE;
}

BOOL ListItems(HWND hWnd)
{
  sCharItem     *Item;
  long Num;
  char Text[256];

  SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_RESETCONTENT, 0, 0);

  if((Item = g_CharICS.GetParentItem()) != NULL) {
   Num = 0;
   while(Item != NULL) {
     if(Item->Quantity != 1)
       sprintf(Text, "%s  x  %lu", g_Items[Item->ItemNum].Name, Item->Quantity);
      else
        sprintf(Text, "%s", g_Items[Item->ItemNum].Name);
     SendMessage(GetDlgItem(hWnd, IDC_ITEMS), LB_INSERTSTRING, Num++, (LPARAM)Text);
     Item = Item->Next;
    }
  }

  // Disable buttons
  EnableWindow(GetDlgItem(hWnd, IDC_EQUIP), FALSE);
  EnableWindow(GetDlgItem(hWnd, IDC_USE), FALSE);
  EnableWindow(GetDlgItem(hWnd, IDC_DROP), FALSE);

  UpdateWindow(hWnd);

  return TRUE;
}
