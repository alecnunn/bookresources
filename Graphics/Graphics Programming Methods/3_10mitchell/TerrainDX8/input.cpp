// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "input.h"

// Sets up the mouse device using the flags from the dialog.
Input_Class::Input_Class()
{
	HRESULT result;

	// Create a DInput object
	result=DirectInput8Create
	(
		GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(VOID**)&DI, 
		NULL 
	);
	FAILMSG("Failed to create direct input");

	// Obtain an interface to the system mouse device.
	result=DI->CreateDevice
	(
		GUID_SysMouse, 
		&Mouse, 
		NULL 
	);
	FAILMSG("Failed to create mouse");

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
	result=Mouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND);
	FAILMSG("Failed to set exclusive forground mouse cooperative level");

	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
	result=Mouse->SetDataFormat(&c_dfDIMouse2);
	FAILMSG("Failed to set mouse data format");

	// Acquire the newly created device
	Mouse->Acquire();

	memset(Keys,0,sizeof(BYTE)*256);
}

Input_Class::~Input_Class()
{
	// Unacquire the device one last time just in case 
	// the app tried to exit while the device is still acquired.
	if (Mouse) 
	{
		Mouse->Unacquire();
	}

	// Release any DirectInput objects.
	Safe_Release(Mouse);
	Safe_Release(DI);
}



// Read the input device's state when in immediate mode and display it.
void Input_Class::Read_Immediate_Data()
{
	HRESULT result;

	if (!Mouse) 
		return;

	// Get the input's device state, and put the state in dims
	memset(&Mouse_State, 0, sizeof(DIMOUSESTATE2));

	result=Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &Mouse_State);
	if (FAILED(result)) 
	{
		// DirectInput may be telling us that the input stream has been
		// interrupted.  We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done.
		// We just re-acquire and try again.

		// If input is lost then acquire and keep trying 
		result=Mouse->Acquire();
		while (result==DIERR_INPUTLOST) 
		{
			result=Mouse->Acquire();
		}

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return; 
	}
}




// Read the input device's state when in buffered mode and display it.
void Input_Class::Read_Buffered_Data()
{
	DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];  // Receives buffered data 
	DWORD              Num_Elements;
	HRESULT            result;

	if (!Mouse)
		return;

	Num_Elements=SAMPLE_BUFFER_SIZE;

	result=Mouse->GetDeviceData
	(
		sizeof(DIDEVICEOBJECTDATA),
		didod, 
		&Num_Elements, 
		0 
	);

	if (result!=DI_OK)
	{
		// We got an error or we got DI_BUFFEROVERFLOW.
		//
		// Either way, it means that continuous contact with the
		// device has been lost, either due to an external
		// interruption, or because the buffer overflowed
		// and some events were lost.
		//
		// Consequently, if a button was pressed at the time
		// the buffer overflowed or the connection was broken,
		// the corresponding "up" message might have been lost.
		//
		// But since our simple sample doesn't actually have
		// any state associated with button up or down events,
		// there is no state to reset.  (In a real game, ignoring
		// the buffer overflow would result in the game thinking
		// a key was held down when in fact it isn't; it's just
		// that the "up" event got lost because the buffer
		// overflowed.)
		//
		// If we want to be cleverer, we could do a
		// GetDeviceState() and compare the current state
		// against the state we think the device is in,
		// and process all the states that are currently
		// different from our private state.
		result=Mouse->Acquire();
		while (result==DIERR_INPUTLOST) 
		{
			result=Mouse->Acquire();
		}

		// result may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return;
	}

	if (FAILED(result))  
		return;
}

void Input_Class::Update()
{
	Read_Immediate_Data(); 
}
 
void Input_Class::Process_Msg(UINT msg, WPARAM wParam)
{
	switch (msg)
	{
	case WM_KEYUP		: 
		Key_Up(wParam); 
		break;
	case WM_KEYDOWN	: 
		Key_Down(wParam); 
		break;
	case WM_ENTERMENULOOP:
		Mouse_Unacquire();		// Release the device, so if we are in exclusive mode the  cursor will reappear
		break;
	case WM_EXITMENULOOP:
		Mouse_Acquire(); // Make sure the device is acquired when coming out of a menu loop
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE!=wParam)
		{
			// Make sure the device is acquired, if we are gaining focus.
			Mouse_Acquire();
		}
		break;
	};
}