//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Observer.h
//----------------------------------------------------------------------------------------------

#ifndef __OBSERVER_H
#define __OBSERVER_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "array.h"

//----------------------------------------------------------------------------------------------
// Event list of messages sent by subjects
//----------------------------------------------------------------------------------------------

#define HIT_GROUND_EVENT	1
#define HIT_BALL_EVENT		2
#define BALL_ROLLING_EVENT	3

//----------------------------------------------------------------------------------------------
// CObserver: an observer can receive notifications from subjects
//----------------------------------------------------------------------------------------------

class CObserver {
public:
	virtual BOOL		Notify(int msg, DWORD param) = 0;
};

//----------------------------------------------------------------------------------------------
// CSubject: a subject maintains a list of registered observers, and notifies them of events
//----------------------------------------------------------------------------------------------

class CSubject {

public:
	void		RegisterObserver(CObserver * pObserver); 
	void		UnregisterObserver(CObserver * pObserver);
	BOOL		NotifyAll(int msg, DWORD param = 0);

protected:
	CArray <CObserver*>	observers;
};

//----------------------------------------------------------------------------------------------
#endif // __OBSERVER_H



