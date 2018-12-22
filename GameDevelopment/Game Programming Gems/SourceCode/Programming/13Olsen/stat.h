/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
#ifndef STAT_H
#define STAT_H
//----------------------------------
// stat.h
// Realtime Statistics Display classes
//
// To use this class set:
// Call CStatBase::Initialize() at startup.
// Call CStatBase::CheckInput() with an array of what keys are down each frame.
// Call CStatBase::Print() with a buffer to be filled in with the stat text each frame.
// Call some function to put the text onto the screen during your render loop.
// 
// You will also probably want to use some #ifdefs to keep it from compiling stats
// into a release build.
//----------------------------------

#define LABEL_MAX 30
#define PRINT_BUFFER_LENGTH 1000

#ifdef _WIN32
#include <windows.h>  // for all the VK_ constants.
#endif
#include <stdio.h>    // for sprintf
//#include <string.h> // Use this if not using Windows.h for string stuff.

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Forward references.
class CStatPage;

class CStatEntry
{
public:
	CStatEntry(CStatPage *page, char *label, int priority); // truncate if necessary, set pri, insert into specified list.
	~CStatEntry(void);
	char *GetLabel();
	void SetLabel(char *label);
	int GetPriority(void);
	virtual int Print(char *outputBuffer, int maxLength); // return #lines printed.
	virtual void DeltaValue(int delta); // This should be scaled per derived type.
	virtual void SetValue(char *name);
	// CStatPage wants to use these.
	CStatEntry			*_nextEntry;
	CStatEntry			*_previousEntry;
	int					_priority;
	char				_label[LABEL_MAX];
protected:
};

class CStatPage
{
public:
	CStatPage(char *label, int priority);
	~CStatPage(void);
	char *GetLabel(void);
	void SetLabel(char *label);
	CStatEntry *GetTopEntry(void);
	int GetPriority(void);
	void AddEntry(CStatEntry *entry);
	void Print(char *outputBuffer, int maxViewedLines);
	// CStatBase wants to use these.
	CStatPage			*_nextPage;
	CStatPage			*_previousPage;
	CStatEntry			*_topEntry;
	CStatEntry			*_currentEntry;
	CStatEntry			*_topViewedEntry;
	int					_priority;
	char				_label[LABEL_MAX];
protected:
};

class CStatBase
{
public:
	// Don't bother with a constructor since it's a fully static class.
	static void Initialize(void);
	static void CheckInput(unsigned char *keyboardState);
	static void AddPage(CStatPage *page);
	static void Print(char *outputBuffer);
	// CStatPage wants to see some of these.
	static CStatPage	*_currentPage;
	static CStatPage	*_firstPage;
	static int			_maxViewedLines;
	static bool			_statsVisible;
	static bool			_statsEditable;
	static char			_entryBuffer[LABEL_MAX]; // used for editing a value.
	static int			_frameAccumulator;
	static bool			_debounceOnOff;
	static bool			_debounceEditView;
};

// Now for the actual numeric stats, derived from StatEntry.

class CStatInt : public CStatEntry
{
public:
	CStatInt(CStatPage *page, char *title, int priority, int value);
	int Print(char *outputBuffer, int maxLength);
	void SetValue(int value);
	void SetValue(char *name);
	int GetValue(void);
	void SetRange(int min, int max);
	void ClearRange(void);
	void DeltaValue(int delta); // This should be scaled per derived type.
protected:
	int _value;
	bool _rangeUsed;
	int _min;
	int _max;
};

class CStatIntPtr : public CStatEntry
{
public:
	CStatIntPtr(CStatPage *page, char *title, int priority, int *value);
	int Print(char *outputBuffer, int maxLength);
	void SetValue(int *value); // Set the pointer
	void SetValue(char *name); // Set the value (yea, it's a bit confusing)
	int *GetValue(void);
	void SetRange(int min, int max);
	void ClearRange(void);
	void DeltaValue(int delta); // This should be scaled per derived type.
protected:
	int *_value;
	bool _rangeUsed;
	int _min;
	int _max;
};

class CStatFloat : public CStatEntry
{
public:
	CStatFloat(CStatPage *page, char *title, int priority, float value);
	int Print(char *outputBuffer, int maxLength);
	void SetValue(float value);
	void SetValue(char *name);
	float GetValue(void);
	void SetRange(float min, float max);
	void ClearRange(void);
	void DeltaValue(int delta); // This should be scaled per derived type.
protected:
	float _value;
	bool _rangeUsed;
	float _min;
	float _max;
};

class CStatFloatPtr : public CStatEntry
{
public:
	CStatFloatPtr(CStatPage *page, char *title, int priority, float *value);
	int Print(char *outputBuffer, int maxLength);
	void SetValue(float *value); // Set the pointer
	void SetValue(char *name); // Set the value (yea, it's a bit confusing)
	float *GetValue(void);
	void SetRange(float min, float max);
	void ClearRange(void);
	void DeltaValue(int delta); // This should be scaled per derived type.
protected:
	float *_value;
	bool _rangeUsed;
	float _min;
	float _max;
};
#endif // STAT_H
