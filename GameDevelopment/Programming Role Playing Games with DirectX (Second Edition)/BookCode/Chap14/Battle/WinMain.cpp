/**************************************************
WinMain.cpp
Chapter 14 Combat Sequence Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"

#include "Window.h"
#include "Chars.h"
#include "WinMain.h"

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
    { "..\\Data\\fireball.x"   },
    { "..\\Data\\Explosion.x"  },
    { "..\\Data\\Groundball.x" },
    { "..\\Data\\ice.x" },
    { "..\\Data\\bomb.x" },
    { "..\\Data\\heal.x" },
    { "..\\Data\\force.x" }
  };

///////////////////////////////////////////////////////////
// Overloaded application class functions
//////////////////////////////////////////////////////////
cApp::cApp()
{ 
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU;
  strcpy(m_Class, "BattleClass");
  strcpy(m_Caption, "Battle Demo by Jim Adams");
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
  
  // Create the target vertex buffer
  typedef struct {
    float x, y, z;
    D3DCOLOR Diffuse;
  } sVertex;
  sVertex Vert[6] = {
    { -20.0f,  40.0f, 0.0f, 0xFFFF4444 },
    {  20.0f,  40.0f, 0.0f, 0xFFFF4444 },
    {   0.0f,  20.0f, 0.0f, 0xFFFF4444 },
    {   0.0f, -20.0f, 0.0f, 0xFFFF4444 },
    {  20.0f, -40.0f, 0.0f, 0xFFFF4444 },
    { -20.0f, -40.0f, 0.0f, 0xFFFF4444 }
  };
  m_Target.Create(&m_Graphics, 6, D3DFVF_XYZ|D3DFVF_DIFFUSE,
                  sizeof(sVertex));
  m_Target.Set(0,6,&Vert);

  // Load the buttons and other graphics
  m_Buttons.Load(&m_Graphics, "..\\Data\\Buttons.bmp");

  // Load the terrain mesh and set object
  m_TerrainMesh.Load(&m_Graphics, "..\\Data\\Battle.x",       \
                     "..\\Data\\");
  m_TerrainObject.Create(&m_Graphics, &m_TerrainMesh);

  // Create text windows
  m_Stats.Create(&m_Graphics, &m_Font);
  m_Stats.Move(508, 400, 128, 48);
  m_Options.Create(&m_Graphics, &m_Font);
  m_Options.Move(4, 4, 632, 328);

  // Load the master item list
  for(i=0;i<1024;i++)
    ZeroMemory(&m_MIL[i], sizeof(sItem));
  if((fp=fopen("..\\Data\\Default.mil", "rb")) != NULL) {
    for(i=0;i<1024;i++)
      fread(&m_MIL[i], 1, sizeof(sItem), fp);
    fclose(fp);
  }

  // Initialize the character controller
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
                       200.0f, 0.0f, 0.0f, 4.71f);

  // Hardcoded - add some other characters
  m_CharController.Add(1, 1, CHAR_MONSTER, CHAR_STAND,        \
                       -200.0f, 0.0f, 0.0f, 1.57f);
  m_CharController.Add(2, 1, CHAR_MONSTER, CHAR_STAND,        \
                       -100.0f, 0.0f, -200.0f, 1.57f);
  m_CharController.Add(3, 1, CHAR_MONSTER, CHAR_STAND,        \
                       0.0f, 0.0f, 100.0f, 1.57f);

  // Give an axe to one of the monsters
  m_CharController.Equip(m_CharController.GetCharacter(1),    \
                         8, WEAPON, TRUE);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free controllers
  m_CharController.Free();
  m_SpellController.Free();

  // Free objects and meshes
  m_TerrainMesh.Free();
  m_TerrainObject.Free();

  // Free windows
  m_Stats.Free();
  m_Options.Free();

  // Free target vertex buffer
  m_Target.Free();
  m_Buttons.Free();

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
  static sCharacter *PCChar=m_CharController.GetCharacter(0);
  static BOOL SelectSpell = FALSE;
  static long TargetID = -1;
  cWorldPosition Pos;
  sCharacter *CharPtr;
  sSpell     *SpellPtr;
  char Text[128];
  long  x, y, Num, i;
  float MinY, MaxY, YOff;

  // Lock to 30 fps
  if(timeGetTime() < UpdateCounter + 33)
    return TRUE;
  UpdateCounter = timeGetTime();

  // Read in input
  m_Keyboard.Acquire(TRUE);
  m_Keyboard.Read();
  m_Mouse.Acquire(TRUE);
  m_Mouse.Read();

  // Exit if ESC pressed
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Get selected character if left button pressed
  if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {

    // Get mouse coordinates
    x = m_Mouse.GetXPos();
    y = m_Mouse.GetYPos();

    // Lock the mouse button
    m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
    m_Mouse.SetButtonState(MOUSE_LBUTTON, FALSE);

    // See if selecting a spell
    if(SelectSpell == TRUE) {

      // Get pointer to spell
      Num = ((y-8)/20) * 4 + ((x-8)/150);

      // Make sure player knows spell (and has enough MP)
      if(Num >= 0 && Num < 64) {
        SpellPtr = m_SpellController.GetSpell(Num);
        if(PCChar->Def.MagicSpells[Num/32] & (1<<(Num&31)) && \
           SpellPtr->Name[0] &&                               \
           PCChar->ManaPoints >= SpellPtr->Cost) {
          PCChar->SpellNum = Num;
          PCChar->SpellTarget = CHAR_MONSTER;
          m_CharController.SetAction(PCChar, CHAR_SPELL);
          SelectSpell = FALSE;
        }
      }
    } else {

      // See if a button pressed (if target picked and charged)
      if(TargetID != -1 && PCChar->Charge >= 100.0f) {
        // Set victim and attacker info
        CharPtr = m_CharController.GetCharacter(TargetID);

        PCChar->Victim = CharPtr;
        CharPtr->Attacker = PCChar;
        PCChar->TargetX = CharPtr->XPos;
        PCChar->TargetY = CharPtr->YPos;
        PCChar->TargetZ = CharPtr->ZPos;

        // Determine if attack selected by checking
        // the coordinates of the mouse against the
        // coordinates of the attack button on screen 
        // button coordinates range from 572,328 – 636,360
        if(x >= 572 && x < 636 && y >= 328 && y < 360)
          m_CharController.SetAction(PCChar,CHAR_ATTACK);

        // Determine if spell selected by checking
        // the coordinates of the mouse against the
        // coordinates of the spell button on screen 
        // button coordinates range from 572,364 – 636,396
        if(x >= 572 && x < 636 && y >= 364 && y < 396)
          SelectSpell = TRUE;
      }

      // See if a character picked
      TargetID = GetCharacterAt(x, y);
    }
  }

  // Clear spell state if right mouse button clicked
  if(m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
    // Lock the mouse button
    m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);
    m_Mouse.SetButtonState(MOUSE_RBUTTON, FALSE);
    SelectSpell = FALSE;
  }

  // Update controllers
  m_CharController.Update(33);
  m_SpellController.Update(33);

  // Set the camera 
  m_Camera.Point(300.0f, 300.0f, -340.0f, 0.0f, 0.0f, 0.0f);
  m_Graphics.SetCamera(&m_Camera);

  // Render everything
  m_Graphics.Clear(D3DCOLOR_RGBA(0,32,64,255));
  if(m_Graphics.BeginScene() == TRUE) {

    // Render terrain
    m_Graphics.EnableZBuffer(TRUE);
    m_TerrainObject.Render();

    // Render all characters
    m_CharController.Render();

    // Render spells
    m_SpellController.Render();

    // Check if target needs rendering
    if(TargetID != -1) {

      // Move target to target character position
      CharPtr = m_CharController.GetCharacter(TargetID);
      Pos.EnableBillboard(TRUE);
      Pos.Move(CharPtr->XPos,CharPtr->YPos,CharPtr->ZPos);
      Pos.Rotate(0.0f, 0.0f, (float)timeGetTime() / 100.0f);

      // Offset to half of character height
      CharPtr->Object.GetBounds(NULL,&MinY,NULL,
                                NULL,&MaxY,NULL,NULL);
      YOff = MinY + ((MaxY-MinY)*0.5f);
      Pos.MoveRel(0.0f, YOff, 0.0f);

      // Render the target
      m_Graphics.SetTexture(0, NULL);
      m_Graphics.EnableZBuffer(FALSE);
      m_Graphics.SetWorldPosition(&Pos);
      m_Target.Render(0,2,D3DPT_TRIANGLELIST);
      m_Graphics.EnableZBuffer(TRUE);
    }

    // Display stats screen
    sprintf(Text, "HP: %ld / %ld\r\nMP: %ld / %ld",           \
            PCChar->HealthPoints, PCChar->Def.HealthPoints,   \
            PCChar->ManaPoints, PCChar->Def.ManaPoints);
    m_Stats.Render(Text);

    // Display charge meter
    m_Graphics.BeginSprite();
    m_Buttons.Blit(508,450,0,64,128,16);
    m_Buttons.Blit(510,452,0,80,(long)(1.24f*PCChar->Charge),12);
    m_Graphics.EndSprite();

    // Display attack options
    if(m_CharController.GetCharacter(0)->Charge >= 100.0f) {
      m_Graphics.BeginSprite();
      m_Buttons.Blit(572,328,0,0,64,32);
      m_Buttons.Blit(572,364,0,32,64,32);
      m_Graphics.EndSprite();
    }

    // Display spell list
    if(SelectSpell == TRUE) {
      m_Options.Render();

      // Display known spells
      for(i=0;i<64;i++) {
        SpellPtr = m_SpellController.GetSpell(i);

        if(PCChar->Def.MagicSpells[i/32] & (1<<(i&31)) &&     \
           SpellPtr->Name[0] &&                               \
           PCChar->ManaPoints >= SpellPtr->Cost) {
          x = i % 4 * 150;
          y = i / 4 *  20;
          m_Font.Print(m_SpellController.GetSpell(i)->Name,   \
                       x+8, y+8);
        }
      }
    }

    m_Graphics.EndScene();
  }
  m_Graphics.Display();

  return TRUE;
}

long cApp::GetCharacterAt(long XPos, long YPos)
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
    return -1;

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
        return CharPtr->ID;

      // Go to next mesh
      MeshPtr = MeshPtr->m_Next;
    }

    // Go to next character
    CharPtr = CharPtr->Next;
  }

  return -1;  // Return no hit
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}
