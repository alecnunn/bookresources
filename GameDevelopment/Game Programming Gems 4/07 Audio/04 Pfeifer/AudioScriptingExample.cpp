#include "stdafx.h"
#include <windows.h>
#include "audiomanager.h"
#include "resource.h"
#include "camera.h"

#include "musicoggvorbis.h"

#define MS_UPDATE_FREQ (100)

BOOL CALLBACK ExampleDlgFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool ExampleInit(HWND hWnd);
void ExamplePlayTag(HWND hWnd);

static Camera g_cam;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	INITCOMMONCONTROLSEX initctrl;

	CoInitialize(NULL);

	initctrl.dwSize = sizeof(INITCOMMONCONTROLSEX);
	initctrl.dwICC = ICC_UPDOWN_CLASS;
	InitCommonControlsEx(&initctrl);

	// run the example app dialog box
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_AUDIODIALOG), NULL, ExampleDlgFunc);

	CoUninitialize();

	return 0;
}

// Window handler for the example app - it handles the appropriate
// messages from the user
BOOL CALLBACK ExampleDlgFunc(HWND hWnd, UINT uMsg,
							 WPARAM wParam, LPARAM lParam)
{
	char txt[64];

	switch (uMsg)
	{
		case WM_INITDIALOG:
			if (!ExampleInit(hWnd))
			{
				MessageBox(NULL, "Could not load audio tag database.", "File Error", MB_OK);
				EndDialog(hWnd, 0);
			}
			return 1;

		case WM_DESTROY:
			delete AudioManager::Instance();
			break;

		case WM_TIMER:
			AudioManager::Instance()->Update(MS_UPDATE_FREQ);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;

				case IDC_PLAYTAG:
					ExamplePlayTag(hWnd);
					break;

				case IDC_STOPALL:
					AudioManager::Instance()->StopAll();
					break;

				case IDC_XPOS:
					GetDlgItemText(hWnd, IDC_XPOS, txt, 63);
					g_cam.GetPosition().x = atof(txt);
					break;

				case IDC_YPOS:
					GetDlgItemText(hWnd, IDC_YPOS, txt, 63);
					g_cam.GetPosition().y = atof(txt);
					break;

				case IDC_ZPOS:
					GetDlgItemText(hWnd, IDC_ZPOS, txt, 63);
					g_cam.GetPosition().z = atof(txt);
					break;
			}
			return 0;

		case WM_VSCROLL:  // only get vscroll from volume adjust trackbar 
		{
			int pos = SendDlgItemMessage(hWnd, IDC_VOLUME, TBM_GETPOS, 0, 0);
			AudioManager::Instance()->SetOverallVolume(-pos);
			break;
		}

		case WM_NOTIFY:
		{
			LPNMUPDOWN pnmud = (LPNMUPDOWN) lParam;
			switch (wParam)
			{
				case IDC_XSPIN:
					g_cam.GetPosition().x += -pnmud->iDelta;
					sprintf(txt, "%.2f", g_cam.GetPosition().x);
					SetDlgItemText(hWnd, IDC_XPOS, txt);
					break;

				case IDC_YSPIN:
					g_cam.GetPosition().y += -pnmud->iDelta;
					sprintf(txt, "%.2f", g_cam.GetPosition().y);
					SetDlgItemText(hWnd, IDC_YPOS, txt);
					break;

				case IDC_ZSPIN:
					g_cam.GetPosition().z += -pnmud->iDelta;
					sprintf(txt, "%.2f", g_cam.GetPosition().z);
					SetDlgItemText(hWnd, IDC_ZPOS, txt);
					break;
			}
		}
	}

	return 0;
}


// Initialize window and Audio Manager
bool ExampleInit(HWND hWnd)
{
	if (!AudioManager::Instance()->Init(hWnd) ||
		!AudioManager::Instance()->LoadAudioTags("audiodb.xml"))
	{
		return false;
	}

	int numTags = AudioManager::Instance()->GetNumAudioTags();
	HWND hCtrl = GetDlgItem(hWnd, IDC_TAGLIST);

	for (int i=0; i<numTags; ++i)
	{
		SendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)AudioManager::Instance()->GetAudioTagName(i));
	}

	hCtrl = GetDlgItem(hWnd, IDC_XSPIN);
	SendMessage(hCtrl, UDM_SETBASE, (WPARAM)10, 0);
	SendMessage(hCtrl, UDM_SETPOS, 0, 0);

	hCtrl = GetDlgItem(hWnd, IDC_YSPIN);
	SendMessage(hCtrl, UDM_SETBASE, (WPARAM)10, 0);
	SendMessage(hCtrl, UDM_SETPOS, 0, 0);

	hCtrl = GetDlgItem(hWnd, IDC_ZSPIN);
	SendMessage(hCtrl, UDM_SETBASE, (WPARAM)10, 0);
	SendMessage(hCtrl, UDM_SETPOS, 0, 0);

	SetDlgItemText(hWnd, IDC_XPOS, "0.00");
	SetDlgItemText(hWnd, IDC_YPOS, "0.00");
	SetDlgItemText(hWnd, IDC_ZPOS, "0.00");

	hCtrl = GetDlgItem(hWnd, IDC_VOLUME);
	SendMessage(hCtrl, TBM_SETRANGE, 0, (LPARAM)MAKELONG(DSBVOLUME_MAX, -(DSBVOLUME_MIN)));
	SendMessage(hCtrl, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(-(DSBVOLUME_MIN)/2));

	SetTimer(hWnd, 0, MS_UPDATE_FREQ, NULL);
	return true;
}


void ExamplePlayTag(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_TAGLIST);

	int index = SendMessage(hList, CB_GETCURSEL, 0, 0);
	int tagNameLen = SendMessage(hList, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	char* szTagName = new char[tagNameLen+1];
	SendMessage(hList, CB_GETLBTEXT, (WPARAM)index, (LPARAM)szTagName);

	AudioManager::Instance()->SetListenerCamera(&g_cam);

	AudioManager::Instance()->Play(szTagName, NULL, 0);

	delete [] szTagName;
}
