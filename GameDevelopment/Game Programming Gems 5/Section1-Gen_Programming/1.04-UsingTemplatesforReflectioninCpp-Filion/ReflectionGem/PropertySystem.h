//
// CPropertySystem.h
//

#ifndef _PROPERTY_SYSTEM_H
#define _PROPERTY_SYSTEM_H

#include <list>
#include "Property.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPropertySystem
//
// Global manager of all properties. Used to release properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CPropertySystem
{

public :

	//----------------------------------------------------------------------------------------------
	// Constructor
	CPropertySystem();
	//----------------------------------------------------------------------------------------------
	// Destructor
	virtual ~CPropertySystem();

	//----------------------------------------------------------------------------------------------
	// Returns global list of properties.
	inline	static	list<CAbstractProperty*>*	GetProperties();

private :

	static	list<CAbstractProperty*>	ms_Properties;

};

inline list<CAbstractProperty*>* CPropertySystem::GetProperties()
{
	return &ms_Properties;
}

#endif	// _PROPERTY_SYSTEM_H