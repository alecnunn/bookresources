#include<osd/osd.h>

main()
{
	printf("char8     %d\n",8*sizeof(char8));
	printf("int8      %d\n",8*sizeof(int8));
	printf("uint8     %d\n",8*sizeof(uint8));
	printf("int16     %d\n",8*sizeof(int16));
	printf("uint16    %d\n",8*sizeof(uint16));
	printf("int32     %d\n",8*sizeof(int32));
	printf("uint32    %d\n",8*sizeof(uint32));
	printf("int64     %d\n",8*sizeof(int64));
	printf("uint64    %d\n",8*sizeof(uint64));
	printf("real32    %d\n",8*sizeof(real32));
	printf("real64    %d\n",8*sizeof(real64));
	printf("word      %d\n",8*sizeof(word));
	printf("uword     %d\n",8*sizeof(uword));
	printf("\n");
	printf("char      %d\n",8*sizeof(char));
	printf("short     %d\n",8*sizeof(short));
	printf("int       %d\n",8*sizeof(int));
	printf("long      %d\n",8*sizeof(long));
	printf("long long %d\n",8*sizeof(long long));
	printf("\n");
	if(sizeof(char8)  != 1){ printf("ERROR: char8 is wrong size\n");}
	if(sizeof(int8)   != 1){ printf("ERROR: int8 is wrong size\n");}
	if(sizeof(uint8)  != 1){ printf("ERROR: uint8 is wrong size\n");}
	if(sizeof(int16)  != 2){ printf("ERROR: int16 is wrong size\n");}
	if(sizeof(uint16) != 2){ printf("ERROR: uint16 is wrong size\n");}
	if(sizeof(int32)  != 4){ printf("ERROR: int32 is wrong size\n");}
	if(sizeof(uint32) != 4){ printf("ERROR: uint32 is wrong size\n");}
	if(sizeof(int64)  != 8){ printf("ERROR: int64 is wrong size\n");}
	if(sizeof(uint64) != 8){ printf("ERROR: uint64 is wrong size\n");}
	if(sizeof(real32) != 4){ printf("ERROR: real32 is wrong size\n");}
	if(sizeof(real64) != 8){ printf("ERROR: real64 is wrong size\n");}
}


