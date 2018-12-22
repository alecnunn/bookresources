/**************************************************
WinMain.cpp
Chapter 9 3Din2D Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"

class cApp : public cApplication
{
  private:
    // The graphics system and camera class
    cGraphics       m_Graphics;
    cCamera         m_Camera;

    // The input system and keyboard device
    cInput          m_Input;
    cInputDevice    m_Keyboard;

    // The backdrop textures
    cTexture        m_BDTextures[6];

    // The simplified scene mesh and object
    cMesh           m_SceneMesh;
    cObject         m_SceneObject;

    // 3-D meshes and objects
    cMesh           m_Meshes;
    cObject         m_Objects;
    cAnimation      m_Animations;

    // Intersection functions
    float GetHeightBelow(float XPos, float YPos, float ZPos);
    BOOL  CheckIntersect(float XStart, float YStart, float ZStart,
                         float XEnd,   float YEnd,   float ZEnd,
                         float *Length);

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
  strcpy(m_Class, "3Din2DClass");
  strcpy(m_Caption, "3Din2D Graphics Demo by Jim Adams");
}

BOOL cApp::Init()
{
  char Filename[81];

  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

  // Match perspective from modeler
  m_Graphics.SetPerspective(0.6021124f, 1.333333f, 1.0f, 20000.0f);
  ShowMouse(TRUE);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);

  // Load the backdrop textures
  for(short i=0;i<6;i++) {
    sprintf(Filename, "..\\Data\\Scene%u.bmp", i+1);
    m_BDTextures[i].Load(&m_Graphics, Filename);
  }

  // Load the scene mesh and configure object
  m_SceneMesh.Load(&m_Graphics, "..\\Data\\Scene.x");
  m_SceneObject.Create(&m_Graphics, &m_SceneMesh);

  // Load the object mesh and setup object
  m_Meshes.Load(&m_Graphics, "..\\Data\\Yodan.x", "..\\Data\\");
  m_Animations.Load("..\\Data\\Yodan.x", &m_Meshes);
  m_Animations.SetLoop(TRUE, "Idle");
  m_Animations.SetLoop(TRUE, "Walk");
  m_Objects.Create(&m_Graphics, &m_Meshes);

  // Position the camera for the scene
  m_Camera.Move(0.0f, 200.0f, -650.0f);
  m_Camera.Rotate(0.348888f, 0.0f, 0.0f);
  m_Graphics.SetCamera(&m_Camera);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free meshes, objects, and textures
  m_SceneMesh.Free();
  m_SceneObject.Free();
  m_Meshes.Free();
  m_Objects.Free();
  m_Animations.Free();
  for(short i=0;i<6;i++)
    m_BDTextures[i].Free();

  // Shutdown input
  m_Keyboard.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD  Timer = timeGetTime();
  static short LastAnim = -1;
  unsigned long Elapsed;
  long  Dir, i, j;
  float XMove, ZMove, XPos, YPos, ZPos;
  float Dist;
  float Angles[13] = {
       0.0f, 4.71f, 0.0f, 5.495f, 1.57f, 0.0f,  
       0.785f, 0.0f,  3.14f, 3.925f, 0.0f, 0.0f, 2.355f 
     };

  // Calculate elapsed time 
  DWORD ThisTime = timeGetTime();
  Elapsed = (ThisTime - Timer);
  Timer = ThisTime;

  // Get input
  m_Keyboard.Read();

  // Process input and update everything.
  // ESC quits program
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Get movement
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
    m_Objects.Rotate(0.0f, Angles[Dir], 0.0f);
    XMove =  (float)cos(Angles[Dir]) * ((float)Elapsed * 0.25f);
    ZMove = -(float)sin(Angles[Dir]) * ((float)Elapsed * 0.25f);
  } 

  // Get object coordinates in local variables (make it easier)
  XPos = m_Objects.GetXPos();
  YPos = m_Objects.GetYPos();
  ZPos = m_Objects.GetZPos();

  // Check for collision in movement (4 points)
  // I hard-coded the bounding size of the object (25 radius)
  // and added ability to climb up at most 32 units.
  for(i=0;i<4;i++) {
    float XAdd[4] = {  0.0f, 25.0f,   0.0f, -25.0f };
    float ZAdd[4] = { 25.0f,  0.0f, -25.0f,   0.0f };

    if(CheckIntersect(XPos+XAdd[i],       YPos+32.0f, ZPos+ZAdd[i], 
                      XPos+XAdd[i]+XMove, YPos+32.0f, ZPos+ZAdd[i]+ZMove,
                      &Dist) == TRUE) {
      // Clear out movement and break
      XMove = ZMove = 0.0f;
      break;
    }
  }

  // Get object height
  m_Objects.Move(XPos, GetHeightBelow(XPos, YPos+32.0f, ZPos), ZPos);

  // Move object and set new animations as needed
  m_Objects.MoveRel(XMove, 0.0f, ZMove);

  if(XMove != 0.0f || ZMove != 0.0f) {
    if(LastAnim != 1) {
      LastAnim = 1;
      m_Objects.SetAnimation(&m_Animations, "Walk", ThisTime / 20);
    }
  } else {
    if(LastAnim != 2) {
      LastAnim = 2;
      m_Objects.SetAnimation(&m_Animations, "Idle", ThisTime / 20);
    }
  }

  // Update object animations
  m_Objects.UpdateAnimation(ThisTime / 20, TRUE);

  // Render everything
  m_Graphics.ClearZBuffer();
  if(m_Graphics.BeginScene() == TRUE) {
  
    // Render simplified mesh for z-values
    m_Graphics.EnableZBuffer(TRUE);
    m_SceneObject.Render();

    // Draw the backdrop (composed of six textures)
    m_Graphics.EnableZBuffer(FALSE);
    m_Graphics.BeginSprite();
    for(i=0;i<2;i++) {
      for(j=0;j<3;j++)
        m_BDTextures[i*3+j].Blit(j*256,i*256);
    }
    m_Graphics.EndSprite();

    // Draw the 3-D objects
    m_Graphics.EnableZBuffer(TRUE);
    m_Objects.Render();

    m_Graphics.EndScene();
  }
  m_Graphics.Display();

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

BOOL cApp::CheckIntersect(float XStart, float YStart, float ZStart,
                          float XEnd,   float YEnd,   float ZEnd,
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
  D3DXIntersect(m_SceneMesh.GetParentMesh()->m_Mesh, 
                &D3DXVECTOR3(XStart,YStart,ZStart), &vecDir,
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

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}
