#ifndef SGX_CORE_TYPES_PLATFORM_LINUX32
#define SGX_CORE_TYPES_PLATFORM_LINUX32

typedef signed char      tCHAR8;
typedef unsigned char    tUCHAR8;
typedef signed char      tINT8;
typedef unsigned char    tUINT8;
typedef signed short     tINT16;
typedef unsigned short   tUINT16;
typedef signed int       tINT32;
typedef unsigned int     tUINT32;
typedef __int64          tINT64;
typedef unsigned __int64 tUINT64;

// Support for the endian issues
#define SGX_ENDIAN_LITTLE	1
#define SGX_ENDIAN_BIG		0	

#endif	/* SGX_CORE_TYPES_PLATFORM_LINUX32 */

