#ifndef SGX_CORE_TYPES_DATATYPE_TYPEDEFS_HPP
#define SGX_CORE_TYPES_DATATYPE_TYPEDEFS_HPP	1

//
// Basic types
//
typedef unsigned char  tBYTE;
typedef tUINT32        tBOOL;

//
// Simile types
//
typedef tUINT32        tULONG;
typedef tINT32         tLONG;
typedef tUINT32        tULONG32;
typedef tINT32         tSLONG32;

//
// IEEE 754 Standard Types
//
typedef float          tREAL32;
typedef double         tREAL64;

//
// Limiting sizes
//
typedef size_t	tMEMSIZE;
typedef size_t  tDISC_SIZE;


// Some simple constants to replace the C++ types of 
// true and false.
// Most platforms define these too, so use ifndef 
// around each of them,
#ifdef TRUE
#undef TRUE
#endif
#define TRUE	    1


#ifdef FALSE
#undef FALSE
#endif
#define FALSE	    0

#ifdef NULL
#undef NULL
#endif
#define NULL        0


// The size of an enum can vary, so add this element to the end
// of any enumeration we use that's intended for saving to disc
#define SGX_ENUM_PADDING	0x7fffffff

#endif /* SGX_CORE_TYPES_DATATYPE_TYPEDEFS_HPP */
