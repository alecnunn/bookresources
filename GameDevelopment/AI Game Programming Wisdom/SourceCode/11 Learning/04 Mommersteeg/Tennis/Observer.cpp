//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Observer.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "observer.h"

//----------------------------------------------------------------------------------------------
// RegisterObserver(): Registers a new observer with the subject
//----------------------------------------------------------------------------------------------

void CSubject::RegisterObserver(CObserver * pObserver) { 
	observers.Add(pObserver); 
}

//----------------------------------------------------------------------------------------------
// UnregisterObserver(): Unregisters an observer with the subject
//----------------------------------------------------------------------------------------------

void CSubject::UnregisterObserver(CObserver * pObserver) {	
	observers.Remove(pObserver);
}

//----------------------------------------------------------------------------------------------
// NotifyAll(): Notifies all observers of a certain event
//----------------------------------------------------------------------------------------------

BOOL CSubject::NotifyAll(int msg, DWORD param) {
	int i = 0;
	BOOL proceed = TRUE;
	while (i<observers.GetSize() && proceed) {
		CObserver * obs = observers[i];
		proceed = obs->Notify(msg, param);
		i++;
	}
	return !proceed; // did some observer react?

	return FALSE;
}
