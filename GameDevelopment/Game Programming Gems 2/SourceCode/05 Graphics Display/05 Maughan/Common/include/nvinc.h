/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:

	Stuff used by the other nv* components.

  cmaughan@nvidia.com
      
******************************************************************************/

#include <windows.h>
#pragma warning(disable: 4786)
#include <tchar.h>
#pragma warning(disable: 4786)
#include <iostream>
#pragma warning(disable: 4786)
#include <sstream>
#pragma warning(disable: 4786)
#include <iomanip>
#pragma warning(disable: 4786)
#include <math.h>
#pragma warning(disable: 4786)
#include <strstream>
#pragma warning(disable: 4786)
#include <fstream>
#pragma warning(disable: 4786)
#include <assert.h>
#pragma warning(disable: 4786)
#include <list>
#pragma warning(disable: 4786)
#include <vector>
#pragma warning(disable: 4786)
#include <iterator>
#pragma warning(disable: 4786)
#include <map>
#pragma warning(disable: 4786)
#include <string>
#pragma warning(disable: 4786)
#include <stack>
#pragma warning(disable: 4786)
#include <set>
#pragma warning(disable: 4786)
#include <assert.h>

#include <d3d8.h>
#include <d3dx8.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) \
	if ((a)) (a)->Release(); (a) = NULL;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) \
	if ((a)) delete(a); a = NULL;
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) \
	if ((a)) delete [] (a); a = NULL;
#endif

#define CHECK_D3DAPI(a)		\
{							\
	HRESULT hr = (a);		\
	if (hr != S_OK)			\
		assert(!#a);		\
}
