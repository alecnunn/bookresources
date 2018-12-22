#include "InputSystem.h"


/* the size of the keyboard buffer */
#define kisBufferSize	256


/*
 * constructor
 * InputSystem
 *
 * Initializes the input system.
 *
 */
InputSystem::InputSystem(void)
{ 
	this->mInstance				= NULL;
	this->mDirectInput			= NULL;
	this->mDirectInputKeyboard	= NULL;

	this->mModifierState		= kismsNone;

	this->mJustPressedKeys.clear();
	this->mJustUnPressedKeys.clear();
	this->mPressedKeys.clear();
}


/*
 * destructor
 * ~InputSystem
 *
 * Add clean up code here.
 *
 */
InputSystem::~InputSystem(void)
{ 
	/* uninitialize COM */
	::CoUninitialize();
}


/*
 * static
 * GetModifierState
 *
 * Given a scan code, returns the modifier state.
 *
 */
ISModifierState	
InputSystem::GetModifierState(uint8 inScanCode)
{
	ISModifierState theState = kismsNone;

	if ((inScanCode == DIK_LCONTROL) ||
		(inScanCode == DIK_RCONTROL))
	{
		theState = kismsCtrl;
	}
	else
	if ((inScanCode == DIK_LMENU) ||
		(inScanCode == DIK_RMENU))
	{
		theState = kismsAlt;
	}
	else
	if ((inScanCode == DIK_LSHIFT) ||
		(inScanCode == DIK_RSHIFT))
	{
		theState = kismsShift;
	}

	return theState;
}


/*
 * public
 * Initialize
 *
 * Initializes the input system.
 */
bool				
InputSystem::Initialize(HINSTANCE inInstance, HWND inWindow)
{
	HRESULT		theHResult;
	DIPROPDWORD	theBufferData;

	/* clear out some variables */
	this->mModifierState		= kismsNone;
	this->mJustPressedKeys.clear();
	this->mJustUnPressedKeys.clear();
	this->mPressedKeys.clear();

	/* set the application instance */
	this->mInstance = inInstance;

	/* set the window handle */
	this->mWindow = inWindow;

	/* initialize com (this probably wouldnt be here in your app) */
	theHResult = ::CoInitialize(NULL);

	if (SUCCEEDED(theHResult))
	{
		/* initialize directinput */
		theHResult = ::DirectInput8Create(this->mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->mDirectInput, NULL);

		if (SUCCEEDED(theHResult))
		{
			/* create our keyboard interface */
			theHResult = this->mDirectInput->CreateDevice(GUID_SysKeyboard, &this->mDirectInputKeyboard, NULL);

			if (SUCCEEDED(theHResult))
			{
				/* set the data format */
				theHResult = this->mDirectInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
				
				if (SUCCEEDED(theHResult))
				{
					/* set the cooperative level (your application will probably want to use (DISCL_FOREGROUND | DISCL_EXCLUSIVE) */
					theHResult = this->mDirectInputKeyboard->SetCooperativeLevel(this->mWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

					if (SUCCEEDED(theHResult))
					{
						theBufferData.diph.dwSize		= sizeof(DIPROPDWORD);
						theBufferData.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
						theBufferData.diph.dwObj		= 0;
						theBufferData.diph.dwHow		= DIPH_DEVICE;
						theBufferData.dwData			= kisBufferSize;
						
						/* set the size of the keyboard buffer */
						theHResult = this->mDirectInputKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theBufferData.diph);

						if (SUCCEEDED(theHResult))
						{
							/* acquire the keyboard */
							theHResult = this->mDirectInputKeyboard->Acquire();
						}
					}
				}
			}
		}
	}

	return (SUCCEEDED(theHResult));
}


/* 
 * public
 * Refresh
 *
 * Refreshes the input system, querying for new
 * inputs and filling the vector of events.
 */
bool
InputSystem::Refresh(void)
{
	KeyListType::iterator	theIterator;
	DIDEVICEOBJECTDATA		theBufferedKeys[kisBufferSize];
	DWORD					theNumKeys = kisBufferSize; 
    HRESULT					theHResult;
	uint32					theLoop;

	/* make sure we have a direct input interface */
	if (this->mDirectInputKeyboard)
	{
		/* clear the list of just pressed and unpressed keys */
		this->mJustPressedKeys.clear();
		this->mJustUnPressedKeys.clear();

		/* query the direct input buffer */
		theHResult = this->mDirectInputKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), theBufferedKeys, &theNumKeys, 0); 

		if (SUCCEEDED(theHResult))
		{
			if (theHResult == DI_BUFFEROVERFLOW)
			{
				/* it's possible to have keyboard overflow.  in this case,
				 * it would be wise to call GetDeviceState and
				 * compare the keys that are down with what the vectors
				 * have in them... adding the appropriate keys to the pressed
				 * vector.  exercise for the reader =)
				 */
			}

			/* loop over the number of key inputs */
			for (theLoop = 0; theLoop < theNumKeys; theLoop++)
			{
				InputKey			theTempKey;
				ISModifierState		theTempModifier;
				
				/* wrap the key in an InputKey object */
				theTempKey.mTimeStamp = theBufferedKeys[theLoop].dwTimeStamp;
				theTempKey.mScanCode  = theBufferedKeys[theLoop].dwOfs;
				theTempKey.mModifier  = this->mModifierState;

				/* check if the key is a modifer */
				theTempModifier = InputSystem::GetModifierState(theBufferedKeys[theLoop].dwOfs);

				/* is the key is down? */
				if (theBufferedKeys[theLoop].dwData & 0x80)
				{
					/* set the modifier state if necessary */
					if (theTempModifier != kismsNone)
						this->mModifierState |= theTempModifier;

					/* this key was just pressed... */
					this->mJustPressedKeys.push_back(theTempKey);

					/* ...as well as "pressed" */
					theIterator = std::find(this->mPressedKeys.begin(), this->mPressedKeys.end(), theTempKey);
					if (theIterator == this->mPressedKeys.end())
						this->mPressedKeys.push_back(theTempKey);
				}
				else
				{
					/* set the modifier state if necessary */
					if (theTempModifier != kismsNone)
					{
						this->mModifierState ^= theTempModifier;
						theTempKey.mModifier ^= theTempModifier;
					}

					/* this key was just released */
					this->mJustUnPressedKeys.push_back(theTempKey);

					/* erase this key from the pressed list, if we can find it. */
					theIterator = this->mPressedKeys.begin();
					while (theIterator != this->mPressedKeys.end())
					{
						if ((*theIterator).mScanCode == theTempKey.mScanCode)
							this->mPressedKeys.erase(theIterator);
						else
							theIterator++;
					}
				}
			}
		}
		else
		{
			if ((theHResult == DIERR_INPUTLOST) ||
				(theHResult == DIERR_NOTACQUIRED))
			{
				/* we lost the device, reaquire it. */
				theHResult = this->mDirectInputKeyboard->Acquire();
			}
		}
	}

	return (SUCCEEDED(theHResult));
}