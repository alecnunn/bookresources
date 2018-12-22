#include<stdio.h>
#include<stdlib.h>

#ifdef WINDOWS_32
#include<win32.c>		
#endif
#ifdef LINUX_x86
#include<linux.c>		
#endif

#define FATAL_ERROR();		exit(1);
#define FATAL_ERROR1(str);	printf(str);exit(1);

#define FCLOSE(arg);	if(fclose(arg)){ printf("could not close executable file\n"); }

#include<dbgdata.c>


void main(int argc, char *argv[])
{
	struct HeaderRec hr;
	FILE *fptr;
	U4 i;
	U4 str_index;

	if(argc==1)
	{
		printf("file name must be specified\n");
		return;
	}

	fptr = fopen(argv[1],"rb");
	if(fptr==NULL)
	{
		printf("cannot open %s\n",argv[1]);
		return;
	}

	populateDebugData(&hr, &debugData,fptr);

	printf("\n\nHEADER------------------------->\n");
	printf("magic=%lX\n",hr.magic);
	printf("bytes in symbol table %lu\n",hr.szSymTbl);
	printf("bytes in string table %lu\n",hr.szStrTbl);
	printf("bytes in bytecode %lu\n\n",hr.szByteCode);

	printf("SYMBOL TABLE------------------->\n");
	printDebugData(&debugData);

	if(fseek(fptr,(U4)(SIZE_HEADER+(hr.szSymTbl)),SEEK_SET))
	{
		printf("cannot open file to read string table\n");
		return;
	}

	printf("\nSTRING TABLE------------------->\n");
	str_index=1;
	printf("0)");
	for(i=0;i<hr.szStrTbl;i++)
	{
		char ch;
		ch = (char)fgetc(fptr);
		if(ch!='\0'){ printf("%c",ch); }
		else if(i<(hr.szStrTbl-1))
		{
			printf("(null)\n%lu)",str_index);
			str_index++;
		}
		else
		{
			printf("(null)\n");
		}
	}
	
	printf("\nBYTECODE----------------------->");
	for(i=0;i<hr.szByteCode;i++)
	{
		U1 ch;
		if(i%8==0){ printf("\naddress=%-20lu\t",i); }
		ch=(U1)fgetc(fptr);
		printf("[%3u]",ch);

	}

	if(fclose(fptr))
	{
		printf("could not close %s\n",argv[1]);
	}
	return;

}/*end main*/

