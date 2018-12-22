/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
#include <assert.h>

BaseClass* ClassFactory::CreateObject(int id)
{
	BaseClass* pClass = 0;
	switch(id)
	{
	case 1:
		pClass = new Class1;
		break;
	case 2:
		pClass = new Class2;
		break;
	case 3:
		pClass = new Class3;
		break;
	default:
		assert(!"Error! Invalid class ID passed to factory!");
	};

	// perhaps perform some common initialization is needed
	pClass->Init();

	return pClass;
}

