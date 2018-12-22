/**************************************************
WinMain.cpp
Chapter 13 Automapping Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"
#include "Frustum.h"
#include "NodeTree.h"
#include "Trigger.h"
#include "Barrier.h"
#include "Automap.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;

    cInput          m_Input;
    cInputDevice    m_Keyboard;
    cInputDevice    m_Mouse;

    cMesh           m_TerrainMesh;
    cNodeTreeMesh   m_NodeTreeMesh;

    cAutomap        m_Automap;

    cMesh           m_CharacterMesh;
    cObject         m_CharacterObject;
    cAnimation      m_CharacterAnim;

    cTrigger        m_Trigger;
    cBarrier        m_Barrier;

    cMesh           m_DoorMesh;
    cAnimation      m_DoorAnim;

    float           m_XPos, m_YPos, m_ZPos;

  public:
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
};

cApp::cApp()
{ 
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "MapClass");
  strcpy(m_Caption, "Map Demo by Jim Adams");
}

BOOL cApp::Init()
{
  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
//  m_Graphics.SetMode(GethWnd(), FALSE, TRUE, 640, 480, 16);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 20000.0f);
  ShowMouse(TRUE);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);

  // Load the mesh and create an NodeTree mesh from it
  m_TerrainMesh.Load(&m_Graphics, "..\\Data\\Level.x", "..\\Data\\");
  m_NodeTreeMesh.Create(&m_Graphics, &m_TerrainMesh, QUADTREE);

  // Create an automap
  m_Automap.Create(&m_Graphics, "..\\Data\\AutoSrc.x", D3DCOLOR_RGBA(0,0,255,255));
  m_Automap.SetWindow(440,0,200,200);
  
  // Load character data
  m_CharacterMesh.Load(&m_Graphics, "..\\Data\\Warrior.x", "..\\Data\\");
  m_CharacterAnim.Load("..\\Data\\Warrior.x", &m_CharacterMesh);
  m_CharacterAnim.SetLoop(TRUE, "Idle");
  m_CharacterAnim.SetLoop(TRUE, "Walk");
  m_CharacterObject.Create(&m_Graphics, &m_CharacterMesh);

  // Move character into position
  m_XPos = -800.0f;
  m_YPos = 0.0f;
  m_ZPos = -800.0f;
  m_CharacterObject.Move(0.0f, 0.0f, 0.0f);

  // Load triggers and barriers
  m_Trigger.Load("..\\Data\\Level.trg");
  m_Barrier.Load("..\\Data\\Level.bar");

  // Load door mesh and animation
  m_DoorMesh.Load(&m_Graphics, "..\\Data\\Door.x", "..\\Data\\");
  m_DoorAnim.Load("..\\Data\\Door.x", &m_DoorMesh);
  m_DoorAnim.SetLoop(FALSE, "Open");
  m_DoorAnim.SetLoop(FALSE, "Close");

  // Configure door data
  for(short i=0;i<4;i++) {
    m_Barrier.SetMesh(i+1, &m_Graphics, &m_DoorMesh);
    if(m_Barrier.GetEnableState(i+1) == TRUE)
      m_Barrier.SetAnim(i+1, &m_DoorAnim, "Close", 0);
    else
      m_Barrier.SetAnim(i+1, &m_DoorAnim, "Open", 0);
  }

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free triggers and barriers
  m_Trigger.Free();
  m_Barrier.Free();

  // Free meshes, objects, and animations
  m_NodeTreeMesh.Free();
  m_TerrainMesh.Free();
  m_CharacterObject.Free();
  m_CharacterAnim.Free();
  m_CharacterMesh.Free();

  m_DoorAnim.Free();
  m_DoorMesh.Free();

  // Shutdown input
  m_Keyboard.Free();
  m_Input.Shutdown();
  m_Mouse.Free();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD  Timer = timeGetTime(), AutoUpdate = 0;
  static long LastAnim = -1;
  static float MapHeight = 300.0f;
  unsigned long Elapsed;
  cFrustum      Frustum;
  static float  CamAngle = 0.0f, MoveAngle = 0.0f;
  float         XMove, ZMove;
  long          Dir, TriggerID;
  short         i;
  float Angles[13] = { 0.0f, 0.0f, 1.57f, 0.785f, 3.14f, 
                       0.0f, 2.355f, 0.0f, 4.71f, 5.495f, 
                       0.0f, 0.0f, 3.925f };

  // Calculate elapsed time (plus speed boost)
  Elapsed = (timeGetTime() - Timer);
  Timer = timeGetTime();

  // Reacquire input
  m_Mouse.Acquire(TRUE);
  m_Keyboard.Acquire(TRUE);

  // Get input
  m_Keyboard.Read();
  m_Mouse.Read();

  // Process input and update everything.
  // ESC quits program
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Process automap 'zoom'
  if(m_Keyboard.GetKeyState(KEY_PAGEUP) == TRUE) {
    if(MapHeight < 700.0f)
      MapHeight += 4.0f;
  }
  if(m_Keyboard.GetKeyState(KEY_PAGEDOWN) == TRUE) {
    if(MapHeight > 100.0f)
      MapHeight -= 4.0f;
  }

  // Process barrier changes
  if(m_Keyboard.GetKeyState(KEY_SPACE) == TRUE) {
    m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
    m_Keyboard.SetLock(KEY_SPACE, TRUE);

    for(i=0;i<4;i++) {
      if(m_Barrier.GetEnableState(i+1) == TRUE) {
        m_Barrier.Enable(i+1, FALSE);
        m_Barrier.SetAnim(i+1, &m_DoorAnim, "Open", timeGetTime() / 20);
      } else {
        m_Barrier.Enable(i+1, TRUE);
        m_Barrier.SetAnim(i+1, &m_DoorAnim, "Close", timeGetTime() / 20);
      }
    }
  }

  // Process movement
  Dir = 0;
  if(m_Keyboard.GetKeyState(KEY_UP) == TRUE) 
    Dir |= 1;
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE) 
    Dir |= 2;
  if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE) 
    Dir |= 4;
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE) 
    Dir |= 8;

  XMove = ZMove = 0.0f;
  if(Dir) {
    MoveAngle = Angles[Dir] + m_Camera.GetYRotation();

    float Speed = (float)Elapsed / 1000.0f * 512.0f;

    XMove = (float)sin(MoveAngle) * Speed;
    ZMove = (float)cos(MoveAngle) * Speed;

    // Check for movement collisions -
    // can't walk past anything blocking path
    if(m_NodeTreeMesh.CheckIntersect(
                   m_XPos,         32.0f, m_ZPos,
                   m_XPos + XMove, 32.0f, m_ZPos + ZMove,
                   NULL) == TRUE)
      XMove = ZMove = 0.0f;

    // Check for barrier collisions
    if(m_Barrier.GetBarrier(m_XPos+XMove, 0.0f, m_ZPos+ZMove))
      XMove = ZMove = 0.0f;

    // Update character
    m_XPos += XMove;
    m_ZPos += ZMove;

    // Change animation and check triggers
    if(XMove != 0.0f || ZMove != 0.0f) {
      if(LastAnim != 0) {
        LastAnim = 0;
        m_CharacterObject.SetAnimation(&m_CharacterAnim, "Walk", timeGetTime() / 20);
      }

      // Trigger showing map if any triggers hit
      if((TriggerID = m_Trigger.GetTrigger(m_XPos, 0.0f, m_ZPos))) {
        // Process a trigger
        m_Automap.EnableSection(TriggerID-1, TRUE);

        // Disable trigger
        m_Trigger.Enable(TriggerID, FALSE);
      }
    }
  } else {
    // Change animation to idle
    if(LastAnim != 1) {
      LastAnim = 1;
      m_CharacterObject.SetAnimation(&m_CharacterAnim, "Idle", timeGetTime() / 20);
    }
  }

  // Position character and update
  m_CharacterObject.Move(m_XPos, 0.0f, m_ZPos);
  m_CharacterObject.Rotate(0.0f, MoveAngle, 0.0f);

  // Position camera
  CamAngle -= ((float)m_Mouse.GetXDelta() * (float)Elapsed / 800.0f);
  m_Camera.Point(m_XPos + (float)cos(CamAngle) * 300.0f, 
                 m_YPos + 100.0f, 
                 m_ZPos + (float)sin(CamAngle) * 300.0f,
                 m_XPos, 0.0f, m_ZPos);

  // Set camera and calculate frustum
  m_Graphics.SetCamera(&m_Camera);
  Frustum.Construct(&m_Graphics);

  // Render everything
  m_Graphics.Clear();
  if(m_Graphics.BeginScene() == TRUE) {
    // Render terrain
    m_Graphics.EnableZBuffer(TRUE);
    m_NodeTreeMesh.Render(&Frustum);

    // Render barriers
    m_Barrier.Render(timeGetTime() / 20, &Frustum);

    // Render character
    m_CharacterObject.UpdateAnimation(timeGetTime() / 20, TRUE);
    m_CharacterObject.Render();

    // Render automap using alphablending
    m_Graphics.EnableAlphaBlending(TRUE, D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR);
    m_Automap.Render(&m_Camera, m_XPos, MapHeight, m_ZPos, 1, &m_XPos, &m_ZPos, &MoveAngle);

    m_Graphics.EndScene();
  }
  m_Graphics.Display();

  return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}
