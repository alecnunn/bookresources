/**************************************************
WinMain.cpp
Chapter 8 Object Drawing Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"
#include "Frustum.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;
    cFont           m_Font;

    cInput          m_Input;
    cInputDevice    m_Mouse;

    cMesh           m_Mesh;
    cObject         m_Objects[256];

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
  strcpy(m_Class, "ObjectClass");
  strcpy(m_Caption, "Object Clipping Demo by Jim Adams");
}

BOOL cApp::Init()
{
  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 10000.0f);
  ShowMouse(TRUE);

  // Create a font
  m_Font.Create(&m_Graphics, "Arial");

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Mouse.Create(&m_Input, MOUSE, TRUE);  

  // Load the mesh and create the objects
  m_Mesh.Load(&m_Graphics, "..\\Data\\Yodan.x", "..\\Data\\");
  for(short i=0;i<256;i++) {
    m_Objects[i].Create(&m_Graphics, &m_Mesh);
    m_Objects[i].Move((float)(rand() % 4000) - 2000.0f,0.0f,(float)(rand() % 4000) - 2000.0f);
  }

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Free objects and meshes
  for(short i=0;i<256;i++)
    m_Objects[i].Free();
  m_Mesh.Free();

  // Free font
  m_Font.Free();

  // Shutdown input
  m_Mouse.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  long     NumDrawn = 0;
  float    Radius;
  char     Stats[128];
  cFrustum Frustum;

  // Read in mouse input
  m_Mouse.Read();

  // Position camera
  m_Camera.Move(0.0f, 100.0f, 0.0f);
  m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f, 
                     (float)m_Mouse.GetXDelta() / 200.0f, 
                     0.0f);

  // Set camera and calculate frustum
  m_Graphics.SetCamera(&m_Camera);
  Frustum.Construct(&m_Graphics);

  // Render everything
  m_Graphics.Clear(D3DCOLOR_RGBA(0,64,128,255));
  if(m_Graphics.BeginScene() == TRUE) {
    for(short i=0;i<256;i++) {
      m_Objects[i].GetBounds(NULL,NULL,NULL,NULL,NULL,NULL,&Radius);
      if(Frustum.CheckSphere(m_Objects[i].GetXPos(), m_Objects[i].GetYPos(), m_Objects[i].GetZPos(), Radius) == TRUE) {
        m_Objects[i].Render();
        NumDrawn++;
      }
    }

    // Display statistics
    sprintf(Stats, "%lu of 256 objects drawn.", NumDrawn);
    m_Font.Print(Stats,0,0,400,100);

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
