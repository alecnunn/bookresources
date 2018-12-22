#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "Globals.h"
#include "CObjectManager.h"
#include "CTimer.h"
#include "CTextAdventureEngine.h"

// Some handy foreground and background console colours
enum EForegroundColour
{
	FCBlack		= 0,	
	FCDarkRed	= FOREGROUND_RED,
	FCDarkGreen = FOREGROUND_GREEN,
	FCDarkBlue	= FOREGROUND_BLUE,
	FCDarkCyan  = FOREGROUND_GREEN | FOREGROUND_BLUE,
	FCPurple	= FOREGROUND_RED | FOREGROUND_BLUE,
	FCBrown		= FOREGROUND_RED | FOREGROUND_GREEN,
	FCLightGrey	= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	FCDarkGrey	= FOREGROUND_INTENSITY,
	FCRed		= FOREGROUND_INTENSITY | FOREGROUND_RED,
	FCGreen		= FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	FCBlue		= FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	FCCyan		= FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	FCPink		= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	FCYellow	= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	FCWhite		= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};

enum EBackgroundColour
{
	BCBlack		= 0,	
	BCDarkRed	= BACKGROUND_RED,
	BCDarkGreen = BACKGROUND_GREEN,
	BCDarkBlue	= BACKGROUND_BLUE,
	BCDarkCyan  = BACKGROUND_GREEN | BACKGROUND_BLUE,
	BCPurple	= BACKGROUND_RED | BACKGROUND_BLUE,
	BCBrown		= BACKGROUND_RED | BACKGROUND_GREEN,
	BCLightGrey	= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	BCDarkGrey	= BACKGROUND_INTENSITY,
	BCRed		= BACKGROUND_INTENSITY | BACKGROUND_RED,
	BCGreen		= BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	BCBlue		= BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	BCCyan		= BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	BCPink		= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	BCYellow	= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	BCWhite		= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
};

// This is the colour table for the text types
uint32 TextColours[NUM_TEXT_TYPES] =
{
	FCWhite,			//TTInventoryHeading
	FCLightGrey,		//TTInventoryItem
	FCLightGrey,		//TTExitHeading
	FCGreen,			//TTExit
	FCLightGrey, 		//TTObjectHeading
	FCBlue,				//TTObject
	FCLightGrey,		//TTCharacterHeading
	FCPink, 			//TTCharacter
	FCDarkGreen,		//TTRoomDesc,
	FCGreen,			//TTResponse,
	FCRed,				//TTStateChange
	FCYellow,			//TTShortDesc
	FCBrown,			//TTGameOver
	FCDarkGreen,		//TTTellRoom
	FCLightGrey			//TTEvent
};

CObjectManager &Globals::GetObjectManager()
{
	static CObjectManager obj;
	return obj;
}

CTimer &Globals::GetTimer()
{
	static CTimer obj;
	return obj;
}

CTextAdventureEngine &Globals::GetTextAdventureEngine()
{
	static CTextAdventureEngine eng;
	return eng;
}

char* Globals::GetFirstToken(char *pString, const char *pDelimiter, char** ppStringRemainderOut)
{
	int stringLen = static_cast<int>(strlen(pString));
	if (stringLen == 0)
		return NULL;

	char* token = strtok(pString, pDelimiter);
	int tokenLen = static_cast<int>(strlen(token));
	if(stringLen > tokenLen + 1)
	{
		*ppStringRemainderOut = pString + tokenLen + 1;
	}
	else
	{
		*ppStringRemainderOut = pString;
	}
	return token;
}

bool Globals::SplitOnToken(char* pString, char* pToken, char** ppBefore, char** ppAfter)
{
	char* pTokenPos = strstr(pString, pToken);
	if(!pTokenPos)
		return false;
	
	*ppBefore = pString;
	*pTokenPos = 0; // terminate before string
	int tokenLen = static_cast<int>(strlen(pToken));
	*ppAfter = pTokenPos + tokenLen; // point to first char after token

	return true;
}

void PrintWrappedString(char* pStr, int consoleWidth)
{
	int lastSpacePos = 0;
	int numCharsOnLineSoFar = 0;
	int numCharsAfterSpace = 0;

	int strLength = static_cast<int>(strlen(pStr));
	for(int i = 0; i < strLength; ++i)
	{
		if(numCharsOnLineSoFar == consoleWidth - 1)
		{
			pStr[lastSpacePos] = '\n';
			numCharsOnLineSoFar = numCharsAfterSpace;
		}
		if(pStr[i] == ' ')
		{
			lastSpacePos = i;
			numCharsAfterSpace = 0;
		}
		++numCharsAfterSpace;
		++numCharsOnLineSoFar;
	}
	printf(pStr);
}

void Globals::Print(char* pStr, ...)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	int consoleWidth = csbi.dwSize.X;

	char pTempStr[1024];

	va_list args;
	va_start(args, pStr);
	vsprintf(pTempStr, pStr, args);

	PrintWrappedString(pTempStr, consoleWidth);
}

void Globals::PrintWithType(ETextType type, char* pStr, ...)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	int consoleWidth = csbi.dwSize.X;

	char pTempStr[1024];

	va_list args;
	va_start(args, pStr);
	vsprintf(pTempStr, pStr, args);

	SetConsoleTextAttribute(handle, TextColours[type]);
	PrintWrappedString(pTempStr, consoleWidth);
	SetConsoleTextAttribute(handle, FCLightGrey);
}







