/**************************************************
WinMain.cpp
Chapter 12 Master Character List Editor

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "MCL.h"

// Application variables ////////////////////////////////////////////
HWND g_hWnd;                   // Window handle
char g_szClass[] = "MCLEDIT";  // Class name

OPENFILENAME g_ofn;            // Open/Save dialog data
char g_MCLFile[MAX_PATH];      // Filename for character file

sCharacterDefinition g_Characters[256]; // Character list
long g_EditCharacter;                   // Character to modify

// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ModifyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL UpdateEntry(HWND hWnd, long CharNum);
BOOL LoadCharacters(char *Filename);
BOOL NewMCL();
BOOL LoadMCL();
BOOL SaveMCL();

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

  // Force a load of Characters from default.MCL
  LoadCharacters("..\\Data\\Default.MCL");

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

        // New MCL file
        case IDC_NEW: 
          NewMCL(); 
          break;

        // Load a MCL file
        case IDC_LOAD: 
          LoadMCL();
          break;

        // Save a MCL file
        case IDC_SAVE: 
          SaveMCL();
          break;

        // Clear an entry
        case IDC_CLEAR:
          // See if a character was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          ZeroMemory(&g_Characters[Selection], sizeof(sCharacterDefinition));
          g_Characters[Selection].Weapon    = -1;
          g_Characters[Selection].Armor     = -1;
          g_Characters[Selection].Shield    = -1;
          g_Characters[Selection].Accessory = -1;
          SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Characters[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_INSERTSTRING, Selection, (LPARAM)Text);
          break;

        // Edit an entry
        case IDC_CHARACTERS:
          if(HIWORD(wParam) != LBN_DBLCLK)
            break;
        case IDC_EDIT:
          // See if an character was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Set the character to edit
          g_EditCharacter = Selection;

          // Call the modify character dialog
          DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc);

          // Update character
          SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_DELETESTRING, Selection, 0);
          sprintf(Text, "%5lu: %s", Selection, g_Characters[Selection].Name);
          SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_INSERTSTRING, Selection, (LPARAM)Text);
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

      // Set default MCL filename
      strcpy(g_MCLFile, "..\\Data\\Default.MCL");

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
  char Text[32];
  long i;

  switch(uMsg) {
    case WM_INITDIALOG:
      // Return an error if there's no character to modify
      if(g_EditCharacter >= 256) {
        EndDialog(hWnd, FALSE);
        return FALSE;
      }

      // Display character number
      sprintf(Text, "%lu", g_EditCharacter);
      SetWindowText(GetDlgItem(hWnd, IDC_NUM), Text);

      // Display text entries
      SetWindowText(GetDlgItem(hWnd, IDC_NAME), g_Characters[g_EditCharacter].Name);
      SetWindowText(GetDlgItem(hWnd, IDC_ITEM), g_Characters[g_EditCharacter].ItemFilename);

      // Display numerical entries
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Class);
      SetWindowText(GetDlgItem(hWnd, IDC_CLASS), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].HealthPoints);
      SetWindowText(GetDlgItem(hWnd, IDC_HEALTH), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].ManaPoints);
      SetWindowText(GetDlgItem(hWnd, IDC_MANA), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Level);
      SetWindowText(GetDlgItem(hWnd, IDC_LEVEL), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Experience);
      SetWindowText(GetDlgItem(hWnd, IDC_EXPERIENCE), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Money);
      SetWindowText(GetDlgItem(hWnd, IDC_MONEY), Text);

      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Attack);
      SetWindowText(GetDlgItem(hWnd, IDC_ATTACK), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Defense);
      SetWindowText(GetDlgItem(hWnd, IDC_DEFENSE), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Agility);
      SetWindowText(GetDlgItem(hWnd, IDC_AGILITY), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Resistance);
      SetWindowText(GetDlgItem(hWnd, IDC_RESISTANCE), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Mental);
      SetWindowText(GetDlgItem(hWnd, IDC_MENTAL), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].ToHit);
      SetWindowText(GetDlgItem(hWnd, IDC_TOHIT), Text);
      sprintf(Text, "%lf", g_Characters[g_EditCharacter].Speed);
      SetWindowText(GetDlgItem(hWnd, IDC_SPEED), Text);

      sprintf(Text, "%lf", g_Characters[g_EditCharacter].Range);
      SetWindowText(GetDlgItem(hWnd, IDC_RANGE), Text);
      sprintf(Text, "%lf", g_Characters[g_EditCharacter].ChargeRate);
      SetWindowText(GetDlgItem(hWnd, IDC_CHARGE), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].ToAttack);
      SetWindowText(GetDlgItem(hWnd, IDC_TOATTACK), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].ToMagic);
      SetWindowText(GetDlgItem(hWnd, IDC_TOMAGIC), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].DropItem);
      SetWindowText(GetDlgItem(hWnd, IDC_DROPITEM), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].DropChance);
      SetWindowText(GetDlgItem(hWnd, IDC_TODROP), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].EffectChance);
      SetWindowText(GetDlgItem(hWnd, IDC_TOEFFECT), Text);
      sprintf(Text, "%lu", g_Characters[g_EditCharacter].Effects);
      SetWindowText(GetDlgItem(hWnd, IDC_EFFECT), Text);

      sprintf(Text, "%lu", g_Characters[g_EditCharacter].MeshNum);
      SetWindowText(GetDlgItem(hWnd, IDC_MESH), Text);
      sprintf(Text, "%ld", g_Characters[g_EditCharacter].Weapon);
      SetWindowText(GetDlgItem(hWnd, IDC_WEAPON), Text);
      sprintf(Text, "%ld", g_Characters[g_EditCharacter].Armor);
      SetWindowText(GetDlgItem(hWnd, IDC_ARMOR), Text);
      sprintf(Text, "%ld", g_Characters[g_EditCharacter].Shield);
      SetWindowText(GetDlgItem(hWnd, IDC_SHIELD), Text);
      sprintf(Text, "%ld", g_Characters[g_EditCharacter].Accessory);
      SetWindowText(GetDlgItem(hWnd, IDC_ACCESSORY), Text);

      // Display spell list
      SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_RESETCONTENT, 0, 0);
      for(i=0;i<64;i++) {
        sprintf(Text, "%5lu", i);
        SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_INSERTSTRING, i, (LPARAM)Text);

        if(i < 32) {
          if(g_Characters[g_EditCharacter].MagicSpells[0] & (1<<i))
            SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_SETSEL, TRUE, i);
        } else {
          if(g_Characters[g_EditCharacter].MagicSpells[1] & (1<<(i-32)))
            SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_SETSEL, TRUE, i);
        }
      }

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_OK: 
          UpdateEntry(hWnd, g_EditCharacter);
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

BOOL UpdateEntry(HWND hWnd, long CharNum)
{
  char Text[32];
  long i;

  // Get text entries
  GetWindowText(GetDlgItem(hWnd, IDC_NAME), g_Characters[CharNum].Name, 32);
  GetWindowText(GetDlgItem(hWnd, IDC_ITEM), g_Characters[CharNum].ItemFilename, MAX_PATH);

  // Get numerical entries
  GetWindowText(GetDlgItem(hWnd, IDC_CLASS),      Text, 32);
  g_Characters[CharNum].Class = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_HEALTH),     Text, 32);
  g_Characters[CharNum].HealthPoints = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_MANA),       Text, 32);
  g_Characters[CharNum].ManaPoints = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_LEVEL),      Text, 32);
  g_Characters[CharNum].Level = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_EXPERIENCE), Text, 32);
  g_Characters[CharNum].Experience = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_MONEY),      Text, 32);
  g_Characters[CharNum].Money = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_ATTACK),     Text, 32);
  g_Characters[CharNum].Attack = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_DEFENSE),    Text, 32);
  g_Characters[CharNum].Defense = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_AGILITY),    Text, 32);
  g_Characters[CharNum].Agility = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_RESISTANCE), Text, 32);
  g_Characters[CharNum].Resistance = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_MENTAL),     Text, 32);
  g_Characters[CharNum].Mental = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_TOHIT),      Text, 32);
  g_Characters[CharNum].ToHit = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SPEED),      Text, 32);
  g_Characters[CharNum].Speed = (float)atof(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_RANGE),     Text, 32);
  g_Characters[CharNum].Range = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_CHARGE),     Text, 32);
  g_Characters[CharNum].ChargeRate = (float)atof(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_TOATTACK),   Text, 32);
  g_Characters[CharNum].ToAttack = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_TOMAGIC),    Text, 32);
  g_Characters[CharNum].ToMagic = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_DROPITEM),   Text, 32);
  g_Characters[CharNum].DropItem = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_TODROP),     Text, 32);
  g_Characters[CharNum].DropChance = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_TOEFFECT),   Text, 32);
  g_Characters[CharNum].EffectChance = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_EFFECT),     Text, 32);
  g_Characters[CharNum].Effects = atol(Text);

  GetWindowText(GetDlgItem(hWnd, IDC_MESH),       Text, 32);
  g_Characters[CharNum].MeshNum = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_WEAPON),     Text, 32);
  g_Characters[CharNum].Weapon = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_ARMOR),      Text, 32);
  g_Characters[CharNum].Armor = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_SHIELD),     Text, 32);
  g_Characters[CharNum].Shield = atol(Text);
  GetWindowText(GetDlgItem(hWnd, IDC_ACCESSORY),  Text, 32);
  g_Characters[CharNum].Accessory = atol(Text);

  // Get spell list
  g_Characters[CharNum].MagicSpells[0] = 0;
  g_Characters[CharNum].MagicSpells[1] = 0;
  for(i=0;i<64;i++) {
    if(SendMessage(GetDlgItem(hWnd, IDC_SPELLS), LB_GETSEL, i, 0) == TRUE) {
      if(i < 32)
        g_Characters[CharNum].MagicSpells[0] |= (1<<i);
      else
        g_Characters[CharNum].MagicSpells[1] |= (1<<(i-32));
    }
  }

  return TRUE;
}

BOOL LoadCharacters(char *Filename)
{
  FILE *fp;
  long i;
  char Text[256];

  // Clear item structures
  for(i=0;i<256;i++) {
    ZeroMemory(&g_Characters[i], sizeof(sCharacterDefinition));
    g_Characters[i].Weapon    = -1;
    g_Characters[i].Armor     = -1;
    g_Characters[i].Shield    = -1;
    g_Characters[i].Accessory = -1;
  }

  // Open the file
  if((fp=fopen(Filename, "rb")) != NULL) {

    // Read in all Characters
    for(i=0;i<256;i++)
      fread(&g_Characters[i], 1, sizeof(sCharacterDefinition), fp);

    // Close file 
    fclose(fp);
  }

  // Update characters list box
  SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_RESETCONTENT, 0, 0);
  for(i=0;i<256;i++) {
    sprintf(Text, "%5lu: %s", i, g_Characters[i].Name);
    SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_INSERTSTRING, i, (LPARAM)Text);
  }

  return TRUE;
}

BOOL NewMCL()
{
  char Buf[16];

  if(MessageBox(g_hWnd, "Are you sure? (Looses any unsaved MCL information)", "New MCL", MB_YESNO) == IDYES) {
    SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_RESETCONTENT, 0, 0);
    for(long i=0;i<256;i++) {
      ZeroMemory(&g_Characters[i], sizeof(sCharacterDefinition));
      g_Characters[i].Weapon    = -1;
      g_Characters[i].Armor     = -1;
      g_Characters[i].Shield    = -1;
      g_Characters[i].Accessory = -1;

      sprintf(Buf, "%5lu:", i);
      SendMessage(GetDlgItem(g_hWnd, IDC_CHARACTERS), LB_INSERTSTRING, i, (LPARAM)Buf);
    }
    return TRUE;
  }

  return FALSE;
}

BOOL LoadMCL()
{
  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MCLFile;
  g_ofn.lpstrTitle  = "Load MCL File";
  g_ofn.lpstrFilter = "MCL Character Files (*.MCL)\0*.MCL\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "MCL";

  // Ask for filename
  if(!GetOpenFileName(&g_ofn))
    return FALSE;

  // Load the file and return result
  if(LoadCharacters(g_MCLFile) == FALSE) {
    MessageBox(g_hWnd, g_MCLFile, "Unable to open file.", MB_OK);
    return FALSE;
  }

  return TRUE;
}

BOOL SaveMCL()
{
  FILE *fp;
  long i;

  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_MCLFile;
  g_ofn.lpstrTitle  = "Load MCL File";
  g_ofn.lpstrFilter = "MCL Character Files (*.MCL)\0*.MCL\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "MCL";

  // Ask for filename
  if(!GetSaveFileName(&g_ofn))
    return FALSE;

  // Open filename for saving
  if((fp=fopen(g_MCLFile, "wb")) == NULL)
    return FALSE;

  // Save all objects
  for(i=0;i<256;i++)
    fwrite(&g_Characters[i], 1, sizeof(sCharacterDefinition), fp);

  // Close file and return success
  fclose(fp);
  
  return TRUE;
}
