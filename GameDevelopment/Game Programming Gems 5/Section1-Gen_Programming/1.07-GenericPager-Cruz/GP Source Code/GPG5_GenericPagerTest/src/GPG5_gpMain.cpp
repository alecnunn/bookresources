#include <GPG5_gpPch.h>

//Test Includes
#include <GPG5_gpWindowOpenGL.h>
#include <GPG5_gpNodeManager.h>
#include <GPG5_gpEventManager.h>
#include <GPG5_gpCamera2D.h>
#include <GPG5_gpNodeGrid.h>
#include <GPG5_gpNodeAxis.h>
#include <GPG5_gpNodeTexture.h>
#include <GPG5_gpNodeQuad.h>
#include <GPG5_gpNodeWorld.h>
#include <GPG5_gpNodeTile.h>
#include <GPG5_gpNodeWindow.h>

#include <GPsync.h>

#include <GPwindow.h>

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	// +--------------------------
	// |	Window
	// +--------------------------
	GPG5_gpWindowOpenGL genericPagerWindow;
	genericPagerWindow.SetName("GPG5 Generic Pager Test. Press F1 for Help.");
	genericPagerWindow.SetPosition(100,100);
	genericPagerWindow.SetSize(1000,500);
	genericPagerWindow.CreateWin();
	genericPagerWindow.Show();

	glEnable(GL_DEPTH_TEST);

	// +--------------------------
	// |	Camera Node
	// +--------------------------
	GPG5_gpCamera2D* pCamera = new GPG5_gpCamera2D();
	pCamera->SetName("Camera Node");
	pCamera->Situate( GPG5_gpVector(1000.0, 500.0, 1500.0) );
	GPG5_gpNodeManager::GetInstance().AddNode(pCamera);
	GPG5_gpNodeManager::GetInstance().GetEventManager().AddNode(pCamera);

	// +--------------------------
	// |	World
	// +--------------------------
	GPG5_gpNodeWorld* pWorld= new GPG5_gpNodeWorld();
	pWorld->SetName("World Node");
	pWorld->SetWorldLimits(0, 2000, 0, 1000, 20.0, 10.0);
	pWorld->SetTexture
					  (
							".\\Data\\unloaded_world_3x2048x1024.raw",
							3,
							2048,
							1024
						);

	pWorld->ShowTexture(true);
	pWorld->ShowGrid(true);
	GPG5_gpNodeManager::GetInstance().AddNode(pWorld);

 	// +--------------------------
	// |	Main Index
	// +--------------------------
	GPindex mainIndex(0,0,100,100);
	
 	// +--------------------------
	// |	Window Red
	// +--------------------------
	GPindex indexRed = mainIndex.GetIndex(0,0);
	GPG5_gpNodeWindow* pWindowRed = new GPG5_gpNodeWindow();
	pWindowRed->SetName("Red Window");
	pWindowRed->SetWorld( pWorld );
	pWindowRed->Init( indexRed );	
	pWindowRed->SetColor( GPG5_gpVector(1.0, 0.0, 0.0) );
	pWindowRed->SetLineWidth(3.0f);
	GPG5_gpNodeManager::GetInstance().AddNode(pWindowRed);
	
	
 	// +--------------------------
	// |	Window Green
	// +--------------------------
	GPindex indexGreen = mainIndex.GetIndex(900,700);
	GPG5_gpNodeWindow* pWindowGreen = new GPG5_gpNodeWindow();
	pWindowGreen->SetName("Green Window");
	pWindowGreen->SetWorld( pWorld );
	pWindowGreen->Init( indexGreen );	
	pWindowGreen->SetColor( GPG5_gpVector(0.0, 1.0, 0.0) );
	pWindowGreen->SetLineWidth(3.0f);
	GPG5_gpNodeManager::GetInstance().AddNode(pWindowGreen);
	pWindowGreen->Active(true);

 	// +--------------------------
	// |	Window Blue
	// +--------------------------
	GPindex indexBlue= mainIndex.GetIndex(1500,300);
	GPG5_gpNodeWindow* pWindowBlue = new GPG5_gpNodeWindow();
	pWindowBlue->SetName("Blue Window");
	pWindowBlue->SetWorld( pWorld );
	pWindowBlue->Init( indexBlue );	
	pWindowBlue->SetColor( GPG5_gpVector(0.0, 0.0, 1.0) );
	pWindowBlue->SetLineWidth(3.0f);
	GPG5_gpNodeManager::GetInstance().AddNode(pWindowBlue);
	pWindowBlue->UpdateRadius(2);

 	// +--------------------------
	// |	Synchronization
	// +--------------------------
	GPsync	::GetInstance().SetCriticalSection( &GPG5_gpNodeManager::GetInstance().GetCriticalSection() );
	
	while( !genericPagerWindow.MainProcess() )
	{
		Sleep(0);
	}
	return 0;
}
