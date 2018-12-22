/**************************************************
WinMain.cpp
Chapter 7 Scaled Tile Demo (Big Bitmap)

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
  m_Width  = 480; 
  m_Height = 480;
  m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "ScaledTileClass");
  strcpy(m_Caption, "Scaled Tile Demo by Jim Adams");
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
  char MapData[3][3] = { 
    { 0, 1, 0 },
    { 2, 2, 2 },
    { 1, 2, 3 }
  };
  m_Map.Create(1, 3, 3);
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
  // Begin the scene
  m_Graphics.ClearDisplay();
  if(m_Graphics.BeginScene() == TRUE) {
    // Draw the map
    m_Map.Render(0,0,3,3,0);

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
