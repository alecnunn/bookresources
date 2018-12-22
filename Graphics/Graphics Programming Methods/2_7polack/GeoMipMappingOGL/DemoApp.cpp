//==============================================================
// DemoApp.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Implementation of a demo event handler
//==============================================================
#include <SDL_opengl.h>

#include "DemoApp.h"
#include "Framework\\GLExtensions.h"
#include "Framework\\Log.h"

DemoApp g_demo;

//get rid of these lines for non-VC++ compiling
#pragma comment( lib, "SDL.lib" )
#pragma comment( lib, "SDLmain.lib" )
#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "GLU32.lib" )

//the creation routine
bool DemoApp::OnCreate( void ) {
	m_screenWidth= 640;
	m_screenHeight= 480;
	DemoFramework::Application::Create( SDL_INIT_VIDEO | SDL_INIT_TIMER,
										m_screenWidth, m_screenHeight, 32, "Geomipmapping Demo" );

	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0 );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	ResizeScene( );

	m_fontEngine.Create( m_screenWidth, m_screenHeight );

	GLExtensions::SetupExtensionSystem( );
	GLExtensions::SetupCVA( );

	//set the camera's position
	m_camera.SetPosition( 128.0f, 512.0f, -256.0f );

	m_camera.m_yaw  += 175;
	m_camera.m_pitch-= 40;

	m_geoMipMapping.LoadDataSet( "Data\\ps_height_1k.raw", 1025 );
	m_geoMipMapping.LoadTextureMap( "Data\\ps_texture_1k.tga" );
	m_geoMipMapping.Create( 0.6f );

	g_log.Write( DemoFramework::LOG_SUCCESS, "Demo initialized successfully" );
	return true;
}

//what to do when the program is idle
void DemoApp::OnIdle( void ) {
}

//the demo's main loop
bool DemoApp::OnMainLoop( void ) {
	Update( );
	Render( );

	return true;
}

//the function that performs the demo's logic
void DemoApp::Update( void ) {
	m_timer.Update( );
}

void DemoApp::Render( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity( );

	m_camera.ComputeViewMatrix( );
	m_camera.SetViewMatrix( );

	m_camera.ComputeViewFrustum( );

	if( m_solidFill )	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	else	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glEnable( GL_DEPTH_TEST );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	m_geoMipMapping.Update( &m_camera );
	m_geoMipMapping.Render( );

	m_fontEngine.BeginTextMode( );

		m_fontEngine.SetColor( 0.0f, 1.0f, 0.2f, 1.0f );

		m_fontEngine.Print( 0, 0, "FPS:%7.0f", m_timer.GetFPS( ) );
		m_fontEngine.Print( 0, 8, "Vertices: %d", m_geoMipMapping.GetNumVertices( ) );
		m_fontEngine.Print( 0, 16, "Tris:     %d", m_geoMipMapping.GetNumTriangles( ) );
		m_fontEngine.Print( 0, 24, "MTris/s:  %f", ( m_geoMipMapping.GetNumTriangles( )*m_timer.GetFPS( ) )/1e6 );

	m_fontEngine.EndTextMode( );

	glFlush( );
	SDL_GL_SwapBuffers( );
}

//resize the viewing area (correct 3D perspective)
void DemoApp::ResizeScene( void ) {
	glViewport( 0, 0, m_screenWidth, m_screenHeight );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	//Calculate the aspect ratio of the window
	gluPerspective( 45, ( double )m_screenWidth/( double )m_screenHeight, 1.0f, 2000.0f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
}

//the procedures for handling a key down message
bool DemoApp::OnKeyDown( void ) {
	if( KeyDown( SDLK_ESCAPE ) ) {
		return false;
	}

	if( KeyDown( SDLK_UP ) )
		m_camera.m_eyePos+= m_camera.m_forward*m_movementSpeed;
	else if( KeyDown( SDLK_DOWN ) )
		m_camera.m_eyePos-= m_camera.m_forward*m_movementSpeed;	

	if( KeyDown( SDLK_RIGHT ) )
		m_camera.m_eyePos+= m_camera.m_side*m_movementSpeed;
	else if( KeyDown( SDLK_LEFT ) )
		m_camera.m_eyePos-= m_camera.m_side*m_movementSpeed;

	if( KeyDown( SDLK_s ) )	m_solidFill= true;
	if( KeyDown( SDLK_w ) )	m_solidFill= false;

	return true;
}

bool DemoApp::OnMouseAction( int x, int y, bool left, bool right, bool middle ) {
	if( left ) {
		//update the camera view
		m_camera.m_yaw  += ( ( float )x - m_mouseX )*m_mouseSensitivity;
		m_camera.m_pitch-= ( ( float )y - m_mouseY )*m_mouseSensitivity;
	}

	m_mouseX= x;
	m_mouseY= y;

	return true;
}

bool DemoApp::OnRelease( void ) {
	m_geoMipMapping.Release( );
	m_geoMipMapping.ReleaseDataSet( );

	g_log.Write( DemoFramework::LOG_SUCCESS, "Demo shutdown successfully" );
	return true;
}
