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
#ifndef __NVINC_H
#define __NVINC_H

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) \
	if ((a)) (a)->Release(); (a) = NULL;
#endif // SAFE_RELEASE

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) \
	if ((a)) delete(a); a = NULL;
#endif // SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) \
	if ((a)) delete [] (a); a = NULL;
#endif // SAFE_DELETE_ARRAY

#ifndef CHECK_D3DAPI
#define CHECK_D3DAPI(a)		\
{							\
	HRESULT hr = (a);		\
	if (hr != S_OK)			\
		assert(!#a);		\
}
#endif // CHECK_D3DAPI

#ifndef ASSERT_IF_FAILED
#define ASSERT_IF_FAILED( hres )	\
{									\
	if( FAILED(hres) )				\
	   assert( false );				\
}
#endif // ASSERT_IF_FAILED

#endif // __NVINC_H

