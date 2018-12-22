/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ globvar.cpp - represents global variable metadata			  		+
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ macros                                                            + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define	SZ_BYTE		1	/*used to indicate GlobVar type*/
#define SZ_WORD		2
#define SZ_DWORD	4
#define SZ_QWORD	8

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ decalaration                                                      + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *globSz[]={"","SZ_BYTE","SZ_WORD","","SZ_DWORD",
                "","","","SZ_QWORD"};

struct GlobalVariable
{
	U8 text;		/*index into StrTbl of where identifier begins*/
	U1 dType;		/* SZ_BYTE, SZ_WORD, SZ_DWORD, SZ_QWORD */
	U8 len;			/* # elements if array */
	U8 size;		/* total byte size */
	U8 offset;      /* offset below $TOP, address(g) = $TOP - offset*/
	U4 line;		/* source code line containing declaration */
};

