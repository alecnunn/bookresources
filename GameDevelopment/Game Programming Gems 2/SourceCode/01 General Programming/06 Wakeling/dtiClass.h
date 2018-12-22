/* Copyright (C) Scott Wakeling, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Wakeling, 2001"
 */

#ifndef __DTICLASS_H
#define __DTICLASS_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

class dtiClass
{

private:
	
	char* szName;
	dtiClass* pdtiParent;
	
public:
	
	dtiClass();
	dtiClass( char* szSetName, dtiClass* pSetParent );
	virtual ~dtiClass();

	const char* GetName();
	bool SetName( char* szSetName );
	
	dtiClass* GetParent();
	bool SetParent( dtiClass* pSetParent );

	bool IsA( dtiClass* pType );
};

#define EXPOSE_TYPE \
	public: \
		static dtiClass Type;

#define DECLARE_SUPER(SuperClass) \
	public: \
		typedef SuperClass Super;
		
#define SAFE_CAST(pObj,castto) \
	( pObj ? (castto*)pObj->SafeCast(&castto::Type) : 0 )

#endif // __DTICLASS_H