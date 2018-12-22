/**************************************************
WinMain.cpp
Chapter 12 Master Spell List Editor

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "MSL.h"

// Application variables ////////////////////////////////////////////
HWND g_hWnd;                   // Window handle
char g_szClass[] = "MSLEDIT";  // Class name

OPENFILENAME g_ofn;            // Open/Save dialog data
char g_MSLFile[MAX_PATH];      // Filename for spell file

sSpell g_Spells[64];           // Spell list
long   g_EditSpell;            // Spell to modify

// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ModifyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL UpdateEntry(HWND hWnd, long SpellNum);
BOOL LoadSpells(char *Filename);
BOOL NewMSL();
BOOL LoadMSL();
BOOL SaveMSL();

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

  // Force a load of spells from default.MSL
  LoadSpells("..\\Data\\Default.MSL");

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

        // New MSL file
        case IDC_NEW: 
          NewMSL(); 
          break;

        // Load a MSL file
        case IDC_LOAD: 
          LoadMSL();
          break;

        // Save a MSL file
        case IDC_SAVE: 
          SaveMSL();
          break;

        // Clear an entry
        case IDC_CLEAR:
          // See if a spell was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          ZeroMemory(&g_Spells[Selection], sizeof(sSpell));

          // Reset spell damage and cure classes
          g_Spells[Selection].DmgClass = -1;
          g_Spells[Selection].CureClass = -1;

          // Reset sounds
          g_Spells[Selection].MeshSound[0] = -1;
          g_Spells[Selection].MeshSound[1] = -1;
          g_Spells[Selection].MeshSound[2] = -1;

          SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Spells[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_INSERTSTRING, Selection, (LPARAM)Text);
          break;

        // Edit an entry
        case IDC_SPELLS:
          if(HIWORD(wParam) != LBN_DBLCLK)
            break;
        case IDC_EDIT:
          // See if a spell was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Set the spel to edit
          g_EditSpell = Selection;

          // Call the modify spell dialog
          DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc);

          // Update spell
          SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Spells[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_INSERTSTRING, Selection, (LPARAM)Text);
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

      // Set default MSL filename
      strcpy(g_MSLFile, "..\\Data\\Default.MSL");

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
  WORD Effects[8] = {
         IDC_HEALTH, IDC_MANA,
         IDC_CURE, IDC_CAUSE, IDC_RAISE, IDC_KILL,
         IDC_DISPEL, IDC_TELEPORT 
       };
  WORD Targets[3] = {
         IDC_SINGLE, IDC_SELF, IDC_AREA
       };
  WORD Positions[18] = {
         IDC_NONE1, IDC_CASTER1, IDC_TOTARGET1, IDC_TOCASTER1, IDC_TARGET1, IDC_SCALE1,
         IDC_NONE2, IDC_CASTER2, IDC_TOTARGET2, IDC_TOCASTER2, IDC_TARGET2, IDC_SCALE2,
         IDC_NONE3, IDC_CASTER3, IDC_TOTARGET3, IDC_TOCASTER3, IDC_TARGET3, IDC_SCALE3,
       };
  WORD Loops[3] = { IDC_LOOP1, IDC_LOOP2, IDC_LOOP3 };
  char Text[32];
  long i;

  switch(uMsg) {
    case WM_INITDIALOG:
      // Return an error if there's no spell to modify
      if(g_EditSpell >= 64) {
        EndDialog(hWnd, FALSE);
        return FALSE;
      }

      // Display spell number
      sprintf(Text, "%lu", g_EditSpell);
      SetWindowText(GetDlgItem(hWnd, IDC_NUM), Text);

      // Display text entries
      SetWindowText(GetDlgItem(hWnd, IDC_NAME), g_Spells[g_EditSpell].Name);
      SetWindowText(GetDlgItem(hWnd, IDC_DESCRIPTION), g_Spells[g_EditSpell].Description);

      // Display numerical entries
      sprintf(Text, "%ld", g_Spells[g_EditSpell].DmgClass);
      SetWindowText(GetDlgItem(hWnd, IDC_DMGCLASS), Text);
      sprintf(Text, "%ld", g_Spells[g_EditSpell].CureClass);
      SetWindowText(GetDlgItem(hWnd, IDC_CURECLASS), Text);

      sprintf(Text, "%lu", g_Spells[g_EditSpell].Cost);
      SetWindowText(GetDlgItem(hWnd, IDC_COST), Text);

      sprintf(Text, "%lf", g_Spells[g_EditSpell].Distance);
      SetWindowText(GetDlgItem(hWnd, IDC_DISTANCE), Text);

      sprintf(Text, "%lu", g_Spells[g_EditSpell].Chance);
      SetWindowText(GetDlgItem(hWnd, IDC_CHANCE), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].Value[0]);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE1), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].Value[1]);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE2), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].Value[2]);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE3), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].Value[3]);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE4), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].Range);
      SetWindowText(GetDlgItem(hWnd, IDC_RANGE), Text);

      sprintf(Text, "%lu", g_Spells[g_EditSpell].MeshNum[0]);
      SetWindowText(GetDlgItem(hWnd, IDC_MESH1), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].MeshSpeed[0]);
      SetWindowText(GetDlgItem(hWnd, IDC_SPEED1), Text);
      sprintf(Text, "%ld", g_Spells[g_EditSpell].MeshSound[0]);
      SetWindowText(GetDlgItem(hWnd, IDC_SOUND1), Text);

      sprintf(Text, "%lu", g_Spells[g_EditSpell].MeshNum[1]);
      SetWindowText(GetDlgItem(hWnd, IDC_MESH2), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].MeshSpeed[1]);
      SetWindowText(GetDlgItem(hWnd, IDC_SPEED2), Text);
      sprintf(Text, "%ld", g_Spells[g_EditSpell].MeshSound[1]);
      SetWindowText(GetDlgItem(hWnd, IDC_SOUND2), Text);

      sprintf(Text, "%lu", g_Spells[g_EditSpell].MeshNum[2]);
      SetWindowText(GetDlgItem(hWnd, IDC_MESH3), Text);
      sprintf(Text, "%lf", g_Spells[g_EditSpell].MeshSpeed[2]);
      SetWindowText(GetDlgItem(hWnd, IDC_SPEED3), Text);
      sprintf(Text, "%ld", g_Spells[g_EditSpell].MeshSound[2]);
      SetWindowText(GetDlgItem(hWnd, IDC_SOUND3), Text);

      // Display selection entries
      SendMessage(GetDlgItem(hWnd, Effects[g_Spells[g_EditSpell].Effect]), BM_SETCHECK, 1, 0);
      SendMessage(GetDlgItem(hWnd, Targets[g_Spells[g_EditSpell].Target]), BM_SETCHECK, 1, 0);
      SendMessage(GetDlgItem(hWnd, Positions[g_Spells[g_EditSpell].MeshPos[0]]), BM_SETCHECK, 1, 0);
      SendMessage(GetDlgItem(hWnd, Positions[g_Spells[g_EditSpell].MeshPos[1]+6]), BM_SETCHECK, 1, 0);
      SendMessage(GetDlgItem(hWnd, Positions[g_Spells[g_EditSpell].MeshPos[2]+12]), BM_SETCHECK, 1, 0);

      // Display loops
      SendMessage(GetDlgItem(hWnd, Loops[0]), BM_SETCHECK, g_Spells[g_EditSpell].MeshLoop[0], 0);
      SendMessage(GetDlgItem(hWnd, Loops[1]), BM_SETCHECK, g_Spells[g_EditSpell].MeshLoop[1], 0);
      SendMessage(GetDlgItem(hWnd, Loops[2]), BM_SETCHECK, g_Spells[g_EditSpell].MeshLoop[2], 0);

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_OK: 
          UpdateEntry(hWnd, g_EditSpell);
          EndDialog(hWnd, TRUE);
          return TRUE;

        case IDC_CANCEL: 
          EndDialog(hWnd, FALSE);
          return TRUE;

        case IDC_SETALL:
          for(i=0;i<64;i++)
            SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_SETSEL, TRUE, i);
          break;

        case IDC_CLEARALL:
          for(i=0;i<64;i++)
            SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_SETSEL, FALSE, i);
          break;
      }
      break;
  }

  return FALSE;
}

BOOL UpdateEntry(HWND hWnd, long SpellNum)
{
  WORD Effects[8] = {
         IDC_HEALTH, IDC_MANA,
         IDC_CURE, IDC_CAUSE, IDC_RAISE, IDC_KILL,
         IDC_DISPEL, IDC_TELEPORT 
       };
  WORD Targets[3] = {
         IDC_SINGLE, IDC_SELF, IDC_AREA
       };
  WORD Positions[18] = {
         IDC_NONE1, IDC_CASTER1, IDC_TOTARGET1, IDC_TOCASTER1, IDC_TARGET1, IDC_SCALE1,
         IDC_NONE2, IDC_CASTER2, IDC_TOTARGET2, IDC_TOCASTER2, IDC_TARGET2, IDC_SCALE2,
         IDC_NONE3, IDC_CASTER3, IDC_TOTARGET3, IDC_TOCASTER3, IDC_TARGET3, IDC_SCALE3,
       };
  WORD Loops[3] = { IDC_LOOP1, IDC_LOOP2, IDC_LOOP3 };
  char Text[32];
  long i;


  // Get text entries
  GetWindowText(GetDlgItem(hWnd, IDC_NAME), g_Spells[SpellNum].Name, 32);
  GetWindowText(GetDlgItem(hWnd, IDC_DESCRIPTION), g_Spells[SpellNum].Description, 128);

  // Get numerical entries
  GetWindowText(GetDlgItem(hWnd, IDC_DMGCLASS), Text, 32);
  g_Spells[SpellNum].DmgClass = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_CURECLASS), Text, 32);
  g_Spells[SpellNum].CureClass = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_COST), Text, 32);
  g_Spells[SpellNum].Cost = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_DISTANCE), Text, 32);
  g_Spells[g_EditSpell].Distance = (float)atof(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_CHANCE), Text, 32);
  g_Spells[SpellNum].Chance = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_VALUE1), Text, 32);
  g_Spells[SpellNum].Value[0] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_VALUE2), Text, 32);
  g_Spells[SpellNum].Value[1] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_VALUE3), Text, 32);
  g_Spells[SpellNum].Value[2] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_VALUE4), Text, 32);
  g_Spells[SpellNum].Value[3] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_RANGE), Text, 32);
  g_Spells[SpellNum].Range = (float)atof(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_MESH1), Text, 32);
  g_Spells[SpellNum].MeshNum[0] = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SPEED1), Text, 32);
  g_Spells[SpellNum].MeshSpeed[0] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SOUND1), Text, 32);
  g_Spells[SpellNum].MeshSound[0] = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_MESH2), Text, 32);
  g_Spells[SpellNum].MeshNum[1] = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SPEED2), Text, 32);
  g_Spells[SpellNum].MeshSpeed[1] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SOUND2), Text, 32);
  g_Spells[SpellNum].MeshSound[1] = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_MESH3), Text, 32);
  g_Spells[SpellNum].MeshNum[2] = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SPEED3), Text, 32);
  g_Spells[SpellNum].MeshSpeed[2] = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SOUND3), Text, 32);
  g_Spells[SpellNum].MeshSound[2] = atol(Text);

  // Get selection entries
  for(i=0;i<8;i++) {
    if(SendMessage(GetDlgItem(hWnd, Effects[i]), BM_GETCHECK, 0, 0) == TRUE) {
      g_Spells[SpellNum].Effect = i;
      break;
    }
  }
  for(i=0;i<3;i++) {
    if(SendMessage(GetDlgItem(hWnd, Targets[i]), BM_GETCHECK, 0, 0) == TRUE) {
      g_Spells[SpellNum].Target = i;
      break;
    }
  }
  for(i=0;i<6;i++) {
    if(SendMessage(GetDlgItem(hWnd, Positions[i]), BM_GETCHECK, 0, 0) == TRUE)
      g_Spells[SpellNum].MeshPos[0] = i;
    if(SendMessage(GetDlgItem(hWnd, Positions[i+6]), BM_GETCHECK, 0, 0) == TRUE)
      g_Spells[SpellNum].MeshPos[1] = i;
    if(SendMessage(GetDlgItem(hWnd, Positions[i+12]), BM_GETCHECK, 0, 0) == TRUE)
      g_Spells[SpellNum].MeshPos[2] = i;
  }

  // Get loops
  g_Spells[SpellNum].MeshLoop[0] = SendMessage(GetDlgItem(hWnd, Loops[0]), BM_GETCHECK, 0, 0);
  g_Spells[SpellNum].MeshLoop[1] = SendMessage(GetDlgItem(hWnd, Loops[1]), BM_GETCHECK, 0, 0);
  g_Spells[SpellNum].MeshLoop[2] = SendMessage(GetDlgItem(hWnd, Loops[2]), BM_GETCHECK, 0, 0);

  return TRUE;
}

BOOL LoadSpells(char *Filename)
{
  FILE *fp;
  long i;
  char Text[256];

  // Clear item structures
  for(i=0;i<64;i++) {
    ZeroMemory(&g_Spells[i], sizeof(sSpell));

    // Reset spell damage and cure classes
    g_Spells[i].DmgClass = -1;
    g_Spells[i].CureClass = -1;

    // Reset sounds
    g_Spells[i].MeshSound[0] = -1;
    g_Spells[i].MeshSound[1] = -1;
    g_Spells[i].MeshSound[2] = -1;
  }

  // Open the file
  if((fp=fopen(Filename, "rb")) != NULL) {

    // Read in all spells
    for(i=0;i<64;i++)
      fread(&g_Spells[i], 1, sizeof(sSpell), fp);

    // Close file 
    fclose(fp);
  }

  // Update spell's list box
  SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_RESETCONTENT, 0, 0);
  for(i=0;i<64;i++) {
    sprintf(Text, "%5lu: %s", i, g_Spells[i].Name);
    SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_INSERTSTRING, i, (LPARAM)Text);
  }

  return TRUE;
}

BOOL NewMSL()
{
  char Buf[16];

  if(MessageBox(g_hWnd, "Are you sure? (Looses any unsaved MSL information)", "New MSL", MB_YESNO) == IDYES) {
    SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_RESETCONTENT, 0, 0);
    for(long i=0;i<64;i++) {
      ZeroMemory(&g_Spells[i], sizeof(sSpell));

      // Reset spell damage and cure classes
      g_Spells[i].DmgClass = -1;
      g_Spells[i].CureClass = -1;

      // Reset sounds
      g_Spells[i].MeshSound[0] = -1;
      g_Spells[i].MeshSound[1] = -1;
      g_Spells[i].MeshSound[2] = -1;

      sprintf(Buf, "%5lu:", i);
      SendMessage(GetDlgItem(g_hWnd, IDC_SPELLS), LB_INSERTSTRING, i, (LPARAM)Buf);
    }
    return TRUE;
  }

  return FALSE;
}

BOOL LoadMSL()
{
  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MSLFile;
  g_ofn.lpstrTitle  = "Load MSL File";
  g_ofn.lpstrFilter = "MSL Spell Files (*.MSL)\0*.MSL\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "MSL";

  // Ask for filename
  if(!GetOpenFileName(&g_ofn))
    return FALSE;

  // Load the file and return result
  if(LoadSpells(g_MSLFile) == FALSE) {
    MessageBox(g_hWnd, g_MSLFile, "Unable to open file.", MB_OK);
    return FALSE;
  }

  return TRUE;
}

BOOL SaveMSL()
{
  FILE *fp;
  long i;

  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MSLFile;
  g_ofn.lpstrTitle  = "Load MSL File";
  g_ofn.lpstrFilter = "MSL Spell Files (*.MSL)\0*.MSL\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "MSL";

  // Ask for filename
  if(!GetSaveFileName(&g_ofn))
    return FALSE;

  // Open filename for saving
  if((fp=fopen(g_MSLFile, "wb")) == NULL)
    return FALSE;

  // Save all objects
  for(i=0;i<64;i++)
    fwrite(&g_Spells[i], 1, sizeof(sSpell), fp);

  // Close file and return success
  fclose(fp);
  
  return TRUE;
}
