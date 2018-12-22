#ifndef __INPUT_EVENT_
#define __INPUT_EVENT_


/*
 * InputEvent
 *
 * This class specifies the event
 * to fire when an input system action
 * is registered.  The class derived from
 * this event should safely interact 
 * with the game engine when processed.
 */

class InputEvent
{
public:
					 InputEvent(void) { };
					~InputEvent(void) { };

	virtual void	ProcessEvent(void) = 0;
};


/*
 * IESampleInputEvent
 *
 * This class is a sample input event.
 * This event is the code that will be
 * executed when a user presses a hotkey
 * to which you have assigned this 
 * event.
 */

class IESampleInputEvent : public InputEvent
{
public:
					IESampleInputEvent(HWND inWindow, int inID, string inString)
					{
						this->mWindow	= inWindow;
						this->mString	= inString;
						this->mID		= inID;
					}

	virtual void	ProcessEvent(void)
					{ 
						::SetDlgItemText(this->mWindow, this->mID, this->mString.c_str());
					}
	
	HWND	mWindow;
	string	mString;
	int		mID;
};

#endif /*__INPUT_EVENT_*/
