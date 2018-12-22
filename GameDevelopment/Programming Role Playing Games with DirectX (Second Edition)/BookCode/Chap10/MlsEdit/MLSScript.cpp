/**************************************************
WinMain.cpp
Chapter 14 Mad Lib Script Editor

Programming Role-Playing Games with DirectX
by Jim Adams (01 Jan 2002)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "MLSScript.h"

cMLSScript::cMLSScript()
{
  m_NumScriptActions = 0;
  m_NumActions       = 0;
  m_ActionParent     = NULL;
}

cMLSScript::~cMLSScript()
{
  FreeScript();
  FreeActions();
}

BOOL cMLSScript::LoadScript(char *Filename)
{
  FILE *fp;
  sAction *ActionPtr;
  sEntry  *EntryPtr;
  long i, j, Size;

  // Free a prior script
  FreeScript();

  // Open the script file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Get the number of actions in script
  fread(&m_NumScriptActions, 1, sizeof(long), fp);

  // Return an error if no scripts
  if(!m_NumScriptActions) {
    fclose(fp);
    return FALSE;
  }

  // Read script actions
  ActionPtr = &m_ScriptActionParent;
  for(i=0;i<m_NumScriptActions;i++) {
    // Create a new action structure to use
    ActionPtr->Next = new sAction();
    ActionPtr = ActionPtr->Next;

    // Get action ID
    fread(&ActionPtr->ID, 1, sizeof(long), fp);

    // Get the number of entries
    fread(&ActionPtr->NumEntries, 1, sizeof(long), fp);

    // Get entries (if any)
    if(ActionPtr->NumEntries) {
      ActionPtr->Entries = new sEntry[ActionPtr->NumEntries];

      for(j=0;j<ActionPtr->NumEntries;j++) {
        EntryPtr = &ActionPtr->Entries[i];

        // TEXT
        if(EntryPtr->Type == _TEXT) {
          // Get point to TEXT entry
          sTEXTEntry *Ptr = (sTEXTEntry*)EntryPtr->Data;

          // Get max. length of string and allocate buffer
          fread(&Ptr->Length, 1, sizeof(long), fp);
          Ptr->Text = new char[Ptr->Length];
          ZeroMemory(Ptr->Text, Ptr->Length);

          // Get size of the text string
          fread(&Size, 1, sizeof(long), fp);

          // Get text (if any)
          if(Size) 
            fread(Ptr->Text, 1, Size, fp);
        } else

        // BOOL
        if(EntryPtr->Type == _BOOL) {
          // Get point to BOOL entry
          sBOOLEntry *Ptr = (sBOOLEntry*)EntryPtr->Data;

          // Get BOOL value
          fread(&Ptr->Value, 1, sizeof(BOOL), fp);
        } else

        // INT
        if(EntryPtr->Type == _INT) {
          // Get point to INT entry
          sINTEntry *Ptr = (sINTEntry*)EntryPtr->Data;

          // Get value
          fread(&Ptr->Value, 1, sizeof(long), fp);
        } else

        // FLOAT
        if(EntryPtr->Type == _FLOAT) {
          // Get point to FLOAT entry
          sFLOATEntry *Ptr = (sFLOATEntry*)EntryPtr->Data;

          // Get value
          fread(&Ptr->Value, 1, sizeof(float), fp);
        } else

        // CHOICE
        if(EntryPtr->Type == _CHOICE) {
          // Get point to CHOICE entry
          sCHOICEEntry *Ptr = (sCHOICEEntry*)EntryPtr->Data;

          // Get value
          fread(&Ptr->Selection, 1, sizeof(float), fp);
        }
      }
    }
  }

  // Close file
  fclose(fp);

  // Return success
  return TRUE;
}

BOOL cMLSScript::SaveScript(char *Filename)
{
  FILE *fp;
  sAction *ActionPtr;
  sEntry  *EntryPtr;
  long i;

  // Make sure there's some script actions to write
  if(!m_NumScriptActions)
    return FALSE;

  // Create the new file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Output number of actions in script
  fwrite(&m_NumScriptActions, 1, sizeof(long), fp);

  // Output script actions
  ActionPtr = m_ScriptActionParent.Next;
  while(ActionPtr != NULL) {

    // Output action ID
    fwrite(&ActionPtr->ID, 1, sizeof(long), fp);

    // Output the number of entries
    fwrite(&ActionPtr->NumEntries, 1, sizeof(long), fp);

    // Output entries (if any)
    if(ActionPtr->NumEntries) {
      for(i=0;i<ActionPtr->NumEntries;i++) {
        EntryPtr = &ActionPtr->Entries[i];

        // TEXT
        if(EntryPtr->Type == _TEXT) {
          // Get pointer to TEXT entry
          sTEXTEntry *Ptr = (sTEXTEntry*)EntryPtr->Data;

          // Output max. length of string
          fwrite(&Ptr->Length, 1, sizeof(long), fp);

          // Output size of the text string
          fwrite((void*)(strlen(Ptr->Text)+1), 1, sizeof(long), fp);

          // Output text (if any)
          if(strlen(Ptr->Text))
            fwrite(Ptr->Text, 1, strlen(Ptr->Text)+1, fp);
        } else

        // BOOL
        if(EntryPtr->Type == _BOOL) {
          // Get pointer to BOOL entry
          sBOOLEntry *Ptr = (sBOOLEntry*)EntryPtr->Data;

          // Output BOOL value
          fwrite(&Ptr->Value, 1, sizeof(BOOL), fp);
        } else

        // INT
        if(EntryPtr->Type == _INT) {
          // Get pointer to INT entry
          sINTEntry *Ptr = (sINTEntry*)EntryPtr->Data;

          // Output value
          fwrite(&Ptr->Value, 1, sizeof(long), fp);
        } else

        // FLOAT
        if(EntryPtr->Type == _FLOAT) {
          // Get pointer to FLOAT entry
          sFLOATEntry *Ptr = (sFLOATEntry*)EntryPtr->Data;

          // Output value
          fwrite(&Ptr->Value, 1, sizeof(float), fp);
        } else

        // CHOICE
        if(EntryPtr->Type == _CHOICE) {
          // Get pointer to CHOICE entry
          sCHOICEEntry *Ptr = (sCHOICEEntry*)EntryPtr->Data;

          // Output value
          fwrite(&Ptr->Selection, 1, sizeof(float), fp);
        }
      }
    }

    // Go to next action
    ActionPtr = ActionPtr->Next;
  }

  // Close file
  fclose(fp);

  // Return success
  return TRUE;
}

BOOL cMLSScript::FreeScript()
{
  delete m_ScriptActionParent.Next;
  m_ScriptActionParent.Next = NULL;
  m_NumScriptActions        = 0;
  
  return TRUE;
}

BOOL cMLSScript::AddScriptAction(sAction *Action)
{
  Action->Next = m_ScriptActionParent.Next;
  m_ScriptActionParent.Next = Action;
  return TRUE;
}

sAction *cMLSScript::GetScriptActionParent()
{
  return &m_ScriptActionParent;
}

BOOL cMLSScript::LoadActions(char *Filename)
{
  FILE *fp;
  char Text[2048];
  sAction *Action;
  long i, j;
  
  // Free previous action structures
  FreeActions();

  // Open the action file
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Keep looping until end of file found
  while(1) {
    // Get next quoted action
    if(GetNextQuotedLine(Text, fp, 2048) == FALSE)
      break;

    // Quit if no action text
    if(!Text[0])
      break;

    // Allocate an action structure and link it in
    Action         = new sAction();
    Action->Next   = m_ActionParent;
    m_ActionParent = Action;

    // Copy action text
    strcpy(Action->Text, Text);

    // Store action ID
    Action->ID = m_NumActions;

    // Increate the number of actions loaded
    m_NumActions++;

    // Count the number of entries in the action
    for(i=0;i<(long)strlen(Text);i++) {
      if(Text[i] == '~')
        Action->NumEntries++;
    }

    // Allocate and read in entries (if any)
    if(Action->NumEntries) {
      Action->Entries = new sEntry[Action->NumEntries]();
      for(i=0;i<Action->NumEntries;i++) {

        // Get type of Entry entry
        GetNextWord(Text, fp, 2048);
        
        // TEXT type, get max length
        if(!stricmp(Text, "TEXT")) {
          // Set to text type and allocate TEXT entry
          Action->Entries[i].Type = _TEXT;
          Action->Entries[i].Data = (void*)new sTEXTEntry();
          sTEXTEntry *Ptr = (sTEXTEntry*)Action->Entries[i].Data;

          // Get max length of text
          GetNextWord(Text, fp, 2048);
          Ptr->Length = atol(Text);

          // Create text buffer and clear it out
          Ptr->Text = new char[Ptr->Length];
          ZeroMemory(Ptr->Text, Ptr->Length);
        } else

        // INT type, get min and max values
        if(!stricmp(Text, "INT")) {
          // Set to INT type and allocate INT entry
          Action->Entries[i].Type = _INT;
          Action->Entries[i].Data = (void*)new sINTEntry();
          sINTEntry *Ptr = (sINTEntry*)Action->Entries[i].Data;

          // Get min value
          GetNextWord(Text, fp, 2048);
          Ptr->Min = atol(Text);

          // Get max value
          GetNextWord(Text, fp, 2048);
          Ptr->Max = atol(Text);

          // Reset value to min
          Ptr->Value = Ptr->Min;
        } else

        // FLOAT type, get min and max values
        if(!stricmp(Text, "FLOAT")) {
          // Set to FLOAT type and allocate FLOAT entry
          Action->Entries[i].Type = _FLOAT;
          Action->Entries[i].Data = (void*)new sFLOATEntry();
          sFLOATEntry *Ptr = (sFLOATEntry*)Action->Entries[i].Data;

          // Get min value
          GetNextWord(Text, fp, 2048);
          Ptr->Min = (float)atof(Text);

          // Get max value
          GetNextWord(Text, fp, 2048);
          Ptr->Max = (float)atof(Text);

          // Reset value to min
          Ptr->Value = Ptr->Min;
        } else

        // BOOL type, no options
        if(!stricmp(Text, "BOOL")) {
          // Set to BOOL type and allocate BOOL entry
          Action->Entries[i].Type = _BOOL;
          Action->Entries[i].Data = (void*)new sBOOLEntry();
          sBOOLEntry *Ptr = (sBOOLEntry*)Action->Entries[i].Data;

          // Set value to TRUE
          Ptr->Value = TRUE;
        } else

        // CHOICE type, get number of entries and entry's texts
        if(!stricmp(Text, "CHOICE")) {
          // Set to CHOICE type and allocate CHOICE entry
          Action->Entries[i].Type = _CHOICE;
          Action->Entries[i].Data = (void*)new sCHOICEEntry();
          sCHOICEEntry *Ptr = (sCHOICEEntry*)Action->Entries[i].Data;

          // Get the number of choices
          GetNextWord(Text, fp, 1024);
          Ptr->Num = atol(Text);
          Ptr->Text = new char*[Ptr->Num];

          // Get each entry text
          for(j=0;j<Ptr->Num;j++) {
            Ptr->Text[j] = new char[256];
            GetNextQuotedLine(Ptr->Text[j], fp, 256);
          }

          // Set choice to 1st
          Ptr->Selection = 0;
        }
      }
    }
  }
  
  fclose(fp);

  return TRUE;
}

BOOL cMLSScript::FreeActions()
{
  delete m_ActionParent;
  m_ActionParent = NULL;
  m_NumActions = 0;

  return TRUE;
}

BOOL cMLSScript::GetNextQuotedLine(char *Data, FILE *fp, long MaxSize)
{
  int c;
  long Pos = 0;
  
  // Read until a quote is reached (or EOF)
  while(1) {
    if((c = fgetc(fp)) == EOF)
      return FALSE;

    if(c == '"') {
      // Read until next quote (or EOF)
      while(1) {
        if((c = fgetc(fp)) == EOF)
          return FALSE;

        // Return text when 2nd quote found
        if(c == '"') {
          Data[Pos] = 0;
          return TRUE;
        }

        // Add acceptable text to line
        if(c != 0x0a && c != 0x0d) {
          if(Pos < MaxSize-1)
            Data[Pos++] = c;
        }
      }
    }
  }
}

BOOL cMLSScript::GetNextWord(char *Data, FILE *fp, long MaxSize)
{
  int  c;
  long Pos = 0;

  // Reset word to empty
  Data[0] = 0;

  // Read until an acceptable character found
  while(1) {
    if((c = fgetc(fp)) == EOF) {
      Data[0] = 0;
      return FALSE;
    }

    // Check for start of word
    if(c != 32 && c != 0x0a && c != 0x0d) {
      Data[Pos++] = c;

      // Loop until end of word (or EOF)
      while((c=fgetc(fp)) != EOF) {
        // Break on acceptable word seperators
        if(c == 32 || c == 0x0a || c == 0x0d)
          break;

        // Add if enough room left
        if(Pos < MaxSize-1)
          Data[Pos++] = c;
      }

      // Add end of line to text
      Data[Pos] = 0;

      return TRUE;
    }
  }
}

long cMLSScript::GetNumActions()
{
  return m_NumActions;
}

sAction *cMLSScript::GetActionParent()
{
  return m_ActionParent;
}

sAction *cMLSScript::GetAction(long Num)
{
  long i;
  sAction *Ptr;

  if(Num >= m_NumActions)
    return NULL;

  Ptr = m_ActionParent;
  for(i=0;i<(m_NumActions-Num-1);i++)
    Ptr = Ptr->Next;

  return Ptr;
}

BOOL cMLSScript::ExpandDefaultActionText(char *Buffer, sAction *Action)
{
  long i, Pos, Num;
  char Text[256];

  // Copy action text into buffer if no Entries
  if(!Action->NumEntries) {
    strcpy(Buffer, Action->Text);
    return TRUE;
  }
  
  // Expand Entry types into action text
  Pos = 0;
  Num = 0;
  for(i=0;i<(long)strlen(Action->Text);i++) {

    // Expand the Entry into text based on values, text, etc
    if(Action->Text[i] == '~') {

      if(Action->Entries[Num].Type == _TEXT) {
        memcpy(&Buffer[Pos], "(*TEXT*)", 8);
        Pos += 8;
      } else 

      if(Action->Entries[Num].Type == _INT) {
        sINTEntry *Ptr = (sINTEntry*)Action->Entries[Num].Data;
        sprintf(Text, "(*%lu*)", Ptr->Min);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _FLOAT) {
        sFLOATEntry *Ptr = (sFLOATEntry*)Action->Entries[Num].Data;
        sprintf(Text, "(*%lf*)", Ptr->Min);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _BOOL) {
        memcpy(&Buffer[Pos], "(*TRUE*)", 7);
        Pos += 7;
      } else

      if(Action->Entries[Num].Type == _CHOICE) {
        sCHOICEEntry *Ptr = (sCHOICEEntry*)Action->Entries[Num].Data;
        memcpy(&Buffer[Pos], "(*", 2);
        Pos+=2;
        memcpy(&Buffer[Pos], Ptr->Text[0], strlen(Ptr->Text[0]));
        Pos += strlen(Ptr->Text[0]);
        memcpy(&Buffer[Pos], "*)", 2);
        Pos+=2;
      }

      Num++;
    } else {
      Buffer[Pos++] = Action->Text[i];
    }
  }
  Buffer[Pos] = 0;

  return TRUE;
}

BOOL cMLSScript::ExpandActionText(char *Buffer, sAction *Action)
{
  long i, j, Pos, Num;
  char Text[256];

  // Copy action text into buffer if no Entries
  if(!Action->NumEntries) {
    strcpy(Buffer, Action->Text);
    return TRUE;
  }
  
  // Expand Entry types into action text
  Pos = 0;
  Num = 0;

  for(i=0;i<(long)strlen(Action->Text);i++) {

    // Expand the Entry into text based on values, text, etc
    if(Action->Text[i] == '~') {

      if(Action->Entries[Num].Type == _TEXT) {
        sTEXTEntry *Ptr = (sTEXTEntry*)Action->Entries[Num].Data;

        // Prefix characters
        memcpy(&Buffer[Pos], "(*", 2);
        Pos+=2;

        // Copy over at most 16 characters
        for(j=0;j<16;j++) {
          if(!Ptr->Text[j])
            break;

          Buffer[Pos++] = Ptr->Text[j];
        }

        // Append characters
        memcpy(&Buffer[Pos], "*)", 2);
        Pos+=2;
      } else 

      if(Action->Entries[Num].Type == _INT) {
        sINTEntry *Ptr = (sINTEntry*)Action->Entries[Num].Data;
        sprintf(Text, "(*%lu*)", Ptr->Value);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _FLOAT) {
        sFLOATEntry *Ptr = (sFLOATEntry*)Action->Entries[Num].Data;
        sprintf(Text, "(*%lf*)", Ptr->Value);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _BOOL) {
        sBOOLEntry *Ptr = (sBOOLEntry*)Action->Entries[Num].Data;
        if(Ptr->Value == TRUE)
          memcpy(&Buffer[Pos], "(*TRUE *)", 8);
        else
          memcpy(&Buffer[Pos], "(*FALSE*)", 8);
        Pos += 8;
      } else

      if(Action->Entries[Num].Type == _CHOICE) {
        sCHOICEEntry *Ptr = (sCHOICEEntry*)Action->Entries[Num].Data;
        memcpy(&Buffer[Pos], "(*", 2);
        Pos+=2;
        memcpy(&Buffer[Pos], Ptr->Text[Ptr->Selection], strlen(Ptr->Text[Ptr->Selection]));
        Pos += strlen(Ptr->Text[Ptr->Selection]);
        memcpy(&Buffer[Pos], "*)", 2);
        Pos+=2;
      }

      Num++;
    } else {
      Buffer[Pos++] = Action->Text[i];
    }
  }
  Buffer[Pos] = 0;

  return TRUE;
}
