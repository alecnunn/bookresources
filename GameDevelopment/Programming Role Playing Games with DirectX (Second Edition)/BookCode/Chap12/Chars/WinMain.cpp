/**************************************************
WinMain.cpp
Chapter 12 Character Controller Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"

#include "Window.h"
#include "Chars.h"
#include "Script.h"
#include "WinMain.h"

// Application class
cApp g_App;

// Global names of character meshes
char *g_CharMeshNames[] = {
    { "..\\Data\\Warrior.x" },   // Mesh # 0
    { "..\\Data\\Yodan.x"   }    // Mesh # 1
  };
sCharAnimationInfo g_CharAnimations[] = {
    { "Idle",  TRUE  },
    { "Walk",  TRUE  },
    { "Swing", FALSE },
    { "Spell", FALSE },
    { "Swing", FALSE },
    { "Hurt",  FALSE },
    { "Die",   FALSE },
    { "Idle",  TRUE  }
  };

char *g_SpellMeshNames[] = {
    { "..\\Data\\Fireball.x"   },
    { "..\\Data\\Explosion.x"  },
    { "..\\Data\\Groundball.x" },
    { "..\\Data\\ice.x" },
    { "..\\Data\\bomb.x" },
  };

///////////////////////////////////////////////////////////
// Overloaded application class functions
//////////////////////////////////////////////////////////
cApp::cApp()
{ 
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU;
  strcpy(m_Class, "CharClass");
  strcpy(m_Caption, "Characters Demo by Jim Adams");
}

BOOL cApp::Init()
{
  long i;
  FILE *fp;

  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 10000.0f);
  ShowMouse(TRUE);

  // Create a font
  m_Font.Create(&m_Graphics, "Arial", 16, TRUE);
  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);

  // Load the terrain mesh and set object
  m_TerrainMesh.Load(&m_Graphics, "..\\Data\\World.x",        \
                     "..\\Data\\");
  m_TerrainObject.Create(&m_Graphics, &m_TerrainMesh);

  // Load the master item list
  for(i=0;i<1024;i++)
    ZeroMemory(&m_MIL[i], sizeof(sItem));
  if((fp=fopen("..\\Data\\Default.mil", "rb")) != NULL) {
    for(i=0;i<1024;i++)
      fread(&m_MIL[i], 1, sizeof(sItem), fp);
    fclose(fp);
  }

  // Initialize the character controller
  m_CharController.SetData(this);
  m_CharController.Init(&m_Graphics, &m_Font,                 \
      "..\\Data\\Default.mcl", (sItem*)&m_MIL,                \
      m_SpellController.GetSpell(0),                          \
      sizeof(g_CharMeshNames)/sizeof(char*), g_CharMeshNames, \
      "..\\Data\\", "..\\Data\\",                             \
      sizeof(g_CharAnimations) / sizeof(sCharAnimationInfo),  \
      (sCharAnimationInfo*)&g_CharAnimations,                 \
      &m_SpellController);

  // Initialize the spell controller
  m_SpellController.Init(&m_Graphics,                         \
     "..\\Data\\Default.msl",                                 \
     sizeof(g_SpellMeshNames)/sizeof(char*),g_SpellMeshNames, \
     "..\\Data\\", &m_CharController);

  // Add the character player
  m_CharController.Add(0, 0, CHAR_PC, CHAR_STAND,             \
                       0.0f, 0.0f, 0.0f, 3.14f);

  // Process the startup script
  m_Script.SetData(this, &m_Graphics, &m_Font, &m_Keyboard,   \
                   &m_CharController);
  m_Script.Execute("..\\Data\\Startup.mls");

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free controllers
  m_CharController.Free();
  m_SpellController.Free();

  // Free script object
  m_Script.Free();

  // Free objects and meshes
  m_TerrainMesh.Free();
  m_TerrainObject.Free();

  // Shutdown input
  m_Keyboard.Free();
  m_Mouse.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Font.Free();
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD UpdateCounter = timeGetTime();
  char Stats[128];
  static sCharacter *CharPtr=m_CharController.GetCharacter(0);

  // Lock to 30 fps
  if(timeGetTime() < UpdateCounter + 33)
    return TRUE;
  UpdateCounter = timeGetTime();

  // Read in keyboard
  m_Keyboard.Acquire(TRUE);
  m_Keyboard.Read();

  // Exit if ESC pressed
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Update controllers
  m_CharController.Update(33);
  m_SpellController.Update(33);

  // Set the camera 
  m_Camera.Point(0.0f, 700.0f, -700.0f, 0.0f, 0.0f, 0.0f);
  m_Graphics.SetCamera(&m_Camera);

  // Render everything
  m_Graphics.Clear();
  if(m_Graphics.BeginScene() == TRUE) {

    // Render terrain
    m_Graphics.EnableZBuffer(TRUE);
    m_TerrainObject.Render();

    // Render all characters
    m_CharController.Render();

    // Render spells
    m_SpellController.Render();

    // Display stats
    sprintf(Stats, "HP: %ld / %ld\r\nMP: %ld / %ld",          \
            CharPtr->HealthPoints, CharPtr->Def.HealthPoints,
            CharPtr->ManaPoints, CharPtr->Def.ManaPoints);
    m_Font.Print(Stats, 2, 2);

    m_Graphics.EndScene();
  }
  m_Graphics.Display();

  return TRUE;
}

long cApp::GetInput()
{
  long Action = 0;

  if(m_Keyboard.GetKeyState(KEY_UP) == TRUE)
    Action |= 1;
  
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE)
    Action |= 2;
  
  if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE)
    Action |= 4;
  
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE)
    Action |= 8;

  if(m_Keyboard.GetKeyState(KEY_SPACE) == TRUE) {
    Action |= 16;
    m_Keyboard.SetLock(KEY_SPACE, TRUE);
    m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  }

  if(m_Keyboard.GetKeyState(KEY_1) == TRUE) {
    Action |= 32;
    m_Keyboard.SetLock(KEY_1, TRUE);
    m_Keyboard.SetKeyState(KEY_1, FALSE);
  }

  if(m_Keyboard.GetKeyState(KEY_2) == TRUE) {
    Action |= 64;
    m_Keyboard.SetLock(KEY_2, TRUE);
    m_Keyboard.SetKeyState(KEY_2, FALSE);
  }

  if(m_Keyboard.GetKeyState(KEY_3) == TRUE) {
    Action |= 128;
    m_Keyboard.SetLock(KEY_3, TRUE);
    m_Keyboard.SetKeyState(KEY_3, FALSE);
  }

  return Action;
}

BOOL cApp::CheckIntersect(                                    \
                 float XStart, float YStart, float ZStart,    \
                 float XEnd,   float YEnd,   float ZEnd)
{
  sMesh *MeshPtr;
  BOOL  Hit;
  float u, v, Dist;
  float XDiff, YDiff, ZDiff, Size;
  DWORD FaceIndex;
  D3DXVECTOR3 vecDir;

  // Start with parent mesh
  if((MeshPtr = m_TerrainMesh.GetParentMesh()) == NULL)
    return FALSE;  // No polygons hit

  // Calculate ray
  XDiff = XEnd - XStart;
  YDiff = YEnd - YStart;
  ZDiff = ZEnd - ZStart;
  Size  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
  D3DXVec3Normalize(&vecDir, &D3DXVECTOR3(XDiff, YDiff, ZDiff));

  // Go through each mesh looking for intersection
  while(MeshPtr != NULL) {
    D3DXIntersect(MeshPtr->m_Mesh,                            \
                &D3DXVECTOR3(XStart,YStart,ZStart), &vecDir,  \
                &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);

    if(Hit == TRUE) {
      if(Dist <= Size)
        return TRUE;  // Hit a polygon
    }

    MeshPtr = MeshPtr->m_Next;
  }

  return FALSE;  // No polygons hit
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  return g_App.Run();
}

///////////////////////////////////////////////////////////
// Overloaded character controller class functions
///////////////////////////////////////////////////////////
BOOL cChars::SetData(cApp *App)
{
  if((m_App = App) == FALSE)
    return FALSE;
  return TRUE;
}

BOOL cChars::PCUpdate(sCharacter *Character, long Elapsed,    \
                    float *XMove, float *YMove, float *ZMove)
{
  sCharacter *CharPtr;
  float XDiff, YDiff, ZDiff, Dist;
  long  Action = 0;
  float Speed;
  long  SpellNum;
  float SpellRadius;

  // If not PC, then let controller handle movement
  if(Character->ID != 0)
    return TRUE;

  Speed = (float)Elapsed / 1000.0f * GetSpeed(Character);
  
  // Get the input flags
  if(m_App != NULL)
    Action = m_App->GetInput();

  // Rotate character
  if(Action & 2) {
    Character->Direction += ((float)Elapsed / 1000.0f * 4);
    Character->Action = CHAR_MOVE;
  }
  if(Action & 8) {
    Character->Direction -= ((float)Elapsed / 1000.0f * 4);
    Character->Action = CHAR_MOVE;
  }

  // Walk forward
  if(Action & 1) {
    *XMove = (float)sin(Character->Direction) * Speed;
    *ZMove = (float)cos(Character->Direction) * Speed;
    Character->Action = CHAR_MOVE;
  }

  // Attack a nearby monster or process NPC script
  if(Action & 16) {
    CharPtr = GetParentCharacter();
    while(CharPtr != NULL) {

      // Only check other characters
      if(CharPtr->ID != Character->ID) {
        XDiff = (float)fabs(CharPtr->XPos - Character->XPos);
        YDiff = (float)fabs(CharPtr->YPos - Character->YPos);
        ZDiff = (float)fabs(CharPtr->ZPos - Character->ZPos);
        Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;

        // Only check characters within 100.0 units distance
        if(Dist <= 10000.0f) {
          
          // Process script or attack?
          if(CharPtr->ScriptFilename[0])
            m_App->m_Script.Execute(CharPtr->ScriptFilename);
          else {
            // Turn toward victim
            XDiff = (CharPtr->XPos - Character->XPos);
            ZDiff = (CharPtr->ZPos - Character->ZPos);
            Character->Direction = (float)atan2(XDiff, ZDiff);

            // Set attack data
            Character->Victim = CharPtr;
            CharPtr->Attacker = Character;

            // Set attack action
            m_App->m_CharController.SetAction(Character,      \
                                              CHAR_ATTACK);
          }

          break;
        }
      }

      CharPtr = CharPtr->Next;
    }
  }

  // Cast spells
  if(Action & 32 || Action & 64 || Action & 128) {
    // Get spell number to cast
    if(Action & 32)
      SpellNum = 0;
    if(Action & 64)
      SpellNum = 1;
    if(Action & 128)
      SpellNum = 2;

    // Get spell distance
    SpellRadius =                                             \
        m_App->m_SpellController.GetSpell(SpellNum)->Distance;

    // Search for closest monster
    CharPtr = GetParentCharacter();
    while(CharPtr != NULL) {
      // Only check other characters
      if(CharPtr->Type == CHAR_MONSTER) {
        XDiff = (float)fabs(CharPtr->XPos - Character->XPos);
        YDiff = (float)fabs(CharPtr->YPos - Character->YPos);
        ZDiff = (float)fabs(CharPtr->ZPos - Character->ZPos);
        Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;

        // Only check characters within 100.0 units distance
        if(Dist <= (SpellRadius * SpellRadius)) {
          Character->SpellNum = SpellNum;
          Character->SpellTarget = CHAR_MONSTER;
          Character->TargetX = CharPtr->XPos;
          Character->TargetY = CharPtr->YPos;
          Character->TargetZ = CharPtr->ZPos;

          // Turn toward victim
          XDiff = (CharPtr->XPos - Character->XPos);
          ZDiff = (CharPtr->ZPos - Character->ZPos);
          Character->Direction = (float)atan2(XDiff, ZDiff);

          m_App->m_CharController.SetAction(Character, CHAR_SPELL);
          break;
        }
      }

      CharPtr = CharPtr->Next;
    }

  }

  return TRUE;
}

BOOL cChars::ValidateMove(sCharacter *Character,              \
                    float *XMove, float *YMove, float *ZMove)
{
  // Check against terrain mesh for collision
  if(m_App != NULL) {
    if(m_App->CheckIntersect(                                 \
         Character->XPos,                                     \
         Character->YPos+2.0f,                                \
         Character->ZPos,                                     \
         *XMove + Character->XPos,                            \
         *YMove + Character->YPos + 2.0f,                     \
         *ZMove + Character->ZPos) == TRUE)
      return FALSE;
  }

  return TRUE;
}

///////////////////////////////////////////////////////////
// Overloaded script class functions
///////////////////////////////////////////////////////////
cGameScript::cGameScript()
{
  m_Graphics = NULL;
  m_Font = NULL;
  m_Keyboard = NULL;
  m_App = NULL;
  m_CharController = NULL;
  m_Route = NULL;
}

cGameScript::~cGameScript()
{
  m_Window.Free();
  delete [] m_Route;
}

BOOL cGameScript::SetData(cApp *Application,                  \
                          cGraphics *Graphics, cFont *Font,   \
                          cInputDevice *Keyboard,             \
                          cChars *Controller)
{
  long i;

  if((m_App = Application) == NULL)
    return FALSE;

  if((m_Graphics = Graphics) == NULL)
    return FALSE;

  if((m_Font = Font) == NULL)
    return FALSE;

  if((m_Keyboard = Keyboard) == NULL)
    return FALSE;

  if((m_CharController = Controller) == NULL)
    return FALSE;

  // Clear flags
  for(i=0;i<256;i++)
    m_Flags[i] = FALSE;

  // Create the window object
  m_Window.Create(m_Graphics, m_Font);

  return TRUE;
}

BOOL cGameScript::Prepare()
{
  return TRUE;
}

BOOL cGameScript::Release()
{
  delete [] m_Route;
  m_Route = NULL;
  m_NumRoutePoints = 0;

  return TRUE;
}

sScript *cGameScript::Process(sScript *Script)
{
  switch(Script->Type) {
    case  0: return Script_End(Script);
    case  1: return Script_IfFlagThen(Script);
    case  2: return Script_Else(Script);
    case  3: return Script_EndIf(Script);
    case  4: return Script_SetFlag(Script);
    case  5: return Script_Message(Script);
    case  6: return Script_AddChar(Script);
    case  7: return Script_RemoveChar(Script);
    case  8: return Script_CharMessage(Script);
    case  9: return Script_CharType(Script);
    case 10: return Script_CharAI(Script);
    case 11: return Script_CharDistance(Script);
    case 12: return Script_CharBounds(Script);
    case 13: return Script_TargetChar(Script);
    case 14: return Script_NoTarget(Script);
    case 15: return Script_CreateRoute(Script);
    case 16: return Script_AddPoint(Script);
    case 17: return Script_AssignRoute(Script);
    case 18: return Script_MoveChar(Script);
    case 19: return Script_CharScript(Script);
 }

  return NULL; // Error executing
}

///////////////////////////////////////////////////////////
// Standard script processing functions
///////////////////////////////////////////////////////////
sScript *cGameScript::Script_End(sScript *ScriptPtr)
{
  return NULL;  // Force end of processing
}

sScript *cGameScript::Script_IfFlagThen(sScript *ScriptPtr)
{
  BOOL Skipping;

  // See if a flag matches second entry
  if(m_Flags[ScriptPtr->Entries[0].lValue % 256] ==           \
             ScriptPtr->Entries[1].bValue)
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
    if(ScriptPtr->Type == 2)
      Skipping = (Skipping == TRUE) ? FALSE : TRUE;

    // break on end if
    if(ScriptPtr->Type == 3)
      return ScriptPtr->Next;

    // Process script function in conditional block
    // making sure to skip actions when condition not met.
    if(Skipping == TRUE)
      ScriptPtr = ScriptPtr->Next;
    else {
      if((ScriptPtr = Process(ScriptPtr)) == NULL)
        return NULL;
    }
  }

  return NULL; // End of script reached
}

sScript *cGameScript::Script_Else(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_EndIf(sScript *ScriptPtr)
{
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_SetFlag(sScript *ScriptPtr)
{
  // Set boolean value
  m_Flags[ScriptPtr->Entries[0].lValue % 256] =               \
          ScriptPtr->Entries[1].bValue;

  return ScriptPtr->Next;
}

///////////////////////////////////////////////////////////
// Specialized script processing functions
///////////////////////////////////////////////////////////
sScript *cGameScript::Script_Message(sScript *ScriptPtr)
{
  // Set the text to display
  m_Window.SetText(ScriptPtr->Entries[0].Text);
  m_Window.Move(10, 10, 620, 0);

  // Display the window
  g_App.m_Graphics.Clear();
  if(m_Graphics->BeginScene() == TRUE) {
    
    // Render terrain
    g_App.m_Graphics.EnableZBuffer(TRUE);
    g_App.m_TerrainObject.Render();

    // Render all characters
    g_App.m_CharController.Render();

    // Render spells
    g_App.m_SpellController.Render();

    // Display message
    m_Window.Render();
    m_Graphics->EndScene();
  }
  m_Graphics->Display();

  // Wait for a key press
  m_Keyboard->Acquire(TRUE);
  m_Keyboard->SetLock(KEY_SPACE, TRUE);
  m_Keyboard->SetKeyState(KEY_SPACE, FALSE);
  while(1) {
    m_Keyboard->Read();
    if(m_Keyboard->GetKeyState(KEY_SPACE) == TRUE)
      break;
  }
  m_Keyboard->SetLock(KEY_SPACE, TRUE);
  m_Keyboard->SetKeyState(KEY_SPACE, FALSE);
    
  return ScriptPtr->Next;
}

sScript *cGameScript::Script_AddChar(sScript *ScriptPtr)
{
  m_CharController->Add(ScriptPtr->Entries[0].lValue,         \
                        ScriptPtr->Entries[1].lValue,         \
                        ScriptPtr->Entries[2].Selection,      \
                        ScriptPtr->Entries[3].Selection,      \
                        ScriptPtr->Entries[4].fValue,         \
                        ScriptPtr->Entries[5].fValue,         \
                        ScriptPtr->Entries[6].fValue,         \
                        ScriptPtr->Entries[7].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_RemoveChar(sScript *ScriptPtr)
{
  m_CharController->Remove(ScriptPtr->Entries[0].lValue);

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
  m_Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW,        \
                                           &matView);
  m_Graphics->GetDeviceCOM()->GetTransform(D3DTS_PROJECTION,  \
                                           &matProj);

  // Get the viewport
  m_Graphics->GetDeviceCOM()->GetViewport(&vpScreen);

  // Get the character's coordinates
  CharPtr = m_CharController->GetCharacter(                   \
                                ScriptPtr->Entries[1].lValue);
  CharPtr->Object.GetBounds(NULL,NULL,NULL,                   \
                            NULL,&MaxY,NULL,NULL);
  
  // Project the 3-D coordinates in 2-D coordinates
  D3DXVec3Project(&vecTarget,                                 \
                  &D3DXVECTOR3(CharPtr->XPos,                 \
                               CharPtr->YPos+MaxY,            \
                               CharPtr->ZPos),                \
                  &vpScreen, &matProj, &matView, &matWorld);

  // Set the text
  m_Window.SetText(ScriptPtr->Entries[0].Text, 
                   D3DCOLOR_RGBA(255,255,0,255));
  m_Window.Move(10, 10, 620, 0,                               \
                (long)vecTarget.x, (long)vecTarget.y);

  // Display the window while waiting for a keypress
  m_Keyboard->Acquire(TRUE);
  m_Keyboard->SetLock(KEY_SPACE, TRUE);
  m_Keyboard->SetKeyState(KEY_SPACE, FALSE);
  while(1) {
    m_Keyboard->Read();
    if(m_Keyboard->GetKeyState(KEY_SPACE) == TRUE)
      break;

    g_App.m_Graphics.Clear();
    if(m_Graphics->BeginScene() == TRUE) {
  
      // Render terrain
      g_App.m_Graphics.EnableZBuffer(TRUE);
      g_App.m_TerrainObject.Render();

      // Render all characters
      g_App.m_CharController.Render();

      // Render spells
      g_App.m_SpellController.Render();

      // Display message
      m_Window.Render();
      m_Graphics->EndScene();
    }

    m_Graphics->Display();
  }

  m_Keyboard->SetLock(KEY_SPACE, TRUE);
  m_Keyboard->SetKeyState(KEY_SPACE, FALSE);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharType(sScript *ScriptPtr)
{
  m_CharController->SetType(ScriptPtr->Entries[0].lValue,     \
                        ScriptPtr->Entries[1].Selection);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharAI(sScript *ScriptPtr)
{
  m_CharController->SetAI(ScriptPtr->Entries[0].lValue,       \
                      ScriptPtr->Entries[1].Selection);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharDistance(sScript *ScriptPtr)
{
  m_CharController->SetDistance(ScriptPtr->Entries[0].lValue, \
                            ScriptPtr->Entries[1].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharBounds(sScript *ScriptPtr)
{
  m_CharController->SetBounds(ScriptPtr->Entries[0].lValue,   \
                          ScriptPtr->Entries[1].fValue,       \
                          ScriptPtr->Entries[2].fValue,       \
                          ScriptPtr->Entries[3].fValue,       \
                          ScriptPtr->Entries[4].fValue,       \
                          ScriptPtr->Entries[5].fValue,       \
                          ScriptPtr->Entries[6].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_TargetChar(sScript *ScriptPtr)
{
  m_CharController->SetTargetCharacter(                       \
                            ScriptPtr->Entries[0].lValue,     \
                            ScriptPtr->Entries[1].lValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_NoTarget(sScript *ScriptPtr)
{
  m_CharController->SetTargetCharacter(                       \
                            ScriptPtr->Entries[0].lValue, -1);

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
  m_CharController->SetRoute(ScriptPtr->Entries[0].lValue,    \
                    m_NumRoutePoints, (sRoutePoint*)m_Route);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_MoveChar(sScript *ScriptPtr)
{
  m_CharController->Move(ScriptPtr->Entries[0].lValue,        \
                     ScriptPtr->Entries[1].fValue,            \
                     ScriptPtr->Entries[2].fValue,            \
                     ScriptPtr->Entries[3].fValue);

  return ScriptPtr->Next;
}

sScript *cGameScript::Script_CharScript(sScript *ScriptPtr)
{
  m_CharController->SetScript(ScriptPtr->Entries[0].lValue,   \
                     ScriptPtr->Entries[1].Text);

  return ScriptPtr->Next;
}
