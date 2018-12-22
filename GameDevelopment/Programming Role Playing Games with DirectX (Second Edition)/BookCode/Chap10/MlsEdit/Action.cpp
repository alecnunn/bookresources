/**************************************************
WinMain.cpp
Chapter 10 Mad Lib Script Editor

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

#include <windows.h>
#include <stdio.h>
#include "Action.h"

cActionTemplate::cActionTemplate()
{
  m_NumActions   = 0;
  m_ActionParent = NULL;
}

cActionTemplate::~cActionTemplate()
{
  Free();
}

BOOL cActionTemplate::Load(char *Filename)
{
  FILE *fp;
  char Text[2048];
  sAction *Action, *ActionPtr = NULL;
  sEntry  *Entry;
  long i, j;
  
  // Free previous action structures
  Free();

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

    // Allocate an action structure and append it to list
    Action         = new sAction();
    Action->Next   = NULL;
    if(ActionPtr == NULL)
      m_ActionParent = Action;
    else 
      ActionPtr->Next = Action;
    ActionPtr = Action;

    // Copy action text
    strcpy(Action->Text, Text);

    // Store action ID
    Action->ID = m_NumActions;

    // Increase the number of actions loaded
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
        Entry = &Action->Entries[i];

        // Get type of Entry entry
        GetNextWord(Text, fp, 2048);
        
        // TEXT type, nothing data
        if(!stricmp(Text, "TEXT")) {
          // Set to text type
          Entry->Type = _TEXT;
        } else // If not TEXT, then check INT type

        // INT type, get min and max values
        if(!stricmp(Text, "INT")) {
          // Set to INT type and allocate INT entry
          Entry->Type = _INT;

          // Get min value
          GetNextWord(Text, fp, 2048);
          Entry->lMin = atol(Text);

          // Get max value
          GetNextWord(Text, fp, 2048);
          Entry->lMax = atol(Text);
        } else // If not INT, then check FLOAT type

        // FLOAT type, get min and max values
        if(!stricmp(Text, "FLOAT")) {
          // Set to FLOAT type and allocate FLOAT entry
          Entry->Type = _FLOAT;

          // Get min value
          GetNextWord(Text, fp, 2048);
          Entry->fMin = (float)atof(Text);

          // Get max value
          GetNextWord(Text, fp, 2048);
          Entry->fMax = (float)atof(Text);
        } else // If not FLOAT, then check BOOL type

        // BOOL type, no options
        if(!stricmp(Text, "BOOL")) {
          // Set to BOOL type and allocate BOOL entry
          Entry->Type = _BOOL;
        } else // If not BOOL, then check CHOICE type

        // CHOICE type, get number of entries and entry's texts
        if(!stricmp(Text, "CHOICE")) {
          // Set to CHOICE type and allocate CHOICE entry
          Entry->Type = _CHOICE;

          // Get the number of choices
          GetNextWord(Text, fp, 1024);
          Entry->NumChoices = atol(Text);
          Entry->Choices = new char*[Entry->NumChoices];

          // Get each entry text
          for(j=0;j<Entry->NumChoices;j++) {
            GetNextQuotedLine(Text, fp, 2048);
            Entry->Choices[j] = strdup(Text);
          }
        }
      }
    }
  }
  
  fclose(fp);

  return TRUE;
}

BOOL cActionTemplate::Free()
{
  delete m_ActionParent;
  m_ActionParent = NULL;
  m_NumActions = 0;

  return TRUE;
}

BOOL cActionTemplate::GetNextQuotedLine(char *Data, FILE *fp, long MaxSize)
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

BOOL cActionTemplate::GetNextWord(char *Data, FILE *fp, long MaxSize)
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

long cActionTemplate::GetNumActions()
{
  return m_NumActions; // return # actions in template
}

sAction *cActionTemplate::GetActionParent()
{
  return m_ActionParent; // return parent sAction structure
}

sAction *cActionTemplate::GetAction(long Num)
{
  sAction *Ptr;

  // Return error if higher then # actions
  if(Num >= m_NumActions)
    return NULL;

  // Scan list for ID == Num
  if((Ptr = m_ActionParent) != NULL) {
    while(Ptr != NULL) {
      if(Ptr->ID == Num)
        return Ptr;
      Ptr = Ptr->Next;
    }
  }
  return NULL;
}

sScript *cActionTemplate::CreateScriptAction(long Type)
{
  long i;
  sScript *Script;
  sAction *ActionPtr;

  // Make sure it's a valid action - Type is really the
  // action ID (from the list of actions already loaded).
  if(Type >= m_NumActions)
    return NULL;

  // Get pointer to action
  if((ActionPtr = GetAction(Type)) == NULL)
    return NULL;

  // Create new sScript structure
  Script = new sScript();

  // Set type and number of entries (allocating a list)
  Script->Type       = Type;
  Script->NumEntries = ActionPtr->NumEntries;
  Script->Entries    = new sScriptEntry[Script->NumEntries]();

  // Set up each entry
  for(i=0;i<Script->NumEntries;i++) {
    // Save type
    Script->Entries[i].Type = ActionPtr->Entries[i].Type;

    // Set up entry data based on type
    switch(Script->Entries[i].Type) {
      case _TEXT: 
        Script->Entries[i].Text = NULL;
        break;

      case _INT:
        Script->Entries[i].lValue = ActionPtr->Entries[i].lMin;
        break;

      case _FLOAT:
        Script->Entries[i].fValue = ActionPtr->Entries[i].fMin;
        break;

      case _BOOL:
        Script->Entries[i].bValue = TRUE;
        break;

      case _CHOICE:
        Script->Entries[i].Selection = 0;
        break;
    }
  }

  return Script;
}

long cActionTemplate::GetNumEntries(long ActionNum)
{
  // Get pointer to specific action
  sAction *Ptr = GetAction(ActionNum);

  // Return 0 on error
  if(Ptr == NULL)
    return 0;

  // Return # entries in action
  return Ptr->NumEntries;
}

sEntry *cActionTemplate::GetEntry(long ActionNum, long EntryNum)
{
  // Get pointer to action and return on error
  sAction *Ptr = GetAction(ActionNum);
  if(Ptr == NULL)
    return NULL;

  // Return error if higher then number of entries in action
  if(EntryNum >= Ptr->NumEntries)
    return NULL;

  // Return entry pointer
  return &Ptr->Entries[EntryNum];
}

BOOL cActionTemplate::ExpandDefaultActionText(char *Buffer, sAction *Action)
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
        sprintf(Text, "(*%lu*)", Action->Entries[Num].lMin);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _FLOAT) {
        sprintf(Text, "(*%lf*)", Action->Entries[Num].fMin);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      if(Action->Entries[Num].Type == _BOOL) {
        memcpy(&Buffer[Pos], "(*TRUE*)", 8);
        Pos += 8;
      } else

      if(Action->Entries[Num].Type == _CHOICE) {
        memcpy(&Buffer[Pos], "(*", 2);
        Pos+=2;
        memcpy(&Buffer[Pos], Action->Entries[Num].Choices[0], strlen(Action->Entries[Num].Choices[0]));
        Pos += strlen(Action->Entries[Num].Choices[0]);
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

BOOL cActionTemplate::ExpandActionText(char *Buffer, sScript *Script)
{
  long i, j, Pos, Num;
  char Text[256];
  sAction *ActionPtr;

  // Get a pointer to the specific action
  ActionPtr = GetAction(Script->Type);
  if(ActionPtr == NULL)
    return FALSE;

  // Copy action text into buffer if no entries
  if(!ActionPtr->NumEntries) {
    strcpy(Buffer, ActionPtr->Text);
    return TRUE;
  }
  
  // Expand Entry types into action text
  Pos = 0;
  Num = 0;
  for(i=0;i<(long)strlen(ActionPtr->Text);i++) {

    // Expand the Entry into text based on values, text, etc
    if(ActionPtr->Text[i] == '~') {

      // Expand TEXT type
      if(ActionPtr->Entries[Num].Type == _TEXT) {
        if(Script->Entries[Num].Type == _TEXT) {
          memcpy(&Buffer[Pos], "(*", 2);
          Pos += 2;

          if(Script->Entries[Num].Text != NULL) {
            for(j=0;j<16;j++) {
              if(!Script->Entries[Num].Text[j])
                break;
              Buffer[Pos++] = Script->Entries[Num].Text[j];
            }
          }

          memcpy(&Buffer[Pos], "*)", 2);
          Pos += 2;
        }
      }  else

      // Expand INT type
      if(ActionPtr->Entries[Num].Type == _INT) {
        sprintf(Text, "(*%lu*)", Script->Entries[Num].lValue);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      // Expand FLOAT type
      if(ActionPtr->Entries[Num].Type == _FLOAT) {
        sprintf(Text, "(*%lf*)", Script->Entries[Num].fValue);
        memcpy(&Buffer[Pos], Text, strlen(Text));
        Pos += strlen(Text);
      } else

      // Expand BOOL type
      if(ActionPtr->Entries[Num].Type == _BOOL) {
        if(Script->Entries[Num].bValue == TRUE)
          memcpy(&Buffer[Pos], "(*TRUE *)", 9);
        else
          memcpy(&Buffer[Pos], "(*FALSE*)", 9);
        Pos += 9;
      } else

      // Expand CHOICE type
      if(ActionPtr->Entries[Num].Type == _CHOICE) {
        memcpy(&Buffer[Pos], "(*", 2);
        Pos+=2;

        memcpy(&Buffer[Pos], ActionPtr->Entries[Num].Choices[Script->Entries[Num].Selection], strlen(ActionPtr->Entries[Num].Choices[Script->Entries[Num].Selection]));
        Pos += strlen(ActionPtr->Entries[Num].Choices[Script->Entries[Num].Selection]);

        memcpy(&Buffer[Pos], "*)", 2);
        Pos+=2;
      }

      Num++;
    } else {
      Buffer[Pos++] = ActionPtr->Text[i];
    }
  }
  Buffer[Pos] = 0;

  return TRUE;
}
