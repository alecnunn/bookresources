#ifndef __INPUT_SYSTEM_
#define __INPUT_SYSTEM_

#include "Real Time Input.h"
#include "InputKey.h"


/*
 * InputSystem
 *
 */

class InputSystem
{
public:
	
								InputSystem(void);
								~InputSystem(void);
								
	static ISModifierState		GetModifierState(uint8 inScanCode);

	inline const KeyListType&	GetJustPressedKeys(void)
									{ return this->mJustPressedKeys; }
	inline const KeyListType&	GetPressedKeys(void)
									{ return this->mPressedKeys; }
	inline const KeyListType&	GetJustUnPressedKeys(void)
									{ return this->mJustUnPressedKeys; }

	bool						Initialize(HINSTANCE inInstance, HWND inWindow);
	bool						Refresh(void);

private:							
	KeyListType					mJustPressedKeys;		/* vector of keys just pressed */
	KeyListType					mPressedKeys;			/* vector of keys pressed */
	KeyListType					mJustUnPressedKeys;		/* vector of keys just unpressed */
								
	HINSTANCE					mInstance;				/* the instance of this application */
	HWND						mWindow;				/* the handle to the window */
								
	IDirectInput*				mDirectInput;			/* our directinput interface */
	IDirectInputDevice*			mDirectInputKeyboard;	/* our directinput keyboard interface */
								
	ISModifierState				mModifierState;			/* the current modifier state */
};


#endif /*__INPUT_SYSTEM_*/
