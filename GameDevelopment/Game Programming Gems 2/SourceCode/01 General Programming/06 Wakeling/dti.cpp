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

#include "CRootClass.h"
#include "CChildClass.h"

int main( int argc, char* argv[] )
{
	//-------------------------------------------------------------------
	// Create a new child class and root class for the examples below

	CRootClass* pRoot = new CRootClass();
	CChildClass* pChild = new CChildClass();
	
	// Demonstrate use of SafeCast method
	CRootClass* pRootTest;

	if ( pChild )
	{
		pRootTest = (CRootClass*)pChild->SafeCast( &CRootClass::Type );
	}
	
	// Create a new dtiBin to save some objects out to
	CdtiBin* pObjStore = new CdtiBin();
	pObjStore->Open( "gamesave01.dat" );

	pRoot->iMemberInt = 1;
	pChild->fMemberFloat = 2;
	pChild->iAnotherInt = 2;

	pRoot->Serialize( *pObjStore );
	pChild->Serialize( *pObjStore );

	pObjStore->Close();

	// Clear the member variables
	pRoot->iMemberInt = 0;
	pChild->fMemberFloat = 0;
	pChild->iAnotherInt = 0;

	// Serialize the object data back again

	pObjStore->IsLoading();
	pObjStore->Open( "gamesave01.dat" );

	pRoot->Serialize( *pObjStore );
	pChild->Serialize( *pObjStore );

	pObjStore->Close();

	// Note that pRoot and pChild now point to their original data
	//-------------------------------------------------------------------

	// Populate an array with varying types of object
	CRootClass** pGenericArray = new CRootClass*[10];
	
	srand( (unsigned)time( NULL ) );

	for ( int i = 0; i < 10; i++ )
	{
		switch ( rand() % 2 )
		{
		case 0:
			pGenericArray[i] = new CRootClass();
			break;
		case 1:
			pGenericArray[i] = new CChildClass();
			break;
		}
	}

	// Demonstrate use of dynamic_cast to show how we can tell what a root
	// class pointer is REALLY hiding! =)
	for ( i = 0; i < 10; i++ )
	{
		CChildClass* pChildPtr;

		pChildPtr = dynamic_cast<CChildClass*>( pGenericArray[i] );

		if ( pChildPtr )
		{
			// This pointer REALLY points to a CChildClass, so maybe we'd
			// like to perform some specialised routines here, safe in the
			// knowledge we're operating on the right type of object
			
			int iPutABreakpointHere = 10;
		}
		else
		{
			// This pointer really did just point to a root class, since it's
			// resolved to NULL after the dynamic cast
			
			int iPutABreakpointHere = 10;
		}
	}
	// Demonstrate bad exception from a failed dynamic_cast by reference
	CRootClass rootObj;
	try{
	CChildClass& childObj = dynamic_cast<CChildClass&>(rootObj);
	}catch(...)
	{
		// Catch the failed cast, thus, not a child object
		int iPutABreakPointHere = 10;
	}
    
	//-------------------------------------------------------------------
	// Cleanup
	for ( i = 0; i < 10; i++ )
	{
		delete pGenericArray[i];
	}

	delete[] pGenericArray;

	delete pChild;
	delete pRoot;
	//-------------------------------------------------------------------
	
	return 0;
}
