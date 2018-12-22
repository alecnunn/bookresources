/***
 * File:   Path.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/20/2001 10:25:01 AM
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

#ifndef _PATH_H__
#define _PATH_H__

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Declaration of the PathList Class

struct PathNode;
typedef char pathValue;

class PathList
{
public:
	// ***** Public Member Methods

	PathList( void );
	PathList( const PathList& p );
	~PathList( void );
	
	static void releaseCacheMemory();

	pathValue* getPath( int index );
	void getPath( int index, pathValue *path );

	inline int getLength() { return m_pathLength; }

	void appendPath( const pathValue path[] );
	void removePath( int position );

	void sortListAccending( int weights[] );
	void sortListDecending( int weights[] );

	bool pathsEqual( const pathValue path[], int position );

	void clearPaths( void );

private:

	// ***** Private Member Variables
	PathNode *m_PathList;
	PathNode *m_PathTail;
	int       m_pathLength;

	static PathNode *s_pathNodeCache;
};

#endif /* _PATH_H__ */

// ***** End of Path.h
/*******************************************************************************************/
/*******************************************************************************************/