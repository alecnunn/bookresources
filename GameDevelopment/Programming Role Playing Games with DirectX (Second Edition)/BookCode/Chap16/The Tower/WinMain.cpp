#define FULLSCREENMODE

/**************************************************
WinMain.cpp
Chapter 16 Full Game Demo - The Tower

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, D3DXOF.LIB, DXGUID.LIB,
  DINPUT8.LIB, DSOUND.LIB, STRMIIDS.LIB, and WINMM.LIB 
**************************************************/

#include "Global.h"

///////////////////////////////////////////////////////////
// Global declarations
///////////////////////////////////////////////////////////
#define MENU_BACK   1
#define MENU_LOAD   2
#define MENU_SAVE   4

long g_MenuOptions = 0;

// Global names of character meshes
char *g_CharMeshNames[] = {
    { "..\\Data\\Warrior1.x" },   // Mesh # 0
    { "..\\Data\\Warrior2.x" },   // Mesh # 1
    { "..\\Data\\Yodan1.x"   },   // Mesh # 2
    { "..\\Data\\Yodan2.x"   },   // Mesh # 3
    { "..\\Data\\Yodan3.x"   },   // Mesh # 4
    { "..\\Data\\Yodan4.x"   }    // Mesh # 5
  };

// Global character animation information
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

// Global spell mesh information
char *g_SpellMeshNames[] = {
    { "..\\Data\\Fireball.x"   },
    { "..\\Data\\Explosion.x"  },
    { "..\\Data\\Ice.x"        },
    { "..\\Data\\Heal.x"       },
    { "..\\Data\\Teleport.x"   },
    { "..\\Data\\Groundball.x" },
    { "..\\Data\\Bomb.x"       },
    { "..\\Data\\Force.x"      }
  };

// Global sound effect filenames
#define NUM_SOUNDS 9
char *g_SoundFilenames[NUM_SOUNDS] = {
    { "..\\Data\\Attack1.wav" },
    { "..\\Data\\Attack2.wav" },
    { "..\\Data\\Spell.wav"   },
    { "..\\Data\\Roar.wav"    },
    { "..\\Data\\Hurt1.wav"   },
    { "..\\Data\\Hurt2.wav"   },
    { "..\\Data\\Die1.wav"    },
    { "..\\Data\\Die2.wav"    },
    { "..\\Data\\Beep.wav"    }
  };

// Global music filenames
long g_CurrentMusic = -1;
char *g_MusicFilenames[] = {
    { "..\\Data\\Cathedral_Sunrise.mid" },
    { "..\\Data\\Distant_tribe.mid"     },
    { "..\\Data\\Escape.mid"            },
    { "..\\Data\\Jungle1.mid"           },
    { "..\\Data\\Magic_Harp.mid"        },
    { "..\\Data\\Medi_Strings.mid"      },
    { "..\\Data\\Medi_techno.mid"       },
    { "..\\Data\\Song_of_the_sea.mid"   },
    { "..\\Data\\Storm.mid"             }
  };

// Global character pointer to PC, bartering NPC, and barter ICS
sCharacter *g_PCChar = NULL;
sCharacter *g_BarterChar = NULL;
char        g_BarterICS[MAX_PATH];

///////////////////////////////////////////////////////////
// cApp function code
///////////////////////////////////////////////////////////
cApp::cApp()
{ 
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER | WS_CAPTION |                         \
             WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "GameClass");
  strcpy(m_Caption, "The Tower by Jim Adams");
}

BOOL cApp::Init()
{
  // Initialize the graphics device
  m_Graphics.Init();

  // Determine to use fullscreen mode or not
#ifdef FULLSCREENMODE
  m_Graphics.SetMode(GethWnd(), FALSE, TRUE, 640, 480);
#else
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
#endif

  // Set perspective
  m_Graphics.SetPerspective(0.6021124f,1.33333f,1.0f,20000.0f);

  // Enable cursor
  ShowMouse(TRUE);

  // Create a font
  m_Font.Create(&m_Graphics, "Arial", 16, TRUE);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);

  // Initialize the sound system and channels
  m_Sound.Init(GethWnd(), 22050, 1, 16);
  m_SoundChannel.Create(&m_Sound, 22050, 1, 16);
  m_MusicChannel.Create(&m_Sound);
  
  // Load the master item list
  FILE *fp;
  for(long i=0;i<1024;i++)
    ZeroMemory(&m_MIL[i], sizeof(sItem));
  if((fp=fopen("..\\Data\\Game.mil", "rb")) != NULL) {
    for(i=0;i<1024;i++)
      fread(&m_MIL[i], 1, sizeof(sItem), fp);
    fclose(fp);
  }

  // Initialize the character controller
  m_CharController.SetData(this);
  m_CharController.Init(&m_Graphics, &m_Font,                 \
      "..\\Data\\Game.mcl", (sItem*)&m_MIL,                   \
      m_SpellController.GetSpell(0),                          \
      sizeof(g_CharMeshNames)/sizeof(char*), g_CharMeshNames, \
      "..\\Data\\", "..\\Data\\",                             \
      sizeof(g_CharAnimations) / sizeof(sCharAnimationInfo),  \
      (sCharAnimationInfo*)&g_CharAnimations,                 \
      &m_SpellController);

  // Initialize the spell controller
  m_SpellController.SetData(this);
  m_SpellController.Init(&m_Graphics,                         \
     "..\\Data\\Game.msl",                                    \
     sizeof(g_SpellMeshNames)/sizeof(char*),g_SpellMeshNames, \
     "..\\Data\\", &m_CharController);

  // Get the options bitmap
  m_Options.Load(&m_Graphics, "..\\Data\\Options.bmp");

  // Create the main, header, and stats windows
  m_Window.Create(&m_Graphics, &m_Font);
  m_Header.Create(&m_Graphics, &m_Font);
  m_Stats.Create(&m_Graphics, &m_Font);

  // Position all windows
  m_Window.Move(2,2, 636, 476);
  m_Header.Move(2,2,128,32,-1,-1,D3DCOLOR_RGBA(128,16,16,255));
  m_Stats.Move(2,2,128,48);

  // Set script application pointer
  m_Script.SetData(this);

  // Push the main menu state, setting menu options first
  g_MenuOptions = MENU_LOAD;
  m_StateManager.Push(MenuFrame, this);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Pop all states
  m_StateManager.PopAll(this);

  // Free controllers
  m_CharController.Free();
  m_SpellController.Free();

  // Free script object
  m_Script.Free();

  // Free level data
  FreeLevel();

  // Free the options texture
  m_Options.Free();

  // Free the text windows
  m_Window.Free();
  m_Header.Free();
  m_Stats.Free();

  // Shutdown sound
  m_MusicChannel.Free();
  m_SoundChannel.Free();
  m_Sound.Shutdown();

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
  static DWORD UpdateTimer = timeGetTime();

  // Limit all frame updates to 30 fps
  if(timeGetTime() < UpdateTimer + 33)
    return TRUE;
  UpdateTimer = timeGetTime();

  // Acquire input devices and read input for all states
  m_Keyboard.Acquire(TRUE);   // Read keyboard
  m_Keyboard.Read();
  m_Mouse.Acquire(TRUE);      // Read mouse
  m_Mouse.Read();

  // Process state, returning result
  return m_StateManager.Process(this);
}

BOOL cApp::RenderFrame(long Elapsed)
{
  long i, j;

  // Render simplified mesh for z-values
  m_Graphics.EnableZBuffer(TRUE);
  m_SceneObject.Render();

  // Draw the backdrop (composed of six textures)
  m_Graphics.EnableZBuffer(FALSE);
  m_Graphics.BeginSprite();
  for(i=0;i<2;i++) {
    for(j=0;j<3;j++)
      m_SceneTextures[i*3+j].Blit(j*256,i*256);
  }
  m_Graphics.EndSprite();

  // Draw the 3-D objects
  m_Graphics.EnableZBuffer(TRUE);
  m_CharController.Render(Elapsed);
  m_SpellController.Render();

  return TRUE;
}

float cApp::GetHeightBelow(float XPos, float YPos, float ZPos)
{
  BOOL  Hit;
  float u, v, Dist;
  DWORD FaceIndex;

  D3DXIntersect(m_SceneMesh.GetParentMesh()->m_Mesh, 
                &D3DXVECTOR3(XPos,YPos,ZPos),
                &D3DXVECTOR3(0.0f, -1.0f, 0.0f),
                &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);
  if(Hit == TRUE)
    return YPos-Dist;
  return YPos;
}

BOOL cApp::CheckIntersect(                                    \
                    float XStart, float YStart, float ZStart, \
                    float XEnd,   float YEnd,   float ZEnd,   \
                    float *Length)
{
  BOOL  Hit;
  float u, v, Dist;
  float XDiff, YDiff, ZDiff, Size;
  DWORD FaceIndex;
  D3DXVECTOR3 vecDir;

  XDiff = XEnd - XStart;
  YDiff = YEnd - YStart;
  ZDiff = ZEnd - ZStart;

  D3DXVec3Normalize(&vecDir, &D3DXVECTOR3(XDiff, YDiff, ZDiff));

  D3DXIntersect(m_SceneMesh.GetParentMesh()->m_Mesh,          \
                &D3DXVECTOR3(XStart,YStart,ZStart), &vecDir,  \
                &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);

  if(Hit == TRUE) {
    Size = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
    if(Dist > Size)
      Hit = FALSE;
    else {
      if(Length != NULL)
        *Length = Dist;
    }
  }

  return Hit;
}

float cApp::GetNextFloat(FILE *fp)
{
  char Buf[1024];
  long Pos = 0;
  int c;

  // Read until EOF or EOL
  while(1) {
    if((c = fgetc(fp)) == EOF)
      break;
    if(c == 0x0a || c == ' ')
      break;
    if((c >= '0' && c <= '9') || c == '.' || c == '-')
      Buf[Pos++] = c;
  }
  Buf[Pos] = 0;

  return (float)atof(Buf);
}

BOOL cApp::LoadLevel(long Num)
{
  char Filename[MAX_PATH];
  FILE *fp;
  long i;
  float XPos, YPos, ZPos, XAt, YAt, ZAt;

  FreeLevel(); // Free a prior level

  // Record scene number
  m_SceneNum = Num;

  // Load the backdrop textures
  for(i=0;i<6;i++) {
    sprintf(Filename, "..\\Data\\Scene%u%u.bmp", Num, i+1);
    if(m_SceneTextures[i].Load(&m_Graphics, Filename) == FALSE)
      return FALSE;
  }

  // Load the scene mesh and configure object
  sprintf(Filename, "..\\Data\\Scene%u.x", Num);
  if(m_SceneMesh.Load(&m_Graphics, Filename) == FALSE)
    return FALSE;
  m_SceneObject.Create(&m_Graphics, &m_SceneMesh);

  // Load the camera data
  sprintf(Filename, "..\\Data\\Cam%u.txt", Num);
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;
  XPos = GetNextFloat(fp);
  YPos = GetNextFloat(fp);
  ZPos = GetNextFloat(fp);
  XAt  = GetNextFloat(fp);
  YAt  = GetNextFloat(fp);
  ZAt  = GetNextFloat(fp);
  fclose(fp);
  m_Camera.Point(XPos, YPos, ZPos, XAt, YAt, ZAt);

  // Position the camera for the scene
  m_Graphics.SetCamera(&m_Camera);

  // Set no monsters in last frame
  m_MonstersLastFrame = FALSE;

  // Execute the script for loading this scene
  sprintf(Filename, "..\\Data\\Scene%lu.mls", Num);
  m_Script.Execute(Filename);

  return TRUE;
}

BOOL cApp::FreeLevel()
{
  sCharacter *CharPtr, *NextChar;
  long i;

  // Free scene mesh and textures
  m_SceneMesh.Free();
  m_SceneObject.Free();
  for(i=0;i<6;i++)
    m_SceneTextures[i].Free();

  // Free triggers and barriers
  m_Barrier.Free();
  m_Trigger.Free();

  // Free all non-pc characters
  if((CharPtr=m_CharController.GetParentCharacter()) != NULL) {
    while(CharPtr != NULL) {
      // Remember next character
      NextChar = CharPtr->Next;

      // Remove non-PC character
      if(CharPtr->Type != CHAR_PC)
        m_CharController.Remove(CharPtr);

      // Go to next character
      CharPtr = NextChar;
    }
  }  

  // Free all spell effects
  m_SpellController.Free();

  return TRUE;
}

BOOL cApp::PlaySound(long Num)
{
  if(Num >=0 && Num < NUM_SOUNDS) {
    m_SoundData.Free();

    if(m_SoundData.LoadWAV(g_SoundFilenames[Num]) == TRUE)
      m_SoundChannel.Play(&m_SoundData);

    return TRUE;
  }

  return FALSE;
}

BOOL cApp::PlayMusic(long Num)
{
  // Don't bother changing song if same already playing
  if(g_CurrentMusic == Num)
    return TRUE;

  // Stop and free current song
  m_MusicChannel.Stop();
  m_MusicChannel.Free();

  // Fade music out, giving DirectMusic enough time
  // to finish up last song (or else new song doesn't
  // play correctly.  The 700 is based on play volume
  // of music, so adjust ahead.
  DWORD Timer = timeGetTime() + 700;
  while(timeGetTime() < Timer) {
    DWORD Level = (Timer - timeGetTime()) / 10;
    m_MusicChannel.SetVolume(Level);
  }

  // Load and play new song
  m_MusicChannel.Load(g_MusicFilenames[Num]);
  m_MusicChannel.Play(70,0);

  // Remember new song #
  g_CurrentMusic = Num;

  return TRUE;
}

BOOL cApp::StopMusic()
{
  // Stop and free music, marking current song as none
  m_MusicChannel.Stop();
  m_MusicChannel.Free();
  g_CurrentMusic = -1;

  return TRUE;
}

BOOL cApp::WinGame()
{
  m_StateManager.PopAll(this);
  g_MenuOptions = MENU_LOAD;
  m_StateManager.Push(MenuFrame, this);

  return TRUE;
}

BOOL cApp::StartOfCombat()
{
  long i;
  char Filename[MAX_PATH];

  // Clear combat booty
  m_CombatExp = 0;
  m_CombatMoney = 0;
  for(i=0;i<1024;i++)
    m_CombatItems[i] = 0;

  // Trigger start of combat script
  sprintf(Filename, "..\\Data\\SOC%lu.mls", m_SceneNum);
  m_Script.Execute(Filename);

  return TRUE;
}

BOOL cApp::EndOfCombat()
{
  char Filename[MAX_PATH];
  char Text[2000], Gained[128];
  long LevelUp[6] = { 10, 30, 80, 150, 350, 500 }; 
  long i;

  // Give earned combat stuff
  g_PCChar->Def.Money += m_CombatMoney;
  g_PCChar->Def.Experience += m_CombatExp;
  for(i=0;i<1024;i++) {
    if(m_CombatItems[i])
      g_PCChar->CharICS->Add(i, m_CombatItems[i], NULL);
  }

  // Set the header text
  m_Header.SetText("Victory!");
  m_Window.SetText("");

  // Start constructing the main window text
  strcpy(Text, "\r\n\n");

  // Gold gained
  if(m_CombatMoney) {
    sprintf(Gained, "Gained %lu gold!\r\n", m_CombatMoney);
    strcat(Text, Gained);
  }

  // Experience gained
  sprintf(Gained, "Gained %lu experience!\r\n", m_CombatExp);
  strcat(Text, Gained);

  // Process level up 
  for(i=0;i<6;i++) {
    if(g_PCChar->Def.Experience >= LevelUp[i] &&              \
                               g_PCChar->Def.Level < (i+2)) {
      g_PCChar->Def.Level = i+2;
      strcat(Text, "Level up!\r\n");

      // Add bonuses for leveling up
 
      // Abilities
      g_PCChar->Def.HealthPoints += 10;
      g_PCChar->Def.ManaPoints += 10;
      g_PCChar->Def.Attack += 2;
      g_PCChar->Def.Defense += 1;
      g_PCChar->Def.Agility += 1;
      g_PCChar->Def.Resistance += 1;
      g_PCChar->Def.Mental += 1;
      g_PCChar->Def.ToHit += 5;

      strcat(Text, "Stats up!\r\n");

      // Learn spells
      if(i < 5) {
        g_PCChar->Def.MagicSpells[0] |= (1 << i);
        sprintf(Gained, "Learned spell %s\r\n",               \
                         m_SpellController.GetSpell(i)->Name);
        strcat(Text, Gained);
      }

      // Max health and mana to match definition
      g_PCChar->HealthPoints = g_PCChar->Def.HealthPoints;
      g_PCChar->ManaPoints   = g_PCChar->Def.ManaPoints;
    }
  }

  // Add gained items
  for(i=0;i<1024;i++) {
    if(m_CombatItems[i]) {
      sprintf(Gained, "Found %lu x %s\r\n", m_CombatItems[i], \
              m_MIL[i].Name);
      strcat(Text, Gained);
    }
  }

  // Lock the keyboard and mouse
  m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);

  // Render the scene while waiting for keypress/buttonpress
  while(1) {
    // Break when space pressed
    m_Keyboard.Acquire(TRUE);
    m_Keyboard.Read();
    if(m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)
      break;

    // Break when left mouse button pressed
    m_Mouse.Acquire(TRUE);
    m_Mouse.Read();
    if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
      break;

    // Render the scene w/window
    m_Graphics.ClearZBuffer();
    if(m_Graphics.BeginScene() == TRUE) {
      RenderFrame();
      m_Window.Render(Text);
      m_Header.Render();
      m_Graphics.EndScene();
    }
    m_Graphics.Display();
  }

  // Relock keyboard and mouse
  m_Keyboard.SetLock(KEY_SPACE, TRUE);
  m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
  m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
  m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);

  // Trigger end of combat script
  sprintf(Filename, "..\\Data\\EOC%lu.mls", m_SceneNum);
  m_Script.Execute(Filename);

  return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}

BOOL cApp::SetupTeleport(long Level,                          \
                         float XPos, float YPos, float ZPos)
{
  // Remember which map to teleport to
  m_TeleportMap = Level;

  // Move character to correct teleport location
  m_CharController.Move(0, XPos, YPos, ZPos);

  return TRUE;
}

BOOL cApp::SetupBarter(sCharacter *Character, char *ICSFilename)
{
  g_BarterChar = Character;
  strcpy(g_BarterICS, ICSFilename);
  m_StateManager.Push(BarterFrame, this);

  return TRUE;
}

sCharacter *cApp::GetCharacterAt(long XPos, long YPos)
{
  D3DXVECTOR3 vecRay, vecDir;   
  D3DXVECTOR3 vecMeshRay, vecMeshDir;
  D3DXVECTOR3 vecTemp;          
  D3DXMATRIX  matProj, matView, *matWorld;
  D3DXMATRIX  matInv;           
  DWORD       FaceIndex;
  BOOL        Hit;
  float       u, v, Dist;
  sCharacter  *CharPtr;         
  sMesh      *MeshPtr;

  // Get parent character object  
  if((CharPtr = m_CharController.GetParentCharacter()) == NULL)
    return NULL;

  // Get the project, view, and inversed view matrices
  m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_PROJECTION,   \
                                          &matProj);
  m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_VIEW,         \
                                          &matView);
  D3DXMatrixInverse(&matInv, NULL, &matView);

  // Compute the vector of the pick ray in screen space
  vecTemp.x =  (((2.0f * (float)XPos) /                       \
               (float)m_Graphics.GetWidth()) - 1.0f) /        \
               matProj._11;
  vecTemp.y = -(((2.0f * (float)YPos) /                       \
               (float)m_Graphics.GetHeight()) - 1.0f) /       \
               matProj._22;
  vecTemp.z =  1.0f;

  // Transform the screen space ray
  vecRay.x = matInv._41;
  vecRay.y = matInv._42;
  vecRay.z = matInv._43;
  vecDir.x = vecTemp.x * matInv._11 +                         \
             vecTemp.y * matInv._21 +                         \
             vecTemp.z * matInv._31;
  vecDir.y = vecTemp.x * matInv._12 +                         \
             vecTemp.y * matInv._22 +                         \
             vecTemp.z * matInv._32;
  vecDir.z = vecTemp.x * matInv._13 +                         \
             vecTemp.y * matInv._23 +                         \
             vecTemp.z * matInv._33;

  // Scan through each character and intersect check
  while(CharPtr != NULL) {

    // Scan through character meshes
    MeshPtr = CharPtr->Object.GetMesh()->GetParentMesh();
    while(MeshPtr != NULL) {

      // Transform ray and direction by object's
      // world transformation matrix
      matWorld = CharPtr->Object.GetMatrix();
      D3DXMatrixInverse(&matInv, NULL, matWorld);
      D3DXVec3TransformCoord(&vecMeshRay, &vecRay, &matInv);
      D3DXVec3TransformNormal(&vecMeshDir, &vecDir, &matInv);

      // Check for intersection
      D3DXIntersect(MeshPtr->m_Mesh, &vecMeshRay,&vecMeshDir, \
                 &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);

      // Check if ray hit character and return ID if so
      if(Hit == TRUE)
        return CharPtr;

      // Go to next mesh
      MeshPtr = MeshPtr->m_Next;
    }

    // Go to next character
    CharPtr = CharPtr->Next;
  }

  return NULL;
}

BOOL cApp::LastPointReached(sCharacter *Character)
{
  float XDiff, YDiff, ZDiff;
  float Dist, Radius;

  // Error checking
  if(Character == NULL)
    return FALSE;
  if(Character->AI != CHAR_ROUTE)
    return FALSE;

  // Determine if character has reached point
  XDiff = (float)fabs(Character->XPos -                       \
          Character->Route[Character->NumPoints-1].XPos);
  YDiff = (float)fabs(Character->YPos -                       \
          Character->Route[Character->NumPoints-1].YPos);
  ZDiff = (float)fabs(Character->ZPos -                       \
          Character->Route[Character->NumPoints-1].ZPos);
  Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;
  Radius = m_CharController.GetXZRadius(Character) * 0.25f;
  
  // Return TRUE if point being touched
  if(Dist < (Radius*Radius))
    return TRUE;

  // Last point not being touched
  return FALSE;
}

void cApp::MenuFrame(void *Ptr, long Purpose)
{
  typedef struct {
    float x, y, z;
    float u, v;
  } sMenuVertex;
  sMenuVertex Verts[4] = {
    { -100.0f,  100.0f, 1.0f, 0.0f, 0.0f },
    {  100.0f,  100.0f, 1.0f, 1.0f, 0.0f },
    { -100.0f, -100.0f, 1.0f, 0.0f, 1.0f },
    {  100.0f, -100.0f, 1.0f, 1.0f, 1.0f }
  };
  long MenuFVF = (D3DFVF_XYZ | D3DFVF_TEX1);
  cApp                 *App = (cApp*)Ptr;
  static cVertexBuffer  MenuVB;
  static cTexture       MenuTexture;
  static cTexture       MenuSelect;
  static cCamera        MenuCam;
  static cWorldPosition MenuPos;
  static cFont          MenuFont;
  long                  Num;

  // Initialize menu related data
  if(Purpose == INITPURPOSE) {
    // Create and set the menu vertices
    MenuVB.Create(&App->m_Graphics, 4, MenuFVF,               \
                  sizeof(sMenuVertex));
    MenuVB.Set(0,4,Verts);

    // Load textures
    MenuTexture.Load(&App->m_Graphics, "..\\Data\\MenuBD.bmp");
    MenuSelect.Load(&App->m_Graphics, "..\\Data\\Select.bmp");

    // Create a large font
    MenuFont.Create(&App->m_Graphics, "Times New Roman", 72);

    // Setup the menu camera
    MenuCam.Point(0.0f, 0.0f, -150.0f, 0.0f, 0.0f, 0.0f);

    return;
  }

  // Shutdown resources used in menu
  if(Purpose == SHUTDOWNPURPOSE) {
    MenuVB.Free();
    MenuTexture.Free();
    MenuSelect.Free();
    MenuFont.Free();
    return;
  }

  // Process a frame of menu

  // Exit game or return to game if ESC pressed
  if(App->m_Keyboard.GetKeyState(KEY_ESC) == TRUE) {
    App->m_StateManager.Pop(App);
    return;
  }

  // See which option was selected if mouse button pressed
  if(App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
    
    // Lock the mouse button and clear button state
    App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
    App->m_Mouse.SetButtonState(MOUSE_LBUTTON, FALSE);

    // Determine which, if any selection
    Num = App->m_Mouse.GetYPos() - 126;
    if(Num >= 0) {
      Num /= 64;

      // Make sure option enabled 
      if(Num==1 && !(g_MenuOptions & MENU_BACK))
        return;
      if(Num==2 && !(g_MenuOptions & MENU_LOAD))
        return;
      if(Num==3 && !(g_MenuOptions & MENU_SAVE))
        return;

      // Pop the menu state
      App->m_StateManager.Pop(App);

      // Determine what to do based on selection
      switch(Num) {
        case 0: // New game
          // Prepare game data for new game

          // Pop all states
          App->m_StateManager.PopAll(App);

          // Clear all character and spell data
          App->m_CharController.Free();
          App->m_SpellController.Free();

          // Reset script flags
          App->m_Script.Reset();

          // Create the PC and get pointer to him/her
          App->m_CharController.Add(0,0, CHAR_PC, CHAR_STAND, \
                               -100.0f, 0.0f, 50.0f, 3.14f);
          g_PCChar = App->m_CharController.GetCharacter(0);

          // Clear teleport to map #
          App->m_TeleportMap = -1;

          // Push game frame state
          App->m_StateManager.Push(App->GameFrame, App);
  
          // Start new game and let script process as startup
          App->LoadLevel(1);

          break;

        case 1: // Return to game
          App->m_StateManager.Push(App->GameFrame, App);
          break;

        case 2: // Load game
          // Prepare game data for loading game

          // Pop all states
          App->m_StateManager.PopAll(App);

          // Clear all character and spell data
          App->m_CharController.Free();
          App->m_SpellController.Free();

          // Create the PC and get pointer to him/her
          App->m_CharController.Add(0,0, CHAR_PC, CHAR_STAND, \
                               -100.0f, 0.0f, 50.0f, 3.14f);
          g_PCChar = App->m_CharController.GetCharacter(0);

          // Load character's stats and inventory
          App->m_CharController.Load(0, "..\\Data\\Char.cs");
          g_PCChar->CharICS->Load("..\\Data\\Char.ci");

          // Equip weapon
          Num = g_PCChar->Def.Weapon;
          App->m_CharController.Equip(g_PCChar, 0,            \
                                      WEAPON, FALSE);
          if(Num != -1)
            App->m_CharController.Equip(g_PCChar, Num,        \
                                        WEAPON, TRUE);
         
          // Set health and mana to full
          g_PCChar->HealthPoints = g_PCChar->Def.HealthPoints;
          g_PCChar->ManaPoints = g_PCChar->Def.ManaPoints;

          // Load script flags
          App->m_Script.Load("..\\Data\\Script.sav");

          // Clear teleport to map #
          App->m_TeleportMap = -1;

          // Push game frame state
          App->m_StateManager.Push(App->GameFrame, App);

          // Position character
          App->m_CharController.Move(0, 100.0f, 0.0f, -100.0f);

          // Start in town
          App->LoadLevel(1);

          break;

        case 3: // Save game
          // Save script flags
          App->m_Script.Save("..\\Data\\Script.sav");

          // Save character's stats and inventory
          App->m_CharController.Save(0, "..\\Data\\Char.cs");
          g_PCChar->CharICS->Save("..\\Data\\Char.ci");

          break;

        case 4: // Quit game
          App->m_StateManager.PopAll(App);
          break;
      }

      return;
    }
  }

  // Set the menu camera
  App->m_Graphics.SetCamera(&MenuCam);

  // Rotate backdrop vertex buffer world position
  MenuPos.Rotate(0.0f, 0.0f, (float)timeGetTime() / 4000.0f);

  // Render the frame's graphics
  if(App->m_Graphics.BeginScene() == TRUE) {

    // Render backdrop (turning off Z-Buffering first)
    App->m_Graphics.EnableZBuffer(FALSE);
    App->m_Graphics.SetWorldPosition(&MenuPos);
    App->m_Graphics.SetTexture(0, &MenuTexture);
    MenuVB.Render(0, 2, D3DPT_TRIANGLESTRIP);

    // Draw the game's title
    MenuFont.Print("The Tower",0,16,640,0,0xFFFFFFFF,DT_CENTER);

    // Select option based on mouse position
    Num = App->m_Mouse.GetYPos() - 126;
    if(Num >= 0) {
      Num /= 64;

      if(!Num || 
         (Num==1 && (g_MenuOptions & MENU_BACK)) ||
         (Num==2 && (g_MenuOptions & MENU_LOAD)) ||
         (Num==3 && (g_MenuOptions & MENU_SAVE)) ||
         (Num == 4))
        MenuSelect.Blit(192,Num*64+126);
    }

    // Draw enabled options
    App->m_Font.Print("New Game", 0,150,640,0,                \
                      0xFFFFFFFF,DT_CENTER);

    if(g_MenuOptions & MENU_BACK)
      App->m_Font.Print("Back to Game",0,214,640,0,           \
                        0xFFFFFFFF,DT_CENTER);

    if(g_MenuOptions & MENU_LOAD)
      App->m_Font.Print("Load Game",0,278,640,0,              \
                        0xFFFFFFFF,DT_CENTER);

    if(g_MenuOptions & MENU_SAVE)
      App->m_Font.Print("Save Game",0,342,640,0,              \
                        0xFFFFFFFF,DT_CENTER);

    App->m_Font.Print("Quit",0,410,640,0,0xFFFFFFFF,DT_CENTER);

    App->m_Graphics.EndScene();
  }
  App->m_Graphics.Display();
}

void cApp::GameFrame(void *Ptr, long Purpose)
{
  cApp *App = (cApp*)Ptr;
  sCharacter *CharPtr;
  BOOL        MonstersInLevel;
  long        TriggerNum;
  char        Filename[MAX_PATH], Stats[256];
  float       MaxY;

  // Only process frame states
  if(Purpose != FRAMEPURPOSE)
    return;

  // Quit to menu screen if ESCAPE pressed
  if(App->m_Keyboard.GetKeyState(KEY_ESC) == TRUE) {
    // Setup menu options
    g_MenuOptions = MENU_BACK | MENU_SAVE | MENU_LOAD;

    // Push main menu state
    App->m_StateManager.Push(App->MenuFrame, App);

    return;
  }

  // If teleporting, then handle that first and return
  if(App->m_TeleportMap != -1) {
    // Free level and process a new one
    App->FreeLevel();
    App->LoadLevel(App->m_TeleportMap);
    
    App->m_TeleportMap = -1;  // Clear out teleport map #

    return;  // No more processing this frame
  }

  // Mark no monsters in level
  MonstersInLevel = FALSE;

  // See if any character are in level. If any monsters,
  // flag as such and change their AI to wander if their
  // charge is less then 70, follow AI otherwise.
  // Also, process whenever a character reaches a route point.
  CharPtr = App->m_CharController.GetParentCharacter();
  while(CharPtr != NULL) {

    // Alter monster's AI based on charge
    if(CharPtr->Type == CHAR_MONSTER) {
      MonstersInLevel = TRUE;

      // Change AI based on charge
      if(CharPtr->Charge >= 70.0f) {
        CharPtr->AI = CHAR_FOLLOW;
        CharPtr->TargetChar = g_PCChar;
        CharPtr->Distance = 0.0f;
      } else {
        CharPtr->AI = CHAR_WANDER;
      }
    }

    // Check if an NPC character has reached last route point
    if(CharPtr->Type==CHAR_NPC && CharPtr->AI==CHAR_ROUTE) {

      // Was last point reached?
      if(App->LastPointReached(CharPtr) == TRUE) {
        // Process the route point script for character.
        sprintf(Filename,"..\\Data\\EOR%lu.mls", CharPtr->ID);
        App->m_Script.Execute(Filename);

        // Don't process any more this frame
        return;
      }
    }

    // Go to next character
    CharPtr = CharPtr->Next;
  }

  // Handle start of combat stuff
  if(MonstersInLevel==TRUE && App->m_MonstersLastFrame==FALSE)
    App->StartOfCombat();

  // Handle end of combat stuff if combat over
  if(MonstersInLevel==FALSE && App->m_MonstersLastFrame==TRUE)
    App->EndOfCombat();

  // Remember if monsters where in this frame
  // And reset player's charge to full if no monsters
  if((App->m_MonstersLastFrame = MonstersInLevel) == FALSE)
    g_PCChar->Charge = 100.0f;

  // Update controllers
  App->m_CharController.Update(33);
  App->m_SpellController.Update(33);

  // Check for triggers and execute script
  if((TriggerNum = App->m_Trigger.GetTrigger(g_PCChar->XPos,  \
                                        g_PCChar->YPos,       \
                                        g_PCChar->ZPos))) {
    sprintf(Filename, "..\\Data\\Trig%lu.mls", TriggerNum);
    App->m_Script.Execute(Filename);
    
    return;  // Don't process any more this frame
  }

  // Position the camera for the scene
  App->m_Graphics.SetCamera(&App->m_Camera);

  // Render everything
  App->m_Graphics.ClearZBuffer();
  if(App->m_Graphics.BeginScene() == TRUE) {

    App->RenderFrame(33);

    // Render the player's charge bar, but only during combat
    if(MonstersInLevel == TRUE) {
      D3DXMATRIX matWorld, matView, matProj;
      D3DVIEWPORT9 vpScreen;
      D3DXVECTOR3 vecPos;

      // Get the world, projection, and view transformations
      D3DXMatrixIdentity(&matWorld);
      App->m_Graphics.GetDeviceCOM()->GetTransform(           \
                              D3DTS_VIEW, &matView);
      App->m_Graphics.GetDeviceCOM()->GetTransform(           \
                              D3DTS_PROJECTION, &matProj);

      // Get viewport
      App->m_Graphics.GetDeviceCOM()->GetViewport(&vpScreen);

      // Offset charge bar by character's height
      g_PCChar->Object.GetBounds(NULL,NULL,NULL,              \
                                 NULL,&MaxY,NULL,NULL);

      // Project coordinates to screen  
      D3DXVec3Project(&vecPos, &D3DXVECTOR3(                  \
                                     g_PCChar->XPos,          \
                                     g_PCChar->YPos + MaxY,   \
                                     g_PCChar->ZPos),         \
                      &vpScreen, &matProj, &matView, &matWorld);

      // move 4 pixels right before displaying
      vecPos.x += 8.0f;

      // Display charge bar below player (flash when full)
      App->m_Graphics.EnableZBuffer(FALSE);
      App->m_Graphics.BeginSprite();
      App->m_Options.Blit((long)vecPos.x,(long)vecPos.y,      \
                           0,0,16,4);
      if(g_PCChar->Charge >= 100.0f) {
        if(timeGetTime() & 1)
         App->m_Options.Blit((long)vecPos.x,(long)vecPos.y,   \
                             0,4,16,4);
      } else {
        App->m_Options.Blit((long)vecPos.x,(long)vecPos.y,    \
                 0,4,(long)(g_PCChar->Charge/100.0f*16.0f),4);
      }

      App->m_Graphics.EndSprite();
    }

    // Draw the player's stats at top-left
    sprintf(Stats, "%ld / %ld HP\r\n%ld / %ld MP",            \
      g_PCChar->HealthPoints, g_PCChar->Def.HealthPoints,     \
      g_PCChar->ManaPoints, g_PCChar->Def.ManaPoints);
    App->m_Stats.Render(Stats);

    App->m_Graphics.EndScene();
  }
  App->m_Graphics.Display();

}

void cApp::BarterFrame(void *Ptr, long Purpose)
{
  cApp *App = (cApp*)Ptr;
  static cCharICS ICS;
  sCharItem *ItemPtr, *ScanPtr;
  char Text[256];
  long Color, Num, Sel;

  // Initialize barter data
  if(Purpose == INITPURPOSE) {
    // Load the ICS to use for bartering
    ICS.Load(g_BarterICS);

    // Set text
    App->m_Header.SetText("Shop");
    App->m_Window.SetText("\r\n\nWhat would you like to buy?");

    return;
  }

  // Shutdown resources used in bartering
  if(Purpose == SHUTDOWNPURPOSE) {
    // Free barter ICS
    ICS.Free();

    return;
  }

  // Process a frame of bartering

  // Exit bartering if ESCAPE or right mouse button pressed
  if(App->m_Keyboard.GetKeyState(KEY_ESC) == TRUE ||          \
     App->m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
    App->m_Keyboard.SetLock(KEY_ESC, TRUE);
    App->m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);
    App->m_StateManager.Pop(App);
    return;
  }

  // Determine which item is selected with mouse
  if((Sel = App->m_Mouse.GetYPos() - 128) >= 0)
    Sel /= 32;

  // See if click on item to buy
  if(App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
    App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);

    // See which item was clicked
    if(Sel >= 0 && Sel < ICS.GetNumItems()) {

      // Get pointer to item
      ItemPtr = ICS.GetItem(Sel);

      // Make sure PC has enough gold for item
      if(g_PCChar->Def.Money >=                               \
                        App->m_MIL[ItemPtr->ItemNum].Price) {

        // Search for item already in inventory
        ScanPtr = g_PCChar->CharICS->GetParentItem();
        while(ScanPtr != NULL) {
          // Increase quantity if item already in inventory
          if(ScanPtr->ItemNum == ItemPtr->ItemNum) {
            ScanPtr->Quantity++;
            break;
          }

          // Go to next pointer
          ScanPtr = ScanPtr->Next;
        }

        // Add item to PC's inventory if not already
        if(ScanPtr == NULL)
          g_PCChar->CharICS->Add(ItemPtr->ItemNum, 1, NULL);

        // Deduct price from PC
        g_PCChar->Def.Money -=                                \
                          App->m_MIL[ItemPtr->ItemNum].Price;

        // Play click sound
        App->PlaySound(8);
      }
    }
  }

  // Render the bartering scene
  if(App->m_Graphics.BeginScene() == TRUE) {
    App->RenderFrame();
    App->m_Window.Render();
    App->m_Header.Render();

    // Display items to buy
    Num = 0;
    ItemPtr = ICS.GetParentItem();
    while(ItemPtr != NULL) {

      // Calculate color to draw based on mouse position
      if(Num == Sel)
        Color = D3DCOLOR_RGBA(255,255,0,255);
      else
        Color = D3DCOLOR_RGBA(128,128,128,255);
      
      // Display item name
      App->m_Font.Print(App->m_MIL[ItemPtr->ItemNum].Name,    \
                        32, Num*32+128, 0, 0, Color);

      // Display item price
      sprintf(Text, "$%lu", App->m_MIL[ItemPtr->ItemNum].Price);
      App->m_Font.Print(Text, 300, Num*32+128, 0, 0, Color);

      // Go down one line
      Num++;

      // Go to next item
      ItemPtr = ItemPtr->Next;
    }

    // Display character's gold at top-right
    sprintf(Text, "Money: $%lu", g_PCChar->Def.Money);
    App->m_Font.Print(Text, 320, 32);

    App->m_Graphics.EndScene();
  }
  App->m_Graphics.Display();
}

void cApp::StatusFrame(void *Ptr, long Purpose)
{
  cApp *App = (cApp*)Ptr;
  sCharItem *ItemPtr;
  char Text[256];
  long Color, Num, Sel, i;

  // Initialize status
  if(Purpose == INITPURPOSE) {
    // Set text
    App->m_Header.SetText("Status");
    App->m_Window.SetText("");

    return;
  }

  // Return on shutdown purpose
  if(Purpose == SHUTDOWNPURPOSE)
    return;

  // Process a frame of status screen

  // Exit screen if ESCAPE or right mouse button pressed
  if(App->m_Keyboard.GetKeyState(KEY_ESC) == TRUE ||          \
     App->m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
    App->m_Keyboard.SetLock(KEY_ESC, TRUE);
    App->m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);
    App->m_StateManager.Pop(App);
    return;
  }

  // Determine which item is selected with mouse
  if((Sel = App->m_Mouse.GetYPos() - 128) >= 0)
    Sel /= 32;

  // See if click on item to to use/equip
  if(App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
    App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);

    // See which item was clicked
    if(Sel >= 0 && Sel < g_PCChar->CharICS->GetNumItems()) {

      // Get pointer to item
      ItemPtr = g_PCChar->CharICS->GetItem(Sel);

      // Determine what to do w/item
      switch(App->m_MIL[ItemPtr->ItemNum].Category) {
        case WEAPON:
          // Equip/Unequip weapon
          if(g_PCChar->Def.Weapon == ItemPtr->ItemNum)
            App->m_CharController.Equip(g_PCChar, 0,          \
                                        WEAPON, FALSE);
          else
            App->m_CharController.Equip(g_PCChar,             \
                                        ItemPtr->ItemNum,     \
                                        WEAPON, TRUE);
          break;

        case ARMOR:
          // Equip/Unequip armor
          if(g_PCChar->Def.Armor == ItemPtr->ItemNum)
            App->m_CharController.Equip(g_PCChar, 0,          \
                                        ARMOR, FALSE);
          else
            App->m_CharController.Equip(g_PCChar,             \
                                        ItemPtr->ItemNum,     \
                                        ARMOR, TRUE);
          break;

        case SHIELD:
          // Equip/Unequip shield
          if(g_PCChar->Def.Shield == ItemPtr->ItemNum)
            App->m_CharController.Equip(g_PCChar, 0,          \
                                        SHIELD, FALSE);
          else
            App->m_CharController.Equip(g_PCChar,             \
                                        ItemPtr->ItemNum,     \
                                        SHIELD, TRUE);
          break;

        case HEALING:
          g_PCChar->HealthPoints +=                           \
                           App->m_MIL[ItemPtr->ItemNum].Value;
          if(g_PCChar->HealthPoints>g_PCChar->Def.HealthPoints)
            g_PCChar->HealthPoints=g_PCChar->Def.HealthPoints;
          break;
      }

      // Reduce quantity if flagged as use once
      if(CheckItemFlag(App->m_MIL[ItemPtr->ItemNum].Flags,    \
                                                   USEONCE)) {
        if(ItemPtr->Quantity == 1)
          g_PCChar->CharICS->Remove(ItemPtr);
        else
          ItemPtr->Quantity--;
      }

      // Play click sound
      App->PlaySound(8);
    }
  }

  // Render the scene
  if(App->m_Graphics.BeginScene() == TRUE) {
    App->RenderFrame();
    App->m_Window.Render();
    App->m_Header.Render();

    // Display inventory on left
    App->m_Font.Print("Inventory:", 8, 96);
    Num = 0;
    ItemPtr = g_PCChar->CharICS->GetParentItem();
    while(ItemPtr != NULL) {

      // Calculate color to draw based on mouse position
      if(Num == Sel)
        Color = D3DCOLOR_RGBA(255,255,0,255);
      else
        Color = D3DCOLOR_RGBA(128,128,128,255);
      
      // Display item name w/quantity
      sprintf(Text, "%lu x %s", ItemPtr->Quantity,            \
              App->m_MIL[ItemPtr->ItemNum].Name);
      App->m_Font.Print(Text, 32, Num*32+128, 0, 0, Color);

      // If item is equipped, then show E next to it
      if(g_PCChar->Def.Weapon == ItemPtr->ItemNum ||          \
         g_PCChar->Def.Armor  == ItemPtr->ItemNum ||          \
         g_PCChar->Def.Shield == ItemPtr->ItemNum)
        App->m_Font.Print("E", 16, Num*32+128);

      // Go down one line
      Num++;

      // Go to next item
      ItemPtr = ItemPtr->Next;
    }

    // Display known spells at right
    App->m_Font.Print("Spells:", 300, 220);
    for(i=0;i<64;i++) {
      if(g_PCChar->Def.MagicSpells[i/32] & (1 << (i & 31))) {
        sprintf(Text, "%lu: %s", i+1,                         \
                    App->m_SpellController.GetSpell(i)->Name);
        App->m_Font.Print(Text, 320, i*32+252);
      }
    }

    // Display character's stats at top-right
    App->m_Font.Print("Stats", 300, 32);
    sprintf(Text, "%ld / %ld HP", g_PCChar->HealthPoints,     \
                                  g_PCChar->Def.HealthPoints);
    App->m_Font.Print(Text, 320, 64);
    sprintf(Text, "%ld / %ld MP", g_PCChar->ManaPoints,       \
                                  g_PCChar->Def.ManaPoints);
    App->m_Font.Print(Text, 320, 96);
    sprintf(Text, "$ %lu", g_PCChar->Def.Money);
    App->m_Font.Print(Text, 320, 128);
    sprintf(Text, "Lvl/Exp: %lu / %lu", g_PCChar->Def.Level,  \
                                   g_PCChar->Def.Experience);
    App->m_Font.Print(Text, 320, 160);

    App->m_Graphics.EndScene();
  }
  App->m_Graphics.Display();
}
