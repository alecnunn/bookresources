/* Copyright (C) Scott Wakeling, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Wakeling, 2001"
 */

#ifndef __CCHILDCLASS_H
#define __CCHILDCLASS_H

#include "dtiClass.h"

class CChildClass : public CRootClass
{

public:
	
	EXPOSE_TYPE;
	DECLARE_SUPER(CRootClass);

	CChildClass() { fMemberFloat = 0.0f; iAnotherInt = 0; };
	virtual ~CChildClass() {};

	virtual void Serialize( CdtiBin& ObjStore );

	float fMemberFloat;
	int iAnotherInt;

};

void CChildClass::Serialize( CdtiBin& ObjStore )
{
	Super::Serialize( ObjStore );
	
	ObjStore << fMemberFloat << iAnotherInt;
}

dtiClass CChildClass::Type( "CChildClass", &CRootClass::Type );

#endif