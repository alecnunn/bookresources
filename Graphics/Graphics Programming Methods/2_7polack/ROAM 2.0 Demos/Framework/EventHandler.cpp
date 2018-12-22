//==============================================================
// EventHandler.cpp
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Application base class for a demo framework.
//==============================================================
#include "EventHandler.h"
using namespace DemoFramework;

//key buffer
bool EventHandler::m_keys[512];

//check for the messages that were received by the window
bool EventHandler::HandleEvent( SDL_Event event ) {
	bool left, middle, right;

	switch( event.type ) {
		//the mouse has moved, handle the message
		case SDL_MOUSEMOTION:
			( event.button.button == SDL_BUTTON_LEFT ) ? left= true : left= false;
			( event.button.button == SDL_BUTTON_MIDDLE ) ? middle= true : middle= false;
			( event.button.button == SDL_BUTTON_RIGHT ) ? right= true : right= false;

			return( OnMouseAction( event.button.x, event.motion.y, left, right, middle ) );
			break;

		//the left mouse button is being pressed, handle it
		case SDL_MOUSEBUTTONDOWN:
			( event.button.button == SDL_BUTTON_LEFT ) ? left= true : left= false;
			( event.button.button == SDL_BUTTON_MIDDLE ) ? middle= true : middle= false;
			( event.button.button == SDL_BUTTON_RIGHT ) ? right= true : right= false;

			return( OnMouseAction( event.button.x, event.motion.y, left, right, middle ) );
			break;

		//the left mouse button is being released, handle it
		case SDL_MOUSEBUTTONUP:
			( event.button.button == SDL_BUTTON_LEFT ) ? left= true : left= false;
			( event.button.button == SDL_BUTTON_MIDDLE ) ? middle= true : middle= false;
			( event.button.button == SDL_BUTTON_RIGHT ) ? right= true : right= false;

			return( OnMouseAction( event.button.x, event.motion.y, left, right, middle ) );
			break;

		//a key has been pressed down, so handle the message
		case SDL_KEYDOWN:
			m_keys[event.key.keysym.sym]= true;

			break;

		//a key has been released, so handle the message
		case SDL_KEYUP:
			m_keys[event.key.keysym.sym]= false;
			break;

		//quit the application
		case SDL_QUIT:
			return false;
			break;

		//handle all other messages
		default: return true;
			break;
	}
	return true;
}