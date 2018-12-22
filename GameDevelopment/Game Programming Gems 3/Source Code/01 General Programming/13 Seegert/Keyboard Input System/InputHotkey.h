#ifndef __INPUT_HOTKEY_
#define __INPUT_HOTKEY_

#include "Real Time Input.h"
#include "InputEvent.h"
#include "InputKey.h"


/*
 * InputHotkey
 *
 * This class specifies the key
 * and modifiers that makes up a hotkey
 * in the game, as well as the events
 * to fire when the hotkey is pressed.
 */

class InputHotkey
{
	friend class InputHotkeyManager;

public:
	
	inline const InputKey&	GetKey(void)
								{ return this->mKey; }
	inline const string&	GetName(void)
								{ return this->mName; }

	inline void				FirePressedEvent(void)
								{ if (this->mPressedEvent) this->mPressedEvent->ProcessEvent(); }
	inline void				FireJustPressedEvent(void)
								{ if (this->mJustPressedEvent) this->mJustPressedEvent->ProcessEvent(); }
	inline void				FireJustUnPressedEvent(void)
								{ if (this->mJustUnPressedEvent) this->mJustUnPressedEvent->ProcessEvent(); }

	inline void				SetKey(const InputKey& inKey)
								{ this->mKey = inKey; }

private:
							InputHotkey(const string&	inName,
										const InputKey&	inDefaultKey,
										InputEvent*		inJustPressedEvent		= NULL,
										InputEvent*		inPressedEvent			= NULL,
										InputEvent*		inJustUnPressedEvent	= NULL);
							
							~InputHotkey(void);

							
	string					mName;					/* the unique name of this hotkey */
	InputKey				mKey;					/* the key and modifier */
	InputEvent*				mJustPressedEvent;		/* the event to fire when the hotkey is just pressed */
	InputEvent*				mPressedEvent;			/* the event to fire when the hotkey is being pressed */
	InputEvent*				mJustUnPressedEvent;	/* the event to fire when the hotkey is released */
};


#endif /*__INPUT_HOTKEY_*/
