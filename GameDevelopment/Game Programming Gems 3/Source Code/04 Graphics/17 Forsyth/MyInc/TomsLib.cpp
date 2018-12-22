// Some standard useful classes, templates, etc.

#include "TomsLib.h"




void RecursivelyCheck ( Tweak *ptweakRoot )
{
#ifdef _DEBUG
	ASSERT ( ptweakRoot->pchName != NULL );
	ASSERT ( ptweakRoot->type == Tweak::TweakType_Node );
	Tweak *ptweakCur = ptweakRoot->ptweakChild;
	Tweak *ptweakPrev = NULL;
	while ( ptweakCur != NULL )
	{
		ASSERT ( ptweakCur->pchName != NULL );
		ASSERT ( ptweakCur->ptweakParent == ptweakRoot );
		ASSERT ( 0 == strnicmp ( ptweakCur->pchName, ptweakRoot->pchName, strlen ( ptweakRoot->pchName ) ) );
		if ( ptweakPrev != NULL )
		{
			ASSERT ( 0 > stricmp ( ptweakPrev->pchName, ptweakCur->pchName ) );
		}
		if ( ptweakCur->type == Tweak::TweakType_Node )
		{
			RecursivelyCheck ( ptweakCur );
		}
		else
		{
			ASSERT ( ptweakCur->ptweakChild == NULL );
		}
		ptweakPrev = ptweakCur;
		ptweakCur = ptweakCur->ptweakNext;
	}


#endif
}

void RecursivelyInsert ( Tweak *ptweakRoot, Tweak *ptweakCur )
{
	// Paranoia.
	ASSERT ( ptweakRoot->pchName != NULL );
	ASSERT ( ptweakCur->pchName != NULL );
	ASSERT ( ptweakRoot->type == Tweak::TweakType_Node );
	ASSERT ( ptweakCur->type != Tweak::TweakType_Node );
	ASSERT ( ptweakCur->ptweakChild == NULL );
	ASSERT ( ptweakCur->ptweakParent == NULL );
	ASSERT ( ptweakCur->pchName != NULL );

	RecursivelyCheck ( ptweakRoot );

	// Better have the same root name.
	ASSERT ( 0 == strnicmp ( ptweakRoot->pchName, ptweakCur->pchName, strlen ( ptweakRoot->pchName ) ) );

	// Find the next root chunk.
	char pchChunkName[256];
	strcpy ( pchChunkName, ptweakRoot->pchName );
	char *pchDst = pchChunkName + strlen ( ptweakRoot->pchName );
	const char *pchSrc = ptweakCur->pchName + strlen ( ptweakRoot->pchName );
	while ( ( *pchSrc != '\0' ) && ( *pchSrc != '_' ) )
	{
		*pchDst++ = *pchSrc++;
	}
	*pchDst++ = '_';
	*pchDst++ = '\0';
	// Do any of the children already have that root?
	Tweak *ptweakChild = ptweakRoot->ptweakChild;
	Tweak *ptweakPrev = NULL;
	while ( ptweakChild != NULL )
	{
		if ( 0 == strnicmp ( ptweakChild->pchName, pchChunkName, strlen ( pchChunkName ) ) )
		{
			// Found it.
			break;
		}
		ptweakPrev = ptweakChild;
		ptweakChild = ptweakChild->ptweakNext;
	}
	if ( ptweakChild != NULL )
	{
		// Found a kid that shares the name.
		if ( ptweakChild->type == Tweak::TweakType_Node )
		{
			// It's a node already.
			ASSERT ( 0 == stricmp ( ptweakChild->pchName, pchChunkName ) );
			RecursivelyInsert ( ptweakChild, ptweakCur );
		}
		else
		{
			// Not a node, so create a common node.
			Tweak *ptweakNode = new Tweak;
			ptweakNode->ptweakParent = ptweakRoot;
			ptweakNode->ptweakNext = NULL;
			ptweakNode->ptweakChild = NULL;
			char *pchNewName = new char[strlen(pchChunkName)+1];
			strcpy ( pchNewName, pchChunkName );
			ptweakNode->pchName = pchNewName;
			ptweakNode->bChildrenShown = false;
			ptweakNode->type = Tweak::TweakType_Node;

			// Remove ptweakChild from the list and replace it by the new node.
			if ( ptweakPrev == NULL )
			{
				ASSERT ( ptweakRoot->ptweakChild == ptweakChild );
				ptweakRoot->ptweakChild = ptweakNode;
				ptweakNode->ptweakNext = ptweakChild->ptweakNext;
			}
			else
			{
				ASSERT ( ptweakPrev->ptweakNext == ptweakChild );
				ptweakPrev->ptweakNext = ptweakNode;
				ptweakNode->ptweakNext = ptweakChild->ptweakNext;
			}
			ptweakChild->ptweakNext = NULL;
			ptweakChild->ptweakParent = NULL;
			RecursivelyInsert ( ptweakNode, ptweakChild );
			RecursivelyInsert ( ptweakNode, ptweakCur );
		}
	}
	else
	{
		// No other kids that share this name, so just add it in the right place.
		Tweak *ptweakPrev = NULL;
		Tweak *ptweakChild = ptweakRoot->ptweakChild;
		ptweakCur->ptweakParent = ptweakRoot;
		while ( ptweakChild != NULL )
		{
			if ( 0 < stricmp ( ptweakChild->pchName, ptweakCur->pchName ) )
			{
				break;
			}
			ptweakPrev = ptweakChild;
			ptweakChild = ptweakChild->ptweakNext;
		}
		// Insert before this one then (still works if ptweakChild == NULL, i.e. we fell off the end).
		if ( ptweakPrev == NULL )
		{
			ptweakCur->ptweakNext = ptweakChild;
			ptweakRoot->ptweakChild = ptweakCur;
		}
		else
		{
			ptweakCur->ptweakNext = ptweakChild;
			ptweakPrev->ptweakNext = ptweakCur;
		}
	}

	RecursivelyCheck ( ptweakRoot );

}


void SortTweaksIntoAPrettyTree ( void )
{
	// Takes all the tweaks in the pUnsortedTweakRoot list
	// and adds them to the TweakRoot tree.
	while ( pUnsortedTweakRoot != NULL )
	{
		Tweak *ptweakCur = pUnsortedTweakRoot;
		pUnsortedTweakRoot = ptweakCur->ptweakNext;
		ptweakCur->ptweakNext = NULL;

		ASSERT ( ptweakCur->ptweakChild == NULL );
		ASSERT ( ptweakCur->ptweakParent == NULL );
		ASSERT ( ptweakCur->pchName != NULL );

		RecursivelyInsert ( &TweakRoot, ptweakCur );
	}

	ptweakCurrent = TweakRoot.ptweakChild;
}


void TweakNext ( void )
{
	Tweak *ptweakNext = ptweakCurrent->NextShown();
	if ( ptweakNext != NULL )
	{
		ptweakCurrent = ptweakNext;
	}
}

void TweakPrev ( void )
{
	// Previous one is a pain in the arse to find. Do it the brute-force way.
	Tweak *ptweakPrev = TweakRoot.ptweakChild;
	Tweak *ptweakCur = TweakRoot.ptweakChild;
	while ( ptweakCur != ptweakCurrent )
	{
		ptweakPrev = ptweakCur;
		ptweakCur = ptweakCur->NextShown();
		ASSERT ( ptweakCur != NULL );
	}
	ptweakCurrent = ptweakPrev;
	ASSERT ( ptweakCurrent != NULL );
}

void TweakOpen ( void )
{
	if ( ptweakCurrent->type == Tweak::TweakType_Node )
	{
		ptweakCurrent->bChildrenShown = true;
	}
}

void TweakClose ( void )
{
	if ( ptweakCurrent->type == Tweak::TweakType_Node )
	{
		ptweakCurrent->bChildrenShown = false;
	}
}

void TweakInc ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->Inc();
	}
}

void TweakDec ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->Dec();
	}
}

void TweakIncSlow ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->FineInc();
	}
}

void TweakDecSlow ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->FineDec();
	}
}

void TweakDefault ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->Default();
	}
}

void TweakNeg ( void )
{
	if ( ptweakCurrent->type != Tweak::TweakType_Node )
	{
		ptweakCurrent->Neg();
	}
}



// The root.
Tweak *pUnsortedTweakRoot = NULL;
// The root the tree will hang off.
Tweak TweakRoot;
// The currently selected tweak.
Tweak *ptweakCurrent = &TweakRoot;










