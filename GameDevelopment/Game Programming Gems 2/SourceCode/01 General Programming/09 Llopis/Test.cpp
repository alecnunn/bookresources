/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
// Simple test program to test the deprecation manager.
// Run the program and look at the debugger output.
// A few things to note:
// - FunctionA() is called many times, but only from
//   three separate places (so that's what will be reported).
// - FunctionC() is deprecated but it's never called, so it
//   won't be reported at all.
// - Commenting out all the calls to deprecated functions will
//   stop the deprecation manager from being created/destroyed
//   so there won't be any messages when the program exits.


#include "DeprecationMgr.h"


int FunctionA ( void )
{
	DEPRECATE ( "FunctionA()", "NewFunctionA()" )
	return 0;
}


int FunctionB ( void )
{
	DEPRECATE ( "FunctionB()", "SomeOtherFunction()" )
	return 0;
}


int FunctionC ( void )
{
	DEPRECATE ( "FunctionC", "YetAnotherOne()" )
	return 0;
}



int main ()
{
	FunctionA();
	for (int i=0; i < 10; ++i )
	{
		FunctionA();
	}
	FunctionB();
	FunctionA();

	return 0;
}