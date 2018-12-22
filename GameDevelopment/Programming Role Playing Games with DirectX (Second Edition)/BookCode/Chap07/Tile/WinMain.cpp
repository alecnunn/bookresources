/**************************************************
WinMain.cpp
Chapter 7 Tile Class Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, D3DXOF.LIB, and DXGUID.LIB 
**************************************************/
#include "Core_Global.h"

#include "Tile.h"
#include "Map.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;

    cTiles          m_Tiles;
    cMap            m_Map;

  public:
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
};

cApp::cApp()
{ 
  m_Width  = 384; 
  m_Height = 384;
  m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "TileClass");
  strcpy(m_Caption, "Tile Demo by Jim Adams");
}

BOOL cApp::Init()
{
  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, FALSE);
  ShowMouse(TRUE);

  // Create and load the tile set
  m_Tiles.Create(&m_Graphics, 1);
  m_Tiles.Load(0, "Tiles.bmp", 64, 64);

  // Create and set the map
  char MapData[16][16] = { 
    { 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 1, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 2, 0 },
    { 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 0 },
    { 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 },
    { 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 },
    { 3, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 3, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 0, 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 2, 2, 1, 1, 0, 0, 2, 2, 0, 0, 2, 2, 2, 2, 0 },
    { 0, 1, 2, 2, 2, 0, 0, 2, 2, 0, 0, 2, 1, 1, 2, 0 },
    { 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
    { 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  m_Map.Create(1, 16, 16);
  m_Map.SetMapData(0, (char*)&MapData);
  m_Map.UseTiles(&m_Tiles);

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free map object
  m_Map.Free();

  // Shutdown tiles object
  m_Tiles.Free();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static long XPos = 0, YPos = 0;
  
  // Calculate elapsed time
  static DWORD LastTime = timeGetTime();
  DWORD ThisTime = timeGetTime();
  DWORD Elapsed = ThisTime - LastTime;

  // Frame lock to 33ms per frame
  if(Elapsed < 33)
    return TRUE;
  LastTime = ThisTime;

  // Begin the scene
  if(m_Graphics.BeginScene() == TRUE) {

    // Draw the map
    m_Map.Render(XPos, YPos, 6, 6, 0);

    // Press arrows to scroll map around
    if(GetAsyncKeyState(VK_LEFT))
      XPos -= 8;
    if(GetAsyncKeyState(VK_RIGHT))
      XPos += 8;
    if(GetAsyncKeyState(VK_UP))
      YPos -= 8;
    if(GetAsyncKeyState(VK_DOWN))
      YPos += 8;

    // Bounds check map coordinates
    if(XPos < 0)
      XPos = 0;
    if(XPos > 640)
      XPos = 640;
    if(YPos < 0)
      YPos = 0;
    if(YPos > 640)
      YPos = 640;

    // End the scene and display the graphics
    m_Graphics.EndScene();
    m_Graphics.Display();
  }

  return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}
