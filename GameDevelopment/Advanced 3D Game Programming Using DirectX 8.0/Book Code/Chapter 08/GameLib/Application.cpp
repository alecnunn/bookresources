/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Application.cpp
 *    Desc: Definition for the cApplication class this class 
 *          represents our top-level application controller
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <list>
using namespace std;

#include <mmsystem.h> // for timeGetTime

cApplication* cApplication::m_pGlobalApp = NULL;

HINSTANCE g_hInstance;

HINSTANCE AppInstance()
{
	return g_hInstance;
}

/**
 * WinMain is hidden away in a library because
 * almost everything it does is the same across
 * all applications we'll write.
 */
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	cApplication* pApp;

	g_hInstance = hInstance;

	try
	{
		pApp = CreateApplication();

		pApp->Init();
		pApp->SceneInit();
		pApp->Run();
	}
	catch( cGameError& err )
	{
		/**
		 * Knock out the graphics before we try to pop up a dialog, 
		 * just to be safe.
		 */
		if( Graphics() )
		{
			Graphics()->DestroyAll();
		}
		MessageBox( NULL, err.GetText(), "Error!", MB_OK|MB_ICONEXCLAMATION );

		// Clean everything up
		delete pApp;
		return 0;
	}

	delete pApp;
	return 0;
}



cApplication::cApplication()
{
	if( m_pGlobalApp )
	{
		throw cGameError("Application object already created!\n");
	}
	m_pGlobalApp = this;

	m_title = string( "Default window name" );
	m_width = 640;
	m_height = 480;
	m_bpp = 32;
	m_bActive = true;
}


cApplication::~cApplication()
{
	delete Graphics();
	delete Input();
	delete Sound();
	delete MainWindow();
}


void cApplication::Init()
{
	InitPrimaryWindow();
	InitGraphics();
	InitInput();
	InitSound();
	InitExtraSubsystems();
}


void cApplication::Run()
{
	bool done = false;

	static float lastTime = (float)timeGetTime(); 

	while( !done )
	{


		/**
		 * Message pump
		 */
		while( !done && MainWindow()->HasMessages() )
		{
			if( resFalse == MainWindow()->Pump() )
				done = true;
		}



		/**
		 * We're about ready to let the class draw the frame.
		 * find out how much time elapsed since the last frame
		 * we calc these whether we have focus or not, to avoid
		 * a large delta once we start rendering after idle time
		 */
		float currTime = (float)timeGetTime();
		float delta = (currTime - lastTime)/1000.f;

		if( m_bActive  )
		{
			// Update the Input devices
			if( Input() )
				Input()->UpdateDevices();

			DoFrame( delta );
		}
		else
		{
			DoIdleFrame( delta );
		}

		lastTime = currTime;
	}
}



void cApplication::DoFrame( float timeDelta )
{

}

void cApplication::DoIdleFrame( float timeDelta )
{
}


void cApplication::SceneInit()
{
	// by default, we have no scene, so do nothing.
}


void cApplication::InitPrimaryWindow()
{
	new cFullscreenWindow( m_width, m_height, m_title.c_str() );

	MainWindow()->RegisterClass();
	MainWindow()->InitInstance();
}


void cApplication::InitGraphics()
{

	cGraphicsLayer::Create( 
		MainWindow()->GetHWnd(), 
		m_width, m_height, 
		0,
		NULL );
}


void cApplication::InitInput()
{
	cInputLayer::Create( AppInstance(), MainWindow()->GetHWnd(), NULL, true, true );
}


void cApplication::InitSound()
{
	cSoundLayer::Create( MainWindow()->GetHWnd() );
}


void cApplication::InitExtraSubsystems()
{
}


void cApplication::SceneEnd()
{
}
