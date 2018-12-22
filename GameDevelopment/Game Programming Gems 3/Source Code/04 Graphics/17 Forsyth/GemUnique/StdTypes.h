
#ifndef STDTYPES_INCLUDED
#define STDTYPES_INCLUDED

#pragma once



// Some standard sizes.
// Define these appropriately for each platform.
// The number is the number of bits of course.
typedef unsigned char		ui8;
typedef signed char			si8;
typedef signed char			 i8;
typedef unsigned short		ui16;
typedef signed short		si16;
typedef signed short		 i16;
typedef unsigned int		ui32;
typedef signed int			si32;
typedef signed int			 i32;
typedef unsigned __int64	ui64;
typedef signed __int64		si64;
typedef signed __int64		 i64;
typedef float				 f32;
typedef double				 f64;
// Some compilers treat "bool" as chars, others as ints, but we always
// treat them as ints in our heads...
typedef int					Bool;



#endif //#ifndef STDTYPES_INCLUDED

