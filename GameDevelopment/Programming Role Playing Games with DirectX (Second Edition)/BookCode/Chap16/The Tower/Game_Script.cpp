#include "global.h"

///////////////////////////////////////////////////////////
// cGameScript class code
///////////////////////////////////////////////////////////
cGameScript::cGameScript()
{
  // Clear script data pointers
  m_App   = NULL;
  m_Route = NULL;

  // Clear flags and variables
  for(long i=0;i<256;i++) {
    m_Flags[i] = FALSE;
    m_Vars[i] = 0;
  }
}

cGameScript::~cGameScript()
{
  delete [] m_Route;  // Only need to free the internal route
  m_Window.Free();    // Free the text window object
}

BOOL cGameScript::SetData(cApp *App)
{ 
  // Remember the application class pointer
  if((m_App = App) == NULL)
    return FALSE;
  
  // Create a text window object
  m_Window.Create(&m_App->m_Graphics, &m_App->m_Font);

  return TRUE; 
}

BOOL cGameScript::Reset()
{
  // Clear flags and variables
  for(long i=0;i<256;i++) {
    m_Flags[i] = FALSE;
    m_Vars[i] = 0;
  }

  return TRUE;
}

BOOL cGameScript::Save(char *Filename)
{
  FILE *fp;

  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;
  fwrite(&m_Flags, 1, sizeof(m_Flags), fp);
  fwrite(&m_Vars, 1, sizeof(m_Vars), fp);
  fclose(fp);

  return TRUE;
}

BOOL cGameScript::Load(char *Filename)
{
  FILE *fp;

  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;
  fread(&m_Flags, 1, sizeof(m_Flags), fp);
  fread(&m_Vars, 1, sizeof(m_Vars), fp);
  fclose(fp);

  return TRUE;
}

BOOL cGameScript::Release()
{
  delete [] m_Route;
  m_Route          = NULL;
  m_NumRoutePoints = 0;

  return TRUE;
}

sScript *cGameScript::Process(sScript *Script)
{
  switch(Script->Type) {
    case  0: return Script_End(Script);
    case  1: return Script_Else(Script);
    case  2: return Script_EndIf(Script);
    case  3: return Script_IfFlagThen(Script);
    case  4: return Script_IfVarThen(Script);
    case  5: return Script_SetFlag(Script);
    case  6: return Script_SetVar(Script);
    case  7: return Script_Label(Script);
    case  8: return Script_Goto(Script);
    case  9: return Script_Message(Script);

    case 10: return Script_Add(Script);
    case 11: return Script_Remove(Script);
    case 12: return Script_Move(Script);
    case 13: return Script_Direction(Script);
    case 14: return Script_Type(Script);
    case 15: return Script_AI(Script);
    case 16: return Script_Target(Script);
    case 17: return Script_NoTarget(Script);
    case 18: return Script_Bounds(Script);
    case 19: return Script_Distance(Script);
    case 20: return Script_Script(Script);
    case 21: return Script_CharMessage(Script);
    case 22: return Script_Enable(Script);
    case 23: return Script_CreateRoute(Script);
    case 24: return Script_AddPoint(Script);
    case 25: return Script_AssignRoute(Script);
    case 26: return Script_AlterHPMP(Script);
    case 27: return Script_Ailment(Script);
    case 28: return Script_AlterSpell(Script);
    case 29: return Script_Teleport(Script);
    case 30: return Script_ShortMessage(Script);
    case 31: return Script_Action(Script);
    case 32: return Script_IfExpLevel(Script);

    case 33: return Script_Barter(Script);

    case 34: return Script_IfItem(Script);
    case 35: return Script_AddItem(Script);
    case 36: return Script_RemoveItem(Script);

    case 37: return Script_AddBarrier(Script);
    case 38: return Script_EnableBarrier(Script);
    case 39: return Script_RemoveBarrier(Script);

    case 40: return Script_AddTrigger(Script);
    case 41: return Script_EnableTrigger(Script);
    case 42: return Script_RemoveTrigger(Script);

    case 43: return Script_Sound(Script);
    case 44: return Script_Music(Script);
    case 45: return Script_StopMusic(Script);

    case 46: return Script_WinGame(Script);

    case 47: return Script_CommentOrSeparator(Script);
    case 48: return Script_CommentOrSeparator(Script);

    case 49: return Script_Wait(Script);
    case 50: return Script_IfRandThen(Script);
    case 51: return Script_Render(Script);
  }

  return NULL; // Error executing
}

sScript *cGameScript::Script_End(sScript *ScriptPtr)
{
  return NULL;  // Force end of processing
}

sScript *cGameScript::Script_Else(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_EndIf(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_IfFlagThen(sScript *ScriptPtr)
{
  BOOL Skipping;

  // See if a flag matches second entry
  if(m_Flags[ScriptPtr->Entries[0].lValue] ==                 \
             ScriptPtr->Entries[1].bValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;

  // Let if/then processor handle the rest
  return Script_IfThen(ScriptPtr->Next, Skipping);
}

sScript *cGameScript::Script_IfVarThen(sScript *ScriptPtr)
{
  BOOL Skipping;

  // See if var matches second entry
  if(m_Vars[ScriptPtr->Entries[0].lValue] ==                  \
             ScriptPtr->Entries[1].lValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;

  // Let if/then processor handle the rest
  return Script_IfThen(ScriptPtr->Next, Skipping);
}

sScript *cGameScript::Script_SetFlag(sScript *ScriptPtr)
{
  // Set boolean value
  m_Flags[ScriptPtr->Entries[0].lValue] =                     \
          ScriptPtr->Entries[1].bValue;

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_SetVar(sScript *ScriptPtr)
{
  // Set long value
  m_Vars[ScriptPtr->Entries[0].lValue] =                     \
          ScriptPtr->Entries[1].lValue;

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Label(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Goto(sScript *ScriptPtr)
{
  sScript *Script;

  // Search entire script for label
  Script = GetParentScript();
  while(Script != NULL) {

    // Return next entry if a match
    if(Script->Entries[0].lValue==ScriptPtr->Entries[0].lValue)
      return Script->Next;
    
    // Go to next entry
    Script = Script->Next;
  }

  return NULL;  // End of script, return completion
}

sScript *cGameScript::Script_Message(sScript *ScriptPtr)
{
  // Create the text and position the window
  m_Window.SetText(ScriptPtr->Entries[0].Text);
  m_Window.Move(4,4,624);

  // Lock the keyboard and mouse
  m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);

  // Render the scene while waiting for keypress/buttonpress
  while(1) {
    // Break when space pressed
    m_App->m_Keyboard.Acquire(TRUE);
    m_App->m_Keyboard.Read();
    if(m_App->m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)
      break;

    // Break when left mouse button pressed
    m_App->m_Mouse.Acquire(TRUE);
    m_App->m_Mouse.Read();
    if(m_App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
      break;

    // Render the scene w/window
    m_App->m_Graphics.ClearZBuffer();
    if(m_App->m_Graphics.BeginScene() == TRUE) {
      m_App->RenderFrame();
      m_Window.Render();
      m_App->m_Graphics.EndScene();
    }

    m_App->m_Graphics.Display();
  }

  // Relock keyboard and mouse
  m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);
    
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Add(sScript *ScriptPtr)
{
  m_App->m_CharController.Add(                                \
                        ScriptPtr->Entries[0].lValue,         \
                        ScriptPtr->Entries[1].lValue,         \
                        ScriptPtr->Entries[2].Selection,      \
                        CHAR_STAND,                           \
                        ScriptPtr->Entries[3].fValue,         \
                        ScriptPtr->Entries[4].fValue,         \
                        ScriptPtr->Entries[5].fValue,         \
                        ScriptPtr->Entries[6].fValue);

  // Force an update of all character w/0 elapsed time
  m_App->m_CharController.Update(0);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Remove(sScript *ScriptPtr)
{
  m_App->m_CharController.Remove(ScriptPtr->Entries[0].lValue);

  // Force an update of all character w/0 elapsed time
  m_App->m_CharController.Update(0);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Move(sScript *ScriptPtr)
{
  m_App->m_CharController.Move(ScriptPtr->Entries[0].lValue,  \
                     ScriptPtr->Entries[1].fValue,            \
                     ScriptPtr->Entries[2].fValue,            \
                     ScriptPtr->Entries[3].fValue);

  // Force an update of all character w/0 elapsed time
  m_App->m_CharController.Update(0);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Direction(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                       ScriptPtr->Entries[0].lValue)) != NULL)
    CharPtr->Direction = ScriptPtr->Entries[1].fValue;

  // Force an update of all character w/0 elapsed time
  m_App->m_CharController.Update(0);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Type(sScript *ScriptPtr)
{
  m_App->m_CharController.SetType(                            \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].Selection);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AI(sScript *ScriptPtr)
{
  m_App->m_CharController.SetAI(                              \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].Selection);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Target(sScript *ScriptPtr)
{
  m_App->m_CharController.SetTargetCharacter(                 \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_NoTarget(sScript *ScriptPtr)
{
  m_App->m_CharController.SetTargetCharacter(                 \
                      ScriptPtr->Entries[0].lValue, -1);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Bounds(sScript *ScriptPtr)
{
  m_App->m_CharController.SetBounds(                          \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].fValue,           \
                      ScriptPtr->Entries[2].fValue,           \
                      ScriptPtr->Entries[3].fValue,           \
                      ScriptPtr->Entries[4].fValue,           \
                      ScriptPtr->Entries[5].fValue,           \
                      ScriptPtr->Entries[6].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Distance(sScript *ScriptPtr)
{
  m_App->m_CharController.SetDistance(                        \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Script(sScript *ScriptPtr)
{
  m_App->m_CharController.SetScript(                          \
                      ScriptPtr->Entries[0].lValue,           \
                      ScriptPtr->Entries[1].Text);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharMessage(sScript *ScriptPtr)
{
  sCharacter *CharPtr;
  D3DXMATRIX matWorld, matView, matProj;
  D3DXVECTOR3 vecTarget;
  D3DVIEWPORT9 vpScreen;
  float MaxY;

  // Get the transformation matrices
  D3DXMatrixIdentity(&matWorld);
  m_App->m_Graphics.GetDeviceCOM()->GetTransform(             \
                                   D3DTS_VIEW, &matView);
  m_App->m_Graphics.GetDeviceCOM()->GetTransform(             \
                                   D3DTS_PROJECTION, &matProj);

  // Get the viewport
  m_App->m_Graphics.GetDeviceCOM()->GetViewport(&vpScreen);

  // Get the character's height
  CharPtr = m_App->m_CharController.GetCharacter(             \
                                ScriptPtr->Entries[1].lValue);
  CharPtr->Object.GetBounds(NULL,NULL,NULL,                   \
                            NULL,&MaxY,NULL,NULL);
  
  // Project the 3-D coordinates in 2-D coordinates
  D3DXVec3Project(&vecTarget,                                 \
                  &D3DXVECTOR3(CharPtr->XPos,                 \
                               CharPtr->YPos+MaxY,            \
                               CharPtr->ZPos),                \
                  &vpScreen, &matProj, &matView, &matWorld);

  // Create the text and position the window
  m_Window.SetText(ScriptPtr->Entries[0].Text,                \
                                D3DCOLOR_RGBA(255,255,0,255));
  m_Window.Move(4,4,624,0,(long)vecTarget.x,(long)vecTarget.y);

  // Lock the keyboard and mouse
  m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);

  // Render the scene while waiting for keypress/buttonpress
  while(1) {
    // Break when space pressed
    m_App->m_Keyboard.Acquire(TRUE);
    m_App->m_Keyboard.Read();
    if(m_App->m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)
      break;

    // Break when left mouse button pressed
    m_App->m_Mouse.Acquire(TRUE);
    m_App->m_Mouse.Read();
    if(m_App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
      break;


    // Render the scene w/window
    m_App->m_Graphics.ClearZBuffer();
    if(m_App->m_Graphics.BeginScene() == TRUE) {
      m_App->RenderFrame();
      m_Window.Render();
      m_App->m_Graphics.EndScene();
    }
    m_App->m_Graphics.Display();
  }

  // Relock keyboard and mouse
  m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);
    
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Enable(sScript *ScriptPtr)
{
  m_App->m_CharController.SetEnable(                          \
                      ScriptPtr->Entries[1].lValue,           \
                      ScriptPtr->Entries[0].bValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CreateRoute(sScript *ScriptPtr)
{
  // Release old route
  delete [] m_Route;
  m_Route = NULL;
  m_NumRoutePoints = 0;

  m_NumRoutePoints = ScriptPtr->Entries[0].lValue;
  m_Route = new sRoutePoint[m_NumRoutePoints]();

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AddPoint(sScript *ScriptPtr)
{
  m_Route[ScriptPtr->Entries[0].lValue].XPos =                \
                                ScriptPtr->Entries[1].fValue;
  m_Route[ScriptPtr->Entries[0].lValue].YPos =                \
                                ScriptPtr->Entries[2].fValue;
  m_Route[ScriptPtr->Entries[0].lValue].ZPos =                \
                                ScriptPtr->Entries[3].fValue;

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AssignRoute(sScript *ScriptPtr)
{
  m_App->m_CharController.SetRoute(                           \
                      ScriptPtr->Entries[0].lValue,           \
                      m_NumRoutePoints, (sRoutePoint*)m_Route);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AlterHPMP(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[3].lValue)) != NULL) {
    if(!ScriptPtr->Entries[0].Selection) {
      if(!ScriptPtr->Entries[2].Selection)
        CharPtr->HealthPoints += ScriptPtr->Entries[1].lValue;
      else
        CharPtr->ManaPoints += ScriptPtr->Entries[1].lValue;
    } else {
      if(!ScriptPtr->Entries[2].Selection)
        CharPtr->HealthPoints -= ScriptPtr->Entries[1].lValue;
      else
        CharPtr->ManaPoints -= ScriptPtr->Entries[1].lValue;
    }

    // Bounds check values
    if(CharPtr->HealthPoints > CharPtr->Def.HealthPoints)
      CharPtr->HealthPoints = CharPtr->Def.HealthPoints;
    if(CharPtr->ManaPoints > CharPtr->Def.ManaPoints)
      CharPtr->ManaPoints = CharPtr->Def.ManaPoints;
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Ailment(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[2].lValue)) != NULL) {
    if(!ScriptPtr->Entries[0].Selection) {
      // Cure ailments
      CharPtr->Ailments &= ~ScriptPtr->Entries[1].lValue;
    } else {
      // Cause ailments
      CharPtr->Ailments |= ScriptPtr->Entries[1].lValue;
    }
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AlterSpell(sScript *ScriptPtr)
{
  sCharacter *CharPtr;
  long SpellNum, BitFlag;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[0].lValue)) != NULL) {
    SpellNum = ScriptPtr->Entries[2].lValue;
    BitFlag = (1 << (SpellNum & 31));

    if(!ScriptPtr->Entries[1].Selection) {
      // Learn a spell
      CharPtr->Def.MagicSpells[SpellNum/32] |= BitFlag;
    } else {
      // Forget a spell
      CharPtr->Def.MagicSpells[SpellNum/32] &= ~BitFlag;
    }
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Teleport(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[0].lValue)) != NULL) {
    m_App->SetupTeleport(ScriptPtr->Entries[1].lValue,        \
                           ScriptPtr->Entries[2].fValue,      \
                           ScriptPtr->Entries[3].fValue,      \
                           ScriptPtr->Entries[4].fValue);
    return NULL;  // Stop processing after teleport
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_ShortMessage(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[0].lValue)) != NULL) {
    m_App->m_CharController.SetMessage(CharPtr,               \
                    ScriptPtr->Entries[1].Text,               \
                    ScriptPtr->Entries[2].lValue);
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Action(sScript *ScriptPtr)
{
  sCharacter *CharPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[0].lValue)) != NULL) {

    // Set the character's action
    m_App->m_CharController.SetAction(CharPtr,                \
                    ScriptPtr->Entries[1].lValue,             \
                    ScriptPtr->Entries[2].lValue);
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_IfExpLevel(sScript *ScriptPtr)
{
  BOOL Skipping;
  sCharacter *CharPtr;

  // Get character pointer
  CharPtr = m_App->m_CharController.GetCharacter(             \
                                ScriptPtr->Entries[0].lValue);

  // See if level matches values
  if(CharPtr != NULL && CharPtr->Def.Level >=                 \
                        ScriptPtr->Entries[1].lValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;

  // Let if/then processor handle the rest
  return Script_IfThen(ScriptPtr->Next, Skipping);
}

sScript *cGameScript::Script_Barter(sScript *ScriptPtr)
{
  m_App->SetupBarter(                                         \
              m_App->m_CharController.GetCharacter(0),        \
              ScriptPtr->Entries[0].Text);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_IfItem(sScript *ScriptPtr)
{
  BOOL Skipping;
  sCharacter *CharPtr;

  // Get character pointer
  CharPtr = m_App->m_CharController.GetCharacter(             \
                                ScriptPtr->Entries[2].lValue);

  // See if item in inventory and check count
  if(CharPtr != NULL && CharPtr->CharICS->GetItem(            \
                ScriptPtr->Entries[1].lValue)->Quantity >=    \
                ScriptPtr->Entries[0].lValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;


  // Let if/then processor handle the rest
  return Script_IfThen(ScriptPtr->Next, Skipping);
}

sScript *cGameScript::Script_AddItem(sScript *ScriptPtr)
{
  sCharacter *CharPtr;
  long ItemNum;
  sCharItem *ItemPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[2].lValue)) != NULL) {

    // Only handle add item if character has an ICS
    if(CharPtr->CharICS != NULL) {

      // Get item number in question
      ItemNum = ScriptPtr->Entries[1].lValue;

      // Find matching item and add to it's quantity
      ItemPtr = CharPtr->CharICS->GetParentItem();
      while(ItemPtr != NULL) {

        // If this is matching item, then just add quantity
        if(ItemPtr->ItemNum == ItemNum) {
          ItemPtr->Quantity += ScriptPtr->Entries[0].lValue;
          break;
        }
       
        // Go to next item
        ItemPtr = ItemPtr->Next;
      }

      // Add to inventory if nothing found
      if(ItemPtr == NULL)
        CharPtr->CharICS->Add(ItemNum,                        \
                              ScriptPtr->Entries[0].lValue);
    }
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_RemoveItem(sScript *ScriptPtr)
{
  sCharacter *CharPtr;
  sCharItem *ItemPtr;

  if((CharPtr = m_App->m_CharController.GetCharacter(         \
                    ScriptPtr->Entries[2].lValue)) != NULL) {
    if(CharPtr->CharICS != NULL) {

      // Find item that we're looking for first
      ItemPtr = CharPtr->CharICS->GetParentItem();
      while(ItemPtr != NULL) {

        // If this is item, remove quantity
        if(ItemPtr->ItemNum == ScriptPtr->Entries[1].lValue) {
          ItemPtr->Quantity -= ScriptPtr->Entries[0].lValue;
          if(ItemPtr->Quantity <= 0)
            CharPtr->CharICS->Remove(ItemPtr);
          break;
        }
       
        // Go to next item
        ItemPtr = ItemPtr->Next;
      }
    }
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AddBarrier(sScript *ScriptPtr)
{
  m_App->m_Barrier.AddTriangle(ScriptPtr->Entries[0].lValue,  \
                               TRUE,                          \
                               0.0f, 0.0f, 0.0f,              \
                               0.0f, 0.0f, 0.0f,              \
                               ScriptPtr->Entries[1].fValue,  \
                               ScriptPtr->Entries[2].fValue,  \
                               ScriptPtr->Entries[3].fValue,  \
                               ScriptPtr->Entries[4].fValue,  \
                               ScriptPtr->Entries[5].fValue,  \
                               ScriptPtr->Entries[6].fValue,  \
                               ScriptPtr->Entries[7].fValue,  \
                               ScriptPtr->Entries[8].fValue);
 
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_EnableBarrier(sScript *ScriptPtr)
{
  m_App->m_Barrier.Enable(ScriptPtr->Entries[1].lValue,       \
           (!ScriptPtr->Entries[0].Selection) ? TRUE : FALSE);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_RemoveBarrier(sScript *ScriptPtr)
{
  m_App->m_Barrier.Remove(ScriptPtr->Entries[0].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AddTrigger(sScript *ScriptPtr)
{
  m_App->m_Trigger.AddTriangle(ScriptPtr->Entries[0].lValue,  \
                               TRUE,                          \
                               ScriptPtr->Entries[1].fValue,  \
                               ScriptPtr->Entries[2].fValue,  \
                               ScriptPtr->Entries[3].fValue,  \
                               ScriptPtr->Entries[4].fValue,  \
                               ScriptPtr->Entries[5].fValue,  \
                               ScriptPtr->Entries[6].fValue,  \
                               ScriptPtr->Entries[7].fValue,  \
                               ScriptPtr->Entries[8].fValue);
 
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_EnableTrigger(sScript *ScriptPtr)
{
  m_App->m_Trigger.Enable(ScriptPtr->Entries[1].lValue,       \
           (!ScriptPtr->Entries[0].Selection) ? TRUE : FALSE);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_RemoveTrigger(sScript *ScriptPtr)
{
  m_App->m_Trigger.Remove(ScriptPtr->Entries[0].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Sound(sScript *ScriptPtr)
{
  m_App->PlaySound(ScriptPtr->Entries[0].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Music(sScript *ScriptPtr)
{
  m_App->PlayMusic(ScriptPtr->Entries[0].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_StopMusic(sScript *ScriptPtr)
{
  m_App->StopMusic();

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_WinGame(sScript *ScriptPtr)
{
  m_App->WinGame();

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CommentOrSeparator(              \
                                    sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_Wait(sScript *ScriptPtr)
{
  DWORD Timer = timeGetTime() + ScriptPtr->Entries[0].lValue;

  // Wait for # of milliseconds
  while(timeGetTime() < Timer);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_IfRandThen(sScript *ScriptPtr)
{
  BOOL Skipping;

  // See if random # is >= entry
  if((rand() % ScriptPtr->Entries[0].lValue) >=               \
                                 ScriptPtr->Entries[1].lValue)
    Skipping = FALSE;
  else 
    Skipping = TRUE;

  // Let if/then processor handle the rest
  return Script_IfThen(ScriptPtr->Next, Skipping);
}

sScript *cGameScript::Script_Render(sScript *ScriptPtr)
{
  if(m_App != NULL) {
    m_App->m_Graphics.ClearZBuffer();
    if(m_App->m_Graphics.BeginScene() == TRUE) {
      m_App->RenderFrame();
      m_App->m_Graphics.EndScene();
    }
    m_App->m_Graphics.Display();
  }

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_IfThen(sScript *ScriptPtr,       \
                                    BOOL Skip)
{
  BOOL Skipping = Skip;

  // At this point, Skipping states if the script actions
  // need to be skipped due to a conditional if..then statement.
  // Actions are further processed if skipped = FALSE, looking
  // for an else to flip the skip mode, or an endif to end
  // the conditional block.
  while(ScriptPtr != NULL) {

    // if else, flip skip mode
    if(ScriptPtr->Type == SCRIPT_ELSE)
      Skipping = (Skipping == TRUE) ? FALSE : TRUE;

    // break on end if
    if(ScriptPtr->Type == SCRIPT_ENDIF)
      return ScriptPtr->Next;

    // Process script function in conditional block
    // making sure to skip actions when condition not met.
    if(Skipping == TRUE)
      ScriptPtr = ScriptPtr->Next;
    else {
      // Return to normal processing if goto encountered
      if(ScriptPtr->Type == SCRIPT_GOTO)
        return Process(ScriptPtr);
        
      if((ScriptPtr = Process(ScriptPtr)) == NULL)
        return NULL;
    }
  }

  return NULL; // End of script reached
}
