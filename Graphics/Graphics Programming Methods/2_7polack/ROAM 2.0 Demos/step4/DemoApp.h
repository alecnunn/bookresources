//==============================================================
// DemoApp.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Implementation of the demo application instance
//==============================================================
#ifndef __DemoApp_H__
#define __DemoApp_H__

#include "..\\Framework\\Application.h"
#include "..\\Framework\\EventHandler.h"
#include "..\\Framework\\FontEngine.h"
#include "..\\Framework\\Camera.h"
#include "..\\Framework\\Texture.h"
#include "..\\Framework\\Timer.h"

#include "ROAM.h"

//implementation of the Application class 
class DemoApp : public DemoFramework::Application, public DemoFramework::EventHandler {
	DemoFramework::FontEngine m_fontEngine;

	DemoFramework::Camera m_camera;
	float m_movementSpeed;

	DemoFramework::Timer m_timer;

	Terrain::ROAM m_ROAM;
	DemoFramework::Texture m_shadingTexture;

	int m_screenWidth, m_screenHeight;
	bool m_fullscreen;

	int m_mouseX, m_mouseY;
	float m_mouseSensitivity;

public:
	bool OnCreate( void );

	bool CreateD3D( void );

	void OnIdle( void );
	bool OnMainLoop( void );
	void Update( void );
	void Render( void );

	void ResizeScene( void );

	bool OnKeyDown( void );
	bool OnMouseAction( int x, int y, bool left, bool right, bool middle );

	void ReleaseD3D( void );

	bool OnRelease( void );

DemoApp( void ) : m_mouseSensitivity( 1.0f ), m_movementSpeed( 5.0f ) {
	SetEventHandler( this );
}
virtual ~DemoApp( void ) { }
};

#endif	//__DemoApp_H__