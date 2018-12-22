#ifndef SGX_CORE_TYPES_PPLATFORM_HPP
#define SGX_CORE_TYPES_PPLATFORM_HPP

//
// These defines should be set in project files, so as to
// prevent changing this heading everytime you want to
// build for another platform.
#if !defined(SGX_PLATFORM_WINTEL32) && !defined(SGX_PLATFORM_PS2) && \
	!defined(SGX_PLATFORM_XBOX) && !defined(SGX_PLATFORM_GAMECUBE) && \
	!defined(SGX_PLATFORM_PS3) && !defined(SGX_PLATFORM_XBOX2)


#define SGX_PLATFORM_WINTEL32       1

#endif

//
// They are listed here as a reference for which macros
// have been used within the project
//
#if 0
#define SGX_PLATFORM_WINTEL32       1
#define SGX_PLATFORM_PS2            0
#define SGX_PLATFORM_XBOX           0
#define SGX_PLATFORM_GAMECUBE       0
#define SGX_PLATFORM_PS3            0
#define SGX_PLATFORM_XBOX2          0
#define SGX_PLATFORM_WII	        0
#endif
// 
// Sometimes it can help to create aliases for platform
// names. This is especially true when combining technology
// with different naming conventions.
//
#define SGX_PLATFORM_PLAYSTATION2	SGX_PLATFORM_PS2

//
// Include the appropriate platform-oriented datatypes
//
#if SGX_PLATFORM_WINTEL32
  #include "sgx/core/types/platform/wintel32.hpp"
#elif SGX_PLATFORM_PS2
  #include "sgx/core/types/platform/ps2.hpp"
#elif SGX_PLATFORM_XBOX
  #include "sgx/core/types/platform/xbox.hpp"
#elif SGX_PLATFORM_GAMECUBE
  #include "sgx/core/types/platform/gamecube.hpp"
#endif

#endif /* SGX_CORE_TYPES_PPLATFORM_HPP */
