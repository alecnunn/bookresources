//==============================================================
// Application.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Application base class for a demo framework.
//==============================================================
#ifndef __DemoFramework_Application_H__
#define __DemoFramework_Application_H__

#include <windows.h>
#include "EventHandler.h"

namespace DemoFramework {
	//base application class
	class Application {
		//generic application members
		static Application *m_pApplication;
		static EventHandler *m_pEventHandler;

	public:
		static Application *GetApplication( void ) { return m_pApplication; }

		static EventHandler *GetEventHandler( void ) { return m_pEventHandler; }
		static void SetEventHandler( EventHandler *pEventHandler ) { m_pEventHandler= pEventHandler; }

		virtual bool OnCreate( void )= 0;
		virtual void OnIdle( void )= 0;
		virtual bool OnMainLoop( void )= 0;
		virtual bool OnRelease( void )= 0;

		static void Create( unsigned int SDLFlags, int width, int height, int bpp, const char *windowTitle );
		static int Execute( void );

	Application( void ) { m_pApplication= this; }
	virtual ~Application( void ) { m_pApplication= NULL; }
	};
}

#endif	//__DemoFramework_Application_H__