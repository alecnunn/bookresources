/**************************************************
WinMain.cpp
Chapter 10 Mad Lib Script Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "resource.h"

// Script structures ////////////////////////////////////////////
enum Types { _NONE = 0, _TEXT, _BOOL, _INT, _FLOAT, _CHOICE };

typedef struct sScriptEntry
{
  long   Type;         // Type of entry

  union {
    long   IOValue;    // Used for saving/loading
    long   Length;     // Length of text (w/0 terminator)
    long   Selection;  // Selection in choice
    BOOL   bValue;     // BOOL value
    long   lValue;     // long value
    float  fValue;     // float value
  };
  char    *Text;       // Text buffer

  sScriptEntry()
  {
    Type = _NONE;
    IOValue = 0;
    Text = NULL;
  }

  ~sScriptEntry()
  {
    delete [] Text;
  }
} sScriptEntry;

typedef struct sScript
{
  long          Type;        // 0-(#actions)-1
  long          NumEntries;  // # entries
  sScriptEntry *Entries;     // Array of entries

  sScript      *Prev;        // Prev in list
  sScript      *Next;        // Next in a list

  sScript()
  { 
    Type = 0;
    NumEntries = 0;
    Entries = NULL;
    Prev = Next = NULL;
  }

  ~sScript()
  {
    delete [] Entries;
    delete Next;
  }
} sScript;

// Application variables ////////////////////////////////////////////
HWND g_hWnd;                   // Window handle
char g_szClass[] = "MLSDEMO";  // Class name

long     g_NumScriptActions = 0; // # script actions
sScript *g_ScriptParent = NULL;  // Script data
BOOL     g_Flags[256];           // script flags

OPENFILENAME g_ofn;                   // Open dialog data
char         g_ScriptFile[MAX_PATH];  // Filename for script files

// The script function prototypes
sScript *Script_IfFlagThen(sScript*);
sScript *Script_Else(sScript*);
sScript *Script_EndIf(sScript*);
sScript *Script_SetFlag(sScript*);
sScript *Script_Print(sScript*);
sScript *Script_Move(sScript*);
sScript *Script_GainLoss(sScript*);
sScript *Script_Battle(sScript*);
sScript *Script_End(sScript*);

// The script action execution structure
typedef struct sScriptProcesses {
  sScript *(*Func)(sScript *ScriptPtr);
} sScriptProcesses;

// The list of script action structure functions
sScriptProcesses ScriptProcesses[] = {
  { Script_IfFlagThen },
  { Script_Else       },
  { Script_EndIf      },
  { Script_SetFlag    },
  { Script_Print      }, 
  { Script_Move       },
  { Script_GainLoss   },
  { Script_Battle     },
  { Script_End        } 
};
  
// Application prototypes ///////////////////////////////////////////
int  PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

sScript *LoadScript(char *Filename, long *NumActions = NULL);
BOOL     RunScript();

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
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DEMO), 0, NULL);
  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Free script actions
  delete g_ScriptParent;

  // Clean up
  UnregisterClass(g_szClass, hInst);

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {

        // Load a script file
        case IDC_LOAD: 
          // Ask for filename
          if(!GetOpenFileName(&g_ofn))
            break;

          // Delete the current script
          delete g_ScriptParent;
          if((g_ScriptParent = LoadScript(g_ScriptFile, &g_NumScriptActions)) == NULL)
            MessageBox(hWnd, g_ScriptFile, "Unable to open file.", MB_OK);

          // Display script file name
          SetWindowText(GetDlgItem(g_hWnd, IDC_TITLE), g_ScriptFile);
 
          break;

        // Run a script file
        case IDC_EXECUTE: 
          RunScript();
          break;
      }
      break;

    case WM_CREATE:
      // Initialize the save/load dialog box info
      ZeroMemory(&g_ofn, sizeof(OPENFILENAME));
      g_ofn.lStructSize = sizeof(OPENFILENAME);
      g_ofn.nMaxFile    = MAX_PATH;
      g_ofn.Flags       = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
      g_ofn.hwndOwner   = g_hWnd;
      g_ofn.lpstrFile   = g_ScriptFile;
      g_ofn.lpstrTitle  = "Load Script File";
      g_ofn.lpstrFilter = "MLS Script Files (*.mls)\0*.mls\0All Files (*.*)\0*.*\0\0";
      g_ofn.lpstrDefExt = "mls";
      g_ofn.nMaxFileTitle = MAX_PATH;

      // Clear script filename
      strcpy(g_ScriptFile, "..\\data\\test.mls");

      // Clear script output box
      SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_RESETCONTENT, 0, 0);

      break;


    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

sScript *LoadScript(char *Filename, long *NumActions)
{
  FILE *fp;
  long i, j, Num;
  sScript *ScriptHead = NULL, *Script, *ScriptPtr = NULL;

  // Open the file for input
  if((fp=fopen(Filename, "rb"))==NULL)
    return NULL;

  // Get # of script actions and store them
  fread(&Num, 1, sizeof(long), fp);
  if(NumActions != NULL)
    *NumActions = NULL;

  // Loop through each script action
  for(i=0;i<Num;i++) {
    // Allocate a script structure and link in
    Script = new sScript();
    Script->Next = NULL;
    if(ScriptPtr == NULL)
      ScriptHead = Script;
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
        // Get entry type and data
        fread(&Script->Entries[j].Type, 1, sizeof(long), fp);
        fread(&Script->Entries[j].IOValue, 1, sizeof(long), fp);

        // Get text (if any)
        if(Script->Entries[j].Type == _TEXT && Script->Entries[j].Length) {
          Script->Entries[j].Text = new char[Script->Entries[j].Length];
          fread(Script->Entries[j].Text, 1, Script->Entries[j].Length, fp);
        }
      }
    }
  }

  fclose(fp);

  return ScriptHead;
}

BOOL RunScript()
{
  sScript *ScriptPtr;

  // Clear flags 
  for(short i=0;i<256;i++)
    g_Flags[i] = FALSE;

  // Start at beginning of script
  if((ScriptPtr = g_ScriptParent) == NULL)
    return FALSE;

  // Clear the script output box
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_RESETCONTENT, 0, 0);

  // Loop until no more script actions
  while(ScriptPtr != NULL) {
    // Call script function and break on NULL return value.
    // Any other return type is the pointer to the next function,
    // which is typically ScriptPtr->Next.
    ScriptPtr = ScriptProcesses[ScriptPtr->Type].Func(ScriptPtr);
  }

  return TRUE;
}

sScript *Script_IfFlagThen(sScript *ScriptPtr)
{
  BOOL Skipping;

  // See if a flag matches second entry
  if(g_Flags[ScriptPtr->Entries[0].lValue % 256] == ScriptPtr->Entries[1].bValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;

  // At this point, Skipping states if the script actions
  // need to be skipped due to a conditional if..then statement.
  // Actions are further processed if skipped = FALSE, looking
  // for an else to flip the skip mode, or an endif to end
  // the conditional block.

  // Go to next action to process
  ScriptPtr = ScriptPtr->Next;

  while(ScriptPtr != NULL) {
    // if else, flip skip mode
    if(ScriptPtr->Type == 1)
      Skipping = (Skipping == TRUE) ? FALSE : TRUE;

    // break on end if
    if(ScriptPtr->Type == 2)
      return ScriptPtr->Next;

    // Process script function in conditional block
    // making sure to skip actions when condition not met.
    if(Skipping == TRUE)
      ScriptPtr = ScriptPtr->Next;
    else {
      if((ScriptPtr = ScriptProcesses[ScriptPtr->Type].Func(ScriptPtr)) == NULL)
        return NULL;
    }
  }
  return NULL; // end of script reached
}

sScript *Script_Else(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *Script_EndIf(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *Script_SetFlag(sScript *ScriptPtr)
{
  // Set boolean value
  g_Flags[ScriptPtr->Entries[0].lValue % 256] = ScriptPtr->Entries[1].bValue;
  return ScriptPtr->Next;
}

sScript *Script_Print(sScript *ScriptPtr)
{
  // Display text
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"Print string:");
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)ScriptPtr->Entries[0].Text);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"");

  return ScriptPtr->Next;
}

sScript *Script_Move(sScript *ScriptPtr)
{
  char Text[256];

  // Construct a string containing move coordinates
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"Moving character to:");
  sprintf(Text, "%lf, %lf, %lf", 
                  ScriptPtr->Entries[0].fValue,
                  ScriptPtr->Entries[1].fValue,
                  ScriptPtr->Entries[2].fValue);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)Text);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"");

  return ScriptPtr->Next;
}

sScript *Script_GainLoss(sScript *ScriptPtr)
{
  char Options[7][64] = {
    { "Main character" }, 
    { "Caster"         },
    { "Target"         },
    { "Gains"          },
    { "Looses"         },
    { "Hit"            },
    { "Magic"          }
  };
  char Text[1024];

  // Construct a string containing choices and display
  sprintf(Text, "%s %s %lu %s points", 
               Options[ScriptPtr->Entries[0].Selection],
               Options[ScriptPtr->Entries[1].Selection+3],
               ScriptPtr->Entries[2].lValue,
               Options[ScriptPtr->Entries[3].Selection+5]);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)Text);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"");

  return ScriptPtr->Next;
}

sScript *Script_Battle(sScript *ScriptPtr)
{
  char Text[256];

  // Show battle sequence #
  sprintf(Text, "Engaging in battle #%lu", ScriptPtr->Entries[0].lValue);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)Text);
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"");

  return ScriptPtr->Next;
}

sScript *Script_End(sScript *ScriptPtr)
{
  // Display an end of script dialog
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"End of Script");
  SendMessage(GetDlgItem(g_hWnd, IDC_SCRIPT), LB_ADDSTRING, 0, (LPARAM)"");

  return NULL;
}
