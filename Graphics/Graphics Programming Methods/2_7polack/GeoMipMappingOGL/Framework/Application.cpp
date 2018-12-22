//==============================================================
// Application.cpp
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Application base class for a demo framework.
//==============================================================
#include "application.h"
using namespace DemoFramework;

#include <SDL.h>
#include <windows.h>

Application *Application::m_pApplication;
EventHandler *Application::m_pEventHandler;

int Application::Execute( void ) {
	Application *pApp;
	SDL_Event event;
	static bool active= true;

	//initialize the pApp
	if( !( pApp= GetApplication( ) ) ) {
		return 0;
	}

	if( pApp->OnCreate( ) ) {
		while( active ) {
			while( SDL_PollEvent( &event ) )	active= m_pEventHandler->HandleEvent( event );
			if( !m_pEventHandler->OnKeyDown( ) )
				break;
			if( !m_pEventHandler->OnKeyUp( ) )
				break;

			if( !active )	break;

			if( !pApp->OnMainLoop( ) )
				break;
		}
	}

	//terminate application
	pApp->OnRelease();

	SDL_Quit( );
	return 1;
}

void Application::Create( unsigned int SDLFlags, int width, int height, int bpp, const char *windowTitle ) {
	//initialize the video and timer systems
	SDL_Init( SDLFlags );

	SDL_SetVideoMode( width, height, bpp, SDL_OPENGL );
	SDL_WM_SetCaption( windowTitle, NULL );
}

int main( int argc, char **argv ) {
	return( Application::Execute( ) );
}