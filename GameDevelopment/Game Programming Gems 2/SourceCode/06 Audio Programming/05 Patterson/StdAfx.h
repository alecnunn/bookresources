/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__58766E42_3049_11D5_B6CE_0001026727DB__INCLUDED_)
#define AFX_STDAFX_H__58766E42_3049_11D5_B6CE_0001026727DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include <list>
#include <vector>
#include <map>
#include <string>

#include "Def.h"

typedef std::string String_t;
typedef std::list<String_t> StringList_t;
typedef std::vector<int> IntVector_t;


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__58766E42_3049_11D5_B6CE_0001026727DB__INCLUDED_)
