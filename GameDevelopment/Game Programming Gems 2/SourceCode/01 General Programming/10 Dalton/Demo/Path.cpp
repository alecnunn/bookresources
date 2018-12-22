/***
 * File:   Path.cpp - Implements Path.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/20/2001 10:24:56 AM
 *
 * Description:
			This class provides a linked list structure for holding possible moves.  
 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

#include <assert.h>

#include "MemoryManager.h"

#include "Board.h"
#include "Path.h"

// For optimal performance turn all flags on except for USE_DYNAMIC_MEMORY

#define USE_MEMORY_CACHE   1     // 1 - Enables Memory Caching  0 - Disables Memory Caching
#define CONSERVE_MEMORY    1     // 1 - Enables                 0 - Disables
#define USE_DYNAMIC_MEMORY 0     // 1 - Enables                 0 - Disables

/*******************************************************************************************/
// ***** PathNode -> A node for creating linked lists.
struct PathNode
{
#if USE_DYNAMIC_MEMORY
	pathValue *m_path;
	char       m_size;
#else
#if CONSERVE_MEMORY
	pathValue m_path[5];
#else
	pathValue m_path[MAX_MOVES];
#endif
#endif

	PathNode *next, *prev;

	PathNode( const pathValue path[] )
	{
#if USE_DYNAMIC_MEMORY
		m_size = path[0];
		m_path = new pathValue[m_size];
#endif
		for (int ii = 0; ii <= path[0]; ++ii) {
			m_path[ii] = path[ii];
		}
	}
	~PathNode( void ) 
	{
#if USE_DYNAMIC_MEMORY
		delete[] m_path;
#endif
	}
	void InitalizePath( const pathValue path[] ) 
	{
#if USE_DYNAMIC_MEMORY
		if (path[0] > m_size) {
			delete[] m_path;
			m_size = path[0];
			m_path = new pathValue[m_size];
		}
#endif
		for (int ii = 0; ii <= path[0]; ++ii) {
			m_path[ii] = path[ii];
		}
	}
};

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation of the PathList Class

PathNode *PathList::s_pathNodeCache = NULL;      // Static Member Variable

/**
 * PathList::PathList():
 *  Default Constructor.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
PathList::PathList( void ) : m_PathList(NULL), m_PathTail(NULL), m_pathLength(0)
{}

/*******************************************************************************************/

/**
 * PathList::PathList():
 *  Copy Constructor.
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	const PathList& p	: The PathList object to be copied.
 */
PathList::PathList( const PathList& p )
{
	PathNode *ptr = p.m_PathList;
	while (ptr) {
		appendPath( ptr->m_path );
		ptr = ptr->next;
	}
}

/*******************************************************************************************/

/**
 * PathList::~PathList():
 *  Destructor, free dynamically allocated memory.
 * 
 *  Return Type : 
 *  Arguments   : 
 */
PathList::~PathList( void )
{
	while (m_PathList) {
		removePath( 0 );
	}
}

/*******************************************************************************************/

/**
 * PathList::releaseCacheMemory():
 *  This method is responsible for releasing all cached memory, this method must be explicitly
 *  called upon shut down.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void PathList::releaseCacheMemory( void )
{
	PathNode *ptr;
	while (s_pathNodeCache) {
		ptr = s_pathNodeCache;
		s_pathNodeCache = s_pathNodeCache->next;
		delete ptr;
	}	
}
/*******************************************************************************************/

/**
 * PathList::getPath():
 *  Given a valid index this method will return the cooresponding path.  If an invalid 
 *  index is provided a NULL pointer is returned.
 * 
 *  Return Type : pathValue* -> A pointer to the path.
 *  Arguments   : 
 *  	int index	: The index of the path to be returned.
 */
pathValue* PathList::getPath( int index )
{
	if (index >= m_pathLength || index < 0) return NULL;

	PathNode *ptr = m_PathList;
	for (int ii = 0; ii < index; ++ii) {
		ptr = ptr->next;
	}
	return ptr->m_path;
}

/*******************************************************************************************/

/**
 * PathList::getPath():
 *  This method returns a hard copy of the desired path.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int index	      : The desired path.
 *  	pathValue *path	: A buffer to hold the path.
 */
void PathList::getPath( int index, pathValue *path )
{
	pathValue *t = getPath( index );
	if (t && path) {
		for (int ii = 0; ii <= t[0]; ++ii) {
			path[ii] = t[ii];
		}
	}
}

/*******************************************************************************************/

/**
 * PathList::appendPath():
 *  Given a path this method will append it to the end of the path list.  
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	const pathValue path[] : The path to be added to the end of the list.
 */
void PathList::appendPath( const pathValue path[] )
{
	PathNode *newNode;

#if USE_MEMORY_CACHE
	if (s_pathNodeCache) {
		newNode = s_pathNodeCache;
		s_pathNodeCache = s_pathNodeCache->next;
		newNode->InitalizePath( path );
	}
	else {
		newNode = new PathNode( path );
	}
#else
	newNode = new PathNode( path );
#endif

	newNode->next     = NULL;
	newNode->prev     = m_PathTail;

	if (!m_PathTail) {
		m_PathList = m_PathTail = newNode;
	}
	else {
		m_PathTail->next = newNode;
		m_PathTail       = newNode;
	}
	m_pathLength++;
}

/*******************************************************************************************/

/**
 * PathList::removePath():
 *  Given a position into the path list this method will remove the path.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int position : The position of the path to be removed.
 */
void PathList::removePath( int position )
{
	if( position < m_pathLength) {
		PathNode *ptr = m_PathList;
		for (int ii = 1; ii < position; ++ii) {
			ptr = ptr->next;
		}
		
		if (ptr->next) ptr->next->prev = ptr->prev;
		else           m_PathTail = ptr->prev;

		if (ptr->prev) ptr->prev->next = ptr->next;
		else           m_PathList = ptr->next;
		
#if USE_MEMORY_CACHE
		ptr->next = s_pathNodeCache;
		s_pathNodeCache = ptr;
#else
		delete ptr;
#endif

		m_pathLength--;
	}
}

/*******************************************************************************************/

/**
 * PathList::sortListAccending():
 *  Sorts the linked list in accending order.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int weights[]	: The value associated with each of the list items.
 */
void PathList::sortListAccending( int weights[] )
{
}

/*******************************************************************************************/

/**
 * PathList::sortListDecending():
 *  Sorts the linked list in decending order.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int weights[]	: The value associated with each of the list items.
 */
void PathList::sortListDecending( int weights[] )
{
}

/*******************************************************************************************/

/**
 * PathList::pathsEqual():
 *  Returns TRUE is the two paths are equal, otherwise FALSE.
 * 
 *  Return Type : bool 
 *  Arguments   : 
 *  	const pathValue path[] : The path to be compared.
 *  	int position	         : The position of the path within the list to be compared.
 */
bool PathList::pathsEqual( const pathValue path[], int position )
{
	pathValue *curPath = getPath( position );
	
	if (!curPath || curPath[0] != path[0]) return false;

	for (int ii = 1; ii <= curPath[0]; ++ii) {
		if (curPath[ii] != path[ii]) return false;
	}
	return true;
}

/*******************************************************************************************/

/**
 * PathList::clearPaths():
 *  Cleans up the list by removing all entries.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void PathList::clearPaths( void )
{
	while (m_PathList) {
		removePath( 0 );
	}
}

// ***** End of Path.cpp
/*******************************************************************************************/
/*******************************************************************************************/