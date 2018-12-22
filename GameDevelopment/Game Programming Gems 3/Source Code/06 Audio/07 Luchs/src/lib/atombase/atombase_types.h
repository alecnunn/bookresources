
#ifndef _atombase_types_
#define _atombase_types_

// basic typedefs 

typedef signed char INT8; 
typedef unsigned char UINT8; 
typedef short INT16; 
typedef unsigned short UINT16; 

typedef int INT32; 
typedef unsigned int UINT32; 
typedef __int64 INT64; 

typedef unsigned short CHAR16; //wchar_t 
typedef char CHAR8;

typedef CHAR16* STR16;
typedef char* STR8;

typedef unsigned int	ATOM_ID;
typedef float			ATOM_FLOAT;
typedef float			FLOAT32;
typedef double			FLOAT64;

//typedef UINT32	OID; // objectid
//typedef UINT32	CID; // classid

typedef long ATOM_RESULT;

#ifndef TRUE
typedef int	BOOL;
#define TRUE	true
#define FALSE	false
#endif // TRUE

typedef short ATOM_BOOL;	// automation type bool is a short, -1 for true,0 for false
#define ATOM_TRUE	-1
#define ATOM_FALSE	0

#define ATOM_MAXPATH	260
typedef CHAR16	ATOM_NAME[ATOM_MAXPATH];

#ifndef TCHAR
typedef char	TCHAR;
#endif

typedef TCHAR	ATOM_CHAR;
typedef ATOM_CHAR ATOM_PATHNAME[ATOM_MAXPATH];

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


typedef struct atomRect { 
	INT32 left; 
	INT32 top; 
	INT32 right; 
	INT32 bottom; 
} ATOM_RECT; 



#endif //  _atombase_types_

