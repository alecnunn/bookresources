// input
// (c) Kenny Mitchell - Westwood Studios EA 2002
#include "stdafx.h"

#include "input.h"
#include "MyDebug.h"

extern LPDIRECT3DDEVICE8 g_Device;
extern HWND g_hWnd;

#define SAMPLE_BUFFER_SIZE 16  // arbitrary number of buffer elements

// Sets up the mouse device using the flags from the dialog.
Input::Input()
{
	HRESULT result;

	// Create a DInput object
	result=DirectInput8Create
	(
		GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(VOID**)&m_pDI, 
		NULL 
	);
	FAILMSG("Failed to create direct input");

	// Obtain an interface to the system mouse device.
	result=m_pDI->CreateDevice
	(
		GUID_SysMouse, 
		&m_pMouse, 
		NULL 
	);
	FAILMSG("Failed to create mouse");

	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
	result=m_pMouse->SetDataFormat(&c_dfDIMouse2);
	FAILMSG("Failed to set mouse data format");

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
	result=m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	FAILMSG("Failed to set non exclusive forground mouse cooperative level");

/*	if( !bImmediate )
	{
	// IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
	//
	// DirectInput uses unbuffered I/O (buffer size = 0) by default.
	// If you want to read buffered data, you need to set a nonzero
	// buffer size.
	//
	// Set the buffer size to SAMPLE_BUFFER_SIZE (defined above) elements.
	//
	// The buffer size is a DWORD property associated with the device.
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

	if( FAILED( hr = g_pMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
	return hr;
	}*/

	// Acquire the newly created device
	m_pMouse->Acquire();

	memset(m_Keys,0,sizeof(BYTE)*256);
}

Input::~Input()
{
	// Unacquire the device one last time just in case 
	// the app tried to exit while the device is still acquired.
	if (m_pMouse) 
	{
		m_pMouse->Unacquire();
	}

	// Release any DirectInput objects.
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pDI);
}



// Read the input device's state when in immediate mode and display it.
void Input::ReadImmediateData()
{
	HRESULT result;

	if (!m_pMouse) 
		return;

	// Get the input's device state, and put the state in dims
	memset(&m_MouseState, 0, sizeof(DIMOUSESTATE2));

	result=m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);
	if (FAILED(result)) 
	{
		// DirectInput may be telling us that the input stream has been
		// interrupted.  We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done.
		// We just re-acquire and try again.

		// If input is lost then acquire and keep trying 
		result=m_pMouse->Acquire();
		while (result==DIERR_INPUTLOST) 
		{
			result=m_pMouse->Acquire();
		}

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return; 
	}

	// The dims structure now has the state of the mouse, so 
	// display mouse coordinates (x, y, z) and buttons.
	/*    sprintf( strNewText, "(X=% 3.3d, Y=% 3.3d, Z=% 3.3d) B0=%c B1=%c B2=%c B3=%c B4=%c B5=%c B6=%c B7=%c",
	MouseState.lX, MouseState.lY, MouseState.lZ,
	(MouseState.rgbButtons[0] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[1] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[2] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[3] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[4] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[5] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[6] & 0x80) ? '1' : '0',
	(MouseState.rgbButtons[7] & 0x80) ? '1' : '0');*/
}




// Read the input device's state when in buffered mode and display it.
void Input::ReadBufferedData()
{
	DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];  // Receives buffered data 
	DWORD              Num_Elements;
	HRESULT            result;

	if (!m_pMouse)
		return;

	Num_Elements=SAMPLE_BUFFER_SIZE;

	result=m_pMouse->GetDeviceData
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
		result=m_pMouse->Acquire();
		while (result==DIERR_INPUTLOST) 
		{
			result=m_pMouse->Acquire();
		}

		// rusult may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return;
	}

	if (FAILED(result))  
		return;

	/*    // Study each of the buffer elements and process them.
	//
	// Since we really don't do anything, our "processing"
	// consists merely of squirting the name into our
	// local buffer.
	for( i = 0; i < dwElements; i++ ) 
	{
	// this will display then scan code of the key
	// plus a 'D' - meaning the key was pressed 
	//   or a 'U' - meaning the key was released
	switch( didod[ i ].dwOfs )
	{
	case DIMOFS_BUTTON0:
	strcpy( strElement, "B0" );
	break;

	case DIMOFS_BUTTON1:
	strcpy( strElement, "B1" );
	break;

	case DIMOFS_BUTTON2:
	strcpy( strElement, "B2" );
	break;

	case DIMOFS_BUTTON3:
	strcpy( strElement, "B3" );
	break;

	case DIMOFS_X:
	strcpy( strElement, "X" );
	break;

	case DIMOFS_Y:
	strcpy( strElement, "Y" );
	break;

	case DIMOFS_Z:
	strcpy( strElement, "Z" );
	break;

	default:
	strcpy( strElement, "" );
	}

	switch( didod[ i ].dwOfs )
	{
	case DIMOFS_BUTTON0:
	case DIMOFS_BUTTON1:
	case DIMOFS_BUTTON2:
	case DIMOFS_BUTTON3:
	if( didod[ i ].dwData & 0x80 )
	strcat( strElement, "U " );
	else
	strcat( strElement, "D " );
	break;

	case DIMOFS_X:
	case DIMOFS_Y:
	case DIMOFS_Z:
	{
	TCHAR strCoordValue[255];
	wsprintf( strCoordValue, "%d ", didod[ i ].dwData );
	strcat( strElement, strCoordValue );
	break;
	}
	}

	strcat( strNewText, strElement );
	}*/
}

void Input::Update()
{
	ReadImmediateData(); 

	if (GetKey('W'))	g_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (GetKey('S'))	g_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
 
void Input::ProcessMsg(UINT msg, WPARAM wParam)
{
	switch (msg)
	{
	case WM_KEYUP		: 
		KeyUp(wParam); 
		break;
	case WM_KEYDOWN	: 
		KeyDown(wParam); 
		break;
	case WM_ENTERMENULOOP:
		MouseUnacquire();		// Release the device, so if we are in exclusive mode the  cursor will reappear
		break;
	case WM_EXITMENULOOP:
		MouseAcquire(); // Make sure the device is acquired when coming out of a menu loop
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE!=wParam)
		{
			// Make sure the device is acquired, if we are gaining focus.
			MouseAcquire();
		}
		break;

	};

}