
#ifndef __CTEXTADVENTUREENGINE_H
#define __CTEXTADVENTUREENGINE_H

#include <windows.h>
#undef PostMessage

class CTextAdventureEngine
{
public:
	CTextAdventureEngine();
	~CTextAdventureEngine();

	void Init();
	void Deinit();
	void UpdateInput();
	void EndGame();
	
	void ClearInputStringDisplay();
	void DisplayInputString();

	void RunGame(); // Call this once. Runs until the game is over.
private:

	// This will mostly get the currently depressed key. Mostly.
	bool GetKeyPress(char &keyPressed);

	void ProcessInputString();

private:

	HWND InitConsoleWindow(const char* pTitleStr);

	char	mInputBuffer[MAX_STR_LEN];
	int		mInputLength;
	HWND	mHwnd;
	bool	mbGameRunning;
	uint32	mLastUpdateTick;
};

#endif // __CTEXTADVENTUREENGINE_H




