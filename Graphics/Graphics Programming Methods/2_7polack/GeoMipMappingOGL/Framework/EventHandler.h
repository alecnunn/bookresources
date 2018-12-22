//==============================================================
// EventHandler.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// The event handler base class for a demo framework.
//==============================================================
#ifndef __DemoFramework_EventHandler_H__
#define __DemoFramework_EventHandler_H__

#include <SDL.h>

namespace DemoFramework {
	//application event handler
	class EventHandler {
		static bool m_keys[512];

	public:

		virtual bool HandleEvent( SDL_Event event );

		virtual bool OnMouseAction( int x, int y, bool left, bool right, bool middle ) {
			return true;
		}

		virtual bool OnKeyDown( void ) { return true; }
		virtual bool OnKeyUp( void ) { return true; }

		virtual bool OnCreate( void ) { return true; }
		virtual bool OnRelease( void ) { return true; }

		inline bool KeyDown( SDLKey key ) { return m_keys[key]; }
		inline bool KeyUp( SDLKey key ) { return !m_keys[key]; }

	EventHandler( void ) { }
	~EventHandler( void ) { }
	};
}


#endif	//__DemoFramework_EventHandler_H__