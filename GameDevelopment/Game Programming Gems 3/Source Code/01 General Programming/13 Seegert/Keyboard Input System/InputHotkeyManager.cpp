#include "InputHotkeyManager.h"

/* globals */
string gWriteDelimeter = ", ";
string gReadDelimeter  = ::gWriteDelimeter + "-\t";


/*
 * constructor
 * InputHotKeyManager
 *
 * Clears the list of hotkeys.
 *
 */
InputHotkeyManager::InputHotkeyManager(void)
{
	this->mInstance = NULL;
	this->mHotkeyMap.clear();
}


/*
 * destructor
 * ~InputHotKeyManager
 *
 * Deletes the allocated hotkeys.
 * Add clean up code here.
 *
 */
InputHotkeyManager::~InputHotkeyManager(void)
{ 
	HotkeyPtrMapType::iterator	theIterator;

	for (theIterator = this->mHotkeyMap.begin(); theIterator != this->mHotkeyMap.end(); theIterator++)
	{
		delete (*theIterator).second;
		(*theIterator).second = NULL;
	}

	this->mHotkeyMap.clear();
}


/*
 * private
 * AddHotkey
 *
 * Adds a hotkey to the managers map.
 *
 */
void				
InputHotkeyManager::AddHotkey(	const string&	inName,
								const InputKey&	inDefaultKey,
								InputEvent*		inJustPressedEvent,
								InputEvent*		inPressedEvent,
								InputEvent*		inJustUnPressedEvent)
{
	/* create the new hotkey */
	InputHotkey* theHotkeyPtr = new InputHotkey(inName, 
												inDefaultKey,
												inJustPressedEvent,
												inPressedEvent,
												inJustUnPressedEvent);
	
	/* insert it into our map */
	this->mHotkeyMap.insert( pair < InputKey, InputHotkey* > (theHotkeyPtr->GetKey(), theHotkeyPtr));
}


/*
 * public
 * FireEvents
 *
 * Given a list of keys that have been pressed, fires the appropriate
 * hotkey events.
 *
 */
void				
InputHotkeyManager::FireEvents(const KeyListType& inJustPressed, const KeyListType& inPressed, const KeyListType& inJustUnPressed)
{
	HotkeyPtrMapType::iterator	theHotkeyIterator;
	KeyListType::const_iterator	theKeyIterator;

	/* the potential drawback with this method is that events
	 * may not fire in the order you expect them to.  i.e, some
	 * key may have only a pressed event, and another may have
	 * a just pressed event.  if both keys are processed during
	 * the same InputSystem::Refresh(), the just pressed event
	 * will ALWAYS fire first, even if it wasn't the first key
	 * pressed.  the way to resolve this issue would probably be 
	 * to pass one vector of keys, each key with an associated
	 * "type" so that the appropriate event can be fired.  
	 * alternatively, make use of the InputKey.mTimeStamp member
	 * and sort the key presses, since I made sure each key press 
	 * contains the timestamp but does not base equality on it.
	 * it's not a likely occurence however, and when it does
	 * occur will rarely cause problems.  it's good to be aware
	 * of such issues in case they do occur. */

	/* loop through the just pressed keys */
	for (theKeyIterator = inJustPressed.begin(); theKeyIterator != inJustPressed.end(); theKeyIterator++)
	{
		/* this is a fast lookup since it is a map.  if we find a hotkey for the
		 * key press, fire the event. */
		theHotkeyIterator = this->mHotkeyMap.find((*theKeyIterator));
		if (theHotkeyIterator != this->mHotkeyMap.end())
			(*theHotkeyIterator).second->FireJustPressedEvent();
	}

	/* loop through the pressed keys */
	for (theKeyIterator = inPressed.begin(); theKeyIterator != inPressed.end(); theKeyIterator++)
	{
		/* this is a fast lookup since it is a map.  if we find a hotkey for the
		 * key press, fire the event. */
		theHotkeyIterator = this->mHotkeyMap.find((*theKeyIterator));
		if (theHotkeyIterator != this->mHotkeyMap.end())
			(*theHotkeyIterator).second->FirePressedEvent();
	}

	/* loop through the just unpressed keys */
	for (theKeyIterator = inJustUnPressed.begin(); theKeyIterator != inJustUnPressed.end(); theKeyIterator++)
	{
		/* this is a fast lookup since it is a map.  if we find a hotkey for the
		 * key press, fire the event. */
		theHotkeyIterator = this->mHotkeyMap.find((*theKeyIterator));
		if (theHotkeyIterator != this->mHotkeyMap.end())
			(*theHotkeyIterator).second->FireJustUnPressedEvent();
	}
}


/*
 * public
 * Initialize
 *
 * Initializes the manager, creating the list of hotkeys.
 *
 */
void				
InputHotkeyManager::Initialize(HINSTANCE inInstance)
{
	this->mInstance = inInstance;
}


/*
 * private
 * GetInputKeyFromName
 *
 * Given a combined scan code and modifier "nice name",
 * this function creates the key press object.
 *
 */
bool				
InputHotkeyManager::GetInputKeyFromName(const string& inName, InputKey& outKey)
{
	string	theCtrlString	= "";
	string	theAltString	= "";
	string	theShiftString	= "";
	string	theToken;
	char	theTempTokenString[kihmMaxHotkeyLength];
	char	theTempString[kihmMaxHotkeyLength];
	char*	theTempToken;
	uint32	theLoop;

	/* clear the output key */
	outKey = InputKey();

	/* get the modifier strings from the string table */
	if (::LoadString(this->mInstance, kModifierNameCtrl, theTempString, kihmMaxHotkeyLength) > 0)
		theCtrlString = string(theTempString);
	if (::LoadString(this->mInstance, kModifierNameAlt, theTempString, kihmMaxHotkeyLength) > 0)
		theAltString = string(theTempString);
	if (::LoadString(this->mInstance, kModifierNameShift, theTempString, kihmMaxHotkeyLength) > 0)
		theShiftString = string(theTempString);
	
	/* make sure we have values for the modifiers, or we won't be 
	 * reading in much from our string. */
	if ((theCtrlString.length() > 0) &&
		(theAltString.length() > 0) &&
		(theShiftString.length() > 0))
	{
		/* copy off the string */
		::ZeroMemory(theTempTokenString, kihmMaxHotkeyLength);
		inName.copy(theTempTokenString, kihmMaxHotkeyLength);
		
		/* tokenize the string */
		theTempToken = ::strtok(theTempTokenString, ::gReadDelimeter.c_str());
		while (theTempToken != NULL)
		{
			/* convert this char* to a string */
			theToken = string(theTempToken);

			/* first, check if the token is a modifier */
			if (theToken == theCtrlString)
			{
				outKey.mModifier |= kismsCtrl;
			}
			else 
			if (theToken == theAltString)
			{
				outKey.mModifier |= kismsAlt;
			}
			else
			if (theToken == theShiftString)
			{
				outKey.mModifier |= kismsShift;
			}
			else
			{
				/* it's not a modifier, so lets check all 
				 * the scan code strings for a match. */
				for (theLoop = kisFirstScanCode; (theLoop <= kisLastScanCode) && (outKey.mScanCode == 0); theLoop++)
				{
					if (::LoadString(this->mInstance, theLoop, theTempString, kihmMaxHotkeyLength) > 0)
					{
						if (theToken == string(theTempString))
						{
							/* we found the scan code */
							outKey.mScanCode = theLoop;
							break;
						}
					}
				}
			}
			
			/* get the next token */
			theTempToken = ::strtok(NULL, ::gReadDelimeter.c_str());
		}
	}

	return (outKey.mScanCode > 0);	
}


/*
 * private
 * GetNameFromInputKey
 *
 * Gets a combined scan code and modifier "nice name" from a key press.
 *
 */
bool				
InputHotkeyManager::GetNameFromInputKey(const InputKey& inKey, string& outName)
{
	char	theTempString[kihmMaxHotkeyLength];

	/* clear the result string */
	outName = "";

	/* first, get the scan code name */
	if (::LoadString(this->mInstance, inKey.mScanCode, theTempString, kihmMaxHotkeyLength) > 0)
	{
		outName = theTempString;

		/* now build the rest of the string based on the modifiers it has */
		if ((inKey.mModifier & kismsCtrl) && 
			(::LoadString(this->mInstance, kModifierNameCtrl, theTempString, kihmMaxHotkeyLength) > 0))
		{
			outName += ::gWriteDelimeter + string(theTempString);
		}
		if ((inKey.mModifier & kismsShift) && 
			(::LoadString(this->mInstance, kModifierNameShift, theTempString, kihmMaxHotkeyLength) > 0))
		{
			outName += ::gWriteDelimeter + string(theTempString);
		}
		if ((inKey.mModifier & kismsAlt) && 
			(::LoadString(this->mInstance, kModifierNameAlt, theTempString, kihmMaxHotkeyLength) > 0))
		{
			outName += ::gWriteDelimeter + string(theTempString);
		}
	}

	return (outName.length() > 0);
}


/*
 * public
 * ReadHotkeysFromFile
 *
 * Reads a file, comparing the key assignments to the
 * internal hotkey list.  If there is a match, the new
 * key assignment is set for the hotkey.
 *
 */
void				
InputHotkeyManager::ReadHotkeysFromFile(const string& inFileName)
{
	HotkeyPtrMapType			theNewMap;
	HotkeyPtrMapType::iterator	theIterator;
	InputKey					theNewKey;
	string						theCurrentName;
	string						theNewName;
	char						theTempKey[kihmMaxHotkeyLength];

	/* clear out our new map */
	theNewMap.clear();

	/* iterate through the old map */
	for (theIterator = this->mHotkeyMap.begin(); theIterator != this->mHotkeyMap.end(); theIterator++)
	{
		/* get the string name of this input key */
		if (this->GetNameFromInputKey((*theIterator).second->GetKey(), theCurrentName))
		{
			/* read the ini file to find the potentially new string name */
			::GetPrivateProfileString(kihmHotkeySectionName, (*theIterator).second->GetName().c_str(), theCurrentName.c_str(), theTempKey, kihmMaxHotkeyLength, inFileName.c_str());

			/* convert this string into an InputKey */
			theNewName = string(theTempKey);
			if (this->GetInputKeyFromName(theNewName, theNewKey))
			{
				/* compare the key we currently have 
				 * with what we read in from the file. */
				if ((*theIterator).second->GetKey() == theNewKey)
				{
					/* the key assignment is the same, do nothing. */
				}
				else
				{
					/* this keys assignment has changed, so update the hotkey. */
					(*theIterator).second->SetKey(theNewKey);
				}

				/* insert the key into the new temporary map */
				theNewMap.insert( pair < InputKey, InputHotkey* > (theNewKey, (*theIterator).second));
			}
		}
	}

	/* clear out and recreate our internal map from the temp copy */
	this->mHotkeyMap.clear();
	for (theIterator = theNewMap.begin(); theIterator != theNewMap.end(); theIterator++)
		this->mHotkeyMap.insert( pair < InputKey, InputHotkey* > ((*theIterator).second->GetKey(), (*theIterator).second));
}


/*
 * public
 * WriteHotkeysToFile
 *
 * Writes the internal hotkeys key assignments to an
 * external file.
 *
 */
void				
InputHotkeyManager::WriteHotkeysToFile(const string& inFileName, bool inPrintHelpText)
{
	HotkeyPtrMapType::iterator	theIterator;
	string						theCurrentName;
	char						theTempString[kihmMaxHotkeyLength];
	FILE*						theFile;
	uint32						theLoop;

	/* if you want to print some help text (a list of all keys) to the file, set this flag to true */
	if (inPrintHelpText)
	{
		/* open the file */
		theFile = ::fopen(inFileName.c_str(), "w");
		if (theFile)
		{
			/* print all scan code keys */
			if (::LoadString(this->mInstance, kAvailableKeys, theTempString, kihmMaxHotkeyLength) > 0)
			{
				::fprintf(theFile, "; %s\n", theTempString);
				
				for (theLoop = kisFirstScanCode; theLoop <= kisLastScanCode; theLoop++)
					if (::LoadString(this->mInstance, theLoop, theTempString, kihmMaxHotkeyLength) > 0)
						::fprintf(theFile, ";\t%s\n", theTempString);
					
					::fprintf(theFile, ";\n");
			}
			
			/* print all modifiers */
			if (::LoadString(this->mInstance, kAvailableModifiers, theTempString, kihmMaxHotkeyLength) > 0)
			{
				::fprintf(theFile, "; %s\n", theTempString);
				if (::LoadString(this->mInstance, kModifierNameCtrl, theTempString, kihmMaxHotkeyLength) > 0)
					::fprintf(theFile, ";\t%s\n", theTempString);
				if (::LoadString(this->mInstance, kModifierNameAlt, theTempString, kihmMaxHotkeyLength) > 0)
					::fprintf(theFile, ";\t%s\n", theTempString);
				if (::LoadString(this->mInstance, kModifierNameShift, theTempString, kihmMaxHotkeyLength) > 0)
					::fprintf(theFile, ";\t%s\n", theTempString);
			}
			
			/* print some help text */
			if (::LoadString(this->mInstance, kHowTo, theTempString, kihmMaxHotkeyLength) > 0)
				::fprintf(theFile, ";\n;%s\n;", theTempString);
			
			/* close the file */
			::fprintf(theFile, "\n\n");
			::fclose(theFile);
		}
	}

	/* this is the important part:
	 * iterate through the internal map */
	for (theIterator = this->mHotkeyMap.begin(); theIterator != this->mHotkeyMap.end(); theIterator++)
	{
		/* get the string name of the input key */
		if (this->GetNameFromInputKey((*theIterator).second->GetKey(), theCurrentName))
		{
			/* write each hotkey to the ini file */
			::WritePrivateProfileString(kihmHotkeySectionName, (*theIterator).second->GetName().c_str(), theCurrentName.c_str(), inFileName.c_str());
		}
	}
}