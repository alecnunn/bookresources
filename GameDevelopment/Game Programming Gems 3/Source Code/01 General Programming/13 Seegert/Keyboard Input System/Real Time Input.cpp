#include "Real Time Input.h"
#include "InputHotkeyManager.h"
#include "InputSystem.h"


/* definitions */
#define kTimerInterval	10		/* the time, in ms, between keyboard updates.  adjust this to simulate lag and see the power of buffered input! */
#define kTimerID		1		/* the unique id of the timer */


/* global objects (kids, dont try this at home) */
InputHotkeyManager	gHotKeyManager;
InputSystem			gInputSystem;


/* declaration of the dialog box procedure */
BOOL CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);


/* 
 * WinMain
 *
 * The main function for the program.  Creates
 * the dialog box.
 */
int APIENTRY 
WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	string	theWorkingDir;
	char	theTempDir[_MAX_PATH];
	HWND	theDlgWindow;
	MSG		theMessage;	
	
	/* create the dialog box */
	theDlgWindow = ::CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProcedure);
	::ShowWindow(theDlgWindow, SW_SHOW);

	/* initialize the hotkey manager */
	gHotKeyManager.Initialize(hInstance);

	/* create all the hotkeys for the game, specifiying the 
	 * default key assignments and the events to fire. */
	gHotKeyManager.AddHotkey(	"GoToBarracks",	
								InputKey(DIK_B, kismsCtrl), 
								new IESampleInputEvent(theDlgWindow, IDC_STATIC1, "Go To Barracks - Just Pressed"/*"Go To Barracks - Just Down"*/),
								new IESampleInputEvent(theDlgWindow, IDC_STATIC1, "Go To Barracks - Pressed"),
								new IESampleInputEvent(theDlgWindow, IDC_STATIC1, "Go To Barracks - Just Released"));

	gHotKeyManager.AddHotkey(	"GoToCapitol",	
								DIK_H, 
								NULL,
								NULL,
								new IESampleInputEvent(theDlgWindow, IDC_STATIC1, "Go To Capitol"));
	
	gHotKeyManager.AddHotkey(	"GoToStable", 
								InputKey(DIK_S, kismsCtrl), 
								NULL,
								NULL,
								new IESampleInputEvent(theDlgWindow, IDC_STATIC1, "Go To Stable"));


	if (::_getcwd(theTempDir, _MAX_PATH) != NULL)
	{
		/* get the current working directory */
		theWorkingDir = theTempDir;

		/* we can write out the current hotkeys and assignments at any time */
		gHotKeyManager.WriteHotkeysToFile(theWorkingDir + "\\_default_keys.ini");

		/* we can read in new key assignments from an external ini file. */
		gHotKeyManager.ReadHotkeysFromFile(theWorkingDir + "\\config.ini");

		/* and we can write the new assignments out again (typically at the end of the game or
		 * when the user clicks OK after changing the keys in your game settings ui screen). */
		gHotKeyManager.WriteHotkeysToFile(theWorkingDir + "\\_new_keys.ini");

		/* specifying true for the second param will print "help" text to the ini file. */
		gHotKeyManager.WriteHotkeysToFile(theWorkingDir + "\\_new_keys_with_help.ini", true);
	}

	/* initializing the input system will grab the directinput interfaces we'll need.
	 * we are hooking the inputsystem to the dialog window. */
	gInputSystem.Initialize(hInstance, theDlgWindow);

	/* process the window messages */
	while (::GetMessage(&theMessage, NULL, NULL, NULL)) 
	{ 
		if (!::IsWindow(theDlgWindow) || !::IsDialogMessage(theDlgWindow, &theMessage)) 
		{ 
			::TranslateMessage(&theMessage); 
			::DispatchMessage(&theMessage); 
		} 
	} 

	return 0;
}


/* 
 * DialogProcedure
 *
 * This procedure handles the input for the dialog box.
 *
 */
BOOL CALLBACK 
DialogProcedure(HWND inDialogWindow, UINT inMessage, WPARAM wParam, LPARAM lParam)
{
    switch (inMessage)
    {
        case WM_INITDIALOG:
			
			/* create the timer */
			::SetTimer(inDialogWindow, kTimerID, kTimerInterval, NULL);
            
			return true; 

		case WM_TIMER:
			
			/* refresh the input system, getting the key
			 * presses since the last call. */
			gInputSystem.Refresh();

			/* now, fire off any hotkey events associated
			 * with the keys pressed. */
			gHotKeyManager.FireEvents(gInputSystem.GetJustPressedKeys(), gInputSystem.GetPressedKeys(), gInputSystem.GetJustUnPressedKeys());

			/* here we are just printing some debug text so you can
			 * see what the input system is doing as it runs. */
			char theDebugText[100];
			::sprintf(theDebugText, "%d - %d - %d", gInputSystem.GetJustPressedKeys().size(), gInputSystem.GetPressedKeys().size(), gInputSystem.GetJustUnPressedKeys().size());
			::SetDlgItemText(inDialogWindow, IDC_STATIC2, theDebugText);
			
			return false;

		case WM_CLOSE:
			
			/* quit */
			::KillTimer(inDialogWindow, kTimerID);
			::DestroyWindow(inDialogWindow);
			::PostQuitMessage(0);
			
			return false;
    }

    return false;
}