/* Copyright (C) Scott Wakeling, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Wakeling, 2001"
 */

#include "dtiClass.h"

dtiClass::dtiClass()
{

}

dtiClass::dtiClass( char* szSetName, dtiClass* pSetParent )
{
	SetName( szSetName );
	
	SetParent( pSetParent );
}

dtiClass::~dtiClass()
{
	delete[] (void*)szName;
}

const char* dtiClass::GetName()
{
	return szName;
}

bool dtiClass::SetName( char* szSetName )
{
	if ( szSetName )
	{
		szName = new char[ strlen( szSetName ) + 1];
		
		if ( unsigned int(sprintf( szName, szSetName )) == strlen( szSetName ) )
			return true;
	}
	
	return false;
}

dtiClass* dtiClass::GetParent()
{
	return pdtiParent;
}

bool dtiClass::SetParent( dtiClass* pSetParent )
{
	pdtiParent = pSetParent;
	
	return false;
}

bool dtiClass::IsA( dtiClass* pType )
{
	dtiClass* pStartType = this;

	while( pStartType )
	{
		if ( pStartType == pType )
		{
			return true;
		}
		else
		{
			pStartType = pStartType->GetParent();
		}
	}

	return false;
}