/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Application.h
 *    Desc: Declaration for the cApplication class this class 
 *          represents our top-level application controller
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <windows.h>

#include "GameErrors.h"
#include "GameTypes.h"
#include "GraphicsLayer.h"
#include "InputLayer.h"

class cWindow;


class cApplication 
{
protected:

	//==========--------------------------  Member variables
	
	string	m_title; // Name the window should have
	int		m_width; // Width of our application
	int		m_height; // Height of our application
	int		m_bpp; // Desired depth (may not be possible)

	bool	m_bActive; // Is the appliation active?

	uint	m_glFlags; // Flags for the graphics layer

	static cApplication*	m_pGlobalApp;

	virtual void InitPrimaryWindow();
	virtual void InitGraphics();
	virtual void InitInput();
	virtual void InitSound();
	virtual void InitExtraSubsystems();

public:

	cApplication();
	virtual ~cApplication();

	/**
	 * Init the application.  All user-level init should go here
	 */
	virtual void Init(); 

	/**
	 * Run the application.  This shouldn't need to change
	 */
	virtual void Run();

	/**
	 * Handles all code that should be run each frame
	 * timeDelta == amount of time (in seconds) since last call
	 */
	virtual void DoFrame( float timeDelta );
	virtual void DoIdleFrame( float timeDelta );


	/**
	 * Scene Init is called by Init.  This way user-level class 
	 * doesn't need to worry about setting up D3D or handling any 
	 * errors during setup.
	 */
	virtual void SceneInit();
	virtual void SceneEnd();

	void Pause() { m_bActive = false; }
	void UnPause() { m_bActive = true; }

	static cApplication* GetApplication() { return m_pGlobalApp; }

	static void KillApplication() 
	{
		delete m_pGlobalApp;
		m_pGlobalApp = NULL;
	}
};

inline cApplication* Application()
{
	return cApplication::GetApplication();
}

HINSTANCE AppInstance();

/**
 * This function is implemented by any client that uses GameLib
 */
cApplication*	CreateApplication(); // returns a ptr to a valid application object




#endif // _APPLICATION_H
