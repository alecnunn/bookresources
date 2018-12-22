/* Copyright (C) Scott Wakeling, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Wakeling, 2001"
 */

#ifndef __CROOTCLASS_H
#define __CROOTCLASS_H

#include "dtiClass.h"
#include "CdtiBin.h"

class CRootClass
{

public:
	
	EXPOSE_TYPE;
	
	CRootClass() { iMemberInt = 0; };
	virtual ~CRootClass() {};

	virtual void Serialize( CdtiBin& ObjStore );

	void* SafeCast( dtiClass* pCastToType );

	int iMemberInt;
};

void CRootClass::Serialize( CdtiBin& ObjStore )
{
	ObjStore << iMemberInt;
}

void* CRootClass::SafeCast( dtiClass* pCastToType )
{
	if ( Type.IsA( pCastToType ) )
	{
		return this;
	}
	else
	{
		return NULL;
	}
}

dtiClass CRootClass::Type( "CRootClass", NULL );

#endif