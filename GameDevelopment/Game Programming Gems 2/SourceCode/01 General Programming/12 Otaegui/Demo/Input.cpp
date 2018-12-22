/************************************************************
 * Input.cpp - Input Functions V2.16.002
 * 
 * (c) Copyright 1996-2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/

#include <windows.h>
#include <dinput.h>

#include "support.hpp"
#include "blitbuf.hpp"
#include "MainPtah7.h"

/****************************************************************************
 *      Global variables
 ****************************************************************************/

BOOL            g_fPaused = TRUE;       /* Should I be paused? */

/* Funciones del DisplayMouse */
blitbuf *g_MouseBackground;
unsigned int ShiftMX, ShiftMY;

/****************************************************************************
 *      DirectInput globals
 ****************************************************************************/

LPDIRECTINPUT           g_pdi;
LPDIRECTINPUTDEVICE     g_pMouse;
LPDIRECTINPUTDEVICE		g_pKeyboard;
char                    g_szText[1024]; /* What we display in client area */

// Globales del mouse
int g_MouseButton, g_MouseX=320, g_MouseY=240;
int g_oldMouseButton;		// Datos de la posicion anterior
int g_oldMouseX, g_oldMouseY;

// Globales del teclado
int g_ShiftStatus;

/****************************************************************************
 *      Complain
 *      Whine and moan.
 ****************************************************************************/

void
Complain(
    HWND hwndOwner,
    HRESULT hr,
    LPCSTR pszMessage
)
{
    MessageBox(hwndOwner, pszMessage, APPTITLE, MB_OK);
}

/****************************************************************************
 *      DIInit
 *      Initialize the DirectInput variables.
 *      This entails the following four functions:
 *          DirectInputCreate
 *          IDirectInput::CreateDevice
 *          IDirectInputDevice::SetDataFormat
 *          IDirectInputDevice::SetCooperativeLevel
 ****************************************************************************/

BOOL
DIInit( HWND hwnd, HINSTANCE g_hinst )
{
    HRESULT hr;

    /*
     *  Register with the DirectInput subsystem and get a pointer
     *  to a IDirectInput interface we can use.
     *
     *  Parameters:
     *
     *      g_hinst
     *
     *          Instance handle to our application or DLL.
     *
     *      DIRECTINPUT_VERSION
     *
     *          The version of DirectInput we were designed for.
     *          We take the value from the <dinput.h> header file.
     *
     *      &g_pdi
     *
     *          Receives pointer to the IDirectInput interface
     *          that was created.
     *
     *      NULL
     *
     *          We do not use OLE aggregation, so this parameter
     *          must be NULL.
     *
     */
    hr = DirectInput8Create(g_hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pdi, NULL);
    if (FAILED(hr))
	{
        Complain(hwnd, hr, "DirectInputCreate");
        return FALSE;
    }

//	hr = g_pdi->CreateDevice(GUID_SysKeyboard, &g_pKeyboard, NULL); 
//	if FAILED(hr) 
//	{ 
//		DI_Term(); 
//		return FALSE; 
//	} 
//
//	hr = g_pKeyboard->SetDataFormat(&c_dfDIKeyboard); 
//	if FAILED(hr)
//	{ 
//		DI_Term(); 
//		return FALSE; 
//	} 
//
//	// Set the cooperative level 
//	hr = g_pKeyboard->SetCooperativeLevel( hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
//	if FAILED(hr)
//	{ 
//		DI_Term(); 
//		return FALSE; 
//	} 
// 
//	hr = g_pKeyboard->Acquire();
//	if FAILED(hr) 
//	{ 
//		DI_Term(); 
//		return FALSE; 
//	} 

    /*
     *  Obtain an interface to the system mouse device.
     *
     *  Parameters:
     *
     *      GUID_SysMouse
     *
     *          The instance GUID for the device we wish to access.
     *          GUID_SysMouse is a predefined instance GUID that
     *          always refers to the system mouse device.
     *
     *      &g_pMouse
     *
     *          Receives pointer to the IDirectInputDevice interface
     *          that was created.
     *
     *      NULL
     *
     *          We do not use OLE aggregation, so this parameter
     *          must be NULL.
     *
     */
    hr = g_pdi->CreateDevice(GUID_SysMouse, &g_pMouse, NULL);
    if (FAILED(hr))
	{
        Complain(hwnd, hr, "CreateDevice");
        return FALSE;
    }

    /*
     *  Set the data format to "mouse format".
     *
     *  A data format specifies which controls on a device we
     *  are interested in, and how they should be reported.
     *
     *  This tells DirectInput that we will be passing a
     *  DIMOUSESTATE structure to IDirectInputDevice::GetDeviceState.
     *
     *  Parameters:
     *
     *      c_dfDIMouse
     *
     *          Predefined data format which describes
     *          a DIMOUSESTATE structure.
     */
    hr = g_pMouse->SetDataFormat(&c_dfDIMouse);

    if (FAILED(hr)) {
        Complain(hwnd, hr, "SetDataFormat");
        return FALSE;
    }


    /*
     *  Set the cooperativity level to let DirectInput know how
     *  this device should interact with the system and with other
     *  DirectInput applications.
     *
     *  Parameters:
     *
     *      DISCL_EXCLUSIVE
     *
     *          When the mouse is acquired, no other application
     *          will be able to acquire the mouse exclusively.
     *          Furthermore, the Windows mouse cursor will not move.
     *
     *      DISCL_FOREGROUND
     *
     *          If the user switches away from our application,
     *          automatically release the mouse back to the system.
     *
     */
    hr = g_pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr))
	{
        Complain(hwnd, hr, "SetCooperativeLevel");
        return FALSE;
    }

	hr = g_pMouse->Acquire();
    if (FAILED(hr))
	{
        Complain(hwnd, hr, "Acquire");
        return FALSE;
    }

    return TRUE;
}

/****************************************************************************
 *
 *      DITerm
 *
 *      Terminate our usage of DirectInput.
 *
 ****************************************************************************/

void
DITerm(void)
{

	/*
	 *	Destroy any lingering IDirectInputDevice object.
	 */
//	if (g_pKeyboard)
//	{
//		g_pKeyboard->Unacquire();
//
//		g_pKeyboard->Release();
//		g_pKeyboard = NULL;
//	}

    if (g_pMouse)
	{

        /*
         *  Cleanliness is next to godliness.  Unacquire the device
         *  one last time just in case we got really confused and tried
         *  to exit while the device is still acquired.
         */
        g_pMouse->Unacquire();

        g_pMouse->Release();
        g_pMouse = NULL;
    }

    /*
     *  Destroy any lingering IDirectInput object.
     */
    if (g_pdi)
	{
        g_pdi->Release();
        g_pdi = NULL;
    }

}

// Verifica el estado individual de las teclas y lo pone en ungetch

void WINAPI ProcessKBInput() 
{ 
    #define KEYDOWN(name,key) (name[key] & 0x80) 
 
    char     buffer[256]; 
    HRESULT  hr; 
	HRESULT  hr2;
 
	do
	{
		hr = g_pKeyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);
		if FAILED(hr) 
		{ 
			if (hr == DIERR_INPUTLOST)
			{
		        hr2 = g_pMouse->Acquire();
				if (FAILED(hr2))
				{
					return;
				}
			}
		} 
	} while( FAILED(hr) );
 
    // Si Control está apretado
    if (KEYDOWN(buffer, DIK_LCONTROL) || KEYDOWN(buffer, DIK_RCONTROL) )
	{
		// Codigo para que control este apretado
	}

	// Delirio de interpretar si CADA tecla está apretada!
	// No hay timeout para saber cuantas veces se apreto la tecla!
} 

/****************************************************************************
 Ex_OneFrame - Devuelve las variaciones de x e y, y los botones apretados 
 ****************************************************************************/

void Ex_OneFrame(int *deltax, int *deltay, int *boton1, int *boton2)
{

    if (g_pMouse) {

        DIMOUSESTATE dims;          /* DirectInput mouse state structure */
        HRESULT hr;

    again:;
	
        hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
        if (hr == DIERR_INPUTLOST)
		{
            /*
             *  DirectInput is telling us that the input stream has
             *  been interrupted.  We aren't tracking any state
             *  between polls, so we don't have any special reset
             *  that needs to be done.  We just re-acquire and
             *  try again.
             */
            hr = g_pMouse->Acquire();
            if (SUCCEEDED(hr))
			{
                goto again;
            }
        }

        if (SUCCEEDED(hr))
		{
         *deltax=dims.lX;
	     *deltay=dims.lY;
	//   *deltaz=dims.lZ;
	     *boton1=(dims.rgbButtons[0] & 0x80);
	     *boton2=(dims.rgbButtons[1] & 0x80);
        }
    }
}

/****************************************************************************
 *
 *      Ex_SyncAcquire
 *
 *      Acquire or unacquire the mouse, depending on the the g_fPaused
 *      flag.  This synchronizes the device with our internal view of
 *      the world.
 *
 ****************************************************************************/

void
Ex_SyncAcquire(HWND hwnd)
{
    if (g_fPaused) {
        if (g_pMouse) g_pMouse->Unacquire();
    } else {
        if (g_pMouse) g_pMouse->Acquire();
    }
}

/*-------------- Funciones Publicas ---------- */

/* Devuelve el estado del mouse (posicion y botones apretados) */

void
MouseEvent(void)
{
	int deltax, deltay, boton1, boton2;

	g_oldMouseButton=g_MouseButton;
	g_oldMouseX=g_MouseX;
	g_oldMouseY=g_MouseY;

	Ex_OneFrame(&deltax, &deltay, &boton1, &boton2);

	g_MouseX+=deltax;
	g_MouseY+=deltay;

	g_MouseButton=(boton1/128|(boton2/64));

	if( g_MouseX<0 ) g_MouseX=0;
	if( g_MouseY<0 ) g_MouseY=0;
	if( g_MouseX>=PhysicalWidth ) g_MouseX=PhysicalWidth-1;
	if( g_MouseY>=PhysicalHeight ) g_MouseY=PhysicalHeight-1;
}

void
DisplayMouse (blitbuf *buf, unsigned short x, unsigned short y)
{
	ShiftMX=g_MouseX-x;
	ShiftMY=g_MouseY-y;
	vs2blitbuf( g_MouseBackground, x, y);
	t_blitbuf( buf, x, y);
}

