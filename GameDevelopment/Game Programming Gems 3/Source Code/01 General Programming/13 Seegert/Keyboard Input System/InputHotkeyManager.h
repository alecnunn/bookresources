#ifndef __INPUT_HOTKEY_MANAGER_
#define __INPUT_HOTKEY_MANAGER_

#include "Real Time Input.h"
#include "InputHotkey.h"

/* definitions */
#define kihmHotkeySectionName			"Hotkeys"
#define kihmMaxHotkeyLength				256

/* typedefs */
typedef map < InputKey, InputHotkey* >	HotkeyPtrMapType;

/* globals */
extern string gReadDelimeter;
extern string gWriteDelimeter;

/*
 * InputHotkeyManager
 *
 * This class maintains a list of
 * hotkeys in the game and initializes
 * the key combinations from a configuration
 * file.
 */

class InputHotkeyManager
{
public:
						 InputHotkeyManager(void);
						~InputHotkeyManager(void);

	void				AddHotkey(	const string&	inName,
									const InputKey&	inDefaultKey,
									InputEvent*		inJustPressedEvent		= NULL,
									InputEvent*		inPressedEvent			= NULL,
									InputEvent*		inJustUnPressedEvent	= NULL);

	void				FireEvents(const KeyListType& inJustPressed, const KeyListType& inPressed, const KeyListType& inJustUnPressed);
	
	void				Initialize(HINSTANCE inInstance);
	
	void				ReadHotkeysFromFile(const string& inFileName);
	void				WriteHotkeysToFile (const string& inFileName, bool inPrintHelpText = false);

private:
	
	bool				GetInputKeyFromName(const string& inName, InputKey& outKey);
	bool				GetNameFromInputKey(const InputKey& inKey, string& outName);


	HotkeyPtrMapType	mHotkeyMap;			/* the map of keypresses to hotkeys, for fast lookup */
	HINSTANCE			mInstance;			/* the instance for the key string table */

};


#endif /*__INPUT_HOTKEY_MANAGER_*/
