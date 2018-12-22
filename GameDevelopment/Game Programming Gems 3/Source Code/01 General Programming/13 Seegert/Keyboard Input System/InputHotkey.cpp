#include "InputHotkey.h"


/*
 * constructor
 * InputHotkey
 *
 * Creates a hotkey.
 *
 */
InputHotkey::InputHotkey(	const string&	inName,
							const InputKey&	inDefaultKey,
							InputEvent*		inJustPressedEvent,
							InputEvent*		inPressedEvent,
							InputEvent*		inJustUnPressedEvent)
{ 
	this->mName					= inName;
	this->mKey					= inDefaultKey;
	this->mJustPressedEvent		= inJustPressedEvent;
	this->mPressedEvent			= inPressedEvent;
	this->mJustUnPressedEvent	= inJustUnPressedEvent;
}


/*
 * destructor
 * ~InputHotkey
 *
 * Add clean up code here.
 *
 */
InputHotkey::~InputHotkey(void)
{ 
	delete this->mJustPressedEvent;
	delete this->mJustUnPressedEvent;
	delete this->mPressedEvent;
}
