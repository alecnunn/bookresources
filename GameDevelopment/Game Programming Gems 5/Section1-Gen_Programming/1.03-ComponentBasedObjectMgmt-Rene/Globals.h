#ifndef __GLOBALS_H
#define __GLOBALS_H

class CObjectManager;
class CTimer;
class CTextAdventureEngine;

enum ETextType
{
	TTInventoryHeading = 0,
	TTInventoryItem,
	TTExitHeading,
	TTExit,
	TTObjectHeading,
	TTObject,
	TTCharacterHeading,
	TTCharacter,
	TTRoomDesc,
	TTResponse,
	TTStateChange,
	TTShortDesc,
	TTGameOver,
	TTTellRoom,
	TTEvent,
	NUM_TEXT_TYPES,
};

namespace Globals
{
	CObjectManager			&GetObjectManager();
	CTimer					&GetTimer();
	CTextAdventureEngine	&GetTextAdventureEngine();
	char*			GetFirstToken(char* pString, const char* pDelimiter, char** ppStringRemainderOut);
	bool			SplitOnToken(char* pString, char* pToken, char** ppBefore, char** ppAfter);
	void			Print(char* pStr, ...);
	void			PrintWithType(ETextType type, char* pStr, ...);
}

#endif //__GLOBALS_H