/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
//----------------------------------
// stat.cpp
// Realtime Statistics Display classes
//----------------------------------


// Possible extensions:
// Multiple output formats (binary, octal, hex, decimal, ascii)
// Dynamic deletion of stats as they go out of scope.

// This is the MFC precompied header thing. Uncomment if using MFC.
//#include "stdafx.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <string.h>
#endif
#include "stat.h"

// Declare statics from CStatBase
CStatPage	*CStatBase::_currentPage;
CStatPage	*CStatBase::_firstPage;
int			CStatBase::_maxViewedLines;
bool		CStatBase::_statsVisible;
bool		CStatBase::_statsEditable;
char		CStatBase::_entryBuffer[LABEL_MAX]; // used for editing a value.
int			CStatBase::_frameAccumulator;
bool		CStatBase::_debounceOnOff = FALSE; // debounce on/off
bool		CStatBase::_debounceEditView = FALSE; // debounce edit/view
//----------------------------------
// entry class
//----------------------------------

// truncate if necessary, set pri, insert into specified list.
CStatEntry::CStatEntry(CStatPage *page, char *label, int priority)
{
	// This truncates the label to the right size if it is too big.
	SetLabel(label);
	_priority = priority;
	_previousEntry = NULL;
	_nextEntry = NULL;

	page->AddEntry(this);
}

CStatEntry::~CStatEntry(void)
{
	// Allowing for the deletion of stat entries is messy, so don't do it.
}

char *CStatEntry::GetLabel()
{
	return(_label);
}

void CStatEntry::SetLabel(char *label)
{
	strncpy(_label,label,LABEL_MAX-1);
	_label[LABEL_MAX-1] = NULL;
}

int CStatEntry::GetPriority()
{
	return(_priority);
}

// The base CStatEntry class does not nave data, so it prints nothing at all.
int CStatEntry::Print(char *outputBuffer, int maxLength)
{
	return 0; // return #lines printed.
}

void CStatEntry::DeltaValue(int delta)
{
	// We have no value to adjust.
}

// Set a stat with string data. Each type must convert the data to internal format.
void CStatEntry::SetValue(char *name)
{
	// We have no data for the base class.
}

//----------------------------------
// page class
//----------------------------------

CStatPage::CStatPage(char *label, int priority)
{
	SetLabel(label);
	_priority = priority;
	_topEntry = NULL;
	_currentEntry = NULL;
	_topViewedEntry = NULL;
	_previousPage = NULL;
	_nextPage = NULL;

	CStatBase::AddPage(this);
}

CStatPage::~CStatPage(void)
{
	// Allowing for the deletion of stat pages is messier than for entries, so don't do it.
}

char *CStatPage::GetLabel()
{
	return(_label);
}

void CStatPage::SetLabel(char *label)
{
	strncpy(_label,label,LABEL_MAX-1);
	_label[LABEL_MAX-1] = NULL;
}

int CStatPage::GetPriority()
{
	return(_priority);
}

CStatEntry *CStatPage::GetTopEntry(void)
{
	return _topEntry;
}

// Look through list and find proper parent. NULL if none.
void CStatPage::AddEntry(CStatEntry *entry)
{
	if(_topEntry == NULL)
	{
		// First entry on this page.
		_topEntry = _currentEntry = _topViewedEntry = entry;
		// Leave entry previous and next pointers set to NULL.
	}
	else if(_topEntry->_priority > entry->_priority)
	{
		// belongs at the head.
		_topEntry->_previousEntry = entry;
		entry->_nextEntry = _topEntry;
		// Leave previous pointer set to NULL.
	}
	else
	{
		// Find proper parent and insert it.
		CStatEntry *looper = _topEntry;
		bool done = FALSE;
		while(looper && !done)
		{
			if((looper->_priority > entry->_priority) || (looper->_nextEntry == NULL))
			{
				// Add after this entry.
				entry->_nextEntry = looper->_nextEntry;
				entry->_previousEntry = looper;
				// Adjust looper to point at entry.
				looper->_nextEntry = entry;
				// If one exists, fix the entry following the new one.
				if(entry->_nextEntry != NULL)
				{
					entry->_nextEntry->_previousEntry = entry;
				}
				done = TRUE;
			}
			looper = looper->_nextEntry;
		}
	}
}

// Start printing from top viewed item,
// and go until printed line count is met,
// or until no more entries.
void CStatPage::Print(char *outputBuffer, int maxViewedLines)
{
	// See how tall lines need to be by using a capital letter and a decender.
	// CSize lineSize = GetTextExtent("Ag",2); // Only works for CDC object.
	// Loop through entries, getting buffer data, formatting it and printing it with a shadow.
	CStatEntry *looper;
	int lineSum = 0;
	int lineCurrent = 0;
	char printBuffer[PRINT_BUFFER_LENGTH];

	// Print the page header.
	strcpy(outputBuffer,_label);
	strcat(outputBuffer,"\n");

	for(looper = _topEntry; looper != NULL && lineSum < maxViewedLines; looper = looper->_nextEntry)
	{
		strcat(outputBuffer, looper->GetLabel());
		for(int i = strlen(looper->GetLabel()); i < LABEL_MAX; i++)
		{
			strcat(outputBuffer," ");
		}

		// (light foreground on top of shadow)
		if(looper == _currentEntry && CStatBase::_statsEditable)
		{
			strcat(outputBuffer, ">");
		}
		else
		{
			strcat(outputBuffer, " ");
		}
		// Get the value so we can display it on the right border.
		lineCurrent = looper->Print(printBuffer, PRINT_BUFFER_LENGTH);
		if(lineCurrent)
		{
			strcat(outputBuffer,printBuffer);
			strcat(outputBuffer,"\n");

			lineSum += lineCurrent;
		}
		else
		{
			strcat(outputBuffer,"\n");
			lineSum++; // We printed a label with no data, so it still used a line.
		}
	}
}

//----------------------------------
// base class
//----------------------------------

void CStatBase::Initialize(void)
{
	_currentPage = NULL;
	_maxViewedLines = 20;
	_statsVisible = TRUE;
	_statsEditable = FALSE;
}

void CStatBase::CheckInput(unsigned char *keyboardState)
{
	// Check for on/off. (f12)
	if(keyboardState[VK_F12] && !_debounceOnOff)
	{
		_statsVisible = !_statsVisible;
		_debounceOnOff = TRUE;
	}
	if(!keyboardState[VK_F12])
	{
		_debounceOnOff = FALSE;
	}
	if(!keyboardState[VK_F11])
	{
		_debounceEditView = FALSE;
	}
	// Make sure we have something worth processing.
	if(_statsVisible)
	{
		int zoomFactor = 1;

		if(keyboardState[VK_SHIFT])
			zoomFactor *= 10;
		if(keyboardState[VK_CONTROL])
			zoomFactor *= 100;

		_frameAccumulator+= zoomFactor; // should actually scale _frameAccumulator based on time this frame

		// If no keys are down, set accumulator to 20 so next key hit is processed immediately.
		bool anyKeyHit = FALSE;
		for(int i = 0; i < 256; i++)
		{
			if(keyboardState[i])
			{
				anyKeyHit = TRUE;
				break;
			}
		}
		if(!anyKeyHit)
		{
			_frameAccumulator = 20;
			return;
		}
		if(_frameAccumulator < 20)
		{
			return;
		}
		// Okay, we're processing a key so reset the accumulator.
		_frameAccumulator = 0;

		// Check for view/edit. (f11)
		if(keyboardState[VK_F11] && !_debounceEditView)
		{
			_statsEditable = !_statsEditable;
			_debounceEditView = TRUE;
		}
		if(_statsEditable)
		{
			int iDelta = 0;
			if(0) // If not in keyboard data entry mode and <return>
			{
				// Put into keyboard data entry mode.
				// clear data entry buffer.
			}
			if(0) // in keyboard data entry mode
			{
				if(0) // return
				{
					// terminate keyboard data entry mode and assign value.
				}
				else
				{
					// append to edited buffer.
				}
			}
			// Check for incriment/decriment. (+,-)
			if(keyboardState[VK_SUBTRACT] && _currentPage && _currentPage->_currentEntry)
			{
				iDelta = - zoomFactor;
				_currentPage->_currentEntry->DeltaValue(iDelta);
			}
			if(keyboardState[VK_ADD] && _currentPage && _currentPage->_currentEntry)
			{
				iDelta = zoomFactor;
				_currentPage->_currentEntry->DeltaValue(iDelta);
			}
			// Check for page selection (page up, page down)
			if(keyboardState[VK_PRIOR] && _currentPage)
			{
				// Go to previous page.
				if(_currentPage->_previousPage)
					_currentPage = _currentPage->_previousPage;
			}
			if(keyboardState[VK_NEXT] && _currentPage)
			{
				// Go to next page.
				if(_currentPage->_nextPage)
					_currentPage = _currentPage->_nextPage;
			}
			// Check for entry selection (up, down)
			if(keyboardState[VK_NUMPAD8] || keyboardState[VK_UP])
			{
				// Move to previous entry on current page.
				if(_currentPage->_currentEntry && _currentPage->_currentEntry->_previousEntry)
					_currentPage->_currentEntry = _currentPage->_currentEntry->_previousEntry;
			}
			if(keyboardState[VK_NUMPAD2] || keyboardState[VK_DOWN])
			{
				// Move to next entry on current page.
				if(_currentPage->_currentEntry && _currentPage->_currentEntry->_nextEntry)
					_currentPage->_currentEntry = _currentPage->_currentEntry->_nextEntry;
			}
		}
	}
}

void CStatBase::AddPage(CStatPage *page)
{
	if(_firstPage == NULL)
	{
		// First entry on this page.
		_firstPage = _currentPage = page;
		// Leave entry previous and next pointers set to NULL.
	}
	else if(_firstPage->_priority > page->_priority)
	{
		// belongs at the head.
		_firstPage->_previousPage = page;
		page->_nextPage = _firstPage;
		// Leave previous pointer set to NULL.
	}
	else
	{
		// Find proper parent and insert it.
		CStatPage *looper = _firstPage;
		bool done = FALSE;
		while(looper && !done)
		{
			if((looper->_priority > page->_priority) || (looper->_nextPage == NULL))
			{
				// Add after this entry.
				page->_nextPage = looper->_nextPage;
				page->_previousPage = looper;
				// Adjust looper to point at entry.
				looper->_nextPage = page;
				// If one exists, fix the entry following the new one.
				if(page->_nextPage != NULL)
				{
					page->_nextPage->_previousPage = page;
				}
				done = TRUE;
			}
			looper = looper->_nextPage;
		}
	}
}

void CStatBase::Print(char *outputBuffer)
{
	if(_currentPage && _statsVisible)
	{
		_currentPage->Print(outputBuffer, _maxViewedLines);
	}
}

//----------------------------------
// Integer stat class.
//----------------------------------

CStatInt::CStatInt(CStatPage *page, char *title, int priority, int value)
: CStatEntry(page, title, priority)
{
	_value = value;
	_min = _max = 0;
	_rangeUsed = FALSE;
}

int CStatInt::Print(char *outputBuffer, int maxLength)
{
	sprintf(outputBuffer,"%d", _value);
	return 1; // We print one line of data into the buffer.
}

void CStatInt::SetValue(int value)
{
	_value = value;
}

void CStatInt::SetValue(char *name)
{
	_value = atoi(name);
}

int CStatInt::GetValue(void)
{
	return _value;
}

void CStatInt::SetRange(int min, int max)
{
	if(min > max)
	{
		_min = max;
		_max = min;
	}
	else
	{
		_min = min;
		_max = max;
	}
	_rangeUsed = TRUE;
	DeltaValue(0); // Force value into range.
}

void CStatInt::ClearRange(void)
{
	_rangeUsed = 0;
}

// Scale delta for non-integer variables.
void CStatInt::DeltaValue(int delta)
{
	_value += delta;
	if(_rangeUsed)
	{
		if(_value < _min)
		{
			_value = _min;
		}
		else if(_value > _max)
		{
			_value = _max;
		}
	}
}

//----------------------------------
// Integer pointer stat class.
//----------------------------------

CStatIntPtr::CStatIntPtr(CStatPage *page, char *title, int priority, int *value)
: CStatEntry(page, title, priority)
{
	_value = value;
	_min = _max = 0;
	_rangeUsed = FALSE;
}

int CStatIntPtr::Print(char *outputBuffer, int maxLength)
{
	sprintf(outputBuffer,"%d", *_value);
	return 1; // We print one line of data into the buffer.
}

void CStatIntPtr::SetValue(int *value)
{
	_value = value;
}

void CStatIntPtr::SetValue(char *name)
{
	*_value = atoi(name);
}

int *CStatIntPtr::GetValue(void)
{
	return _value;
}

void CStatIntPtr::SetRange(int min, int max)
{
	if(min > max)
	{
		_min = max;
		_max = min;
	}
	else
	{
		_min = min;
		_max = max;
	}
	_rangeUsed = TRUE;
	DeltaValue(0); // Force value into range.
}

void CStatIntPtr::ClearRange(void)
{
	_rangeUsed = 0;
}

// Scale delta for non-integer variables.
void CStatIntPtr::DeltaValue(int delta)
{
	*_value += delta;
	if(_rangeUsed)
	{
		if(*_value < _min)
		{
			*_value = _min;
		}
		else if(*_value > _max)
		{
			*_value = _max;
		}
	}
}

//----------------------------------
// Float stat class.
//----------------------------------

CStatFloat::CStatFloat(CStatPage *page, char *title, int priority, float value)
: CStatEntry(page, title, priority)
{
	_value = value;
	_min = _max = 0;
	_rangeUsed = FALSE;
}

int CStatFloat::Print(char *outputBuffer, int maxLength)
{
	sprintf(outputBuffer,"%.5f", _value);
	return 1; // We print one line of data into the buffer.
}

void CStatFloat::SetValue(float value)
{
	_value = value;
}

void CStatFloat::SetValue(char *name)
{
	_value = (float)atof(name);
}

float CStatFloat::GetValue(void)
{
	return _value;
}

void CStatFloat::SetRange(float min, float max)
{
	if(min > max)
	{
		_min = max;
		_max = min;
	}
	else
	{
		_min = min;
		_max = max;
	}
	_rangeUsed = TRUE;
	DeltaValue(0); // Force value into range.
}

void CStatFloat::ClearRange(void)
{
	_rangeUsed = 0;
}

// Scale delta for non-integer variables.
void CStatFloat::DeltaValue(int delta)
{
	_value += delta/1000.0f;
	if(_rangeUsed)
	{
		if(_value < _min)
		{
			_value = _min;
		}
		else if(_value > _max)
		{
			_value = _max;
		}
	}
}

//----------------------------------
// float pointer stat class.
//----------------------------------

CStatFloatPtr::CStatFloatPtr(CStatPage *page, char *title, int priority, float *value)
: CStatEntry(page, title, priority)
{
	_value = value;
	_min = _max = 0;
	_rangeUsed = FALSE;
}

int CStatFloatPtr::Print(char *outputBuffer, int maxLength)
{
	sprintf(outputBuffer,"%.5f", *_value);
	return 1; // We print one line of data into the buffer.
}

void CStatFloatPtr::SetValue(float *value)
{
	_value = value;
}

void CStatFloatPtr::SetValue(char *name)
{
	*_value = (float)atoi(name);
}

float *CStatFloatPtr::GetValue(void)
{
	return _value;
}

void CStatFloatPtr::SetRange(float min, float max)
{
	if(min > max)
	{
		_min = max;
		_max = min;
	}
	else
	{
		_min = min;
		_max = max;
	}
	_rangeUsed = TRUE;
	DeltaValue(0); // Force value into range.
}

void CStatFloatPtr::ClearRange(void)
{
	_rangeUsed = 0;
}

// Scale delta for non-integer variables.
void CStatFloatPtr::DeltaValue(int delta)
{
	*_value += delta/1000.0f;
	if(_rangeUsed)
	{
		if(*_value < _min)
		{
			*_value = _min;
		}
		else if(*_value > _max)
		{
			*_value = _max;
		}
	}
}

