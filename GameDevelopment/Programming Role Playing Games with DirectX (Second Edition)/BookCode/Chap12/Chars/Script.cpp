#include <windows.h>
#include <stdio.h>
#include "Script.h"

cScript::cScript()
{
  m_NumActions = 0;
  m_ScriptParent = NULL;
}

cScript::~cScript()
{
  Free();
}

BOOL cScript::Load(char *Filename)
{
  FILE *fp;
  long i, j;
  sScript *ScriptHead = NULL, *Script, *ScriptPtr = NULL;

  Free();  // Free prior script

  // Open the file for input
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Get # of script actions and store them
  fread(&m_NumActions, 1, sizeof(long), fp);

  // Loop through each script action
  for(i=0;i<m_NumActions;i++) {
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
        fread(&Script->Entries[j].IOValue,1,sizeof(long),fp);

        // Get text (if any)
        if(Script->Entries[j].Type == _TEXT &&                \
           Script->Entries[j].Length) {
          Script->Entries[j].Text =                           \
                          new char[Script->Entries[j].Length];
          fread(Script->Entries[j].Text, 1,                   \
                          Script->Entries[j].Length, fp);
        }
      }
    }
  }

  fclose(fp);

  m_ScriptParent = ScriptHead;

  return TRUE;
}

BOOL cScript::Free()
{
  delete m_ScriptParent;
  m_ScriptParent = NULL;
  m_NumActions = 0;

  return TRUE;
}

BOOL cScript::Execute(char *Filename)
{
  sScript *ScriptPtr;

  // Load script if none already
  if(Filename != NULL)
    Load(Filename);

  // Prepare script data for execution
  if(Prepare() == FALSE)
    return FALSE;

  // Start at beginning of script
  if((ScriptPtr = m_ScriptParent) == NULL)
    return FALSE;

  // Loop until no more script actions
  while(ScriptPtr != NULL) {
    // Call script function and break on NULL return value.
    // Any other return type is the pointer to the next function,
    // which is typically ScriptPtr->Next.
    ScriptPtr = Process(ScriptPtr);
  }

  // Release data
  Release();

  // Release script if execute loaded it
  if(Filename != NULL)
    Free();

  return TRUE;
}

sScript *cScript::GetParentScript()
{
  return m_ScriptParent;
}
