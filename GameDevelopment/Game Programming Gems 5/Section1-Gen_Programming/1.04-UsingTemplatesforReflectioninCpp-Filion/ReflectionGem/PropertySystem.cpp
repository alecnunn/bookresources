//
// PropertySystem.cpp
//
 
#include "stdafx.h"
#include "PropertySystem.h"

list<CAbstractProperty*> CPropertySystem::ms_Properties;

CPropertySystem::CPropertySystem()
{
}

CPropertySystem::~CPropertySystem()
{
	// Destroy properties
	for ( list<CAbstractProperty*>::iterator it = ms_Properties.begin(); it != ms_Properties.end(); ++it )
		delete *it;
	ms_Properties.clear();
}