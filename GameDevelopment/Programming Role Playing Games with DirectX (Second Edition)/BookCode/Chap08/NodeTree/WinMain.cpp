/**************************************************
WinMain.cpp
Chapter 8 NodeTree Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"
#include "Frustum.h"
#include "NodeTree.h"
#include "SkyBox.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;
    cLight          m_Light;

    cSound          m_Sound;
    cSoundData      m_SndData;
    cSoundChannel   m_SndChannel[3];

    cSkyBox         m_SkyBox;

    cInput          m_Input;
    cInputDevice    m_Keyboard;
    cInputDevice    m_Mouse;

    cMesh           m_Mesh;
    cNodeTreeMesh   m_NodeTreeMesh;

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
  strcpy(m_Class, "NodeTreeClass");
  strcpy(m_Caption, "NodeTree Demo by Jim Adams");
}

BOOL cApp::Init()
{
  short i;

  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 100000.0f);
  ShowMouse(TRUE);

  // Enable lighting and setup light
  m_Graphics.EnableLighting(TRUE);
  m_Graphics.SetAmbientLight(24,24,24);
  m_Graphics.EnableLight(0, TRUE);
  m_Light.SetAttenuation0(0.4f);
  m_Light.SetRange(1000.0f);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);  

  // Load the mesh and create an NodeTree mesh from it
  m_Mesh.Load(&m_Graphics, "..\\Data\\Level.x", "..\\Data\\");
  m_NodeTreeMesh.Create(&m_Graphics, &m_Mesh, QUADTREE);

  // Position view at origin
  m_XPos = m_YPos = m_ZPos = 0.0f;

  // Setup the sky box
  m_SkyBox.Create(&m_Graphics);
  for(i=0;i<6;i++)
    m_SkyBox.LoadTexture(i, "..\\Data\\Stars.bmp");

  // Initialize the sound system to play with
  m_Sound.Init(GethWnd());
  m_SndData.LoadWAV("..\\Data\\Cricket.wav");
  for(i=0;i<3;i++)
    m_SndChannel[i].Create(&m_Sound, &m_SndData);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free sounds
  for(short i=0;i<3;i++)
    m_SndChannel[i].Free();
  m_SndData.Free();
  m_Sound.Shutdown();

  // Free meshes
  m_NodeTreeMesh.Free();
  m_Mesh.Free();

  // Shutdown input
  m_Mouse.Free();
  m_Keyboard.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD  Timer = timeGetTime();
  unsigned long Elapsed;
  float         XMove, ZMove, Height, Diff, Dist;
  D3DXVECTOR2   vecDir;
  cFrustum      Frustum;
  cLight        Light;

  // Play a random cricket sound
  for(short i=0;i<3;i++) {
    if(m_SndChannel[i].IsPlaying() == FALSE && rand() % 256 < 16)
      m_SndChannel[i].Play(&m_SndData, 10);
  }

  // Reacquire input
  m_Mouse.Acquire(TRUE);

  // Calculate elapsed time (plus speed boost)
  Elapsed = (timeGetTime() - Timer);
  Timer = timeGetTime();

  // Get input
  m_Keyboard.Read();
  m_Mouse.Read();

  // Process input and update everything.
  // ESC quits program
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Process movement
  XMove = ZMove = 0.0f;

  if(m_Keyboard.GetKeyState(KEY_UP) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation()) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation()) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE) {
    XMove = -(float)sin(m_Camera.GetYRotation()) * Elapsed;
    ZMove = -(float)cos(m_Camera.GetYRotation()) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation() - 1.57f) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation() - 1.57f) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation() + 1.57f) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation() + 1.57f) * Elapsed;
  }

  // Check for height changes (can step up to 64 units)
  Height = m_NodeTreeMesh.GetHeightBelow(m_XPos, m_YPos + 64.0f, m_ZPos);
  if(m_YPos > Height) {
    // Dropping
    if((m_YPos -= (float)Elapsed) < Height)
      m_YPos = Height;
    else
      XMove = ZMove = 0.0f;
  } else {
    // Climbing
    m_YPos = Height;
  }

  // Check for movement collisions -
  // can't walk past anything blocking path
  if(m_NodeTreeMesh.CheckIntersect(m_XPos,         m_YPos + 64.0f, m_ZPos,
                                 m_XPos + XMove, m_YPos + 64.0f, m_ZPos + ZMove,
                                 &Dist) == TRUE) {
    // Adjust coordinates to be exactly 2.5 units away from target
    Diff = Dist - 2.5f;
    D3DXVec2Normalize(&vecDir, &D3DXVECTOR2(XMove, ZMove));
    vecDir *= Diff;
    XMove = vecDir.x;
    ZMove = vecDir.y;
  }

  // Update view coordinates
  m_XPos += XMove;
  m_ZPos += ZMove;

  // Position camera
  m_Camera.Move(m_XPos + XMove, m_YPos + 50.0f, m_ZPos + ZMove);
  m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f, 
                     (float)m_Mouse.GetXDelta() / 200.0f, 
                     0.0f);

  // Position light
  m_Light.Move(m_XPos, m_YPos+60.0f, m_ZPos);
  m_Graphics.SetLight(0, &m_Light);

  // Set camera and calculate frustum
  m_Graphics.SetCamera(&m_Camera);
  Frustum.Construct(&m_Graphics);

  // Render everything
  m_Graphics.ClearZBuffer(1.0f);
  if(m_Graphics.BeginScene() == TRUE) {
    m_Graphics.EnableZBuffer(FALSE);
    m_Graphics.EnableLighting(FALSE);
    m_SkyBox.Render(&m_Camera);

    m_Graphics.EnableZBuffer(TRUE);
    m_Graphics.EnableLighting(TRUE);
    m_NodeTreeMesh.Render(&Frustum);
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
