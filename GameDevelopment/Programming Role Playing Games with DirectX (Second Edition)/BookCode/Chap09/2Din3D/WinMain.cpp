/**************************************************
WinMain.cpp
Chapter 9 2Din3D Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;

    // The input system and keyboard device
    cInput          m_Input;
    cInputDevice    m_Keyboard;

    // The 2-D tile texture
    cTexture        m_Tiles;

    // The level mesh and object
    cMesh           m_LevelMesh;
    cObject         m_LevelObject;

    // The player character coordinates
    float           m_XPos, m_YPos, m_ZPos;

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
  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

  // Match perspective from modeler
  m_Graphics.SetPerspective(D3DX_PI/4.0f, 1.333333f, 1.0f, 10000.0f);
  ShowMouse(TRUE);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);

  // Load the level mesh and configure object
  m_LevelMesh.Load(&m_Graphics, "..\\Data\\Level.x", "..\\Data\\");
  m_LevelObject.Create(&m_Graphics, &m_LevelMesh);

  // Load the 2-D tiles 
  m_Tiles.Load(&m_Graphics, "..\\Data\\Tiles.bmp",D3DCOLOR_RGBA(0,0,0,255),D3DFMT_A1R5G5B5);

  // Set player position
  m_XPos = -900.0f;
  m_YPos = m_ZPos = 0.0f;
    
  // Enable lighting and ambient light level
  m_Graphics.EnableLighting(TRUE);
  m_Graphics.SetAmbientLight(0,0,0);
  m_Graphics.EnableLight(0,TRUE);

  // Create a directional light to illuminate the scene
  cLight Light;
  Light.SetType(D3DLIGHT_DIRECTIONAL);
  Light.SetDiffuseColor(192,192,192);
  Light.SetAmbientColor(0,0,0);
  Light.Point(0.0f, 60.0f, -100.0f, 0.0f, 0.0f, 0.0f);
  m_Graphics.SetLight(0, &Light);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free meshes, objects, and textures
  m_LevelMesh.Free();
  m_LevelObject.Free();
  m_Tiles.Free();

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
  static float Frame = 0.0f;
  static long  Dir = 1;
  unsigned long Elapsed;
  unsigned long Tile, TileX, TileY;
  float XMove, Dist;
  
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

  // Check for movement left or right
  XMove = 0.0f;
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE)
    XMove = -((float)Elapsed * 0.5f);
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE)
    XMove = ((float)Elapsed * 0.5f);

  // Check for intersections (allowing climbing as well)
  if(XMove < 0.0f) {
    if(CheckIntersect(m_XPos-32.0f,       m_YPos+84.0f, m_ZPos,
                      m_XPos-32.0f+XMove, m_YPos+84.0f, m_ZPos,
                      &Dist) == TRUE)
      XMove = 0.0f;
  } else {
    if(CheckIntersect(m_XPos+32.0f,       m_YPos+84.0f, m_ZPos,
                      m_XPos+32.0f+XMove, m_YPos+84.0f, m_ZPos,
                      &Dist) == TRUE)
      XMove = 0.0f;
  }

  // Fix height of player
  m_YPos = GetHeightBelow(m_XPos+XMove,m_YPos+64.0f,m_ZPos);

  // Update animation frame
  if(XMove != 0.0f) {
    // Change direction of travel (for animation)
    if(XMove < 0.0f) {
      Frame += ((float)Elapsed * 0.015f);
      if(Frame >= 8.0f)
        Frame = 0.0f;

      Dir = 1;
    } else {
      Frame -= ((float)Elapsed * 0.015f);
      if(Frame < 0.0f)
        Frame = 7.0f;

      Dir = 0;
    }
  } else {
    Frame = 3.0f;
  }

  // Move player
  m_XPos += XMove;

  // Position the camera by character position
  m_Camera.Point(m_XPos, m_YPos+200.0f, m_ZPos-500.0f,
                 m_XPos, m_YPos, m_ZPos);
  m_Graphics.SetCamera(&m_Camera);

  // Render everything
  m_Graphics.Clear();
  if(m_Graphics.BeginScene() == TRUE) {

    // Render the level mesh
    m_Graphics.EnableZBuffer(TRUE);
    m_LevelObject.Render();

    // Render the tiles
    m_Graphics.EnableZBuffer(FALSE);
    m_Graphics.EnableAlphaTesting(TRUE);
    m_Graphics.BeginSprite();

    Tile  = Dir * 8 + (long)Frame;
    TileX = Tile % 4 * 64;
    TileY = Tile / 4 * 64;

    m_Tiles.Blit(256,112,TileX,TileY,64,64,2.0f,2.0f);
    m_Graphics.EndSprite();
    m_Graphics.EnableAlphaTesting(FALSE);

    // End the scene
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

  D3DXIntersect(m_LevelMesh.GetParentMesh()->m_Mesh, 
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
  D3DXIntersect(m_LevelMesh.GetParentMesh()->m_Mesh, 
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
