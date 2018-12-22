/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#include <NormalMap.h>


#if(DIRECT3D_VERSION >= 0x0800)

#pragma comment( lib, "d3dx8.lib" )

#else

#pragma comment( lib, "d3dx.lib" )

#endif

NormalMap NormalMap::smInstance;

