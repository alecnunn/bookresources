/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#ifndef STL_H

#define STL_H


/***********************************************************************/
/** STL.H       : Header file which includes all the common STL stuff. */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/

#pragma warning( disable:4786 )
#pragma warning(disable:4800) // int forced to bool


#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <iostream>
#include <algorithm>
#include <assert.h>

typedef std::string String;

typedef std::vector<String> StringVector;
typedef std::vector< StringVector > StringVectorVector;

typedef std::vector< int > IntVector;
typedef std::vector< char > CharVector;
typedef std::vector< short > ShortVector;
typedef std::vector< unsigned short > UShortVector;
typedef std::queue< int > IntQueue;


#endif
