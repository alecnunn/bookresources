
#include <stdio.h>

#define _WIN32_WINNT 0x0500

#include <windows.h>

#include "CMessage.h"
#include "CObjectManager.h"
#include "Globals.h"
#include "CTextAdventureEngine.h"
#include "CTimer.h"
#include "types.h"
#include <conio.h>

CTextAdventureEngine::CTextAdventureEngine() :
mInputLength(0), mbGameRunning(false)
{
	mInputBuffer[0] = 0;
}

CTextAdventureEngine::~CTextAdventureEngine()
{
}

void CTextAdventureEngine::Init()
{
	Globals::Print("GDC Quest!\n\nWith only 4 days to go until the next GDC takes place, you are in desperate need of a conference pass. After asking your boss repeatidly to stump up the cash, you are left with no alternative but to steal his personal pass. Hey, he won't need it anyway; it's been years since he touched a line of code. Unfortunately, he's in the building today and the pass will be inside his office somewhere.\n");
	Globals::Print("DISCLAIMER: All locations and characters portrayed in this game are entirely fictional. We hope you see the funny side :-)\n\n");
	CObjectManager &objMan = Globals::GetObjectManager();
	objMan.Init();
	objMan.LoadObjectsFromFile("Data/objects.dat");
	objMan.BroadcastMessage(MT_ALL_OBJECTS_CREATED);
	Globals::GetTimer().Reset();
	printf(">");
	mHwnd = InitConsoleWindow("GDC Quest");
	mLastUpdateTick = Globals::GetTimer().GetSeconds();
	objMan.BroadcastMessage(MT_UPDATE);
}

void CTextAdventureEngine::Deinit()
{
	Globals::GetObjectManager().DestroyAllComponents();
	Globals::Print("Thanks for playing. Press space to exit.");
	char key;
	while(!GetKeyPress(key))
		;
}

void CTextAdventureEngine::EndGame()
{
	mbGameRunning = false;
}

void CTextAdventureEngine::RunGame()
{
	mbGameRunning = true;
	while (mbGameRunning)
	{
		if(mHwnd != GetForegroundWindow())
		{ // The world doesn't belong to us.
			Sleep(10);
			continue;
		}
		else
		{
			Sleep(10);
		}
		UpdateInput();
		if (Globals::GetTimer().GetSeconds() > mLastUpdateTick)
		{
			mLastUpdateTick = Globals::GetTimer().GetSeconds();
			Globals::GetObjectManager().BroadcastMessage(MT_UPDATE);
		}
	}
}

void CTextAdventureEngine::UpdateInput()
{
	char keyPress;
	if(GetKeyPress(keyPress))
	{
		switch(keyPress)
		{
			case '\n': // Return
			{
				// Handle carriage return and finish the buffer off with a NULL
				mInputBuffer[mInputLength] = 0;
				printf("\n");
				ProcessInputString();
				// Clear the input buffer
				mInputLength = 0;
				mInputBuffer[mInputLength] = 0;
				// Start a new line
				printf(">");
				break;
			}
			case '\b': // Backspace
			{
				// Handle backspace
				// Look away now..
				if (mInputLength > 0)
				{
					--mInputLength;
					HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
					CONSOLE_SCREEN_BUFFER_INFO csbi;
					GetConsoleScreenBufferInfo(consoleHandle, &csbi);
					int x = csbi.dwCursorPosition.X;
					int y = csbi.dwCursorPosition.Y;
					if(x > 1)
					{
						COORD pos = csbi.dwCursorPosition;
						pos.X -= 1;
						SetConsoleCursorPosition(consoleHandle, pos);
						printf(" ");
						SetConsoleCursorPosition(consoleHandle, pos);
					}
				}
				// OK, you can look again :)
				break;
			}
			default: // ascii character
			{
				// Add character onto our input buffer
				mInputBuffer[mInputLength] = keyPress;
				++mInputLength;
				printf("%c", keyPress);
				break;
			}
		}
	}
}

void CTextAdventureEngine::ProcessInputString()
{
	// Send input buffer to objects as a command message
	CObjectManager &objMan = Globals::GetObjectManager();
	CMessage msg;
	msg.mType = MT_COMMAND;
	msg.mpData = mInputBuffer;
	objMan.BroadcastMessage(msg);
}

bool CTextAdventureEngine::GetKeyPress(char &keyPressed)
{

	bool bShiftIsDown = false;
	if(((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0x8000))
	{
		bShiftIsDown = true;
	}

	// Check for key presses
	for(int i = 'A'; i <= 'Z'; ++i)
	{
		if((GetAsyncKeyState(i) & 0x1) == 1)
		{
			char k = i;
			if(!bShiftIsDown)
			{
				k += ('a' - 'A');
			}
			keyPressed = k;
			return true;
		}
	}

	if((GetAsyncKeyState(VK_SPACE) & 0x1) == 1)
	{
		keyPressed = ' ';
		return true;
	}

	if ((GetAsyncKeyState(VK_BACK) & 0x1) == 1)
	{
		keyPressed = '\b';
		return true;
	}

	if((GetAsyncKeyState(VK_RETURN) & 0x1) == 1)
	{
		keyPressed = '\n';
		return true;
	}

	return false;
}

HWND CTextAdventureEngine::InitConsoleWindow(const char* pTitleStr)
{	
	// Some rough code to get the console's HWND. Could use GetConsoleWindow() on Win2k and above
	char uniqueTitle[16], currentTitle[256];
	sprintf(uniqueTitle, "_%08X_", GetCurrentThreadId());
	SetConsoleTitle(uniqueTitle);
	const int maxNumIterations = 50;
	for(int i = 0; i<maxNumIterations; ++i)
	{
		Sleep(10); // Need this because console title doesn't change instantly.
		GetConsoleTitle(currentTitle, 256);
		if(strcmp(currentTitle, uniqueTitle) == 0)
			break; // Unique title has been set
	}
	HWND hwnd = FindWindow(NULL, uniqueTitle);
	// Set the correct console title
	SetConsoleTitle(pTitleStr);
	return hwnd;
}

void CTextAdventureEngine::ClearInputStringDisplay()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(consoleHandle, &csbi);
	COORD pos = csbi.dwCursorPosition;
	pos.X = 0;
	SetConsoleCursorPosition(consoleHandle, pos);
	for(int i = 0; i < mInputLength + 1; ++i)
	{
		printf(" ");
	}
	SetConsoleCursorPosition(consoleHandle, pos);
}

void CTextAdventureEngine::DisplayInputString()
{
	mInputBuffer[mInputLength] = 0;
	printf(">%s", mInputBuffer);
}
