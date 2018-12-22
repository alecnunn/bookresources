/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  win32.c - this file has win32 API native code                    +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  macros                                                           +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define S1	signed char
#define S2	signed short
#define S4	signed long
#define S8	signed __int64	

#define U1	unsigned char
#define U2	unsigned short
#define U4	unsigned long
#define U8	unsigned __int64

/*
	Use ANSI-754 standard for single and double precision floats
	single precision = 4 bytes
	double precision = 8 bytes
*/

#define F4	float
#define F8	double

/* platform-specific I/O ------------------------------------------*/

#define PRINT_UREG(rstr,reg)	printf("%-6s=%-21I64u",rstr,reg)
#define PRINT_SREG(rstr,reg)	printf("%-6s=%-21I64d",rstr,reg)
#define PRINT_FREG(rstr,reg)	printf("%-6s=%g",rstr,(F4)reg)
#define PRINT_DREG(rstr,reg)	printf("%-6s=%g",rstr,(F8)reg)

#define pU1(arg)	printf("%u",arg)
#define pU2(arg)	printf("%hu",arg)
#define pU4(arg)	printf("%lu",arg)
#define pU8(arg)	printf("%I64lu",arg)

#define pS1(arg)	printf("%d",arg)
#define pS2(arg)	printf("%hd",arg)
#define pS4(arg)	printf("%ld",arg)
#define pS8(arg)	printf("%I64d",arg)

#define PRINT_MEM(index)	printf("RAM[%I64lu]=%u",index,RAM[index])

#define rU8(arg)	scanf("%I64lu",arg) 

#define fpU8(ptr,arg)	fprintf(ptr,"%I64lu",arg)
#define fpS8(ptr,arg)	fprintf(ptr,"%I64d",arg)

/* macros below used during reformating ---------------------------*/

/*
	reverse the order of bytes within an array 
*/

U1 fb[8];

#define FORMAT_WORD(arr,start)	{ fb[0]=arr[start+1];\
                                  fb[1]=arr[start];\
                                  arr[start]=fb[0];\
                                  arr[start+1]=fb[1]; }

#define FORMAT_DWORD(arr,start)	{ fb[0]=arr[start+3];\
                                  fb[1]=arr[start+2];\
								  fb[2]=arr[start+1];\
								  fb[3]=arr[start];\
                                  arr[start]=fb[0];\
                                  arr[start+1]=fb[1];\
								  arr[start+2]=fb[2];\
								  arr[start+3]=fb[3]; }

#define FORMAT_QWORD(arr,start){ fb[0]=arr[start+7];\
                                  fb[1]=arr[start+6];\
								  fb[2]=arr[start+5];\
								  fb[3]=arr[start+4];\
								  fb[4]=arr[start+3];\
								  fb[5]=arr[start+2];\
								  fb[6]=arr[start+1];\
								  fb[7]=arr[start];\
                                  arr[start]=fb[0];\
                                  arr[start+1]=fb[1];\
								  arr[start+2]=fb[2];\
								  arr[start+3]=fb[3];\
								  arr[start+4]=fb[4];\
								  arr[start+5]=fb[5];\
								  arr[start+6]=fb[6];\
								  arr[start+7]=fb[7]; }

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  public prototypes                                                +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void checkEndian();
void printBytes(U1 bytes[], int nbytes);
void testConversion();

U2 bytecodeToWord(U1 bytes[]);
U4 bytecodeToDWord(U1 bytes[]);
U8 bytecodeToQWord(U1 bytes[]);
F4 bytecodeToFloat(U1 bytes[]);
F8 bytecodeToDouble(U1 bytes[]);

void wordToBytecode(U2 word, U1 arr[]);
void dwordToBytecode(U4 dword, U1 arr[]);
void qwordToBytecode(U8 qword, U1 arr[]);
void floatToBytecode(F4 flt, U1 arr[]);
void doubleToBytecode(F8 dbl, U1 arr[]);

U4 getAvailableMemory();
U4 getFileSize(char *name);
void testNativeFunctions();

/* added for assembler */

U8 stringToU8(char *str);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	Note: bytecode format for numbers must be platform neutral
	      ... So we must agree on a convention ahead of time.

	Integers - Stored in big-endian ( high byte first ) order, 
	           AKA network order

			0xDEED1234  = 0xDE  0xED  0x12  0x34
						  [0]   [1]   [2]   [3]

	Floats - Stored using IEEE 754 format using big-endian order

			Intel floating-point values are little endian 

			[0 --deciaml part -- 22][23 --exponent-- 30] [31=sign]
			byte[0]                                        byte[4]

			[0 --decimal part -- 51][52 --exponent-- 62][63=sign]
			byte[0]                                        byte[7]

			For both, you'll need to convert to and from big-endian

	NOTA BENE: should convert values once upon loading application, 
	           to avoid redundant computation at runtime
*/

/*-----------------------------------------------------------------*/
/*
	This method checks the current platform's endianess
	big-endian		higher-order bytes first ( are at lower mem )
	little endian   lower-order bytes first ( are at lower mem )
	
	On Intel Hardware using NT 4.0 we get:
	value = 0xDEED1234
	machine is LITTLE endian - LOWER order bytes come first
	here are the bytes
	[0]=0x34 [1]=0x12 [2]=0xED [3]=0xDE
*/

void checkEndian()
{
	int i=0xDEED1234;
	int j;
	unsigned char *buff;

	printf("value = %lx\n",i);
	buff = (unsigned char*)&i;
	if(buff[0]==0x34)
	{ 
		printf("machine is LITTLE endian - LOWER order bytes come first"); 
	}
	else
	{ 
		printf("machine is BIG endian - HIGHER order bytes come first"); 
	}

	printf("\nhere are the 4 bytes\n");
	for(j=0;j<4;j++){ printf(" byte [%d]=%x ",j,buff[j]); }
	printf("\n");
	return;

}/*end checkEndian*/

/*-----------------------------------------------------------------*/

/*
	Routines below convert bytecode values to native Intel format
*/

U2 bytecodeToWord(U1 bytes[])
{
	U2 word;
	U1 *buffer;

	buffer = (U1*)&word;
	buffer[0] = bytes[1];
	buffer[1] = bytes[0];

	return(word);

}/*end bytecodeToWord*/

U4 bytecodeToDWord(U1 bytes[])
{
	U4 dword;
	U1 *buffer;

	buffer = (U1*)&dword;
	buffer[0] = bytes[3];
	buffer[1] = bytes[2];
	buffer[2] = bytes[1];
	buffer[3] = bytes[0];

	return(dword);

}/*end bytecodeToDWord*/

U8 bytecodeToQWord(U1 bytes[])
{
	U8 qword;
	U1 *buffer;

	buffer = (U1*)&qword;
	buffer[0] = bytes[7];
	buffer[1] = bytes[6];
	buffer[2] = bytes[5];
	buffer[3] = bytes[4];
	buffer[4] = bytes[3];
	buffer[5] = bytes[2];
	buffer[6] = bytes[1];
	buffer[7] = bytes[0];

	return(qword);

}/*end bytecodeToQWord*/

F4 bytecodeToFloat(U1 bytes[])
{
	F4 flt;
	U1 *buffer;

	buffer = (U1*)&flt;
	buffer[0] = bytes[3];
	buffer[1] = bytes[2];
	buffer[2] = bytes[1];
	buffer[3] = bytes[0];

	return(flt);

}/*end bytecodeToFloat*/

F8 bytecodeToDouble(U1 bytes[])
{
	F8 dbl;
	U1 *buffer;

	buffer = (U1*)&dbl;
	buffer[0] = bytes[7];
	buffer[1] = bytes[6];
	buffer[2] = bytes[5];
	buffer[3] = bytes[4];
	buffer[4] = bytes[3];
	buffer[5] = bytes[2];
	buffer[6] = bytes[1];
	buffer[7] = bytes[0];

	return(dbl);

}/*end bytecodeToDouble*/

/*-----------------------------------------------------------------*/

/*
Routines below convert Intel values to bytecode format
*/

void wordToBytecode(U2 word, U1 arr[])
{
	U1 *buffer;

	buffer = (U1*)&word;
	arr[0] = buffer[1];
	arr[1] = buffer[0];

	return;

}/*end wordToBytecode*/

void dwordToBytecode(U4 dword, U1 arr[])
{
	U1 *buffer;

	buffer = (U1*)&dword;
	arr[0] = buffer[3];
	arr[1] = buffer[2];
	arr[2] = buffer[1];
	arr[3] = buffer[0];

	return;

}/*end dwordToBytecode*/

void qwordToBytecode(U8 qword, U1 arr[])
{
	U1 *buffer;

	buffer = (U1*)&qword;
	arr[0] = buffer[7];
	arr[1] = buffer[6];
	arr[2] = buffer[5];
	arr[3] = buffer[4];
	arr[4] = buffer[3];
	arr[5] = buffer[2];
	arr[6] = buffer[1];
	arr[7] = buffer[0];

	return;

}/*end qwordToBytecode*/

void floatToBytecode(F4 flt, U1 arr[])
{
	U1 *buffer;

	buffer = (U1*)&flt;
	arr[0] = buffer[3];
	arr[1] = buffer[2];
	arr[2] = buffer[1];
	arr[3] = buffer[0];

	return;

}/*end floatToBytecode*/

void doubleToBytecode(F8 dbl, U1 arr[])
{
	U1 *buffer;

	buffer = (U1*)&dbl;
	arr[0] = buffer[7];
	arr[1] = buffer[6];
	arr[2] = buffer[5];
	arr[3] = buffer[4];
	arr[4] = buffer[3];
	arr[5] = buffer[2];
	arr[6] = buffer[1];
	arr[7] = buffer[0];

	return;

}/*end doubleToBytecode*/

/*-----------------------------------------------------------------*/
/*
	this prints out the bytes of a datatype
*/

void printBytes(S1 bytes[], int nbytes)
{
	int i;
	for(i=0; i<nbytes; i++){ printf("byte[%u]=%X ",i,(U1)bytes[i]); }
	printf("\n");
	return;

}/*end printBytes*/

/*-----------------------------------------------------------------*/

/*
	Test native->bytecode and bytecode->native conversion
	Take a value
		i) print out the value
		ii) print out the byte pattern
		iii) convert to bytecode and print byte pattern
		iv) convert back to native and print out byte pattern
		v) print out the value ( should match that from i) )
*/

void testConversion()
{
	S2 w;
	S4 d;
	S8 q;
	F4 f;
	F8 db;
	S1 *b;
	S1 b2[2];
	S1 b4[4];
	S1 b8[8];

	w = (S2)0xAB12;
	d = 0xCAFEBABE;
	q = 0xABABECAB12345678;
	f = (F4)1.1e10;
	db = 1.2e10;

	checkEndian();

	printf("\n---------------------------------\n");
	printf("\nw=%hx\n",w);
	b = (S1 *)&w;
	printf("S1s before w: "); printBytes(b,2);
	wordToBytecode(w,(U1 *)b2); printf("Bytecode w: "); printBytes(b2,2);
	w = bytecodeToWord((U1 *)b2); printf("S1s after w: "); printBytes(b,2);
	printf("w=%hx\n",w);

	printf("\n---------------------------------\n");
	printf("\nd=%x\n",d);
	b = (S1 *)&d;
	printf("before d: "); printBytes(b,4);
	dwordToBytecode(d,(U1*)b4); printf("Bytecode d: "); printBytes(b4,4);
	d = bytecodeToDWord((U1*)b4); printf("after d: "); printBytes(b,4);
	printf("d=%x\n",d);

	printf("\n---------------------------------\n");
	printf("\nq=%I64x\n",q);
	b = (S1 *)&q;
	printf("before q: "); printBytes(b,8);
	qwordToBytecode(q,(U1*)b8); printf("byecode q: ");printBytes(b8,8);
	q = bytecodeToQWord((U1*)b8); printf("after q: "); printBytes(b,8);
	printf("q=%I64x\n",q);

	printf("\n---------------------------------\n");
	printf("\nf=%e\n",f);
	b = (S1 *)&f;
	printf("before f: "); printBytes(b,4);
	floatToBytecode(f,(U1*)b4);printf("Bytecode f: ");printBytes(b4,4);
	f = bytecodeToFloat((U1*)b4); printf("after f: "); printBytes(b,4);
	printf("\nf=%e\n",f);
	
	printf("\n---------------------------------\n");
	printf("\ndb=%e\n",db);
	b = (S1 *)&db;
	printf("before db: "); printBytes(b,8);
	doubleToBytecode(db,(U1*)b8);printf("Bytecode db: ");printBytes(b8,8);
	db = bytecodeToDouble((U1*)b8); printf("after db: "); printBytes(b,8);
	printf("db=%e\n",db);
	return;

}/*end testConversion*/

/*-----------------------------------------------------------------*/

/*
	Returns the amount of free physical memory in bytes.
*/

#include<windows.h> /* AAAAAhhhh! */

U4 getAvailableMemory()
{
	U4 free;
	MEMORYSTATUS mem_status;
	mem_status.dwLength = sizeof(MEMORYSTATUS);
	free = 0;
	GlobalMemoryStatus(&mem_status);
	free = mem_status.dwAvailPhys;
	return(free);

}/*end getAvailableMemory*/

/*
	Returns size of file in bytes
	Note, assume file will not be larger than 4GB
	because nFileSizeLow is lower 32-bits of file size
*/

U4 getFileSize(char *name)
{
	U4 size;
	S1 ret;
	WIN32_FILE_ATTRIBUTE_DATA fdata;

	size=0;
	ret = GetFileAttributesEx(name,GetFileExInfoStandard,&fdata);
	size = fdata.nFileSizeLow;
	
	if(ret > 0){ return(size); }
	else{ return(0); }

}/*end getFileSize*/

void testNativeFunctions()
{
	U4 ram;
	U4 mb;
	ram = getAvailableMemory();
	mb = ram/1048576;
	printf("free physical memory = %lu = %dMB\n",ram,mb);

	/*234,256*/
	ram = getFileSize("C:\\winnt\\explorer.exe");
	mb = ram/1048576;
	printf("size of file = %lu = %dMB\n",ram,mb);

	return;

}/*test native functions*/

/*-----------------------------------------------------------------*/

U8 stringToU8(char *str)
{ 
	return(_atoi64(str));

}/*end stringToU8*/




