/**************************************************
WinMain.cpp
Chapter 10 Mad Lib Script Editor

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "Action.h"

// Application variables ////////////////////////////////////////////
HWND g_hWnd;                   // Window handle
char g_szClass[] = "MLSEDIT";  // Class name

cActionTemplate g_Actions;       // Action template class

long     g_NumScriptActions = 0; // # script actions
sScript *g_ScriptParent = NULL;  // Script data
sScript *g_ModifyScript = NULL;  // Pointer to script to modify
BOOL     g_ShowCancel = FALSE;

OPENFILENAME    g_ofn;         // Open/Save dialog data
char g_ActionFile[MAX_PATH];   // Filename for action files
char g_ScriptFile[MAX_PATH];   // Filename for script files

// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ModifyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void SetModifyDialogText(HWND hWnd, sScript *Script, long Entry);
void UpdateEntry(HWND hWnd, sScript *Script, long Entry);

BOOL LoadActions(char *Filename = NULL);

BOOL NewScript();
BOOL LoadScript();
BOOL SaveScript();

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

  // Force a load of actions in Default.mla
  LoadActions("..\\Data\\Default.mla");

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Free script actions
  delete g_ScriptParent;

  // Free any actions
  g_Actions.Free();

  // Clean up
  UnregisterClass(g_szClass, hInst);

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int Selection, Pos;
  char Text[2048];
  sScript *Script;
  sAction *ActionPtr;

  switch(uMsg) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {

        // New script file
        case IDC_NEW: 
          NewScript(); 
          break;

        // Load a script file
        case IDC_LOAD: 
          LoadScript(); 
          break;

        // Save a script file
        case IDC_SAVE: 
          SaveScript(); 
          break;

        // Load an action file
        case IDC_LOADACTIONS: 
          LoadActions(); 
          break;

        // Edit a script action
        case IDC_SCRIPT:
          if(HIWORD(wParam) != LBN_DBLCLK)
            break;
        case IDC_EDIT:
          // See if a script action was selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Get pointer to script entry
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_GETTEXT, Selection, (LPARAM)Text);
          Script = (sScript*)atol(Text);

          // Setup the action to edit
          g_ModifyScript = Script;

          // Call the modify script action dialog
          g_ShowCancel = FALSE;
          DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc);

          // Update script action
          ActionPtr = g_Actions.GetAction(Selection);
          g_Actions.ExpandActionText(Text, Script);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_INSERTSTRING, Selection, (LPARAM)Text);
          break;

        // Add a script action
        case IDC_ACTIONS:
          if(HIWORD(wParam) != LBN_DBLCLK)
            break;
        case IDC_ADD:
          // Make sure an action is selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_ACTIONS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Create a new script action and add it
          Script = g_Actions.CreateScriptAction(Selection);

          g_ModifyScript = Script;

          // Call the modify script action dialog
          g_ShowCancel = TRUE;
          if(DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc) == TRUE) {

            // Add the script action to script
            Script->Next = g_ScriptParent;
            if(g_ScriptParent != NULL)
              g_ScriptParent->Prev = Script;
            g_ScriptParent = Script;

            // Display script action text
            ActionPtr = g_Actions.GetAction(Selection);
            g_Actions.ExpandActionText(Text, Script);
            SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)Text);

            // Add pointer to script list
            sprintf(Text, "%lu", Script);
            SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_ADDSTRING, 0, (LPARAM)Text);

            // Increase count
            g_NumScriptActions++;
          } else {
            delete Script;
          }
          break;

        // Insert a script action
        case IDC_INSERT:
          // Get location to insert action
          if((Pos = SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Make sure an action is selected
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_ACTIONS), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Create a new script action and add it
          Script = g_Actions.CreateScriptAction(Selection);
          g_ModifyScript = Script;

          // Call the modify script action dialog
          g_ShowCancel = TRUE;
          if(DialogBox(NULL, MAKEINTRESOURCE(IDD_MODIFY), hWnd, ModifyDialogProc) == TRUE) {

            // Add the script action to script
            Script->Prev = NULL;
            Script->Next = g_ScriptParent;
            g_ScriptParent->Prev = Script;
            g_ScriptParent = Script;

            // Display script action text
            ActionPtr = g_Actions.GetAction(Selection);
            g_Actions.ExpandActionText(Text, Script);
            SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_INSERTSTRING, Pos, (LPARAM)Text);

            // Add pointer to script list
            sprintf(Text, "%lu", Script);
            SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_INSERTSTRING, Pos, (LPARAM)Text);

            // Increase count
            g_NumScriptActions++;
          } else {
            delete Script;
          }
          break;

        // Delete a script action
        case IDC_DELETE:
          // Make sure there's a selection
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Get pointer to script entry
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_GETTEXT, Selection, (LPARAM)Text);
          Script = (sScript*)atol(Text);

          // Remove script action from linked list
          if(Script->Prev != NULL)
            Script->Prev->Next = Script->Next;
          else 
            g_ScriptParent = Script->Next;
          if(Script->Next != NULL)
            Script->Next->Prev = Script->Prev;
          Script->Next = NULL; // So whole list isn't deleted
          delete Script;

          // Clear from script and script list
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_DELETESTRING, Selection, 0);

          // Decrease count
          g_NumScriptActions--;

          break;

        // Move script action up in list
        case IDC_UP:
          // Make sure there's a selection
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Make sure it's not topmost selection
          if(!Selection)
            break;

          // Swap selection and one above
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETTEXT, Selection, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_INSERTSTRING, Selection-1, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_GETTEXT, Selection, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_INSERTSTRING, Selection-1, (LPARAM)Text);

          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_SETCURSEL, Selection-1, (LPARAM)Text);
          break;

        // Move script action down in list
        case IDC_DOWN:
          // Make sure there's a selection
          if((Selection = SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCURSEL, 0, 0)) == LB_ERR)
            break;

          // Make sure it's not bottommost
          if(Selection >= SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETCOUNT, 0, 0)-1)
            break;

          // Swap selection and one below
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_GETTEXT, Selection, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_INSERTSTRING, Selection+1, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_GETTEXT, Selection, (LPARAM)Text);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_DELETESTRING, Selection, 0);
          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_INSERTSTRING, Selection+1, (LPARAM)Text);

          SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_SETCURSEL, Selection+1, (LPARAM)Text);
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

      // Clear script filename
      g_ScriptFile[0] = 0;
      g_ActionFile[0] = 0;

      // Clear all list boxes
      SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_RESETCONTENT, 0, 0);
      SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_RESETCONTENT, 0, 0);
      SendMessage(GetDlgItem(g_hWnd, IDC_ACTIONS), LB_RESETCONTENT, 0, 0);

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
  static long EntryNum = 0;

  switch(uMsg) {
    case WM_INITDIALOG:
      // Return an error if there's no script to modify
      if(g_ModifyScript == NULL) {
        EndDialog(hWnd, FALSE);
        return FALSE;
      }

      // Edit first entry
      EntryNum = 0;

      // Display entry text
      SetModifyDialogText(hWnd, g_ModifyScript, EntryNum);

      // Show/Hide Cancel button
      ShowWindow(GetDlgItem(hWnd, IDC_CANCEL), g_ShowCancel);

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_OK: 
          UpdateEntry(hWnd, g_ModifyScript, EntryNum);
          EndDialog(hWnd, TRUE);
          return TRUE;

        case IDC_CANCEL: 
          EndDialog(hWnd, FALSE);
          return TRUE;

        case IDC_PREV:
          // Only bother if not first entry
          if(EntryNum > 0) {
            // Apply changes

            // Go to previous entry
            UpdateEntry(hWnd, g_ModifyScript, EntryNum);
            EntryNum--;
            SetModifyDialogText(hWnd, g_ModifyScript, EntryNum);
          }
          break;

        case IDC_NEXT:
          // Only bother if not last entry
          if(EntryNum < g_ModifyScript->NumEntries-1) {
            // Apply changes

            // Go to next entry
            UpdateEntry(hWnd, g_ModifyScript, EntryNum);
            EntryNum++;
            SetModifyDialogText(hWnd, g_ModifyScript, EntryNum);
          }
          break;
      }
      break;
  }

  return FALSE;
}

void SetModifyDialogText(HWND hWnd, sScript *Script, long Entry)
{
  sAction *ActionPtr;
  char Text[2048];
  long i;

  // Get pointer to action
  ActionPtr = g_Actions.GetAction(Script->Type);

  // Display Full action text
  g_Actions.ExpandActionText(Text, Script);
  SetWindowText(GetDlgItem(hWnd, IDC_ACTIONTEXT), Text);

  // Hide all fields
  ShowWindow(GetDlgItem(hWnd, IDC_PREV),    FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_NEXT),    FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_TEXT),    FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_CHOICE),  FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_TRUE),    FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_FALSE),   FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_MIN),     FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_MAX),     FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_VALUE),   FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_STATIC1), FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_STATIC2), FALSE);
  ShowWindow(GetDlgItem(hWnd, IDC_STATIC3), FALSE);

  // Clear info
  SetWindowText(GetDlgItem(hWnd, IDC_NUM), "");
  SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " No Entries ");

  // Return if no entries to edit
  if(!ActionPtr->NumEntries)
    return;

  // Add entry num and type      
  if(ActionPtr->NumEntries > 1) {
    ShowWindow(GetDlgItem(hWnd, IDC_PREV), TRUE);
    ShowWindow(GetDlgItem(hWnd, IDC_NEXT), TRUE);
  }
  sprintf(Text, "%lu of %lu", Entry+1, ActionPtr->NumEntries);
  SetWindowText(GetDlgItem(hWnd, IDC_NUM), Text);
  EnableWindow(GetDlgItem(hWnd, IDC_NUM), TRUE);

  // Enable and set specific fields based on type
  switch(Script->Entries[Entry].Type) {
    case _TEXT:
      SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " Text Entry ");
      if(Script->Entries[Entry].Text != NULL)
        SetWindowText(GetDlgItem(hWnd, IDC_TEXT), Script->Entries[Entry].Text);
      ShowWindow(GetDlgItem(hWnd, IDC_TEXT), TRUE);
      EnableWindow(GetDlgItem(hWnd, IDC_TEXT), TRUE);
      break;

    case _BOOL:
      SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " Boolean Entry ");
      if(Script->Entries[Entry].bValue == TRUE) {
        SendMessage(GetDlgItem(hWnd, IDC_TRUE), BM_SETCHECK,  BST_CHECKED, 0);
        SendMessage(GetDlgItem(hWnd, IDC_FALSE), BM_SETCHECK, BST_UNCHECKED, 0);
      } else {
        SendMessage(GetDlgItem(hWnd, IDC_TRUE), BM_SETCHECK,  BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hWnd, IDC_FALSE), BM_SETCHECK, BST_CHECKED, 0);
      }
      ShowWindow(GetDlgItem(hWnd, IDC_TRUE),  TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_FALSE), TRUE);
      break;

    case _INT:
      SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " Integer Entry ");
      sprintf(Text, "%lu", ActionPtr->Entries[Entry].lMin);
      SetWindowText(GetDlgItem(hWnd, IDC_MIN), Text);
      sprintf(Text, "%lu", ActionPtr->Entries[Entry].lMax);
      SetWindowText(GetDlgItem(hWnd, IDC_MAX), Text);
      sprintf(Text, "%lu", Script->Entries[Entry].lValue);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text);
      ShowWindow(GetDlgItem(hWnd, IDC_MIN),     TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_MAX),     TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_VALUE),   TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC1), TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC2), TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC3), TRUE);
      break;

    case _FLOAT:
      SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " Float Entry ");
      sprintf(Text, "%lf", ActionPtr->Entries[Entry].fMin);
      SetWindowText(GetDlgItem(hWnd, IDC_MIN), Text);
      sprintf(Text, "%lf", ActionPtr->Entries[Entry].fMax);
      SetWindowText(GetDlgItem(hWnd, IDC_MAX), Text);
      sprintf(Text, "%lf", Script->Entries[Entry].fValue);
      SetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text);
      ShowWindow(GetDlgItem(hWnd, IDC_MIN),     TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_MAX),     TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_VALUE),   TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC1), TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC2), TRUE);
      ShowWindow(GetDlgItem(hWnd, IDC_STATIC3), TRUE);
      break;

    case _CHOICE:
      SetWindowText(GetDlgItem(hWnd, IDC_FRAME), " Choice Entry ");
      SendMessage(GetDlgItem(hWnd, IDC_CHOICE), CB_RESETCONTENT, 0, 0);
      if(ActionPtr->Entries[Entry].NumChoices) {
        for(i=0;i<ActionPtr->Entries[Entry].NumChoices;i++) 
          SendMessage(GetDlgItem(hWnd, IDC_CHOICE), CB_ADDSTRING, 0, (LPARAM)ActionPtr->Entries[Entry].Choices[i]);
        SendMessage(GetDlgItem(hWnd, IDC_CHOICE), CB_SETCURSEL, Script->Entries[Entry].Selection, 0);
        ShowWindow(GetDlgItem(hWnd, IDC_CHOICE), TRUE);
      }
      break;
  }
}

void UpdateEntry(HWND hWnd, sScript *Script, long Entry)
{
  sAction *ActionPtr;
  char Text[2048];

  // Get pointer to action
  ActionPtr = g_Actions.GetAction(Script->Type);

  // Return if no entries to update
  if(!ActionPtr->NumEntries)
    return;

  // Return on incorrect Entry #
  if(Entry >= Script->NumEntries)
    return;

  // Update fields based on type
  switch(Script->Entries[Entry].Type) {
    case _TEXT:
      // Delete old text
      delete [] Script->Entries[Entry].Text;
      Script->Entries[Entry].Text = NULL;
      Script->Entries[Entry].Length = 0;

      // Get new text
      GetWindowText(GetDlgItem(hWnd, IDC_TEXT), Text, 2048);
      if(Text[0]) {
        Script->Entries[Entry].Length = strlen(Text) + 1;
        Script->Entries[Entry].Text = new char[Script->Entries[Entry].Length];
        strcpy(Script->Entries[Entry].Text, Text);
      }
      break;

    case _BOOL:
      // Choose TRUE or FALSE from radio buttons
      if(SendMessage(GetDlgItem(hWnd, IDC_TRUE), BM_GETCHECK, 0, 0) == BST_CHECKED)
        Script->Entries[Entry].bValue = TRUE;
      else
        Script->Entries[Entry].bValue = FALSE;
      break;

    case _INT:
      // Get int value and bounds check with min/max
      GetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text, 2048);
      Script->Entries[Entry].lValue = atol(Text);
      if(Script->Entries[Entry].lValue < ActionPtr->Entries[Entry].lMin)
        Script->Entries[Entry].lValue = ActionPtr->Entries[Entry].lMin;
      if(Script->Entries[Entry].lValue > ActionPtr->Entries[Entry].lMax)
        Script->Entries[Entry].lValue = ActionPtr->Entries[Entry].lMax;
      break;

    case _FLOAT:
      // Get float value and bounds check with min/max
      GetWindowText(GetDlgItem(hWnd, IDC_VALUE), Text, 2048);
      Script->Entries[Entry].fValue = (float)atof(Text);
      if(Script->Entries[Entry].fValue < ActionPtr->Entries[Entry].fMin)
        Script->Entries[Entry].fValue = ActionPtr->Entries[Entry].fMin;
      if(Script->Entries[Entry].fValue > ActionPtr->Entries[Entry].fMax)
        Script->Entries[Entry].fValue = ActionPtr->Entries[Entry].fMax;
      break;

    case _CHOICE:
      // Store choice selection
      Script->Entries[Entry].Selection = SendMessage(GetDlgItem(hWnd, IDC_CHOICE), CB_GETCURSEL, 0, 0);
      break;
  }
}

BOOL LoadActions(char *Filename)
{
  char Text[2048];

  // Ask for filename if none passed
  if(Filename == NULL) {
    // Setup the open dialog info
    g_ofn.hwndOwner   = g_hWnd;
    g_ofn.lpstrFile   = g_ActionFile;
    g_ofn.lpstrTitle  = "Load Actions File";
    g_ofn.lpstrFilter = "MLS Action Files (*.mla)\0*.mla\0All Files (*.*)\0*.*\0\0";
    g_ofn.lpstrDefExt = "mla";

    // Get action template file name
    if(!GetOpenFileName(&g_ofn))
      return FALSE;

    // Ask if sure to make new script
    if(NewScript() == FALSE)
      return FALSE;
  } else {
    // Copy over filename
    strcpy(g_ActionFile, Filename);
  }

  // Attempt to load actions
  if(g_Actions.Load(g_ActionFile) == FALSE) {
    MessageBox(g_hWnd, g_ActionFile, "Unable to open file.", MB_OK);
    return FALSE;
  }

  // Clear the list box
  SendMessage(GetDlgItem(g_hWnd, IDC_ACTIONS), LB_RESETCONTENT, 0, 0);

  // Get a pointer to the parent action
  sAction *Ptr = g_Actions.GetActionParent();

  // Loop through all actions
  for(long i=0;i<g_Actions.GetNumActions();i++) {

    // Get expanded action text
    g_Actions.ExpandDefaultActionText(Text, Ptr);

    // Add action text to action list
    SendMessage(GetDlgItem(g_hWnd, IDC_ACTIONS), LB_ADDSTRING, 0, (LPARAM)Text);

    // Go to next action
    Ptr = Ptr->Next;
  }

  return TRUE;
}

BOOL NewScript()
{
  if(MessageBox(g_hWnd, "Are you sure? (Discards any unsaved script information)", "New Script", MB_YESNO) == IDYES) {

    delete g_ScriptParent;
    g_ScriptParent = NULL;
    g_NumScriptActions = 0;

    SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_RESETCONTENT, 0, 0);
    SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_RESETCONTENT, 0, 0);
 
    return TRUE;
  }

  return FALSE;
}

BOOL LoadScript()
{
  FILE *fp;
  long i, j;
  char Text[2048];
  sScript *Script, *ScriptPtr = NULL;

  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_ScriptFile;
  g_ofn.lpstrTitle  = "Load Script File";
  g_ofn.lpstrFilter = "MLS Script Files (*.mls)\0*.mls\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "mls";

  // Ask for filename
  if(!GetOpenFileName(&g_ofn))
    return FALSE;

  // Open the file for input
  if((fp=fopen(g_ScriptFile, "rb"))==NULL) {
    MessageBox(g_hWnd, g_ScriptFile, "Unable to open file.", MB_OK);
    return FALSE;
  }

  // Delete the current script
  delete g_ScriptParent;

  // Get # of script actions
  fread(&g_NumScriptActions, 1, sizeof(long), fp);

  // Loop through each script action
  for(i=0;i<g_NumScriptActions;i++) {
    // Allocate a script structure and link in
    Script = new sScript();
    Script->Next = NULL;
    if(ScriptPtr == NULL)
      g_ScriptParent = Script;
    else
      ScriptPtr->Next = Script;
    ScriptPtr = Script;

    // Get type of action and # of entries
    fread(&Script->Type, 1, sizeof(long), fp);
    fread(&Script->NumEntries, 1, sizeof(long), fp);

    // Get entry data (if any)
    if(Script->NumEntries) {

      // Allocate entry array
      Script->Entries = new sScriptEntry[Script->NumEntries]();

      // Load in each entry
      for(j=0;j<Script->NumEntries;j++) {

        // Get entry type
        fread(&Script->Entries[j].Type, 1, sizeof(long), fp);
        
        // Get entry data
        fread(&Script->Entries[j].IOValue, 1, sizeof(long), fp);

        // Get text (if any)
        if(Script->Entries[j].Type == _TEXT && Script->Entries[j].Length) {
          // Allocate a buffer and get string
          Script->Entries[j].Text = new char[Script->Entries[j].Length];
          fread(Script->Entries[j].Text, 1, Script->Entries[j].Length, fp);
        }
      }
    }
  }

  fclose(fp);

  // Clear the script and script list boxes
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_RESETCONTENT, 0, 0);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_RESETCONTENT, 0, 0);

  // Add script actions to list
  ScriptPtr = g_ScriptParent;
  while(ScriptPtr != NULL) {
    // Add script action text to list
    g_Actions.ExpandActionText(Text, ScriptPtr);
    SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)Text);

    // Add script action pointer to list
    sprintf(Text, "%lu", ScriptPtr);
    SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_ADDSTRING, 0, (LPARAM)Text);

    ScriptPtr = ScriptPtr->Next;
  }
  
  return TRUE;
}

BOOL SaveScript()
{
  FILE *fp;
  long i, j;
  char Text[256];
  sScript *ScriptPtr;

  // Make sure there's some script actions
  if(!g_NumScriptActions) {
    MessageBox(g_hWnd, "No script actions exist!", "Error", MB_OK);
    return FALSE;
  }

  // Setup the open dialog info
  g_ofn.hwndOwner   = g_hWnd;
  g_ofn.lpstrFile   = g_ScriptFile;
  g_ofn.lpstrTitle  = "Save Script File";
  g_ofn.lpstrFilter = "MLS Script Files (*.mls)\0*.mls\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "mls";

  // Ask for filename
  if(!GetSaveFileName(&g_ofn))
    return FALSE;

  // Open the file for output
  if((fp=fopen(g_ScriptFile, "wb"))==NULL) {
    MessageBox(g_hWnd, g_ScriptFile, "Unable to save file.", MB_OK);
    return FALSE;
  }

  // Output # of script actions
  fwrite(&g_NumScriptActions, 1, sizeof(long), fp);

  // Loop through each script action
  for(i=0;i<g_NumScriptActions;i++) {

    // Get a pointer to the script structure (don't depend on list)
    SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPTLIST), LB_GETTEXT, i, (LPARAM)Text);
    ScriptPtr = (sScript*)atol(Text);

    // Output type of action and # of entries
    fwrite(&ScriptPtr->Type, 1, sizeof(long), fp);
    fwrite(&ScriptPtr->NumEntries, 1, sizeof(long), fp);

    // Output entry data (if any)
    if(ScriptPtr->NumEntries) {
      for(j=0;j<ScriptPtr->NumEntries;j++) {

        // Write entry type
        fwrite(&ScriptPtr->Entries[j].Type, 1, sizeof(long), fp);

        // Write entry data
        fwrite(&ScriptPtr->Entries[j].IOValue, 1, sizeof(long), fp);

        // Write text (if any)
        if(ScriptPtr->Entries[j].Type == _TEXT && ScriptPtr->Entries[j].Text != NULL)
          fwrite(ScriptPtr->Entries[j].Text, 1, ScriptPtr->Entries[j].Length, fp);
      }
    }
  }

  fclose(fp);

  return TRUE;
}
